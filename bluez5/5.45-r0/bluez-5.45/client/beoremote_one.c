#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <wordexp.h>

#include "gdbus/gdbus.h"
#include "display.h"
#include "gatt.h"
#include "beoremote_one.h"

#define REBOOT_TIMEOUT 500
#define INITIALIZE_TIMEOUT 500
#define WRITE_TIMEOUT 1000
#define WRITE_RETRY_MAX 3

static FILE *image = NULL;
static bool force_update = false;
static struct ImageHeader image_header;
static enum CommandType command;
static struct Report report;
static uint32_t offset = 0;
static int sent_bytes = 0;
static int result = 0;
static guint write_timer_id = 0;
static int write_retry_count = 0;
static uint8_t buffer[REQUEST_SIZE];
static uint8_t buffer_index = 0;
static char *address = NULL;

static GMainLoop *main_loop;
static DBusConnection *dbus_conn;
static struct adapter *default_ctrl;
static GDBusProxy *agent_manager;
static GDBusProxy *default_dev;
static GDBusProxy *default_attr;
static GDBusProxy *proxy_in;
static GDBusProxy *proxy_out;
static GList *ctrl_list;

struct adapter {
	GDBusProxy *proxy;
	GList *devices;
};

static void set_default_device(GDBusProxy *proxy, const char *attribute)
{
	default_dev = proxy;

	if (proxy == NULL)
		default_attr = NULL;
}

static void set_default_attribute(GDBusProxy *proxy)
{
	const char *path;

	default_attr = proxy;
	path = g_dbus_proxy_get_path(proxy);
	set_default_device(default_dev, path);
}

static gboolean device_is_child(GDBusProxy *device, GDBusProxy *master)
{
	DBusMessageIter iter;
	const char *adapter, *path;

	if (!master)
		return FALSE;

	if (g_dbus_proxy_get_property(device, "Adapter", &iter) == FALSE)
		return FALSE;

	dbus_message_iter_get_basic(&iter, &adapter);
	path = g_dbus_proxy_get_path(master);

	if (!strcmp(path, adapter))
		return TRUE;

	return FALSE;
}

static gboolean service_is_child(GDBusProxy *service)
{
	GList *l;
	DBusMessageIter iter;
	const char *device, *path;

	if (g_dbus_proxy_get_property(service, "Device", &iter) == FALSE)
		return FALSE;

	dbus_message_iter_get_basic(&iter, &device);

	if (!default_ctrl)
		return FALSE;

	for (l = default_ctrl->devices; l; l = g_list_next(l)) {
		struct GDBusProxy *proxy = l->data;

		path = g_dbus_proxy_get_path(proxy);

		if (!strcmp(path, device))
			return TRUE;
	}

	return FALSE;
}

static struct adapter *find_parent(GDBusProxy *device)
{
	GList *list;

	for (list = g_list_first(ctrl_list); list; list = g_list_next(list)) {
		struct adapter *adapter = list->data;

		if (device_is_child(device, adapter->proxy) == TRUE)
			return adapter;
	}

	return NULL;
}

static gboolean write_timeout(gpointer user_data)
{
   if (write_retry_count++ < WRITE_RETRY_MAX) {
      write_timer_id = g_timeout_add(WRITE_TIMEOUT, &write_timeout, NULL);
      gatt_write_attribute_length(proxy_out, &report, sizeof(struct Report));      
   } else {
      write_retry_count = 0;
      result = -ETIME;
      printf("Timeout\n");
      g_main_loop_quit(main_loop);
   }

	return FALSE;
}

static void start_write_timer()
{
	write_timer_id = g_timeout_add(WRITE_TIMEOUT, &write_timeout, NULL);
}

static void stop_write_timer()
{
	if (write_timer_id) {
		g_source_remove(write_timer_id);
		write_timer_id = 0;
	}
}

static void send_request(enum RequestType request_type)
{
	report.message.request.type_length = TYPE_LENGTH_REQUEST;
	report.message.request.request_type = request_type;
	report.message.request.option = 0;
	memset(report.message.request.data, 0, sizeof(report.message.request.data));

	start_write_timer();
	gatt_write_attribute_length(proxy_out, &report, sizeof(struct Report));
}

static void send_image_info_request(enum ImageType image_type)
{
	report.message.request.type_length = TYPE_LENGTH_REQUEST_WITH_OPTION;
	report.message.request.request_type = REQUEST_IMAGE_INFO;
	report.message.request.option = image_type;
	memset(report.message.request.data, 0, sizeof(report.message.request.data));

	start_write_timer();
	gatt_write_attribute_length(proxy_out, &report, sizeof(struct Report));
}

static void sendImageUpdateAnnouncement()
{
	report.message.image_update_announcement.type_length = TYPE_LENGTH_IMAGE_UPDATE;
	report.message.image_update_announcement.image_type = image_header.image_type;
	report.message.image_update_announcement.size = image_header.size;
	report.message.image_update_announcement.crc = image_header.crc;
	report.message.image_update_announcement.major = image_header.major;
	report.message.image_update_announcement.minor = image_header.minor;
	report.message.image_update_announcement.release = image_header.release;
	report.message.image_update_announcement.min_major = 0x00;
	report.message.image_update_announcement.min_minor = 0x00;
	report.message.image_update_announcement.max_major = 0xFF;
	report.message.image_update_announcement.max_minor = 0xFF;
	memset(report.message.image_update_announcement.filler, 0, 2);

	start_write_timer();
	gatt_write_attribute_length(proxy_out, &report, sizeof(struct Report));
}

static bool open_image(char *filename)
{
	if ((image = fopen(filename, "rb"))) {
		size_t size = sizeof(struct ImageHeader);

		if (fread(&image_header, 1, size, image) == size) {
			fseek(image, 0, SEEK_SET);
			return true;
		}
	}

	fprintf(stderr, "Failed to open image: %s\n", filename);
	return false;
}

static void close_image()
{
	if (image) {
		fclose(image);
		image = NULL;
	}
}

static const char *image_type_to_string(uint8_t image_type)
{
	static const char *strings[IMAGE_MAX] = {"Bootloader", "Application", "Font",
		"User database", "Static database"};

	if (image_type < IMAGE_MAX)
		return strings[image_type];
	else
		return "Unknown";
}

static void image_info_response()
{
	uint32_t local_version, remote_version;
	struct ImageInfoResponse *response = &report.message.image_info_response;

	switch (command) {
	case COMMAND_BOOTLOADER_VERSION:
	case COMMAND_APPLICATION_VERSION:
	case COMMAND_FONT_VERSION:
	case COMMAND_USER_DB_VERSION:
	case COMMAND_STATIC_DB_VERSION:
		printf("%s version: %d.%d.%d\n", image_type_to_string(response->image_type),
			response->major, response->minor, response->release);
		g_main_loop_quit(main_loop);
		break;
	case COMMAND_UPDATE_IMAGE:
		local_version = image_header.major << 24 | image_header.minor << 16 | image_header.release;
		remote_version = response->major << 24 | response->minor << 16 | response->release;

		if ((local_version > remote_version) || force_update) {
			printf("Updating %s to version %d.%d.%d\n", image_type_to_string(image_header.image_type),
				image_header.major, image_header.minor, image_header.release);
			sendImageUpdateAnnouncement();
		} else {
			printf("%s is already version %d.%d.%d\n", image_type_to_string(response->image_type),
				response->major, response->minor, response->release);
			g_main_loop_quit(main_loop);
		}
		break;
	case COMMAND_DEVICE_INFO:
	case COMMAND_KEY_COUNTERS:
	case COMMAND_RESET_KEY_COUNTERS:
	case COMMAND_REBOOT:
	case COMMAND_HELP:
		break;
	}
}

static void device_info_response()
{
	struct DeviceInfoResponse *response = &report.message.device_info_response;

	printf("Device type: 0x%04X\n", response->device_type);
	printf("Serial number: %X%X%X%X%X%X%X%X\n", response->serial_number[0], response->serial_number[1],
		response->serial_number[2], response->serial_number[3], response->serial_number[4],
		response->serial_number[5], response->serial_number[6], response->serial_number[7]);
	printf("BT address: %02X:%02X:%02X:%02X:%02X:%02X\n", response->bt_address[5], response->bt_address[4],
		response->bt_address[3], response->bt_address[2], response->bt_address[1], response->bt_address[0]);

	g_main_loop_quit(main_loop);
}

static void key_counters_response()
{
	struct KeyCountersResponse *response = &report.message.key_counters_response;

	printf("Key %d counter: %d\n", response->key, response->key_counter);
	printf("Key %d counter: %d\n", response->key + 1, response->key_counter_1);
	printf("Key %d counter: %d\n", response->key + 2, response->key_counter_2);
	printf("Key %d counter: %d\n", response->key + 3, response->key_counter_3);

	if (response->key == 40)
		g_main_loop_quit(main_loop);
}

static void reset_key_counters_response()
{
	struct ResetKeyCountersResponse *response = &report.message.reset_key_counters_response;

	if (response->status)
		printf("Failed\n");
	else
		printf("Success\n");

	result = -response->status;
	g_main_loop_quit(main_loop);
}

static void update_complete_response()
{
	struct UpdateCompleteResponse *response = &report.message.update_complete_response;
	static const char *status[STATUS_MAX] = {"Success", "Incompatible version", "Bad size",
		"Bad CRC", "Cancelled", "Timeout"};

	if (response->status < STATUS_MAX) {
		printf("%s\n", status[response->status]);
	} else
		fprintf(stderr, "Invalid status: %d\n", response->status);

	result = -response->status;
	g_main_loop_quit(main_loop);
}

static void printProgress(int offset, int size)
{
	static int percent = 0;

	if (offset >= ((size / 100) * percent)) {
		fprintf(stderr, "%d%%..", percent);
		percent += 10;
	}
}

static void data_request()
{
	int i, bytes;
	struct DataRequest *request = &report.message.data_request;
	struct Report report;
	uint16_t length = sizeof(report.message.data.data);

	stop_write_timer();

	if (request->offset != offset) {
		sent_bytes = 0;
		offset = request->offset;
	}

	if (sent_bytes >= request->length)
		return;

	if ((request->length - sent_bytes) < length)
		length = request->length - sent_bytes;

	report.message.data.type_length = TYPE_LENGTH_DATA + length;
	report.message.data.offset = (offset + sent_bytes) & 0xFFFF;
	bytes = fread(&report.message.data.data[0], 1, length, image);
	for (i = bytes; i < (int)sizeof(report.message.data.data); i++) {
		report.message.data.data[i] = 0x00;
	}

	write_timer_id = g_timeout_add(WRITE_TIMEOUT, &write_timeout, NULL);
	gatt_write_attribute_length(proxy_out, &report, sizeof(struct Report));
	sent_bytes += sizeof(report.message.data.data);
	printProgress(offset + sent_bytes, image_header.size);
}

static void device_added(GDBusProxy *proxy)
{
	DBusMessageIter iter;
	struct adapter *adapter = find_parent(proxy);

	if (!adapter)
		return;

	adapter->devices = g_list_append(adapter->devices, proxy);

	if (default_dev)
		return;

	if (g_dbus_proxy_get_property(proxy, "Connected", &iter)) {
		dbus_bool_t connected;

		dbus_message_iter_get_basic(&iter, &connected);

		if (address && g_dbus_proxy_get_property(proxy, "Address", &iter)) {
			const char *addr;

			dbus_message_iter_get_basic(&iter, &addr);

			if (strncasecmp(addr, address, strlen(addr)))
				return;
		}

		if (connected)
			set_default_device(proxy, NULL);
	}
}

static void adapter_added(GDBusProxy *proxy)
{
	struct adapter *adapter = g_malloc0(sizeof(struct adapter));

	adapter->proxy = proxy;
	ctrl_list = g_list_append(ctrl_list, adapter);

	if (!default_ctrl)
		default_ctrl = adapter;
}

static void proxy_added(GDBusProxy *proxy, void *user_data)
{
	const char *interface;

	interface = g_dbus_proxy_get_interface(proxy);

	if (!strcmp(interface, "org.bluez.Device1")) {
		device_added(proxy);
	} else if (!strcmp(interface, "org.bluez.Adapter1")) {
		adapter_added(proxy);
	} else if (!strcmp(interface, "org.bluez.AgentManager1")) {
		if (!agent_manager)
			agent_manager = proxy;
	} else if (!strcmp(interface, "org.bluez.GattService1")) {
		if (service_is_child(proxy))
			gatt_add_service(proxy);
	} else if (!strcmp(interface, "org.bluez.GattCharacteristic1")) {
		gatt_add_characteristic(proxy);
	} else if (!strcmp(interface, "org.bluez.GattDescriptor1")) {
		gatt_add_descriptor(proxy);
	} else if (!strcmp(interface, "org.bluez.GattManager1")) {
		gatt_add_manager(proxy);
	}
}

static void device_removed(GDBusProxy *proxy)
{
	struct adapter *adapter = find_parent(proxy);
	if (!adapter) {
		return;
	}

	adapter->devices = g_list_remove(adapter->devices, proxy);

	if (default_dev == proxy)
		set_default_device(NULL, NULL);
}

static void adapter_removed(GDBusProxy *proxy)
{
	GList *ll;

	for (ll = g_list_first(ctrl_list); ll; ll = g_list_next(ll)) {
		struct adapter *adapter = ll->data;

		if (adapter->proxy == proxy) {
			if (default_ctrl && default_ctrl->proxy == proxy) {
				default_ctrl = NULL;
				set_default_device(NULL, NULL);
			}

			ctrl_list = g_list_remove_link(ctrl_list, ll);
			g_list_free(adapter->devices);
			g_free(adapter);
			g_list_free(ll);
			return;
		}
	}
}

static void proxy_removed(GDBusProxy *proxy, void *user_data)
{
	const char *interface;

	interface = g_dbus_proxy_get_interface(proxy);

	if (!strcmp(interface, "org.bluez.Device1")) {
		device_removed(proxy);
	} else if (!strcmp(interface, "org.bluez.Adapter1")) {
		adapter_removed(proxy);
	} else if (!strcmp(interface, "org.bluez.GattService1")) {
		gatt_remove_service(proxy);
		if (default_attr == proxy)
			set_default_attribute(NULL);
	} else if (!strcmp(interface, "org.bluez.GattCharacteristic1")) {
		gatt_remove_characteristic(proxy);
		if (default_attr == proxy)
			set_default_attribute(NULL);
	} else if (!strcmp(interface, "org.bluez.GattDescriptor1")) {
		gatt_remove_descriptor(proxy);
		if (default_attr == proxy)
			set_default_attribute(NULL);
	} else if (!strcmp(interface, "org.bluez.GattManager1")) {
		gatt_remove_manager(proxy);
	}
}

static void property_changed(GDBusProxy *proxy, const char *name,
	DBusMessageIter *iter, void *user_data)
{
	uint8_t value;
	const char *interface;
	DBusMessageIter subiter;

	interface = g_dbus_proxy_get_interface(proxy);

	if (!strcmp(interface, "org.bluez.Device1")) {
		if (default_ctrl && device_is_child(proxy, default_ctrl->proxy) == TRUE) {
			if (strcmp(name, "Connected") == 0) {
				dbus_bool_t connected;

				dbus_message_iter_get_basic(iter, &connected);

				if (connected && default_dev == NULL)
					set_default_device(proxy, NULL);
				else if (!connected && default_dev == proxy)
					set_default_device(NULL, NULL);
			}
		}
	} else if (proxy == default_attr) {
		switch (dbus_message_iter_get_arg_type(iter)) {
		case DBUS_TYPE_ARRAY:
			dbus_message_iter_recurse(iter, &subiter);
			while (dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_INVALID) {
				switch (dbus_message_iter_get_arg_type(&subiter)) {
				case DBUS_TYPE_BYTE:
					dbus_message_iter_get_basic(&subiter, &value);
					buffer[buffer_index++] = value;
					break;
				default:
					break;
				}
				dbus_message_iter_next(&subiter);
			}
			break;
		case DBUS_TYPE_BYTE:
			dbus_message_iter_get_basic(iter, &value);
			buffer[buffer_index++] = value;
			break;
		default:
			break;
		}

		if (buffer_index == REQUEST_SIZE) {
			buffer_index = 0;
			memcpy(&report, buffer, sizeof(struct Report));

			stop_write_timer();
			gatt_unregister_write_callback();

			switch (report.message.response.type_length) {
			case TYPE_LENGTH_IMAGE_INFO:
				image_info_response();
				break;
			case TYPE_LENGTH_DATA_REQUEST:
				gatt_register_write_callback(&data_request);
				data_request();
				break;
			case TYPE_LENGTH_UPDATE_COMPLETE:
				update_complete_response();
				break;
			case TYPE_LENGTH_DEVICE_INFO:
				device_info_response();
				break;
			case TYPE_LENGTH_KEY_COUNTERS:
				key_counters_response();
				break;
			case TYPE_LENGTH_REQUEST_WITH_OPTION:
				reset_key_counters_response();
				break;
			default:
				break;
			}
		}
	}
}

static bool string_to_command(const char *string, enum CommandType *command)
{
	int i;
	static const char *commands[IMAGE_MAX] = {"boot", "app", "font", "user", "static"};

	for (i = 0; i < IMAGE_MAX; i++) {
		if (strcmp(commands[i], string) == 0) {
			*command = (enum CommandType)i;
			return true;
		}
	}

	fprintf(stderr, "Invalid argument: %s\n", optarg);
	return false;
}

static bool initialize_attributes()
{
	const char *path;
	char input_char[128], output_char[128];

	path = g_dbus_proxy_get_path(default_dev);

	if (path) {
		strcpy(input_char, path);
		strcat(input_char, "/service0022/char0023");
		strcpy(output_char, path);
		strcat(output_char, "/service0022/char0025");
	}

	proxy_in = gatt_select_attribute(default_attr, input_char);
	proxy_out = gatt_select_attribute(default_attr, output_char);

	if (proxy_in && proxy_out) {
		set_default_attribute(proxy_in);
		gatt_notify_attribute(proxy_in, true);
	} else {
		fprintf(stderr, "No remote connected\n");
		g_main_loop_quit(main_loop);
	}

	return (proxy_in && proxy_out);
}

static gboolean reboot_timeout(gpointer user_data)
{
	g_main_loop_quit(main_loop);
	return FALSE;
}

static gboolean initialize_timeout(gpointer user_data)
{
	if (initialize_attributes()) {
		switch (command) {
		case COMMAND_BOOTLOADER_VERSION:
			send_image_info_request(IMAGE_BOOTLOADER);
			break;
		case COMMAND_APPLICATION_VERSION:
			send_image_info_request(IMAGE_APPLICATION);
			break;
		case COMMAND_FONT_VERSION:
			send_image_info_request(IMAGE_FONT);
			break;
		case COMMAND_USER_DB_VERSION:
			send_image_info_request(IMAGE_USER_DB);
			break;
		case COMMAND_STATIC_DB_VERSION:
			send_image_info_request(IMAGE_STATIC_DB);
			break;
		case COMMAND_UPDATE_IMAGE:
			send_image_info_request(image_header.image_type);
			break;
		case COMMAND_DEVICE_INFO:
			send_request(REQUEST_DEVICE_INFO);
			break;
		case COMMAND_KEY_COUNTERS:
			send_request(REQUEST_KEY_COUNTERS);
			break;
		case COMMAND_RESET_KEY_COUNTERS:
			send_request(REQUEST_RESET_KEY_COUNTERS);
			break;
		case COMMAND_REBOOT:
			send_request(REQUEST_REBOOT);
			g_timeout_add(REBOOT_TIMEOUT, &reboot_timeout, NULL);
			break;
		case COMMAND_HELP:
			break;
		}
	}

	return FALSE;
}

static void help(char **argv)
{
	printf("%s [option]\n", argv[0]);
	printf("  -a [addr]  Connect to the specified address\n");
	printf("  -i [type]  Show image information. Possible types are:\n");
	printf("             boot, app, font, user and static\n");
	printf("  -r         Reboot the remote\n");
	printf("  -f         Force update even if version is lower\n");
	printf("  -d         Show device information\n");
	printf("  -k         Show key counters\n");
	printf("  -e         Reset key counters\n");
	printf("  -u [file]  Update the remote with the specified file\n");

	exit(-EINVAL);
}

int main(int argc, char **argv)
{
	int opt;
	GDBusClient *client;

	opterr = 0;
	command = COMMAND_HELP;

	while ((opt = getopt(argc, argv, "a:i:u:rfdke")) != -1) {
		switch (opt) {
		case 'a':
			address = optarg;
			break;
		case 'i':
			if (!string_to_command(optarg, &command))
				return -EINVAL;
			break;
		case 'r':
			command = COMMAND_REBOOT;
			break;
		case 'f':
			force_update = true;
			break;
		case 'd':
			command = COMMAND_DEVICE_INFO;
			break;
		case 'k':
			command = COMMAND_KEY_COUNTERS;
			break;
		case 'e':
			command = COMMAND_RESET_KEY_COUNTERS;
			break;
		case 'u':
			if (!open_image(optarg))
				return -ENOENT;
			command = COMMAND_UPDATE_IMAGE;
			break;
		default:
			break;
		}
	}

	if (command == COMMAND_HELP)
		help(argv);

	rl_disable_output();
	main_loop = g_main_loop_new(NULL, FALSE);
	dbus_conn = g_dbus_setup_bus(DBUS_BUS_SYSTEM, NULL, NULL);
	client = g_dbus_client_new(dbus_conn, "org.bluez", "/org/bluez");
	g_dbus_client_set_proxy_handlers(client, proxy_added, proxy_removed, property_changed, NULL);
	g_timeout_add(INITIALIZE_TIMEOUT, &initialize_timeout, NULL);

	g_main_loop_run(main_loop);

	g_dbus_client_unref(client);
	dbus_connection_unref(dbus_conn);
	g_main_loop_unref(main_loop);
	g_list_free_full(ctrl_list, NULL);
	close_image();

	return result;
}