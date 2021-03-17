#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <lib/bluetooth.h>
#include <lib/uuid.h>
#include <lib/bluetooth/sdp.h>
#include <src/adapter.h>
#include <src/device.h>
#include <unistd.h>
#include <src/plugin.h>
#include <attrib/gatt-service.h>
#include <attrib/att-database.h>
#include <attrib/gattrib.h>
#include <src/attrib-server.h>
#include <attrib/att.h>
#include <attrib/gatt.h>
#include <src/log.h>

#include "beoremote_one.h"

int beoremote_one_socket = -1;
GMutex beoremote_one_mutex;
volatile gboolean beoremote_one_thread_stop = FALSE;
GThread *beoremote_one_thread = NULL;
bdaddr_t connected_device_address[BEOREMOTE_ONE_CONNECTED_MAX];
beoremote_one_data_t beoremote_one_data[BEOREMOTE_ONE_ATTRIB_MAX];
#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
int lock_count = 0;
#endif

typedef struct {
    struct btd_adapter *adapter;
    GSList *sdp_handles;
    uint16_t value_handle[BEOREMOTE_ONE_ATTRIB_MAX];
    uint16_t ccc_handle[BEOREMOTE_ONE_ATTRIB_MAX];
} beoremote_one_adapter_t;

static beoremote_one_adapter_t *beoremote_one_adapter = NULL;

typedef struct {
    enum beoremote_one_attrib attrib_type;
    size_t size;
} attrib_changed_payload_t;

static int device_address_or_local_to_str(struct btd_device *device, char *str)
{
    const bdaddr_t *device_address = NULL;
    int result;

    if (device) {
        device_address = device_get_address(device);
    }
    result = ba2str(device_address ? device_address : BDADDR_ANY, str);
    if (result != BA_ADDRESS_STRING_BUFFER_SIZE - 1) {
        error("Failed to convert addess to string, ba2str returned %d", result);
    }
    return result;
}

static gboolean do_attrib_changed(gpointer);
static void do_data_free(attrib_changed_payload_t *);
static void notify_attrib_change(enum beoremote_one_attrib, const uint8_t *, size_t);

int attrib_changed(enum beoremote_one_attrib attrib_type, size_t size)
{
    attrib_changed_payload_t *data;

#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
    DBG("(0x%04x)%s", beoremote_one_data[attrib_type].handle, beoremote_one_attrib_to_string(attrib_type));
#endif
    data = g_slice_new(attrib_changed_payload_t);
    data->attrib_type = attrib_type;
    data->size = size;

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("g_slice_new %p", data);
#endif
    g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, do_attrib_changed, (gpointer)data, (GDestroyNotify)do_data_free);

    return 0;
}

static struct btd_adapter *get_adapter()
{
    if (beoremote_one_adapter) {
        return beoremote_one_adapter->adapter;
    }
    return NULL;
}

static gboolean do_attrib_changed(gpointer data)
{
    attrib_changed_payload_t *payload = (attrib_changed_payload_t *)data;
    struct btd_adapter *adapter;
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
    char string_buffer[ATTRIB_DATA_AS_STRING_BUFFER_SIZE];
#endif

    if (payload) {
        aqquire_lock();

        adapter = get_adapter();
        if (adapter) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
            DBG("Update attribute (0x%04x)%s:[%s]",
                beoremote_one_data[payload->attrib_type].handle,
                beoremote_one_attrib_to_string(payload->attrib_type),
                beoremote_one_attrib_data_to_string(
                    payload->attrib_type,
                    beoremote_one_data[payload->attrib_type].value,
                    beoremote_one_attrib_size((enum beoremote_one_attrib)payload->attrib_type),
                    &string_buffer,
                    sizeof(string_buffer)));
#endif
            attrib_db_update(adapter, beoremote_one_data[payload->attrib_type].handle, NULL, beoremote_one_data[payload->attrib_type].value, payload->size, NULL);
        }

        if (payload->attrib_type == BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED
            || payload->attrib_type == BEOREMOTE_ONE_ATTRIB_VOLUME) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
            DBG("notify_attrib_change for attribute %s", beoremote_one_attrib_to_string(payload->attrib_type));
#endif
            notify_attrib_change((enum beoremote_one_attrib)payload->attrib_type, beoremote_one_data[payload->attrib_type].value, payload->size);
        }

        release_lock();
    }

    return FALSE;
}

static void do_data_free(attrib_changed_payload_t *data)
{
#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("g_slice_free %p", data);
#endif
    if (data) {
        g_slice_free(attrib_changed_payload_t, data);
    }
}

static void notify_attrib_change(enum beoremote_one_attrib attrib_type, const uint8_t *data, size_t size)
{
    int i, result;
    struct btd_device *device;
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
    char device_address_str[BA_ADDRESS_STRING_BUFFER_SIZE] = { 0 };
#endif

    if (beoremote_one_adapter && beoremote_one_adapter->adapter) {
        for (i = 0; i < BEOREMOTE_ONE_CONNECTED_MAX; i++) {
            if (bacmp(&connected_device_address[i], BDADDR_ANY)) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                ba2str(&connected_device_address[i], &device_address_str[0]);
#endif
                device = btd_adapter_find_device(beoremote_one_adapter->adapter, &connected_device_address[i], BDADDR_LE_PUBLIC);
                if (device) {
                    uint16_t handle = beoremote_one_adapter->value_handle[attrib_type];
                    uint16_t ccc_handle = beoremote_one_adapter->ccc_handle[attrib_type];
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                    DBG("attrib_notify to device %s for attribute %s(0x%04x,0x%04x)", device_address_str, beoremote_one_attrib_to_string(attrib_type), handle, ccc_handle);
#endif
                    result = attrib_notify(device, handle, ccc_handle, data, size, FALSE);
                }

                if (!device || result) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                    DBG("Device with address %s not found. Clearing index %d", device_address_str, i);
#endif
                    bacpy(&connected_device_address[i], BDADDR_ANY);
                }
            }
        }
    }
}

void update_connected_device_address(struct btd_device *device)
{
    int i;
    char device_address_str[BA_ADDRESS_STRING_BUFFER_SIZE] = { 0 };

    for (i = 0; i < BEOREMOTE_ONE_CONNECTED_MAX; i++) {
        if (bacmp(&connected_device_address[i], device_get_address(device)) == 0) {
            break;
        }
    }

    if (i == BEOREMOTE_ONE_CONNECTED_MAX) {
        for (i = 0; i < BEOREMOTE_ONE_CONNECTED_MAX; i++) {
            if (bacmp(&connected_device_address[i], BDADDR_ANY) == 0) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                device_address_or_local_to_str(device, &device_address_str[0]);
                DBG("Updating index %d with device address %s", i, device_address_str);
#endif
                bacpy(&connected_device_address[i], device_get_address(device));
                break;
            }
        }

        if (i == BEOREMOTE_ONE_CONNECTED_MAX) {
            device_address_or_local_to_str(device, &device_address_str[0]);
            warn("Failed to add device address %s. Too many remotes connected", device_address_str);
        }
    }
}

static gboolean beoremote_one_service_register(beoremote_one_adapter_t *adapter)
{
    gboolean gret;
    bt_uuid_t uuid[BEOREMOTE_ONE_ATTRIB_MAX];
    int attrib_type;

    /* Create the different UUIDs based on the BeoRemote One GATT service specification */
    bt_string_to_uuid(&uuid[0], "00000000-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_VERSION], "00000001-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_FEATURES], "00000002-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED], "00000003-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_INJECT_PRESS], "00000004-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_INJECT_RELEASE], "00000005-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_DISC_TRACK], "00000006-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_STAND_POSITIONS], "00000007-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION], "00000008-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SPEAKER_GROUPS], "0000000D-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP], "0000000E-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOUND_MODES], "0000000F-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE], "00000010-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_FORMATS], "00000011-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT], "00000012-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_MODES], "00000013-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE], "00000014-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_MUTE], "00000015-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_2D_3D_MODES], "00000016-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE], "00000017-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_TV_SOURCES], "00000018-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_MUSIC_SOURCES], "00000019-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE], "0000001A-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_CUSTOM_COMMANDS], "0000001B-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND], "0000001C-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_HOME_CONTROL_SCENES], "0000001D-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE], "0000001E-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_CINEMA_MODE], "0000001F-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_EXPERIENCES], "00000020-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE], "00000021-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_CONTROL_1], "00000022-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_CONTROL_2], "00000023-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_1], "00000024-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_2], "00000025-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_3], "00000026-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_4], "00000027-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_5], "00000028-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_6], "00000029-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_7], "0000002A-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_8], "0000002B-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_9], "0000002C-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_10], "0000002D-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT], "0000002E-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_MY_BUTTONS], "0000002F-0000-1000-1000-00805F9B34FB");
    bt_string_to_uuid(&uuid[BEOREMOTE_ONE_ATTRIB_VOLUME], "00000030-0000-1000-1000-00805F9B34FB");

    /* IMPORTANT: When adding new characterists to the service below they MUST be added to the top of the list
     * This is so the exists characteristics does not get a new handle (which bluez assigsn from the bottom up).
     * BeoRemoteOne is caching the handles, so changing them will break the functionality
     */
    gret = gatt_service_add(beoremote_one_adapter->adapter,
        /* Service UUID */
        GATT_PRIM_SVC_UUID, &uuid[0],

        /* Volume characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_VOLUME],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE | GATT_CHR_PROP_NOTIFY,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CCC_GET_HANDLE, &beoremote_one_adapter->ccc_handle[BEOREMOTE_ONE_ATTRIB_VOLUME],
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_VOLUME],

        /* Home control scenes characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_HOME_CONTROL_SCENES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_HOME_CONTROL_SCENES],

        /* Active home control scene characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE],

        /* Cinema mode characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_CINEMA_MODE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_CINEMA_MODE],

        /* Experiences characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_EXPERIENCES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_EXPERIENCES],

        /* Active experience characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE],

        /* Control 1 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_CONTROL_1],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_CONTROL_1],

        /* Control 2 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_CONTROL_2],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_CONTROL_2],

        /* Source content 1 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_1],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_1],

        /* Source content 2 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_2],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_2],

        /* Source content 3 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_3],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_3],

        /* Source content 4 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_4],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_4],

        /* Source content 5 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_5],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_5],

        /* Source content 6 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_6],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_6],

        /* Source content 7 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_7],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_7],

        /* Source content 8 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_8],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_8],

        /* Source content 9 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_9],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_9],

        /* Source content 10 characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_10],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_10],

        /* Active source content 1-10 characteristics */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT],

        /* Version characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_VERSION],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_VERSION],

        /* Features characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_FEATURES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_FEATURES],

        /* Features changed characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_INDICATE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CCC_GET_HANDLE, &beoremote_one_adapter->ccc_handle[BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED],
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED],

        /* Inject press characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_INJECT_PRESS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_INJECT_PRESS],

        /* Inject release characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_INJECT_RELEASE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_INJECT_RELEASE],

        /* Disc track characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_DISC_TRACK],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_DISC_TRACK],

        /* Stand positions characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_STAND_POSITIONS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_STAND_POSITIONS],

        /* Active stand position characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION],

        /* Speaker groups characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SPEAKER_GROUPS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SPEAKER_GROUPS],

        /* Active speaker group characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP],

        /* Sound modes characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_SOUND_MODES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_SOUND_MODES],

        /* Active sound mode characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE],

        /* Picture formats characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_FORMATS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_PICTURE_FORMATS],

        /* Active picture format characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT],

        /* Picture modes characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_MODES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_PICTURE_MODES],

        /* Active picture mode characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE],

        /* Picture mute characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_PICTURE_MUTE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_PICTURE_MUTE],

        /* 2D/3D modes characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_2D_3D_MODES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_2D_3D_MODES],

        /* Active 2D/3D mode characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE],

        /* TV sources characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_TV_SOURCES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_TV_SOURCES],

        /* Music sources characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_MUSIC_SOURCES],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_MUSIC_SOURCES],

        /* Active source characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE],

        /* Custom commands characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_CUSTOM_COMMANDS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_CUSTOM_COMMANDS],

        /* Active custom command characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ | GATT_CHR_PROP_WRITE,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_WRITE, beoremote_one_attrib_write, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND],

        /* My buttons characteristic */
        GATT_OPT_CHR_UUID, &uuid[BEOREMOTE_ONE_ATTRIB_MY_BUTTONS],
        GATT_OPT_CHR_PROPS, GATT_CHR_PROP_READ,
        GATT_OPT_CHR_VALUE_CB, ATTRIB_READ, beoremote_one_attrib_read, beoremote_one_adapter->adapter,
        GATT_OPT_CHR_VALUE_GET_HANDLE, &beoremote_one_adapter->value_handle[BEOREMOTE_ONE_ATTRIB_MY_BUTTONS],

        GATT_OPT_INVALID);

    if (gret) {
        for (attrib_type = 0; attrib_type < BEOREMOTE_ONE_ATTRIB_MAX; attrib_type++) {
            beoremote_one_data[attrib_type].handle = beoremote_one_adapter->value_handle[attrib_type];
        }

        /* Update version attribute */
        aqquire_lock();
        static const char *ATTRIB_VERSION = "1.0";
        memcpy(beoremote_one_data[BEOREMOTE_ONE_ATTRIB_VERSION].value, ATTRIB_VERSION, strlen(ATTRIB_VERSION));
        release_lock();
        attrib_changed(BEOREMOTE_ONE_ATTRIB_VERSION, strlen(ATTRIB_VERSION));
    }

    return gret;
}

static void beoremote_one_adapter_remove(struct btd_adapter *adapter)
{
    if (beoremote_one_adapter) {
        while (beoremote_one_adapter->sdp_handles) {
            uint32_t handle = GPOINTER_TO_UINT(beoremote_one_adapter->sdp_handles->data);

            attrib_free_sdp(adapter, handle);
            beoremote_one_adapter->sdp_handles = g_slist_remove(beoremote_one_adapter->sdp_handles,
                beoremote_one_adapter->sdp_handles->data);
        }

        if (beoremote_one_adapter->adapter)
            btd_adapter_unref(beoremote_one_adapter->adapter);

        g_free(beoremote_one_adapter);
        beoremote_one_adapter = NULL;
    }
}

static int beoremote_one_adapter_probe(struct btd_adapter *adapter)
{
    if (beoremote_one_adapter == NULL) {
        beoremote_one_adapter = g_new0(beoremote_one_adapter_t, 1);
        beoremote_one_adapter->adapter = btd_adapter_ref(adapter);

        if (!beoremote_one_service_register(beoremote_one_adapter)) {
            beoremote_one_adapter_remove(beoremote_one_adapter->adapter);
            error("Failed to register BeoRemote One service");
            return -EIO;
        }
    }

    DBG("BeoRemote One service registered");

    return 0;
}

static struct btd_adapter_driver beoremote_one_adapter_driver = {
    .name = "beoremote_one_adapter_driver",
    .probe = beoremote_one_adapter_probe,
    .remove = beoremote_one_adapter_remove,
};

static int do_beoremote_one_init(void)
{
    int i;

    for (i = 0; i < BEOREMOTE_ONE_CONNECTED_MAX; i++) {
        bacpy(&connected_device_address[i], BDADDR_ANY);
    }

    beoremote_one_init();
    return btd_register_adapter_driver(&beoremote_one_adapter_driver);
}

static void do_beoremote_one_exit(void)
{
    beoremote_one_exit();
    btd_unregister_adapter_driver(&beoremote_one_adapter_driver);
}

BLUETOOTH_PLUGIN_DEFINE(beoremote_one, VERSION, BLUETOOTH_PLUGIN_PRIORITY_DEFAULT, do_beoremote_one_init, do_beoremote_one_exit)
