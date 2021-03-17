#include <stdint.h>

#define REQUEST_SIZE 20

#pragma pack(push)
#pragma pack(1)
struct ImageHeader {
	uint8_t status;
	uint8_t image_type;
	uint8_t page_state;
	uint8_t page_num;
	uint8_t major;
	uint8_t minor;
	uint16_t release;
	uint32_t size;
	uint32_t crc;
	uint32_t start_address;
};

struct ImageUpdateAnnouncement {
	uint8_t type_length;
	uint8_t image_type;
	uint32_t size;
	uint32_t crc;
	uint8_t major;
	uint8_t minor;
	uint16_t release;
	uint8_t min_major;
	uint8_t min_minor;
	uint8_t max_major;
	uint8_t max_minor;
	uint8_t filler[2];
};

struct Data {
	uint8_t type_length;
	uint16_t offset;
	uint8_t data[REQUEST_SIZE - 3];
};

struct DataRequest {
	uint8_t type_length;
	uint8_t image_type;
	uint32_t offset;
	uint16_t length;
	uint8_t filler[REQUEST_SIZE - 8];
};

struct Request {
	uint8_t type_length;
	uint8_t request_type;
	uint8_t option;
	uint8_t data[REQUEST_SIZE - 3];
};

struct Response {
	uint8_t type_length;
	uint8_t response_type;
	uint8_t data[REQUEST_SIZE - 2];
};

struct ImageInfoResponse {
	uint8_t type_length;
	uint8_t response_type;
	uint8_t image_type;
	uint8_t status;
	uint32_t size;
	uint32_t crc;
	uint8_t major;
	uint8_t minor;
	uint16_t release;
	uint8_t filler[4];
};

struct UpdateCompleteResponse {
	uint8_t type_length;
	uint8_t status;
	uint8_t filler[REQUEST_SIZE - 2];
};

struct DeviceInfoResponse {
	uint8_t type_length;
	uint8_t response_type;
	uint16_t device_type;
	uint8_t serial_number[8];
	uint8_t bt_address[6];
	uint8_t filler[2];
};

struct KeyCountersResponse {
	uint8_t type_length;
	uint8_t response_type;
	uint8_t key;
	uint32_t key_counter;
	uint32_t key_counter_1;
	uint32_t key_counter_2;
	uint32_t key_counter_3;
	uint8_t filler;
};

struct ResetKeyCountersResponse {
	uint8_t type_length;
	uint8_t response_type;
	uint8_t status;
	uint8_t filler[REQUEST_SIZE - 3];
};

struct Report {
	union {
		struct Request request;
		struct Response response;
		struct ImageInfoResponse image_info_response;
		struct ImageUpdateAnnouncement image_update_announcement;
		struct DataRequest data_request;
		struct Data data;
		struct UpdateCompleteResponse update_complete_response;
		struct DeviceInfoResponse device_info_response;
		struct KeyCountersResponse key_counters_response;
		struct ResetKeyCountersResponse reset_key_counters_response;
	} message;
};
#pragma pack(pop)

enum ImageType {
	IMAGE_BOOTLOADER = 0,
	IMAGE_APPLICATION = 1,
	IMAGE_FONT = 2,
	IMAGE_USER_DB = 3,
	IMAGE_STATIC_DB = 4,
	IMAGE_MAX = 5,
};

enum UpdateStatus {
	STATUS_SUCCESS = 0,
	STATUS_INCOMPATIBLE = 1,
	STATUS_BAD_SIZE = 2,
	STATUS_BAD_CRC = 3,
	STATUS_CANCELLED = 4,
	STATUS_TIMEOUT = 5,
	STATUS_MAX = 6,
};

enum TypeLength {
	TYPE_LENGTH_REQUEST = 0x21,
	TYPE_LENGTH_REPORT_SENT = 0x10,
	TYPE_LENGTH_IMAGE_INFO = 0x2F,
	TYPE_LENGTH_DEVICE_INFO = 0x31,
	TYPE_LENGTH_KEY_COUNTERS = 0x32,
	TYPE_LENGTH_REQUEST_WITH_OPTION = 0x22,
	TYPE_LENGTH_IMAGE_UPDATE = 0x51,
	TYPE_LENGTH_DATA_REQUEST = 0x67,
	TYPE_LENGTH_DATA = 0x82,
	TYPE_LENGTH_UPDATE_COMPLETE = 0xA1,
};

enum RequestType {
	REQUEST_IMAGE_INFO = 1,
	REQUEST_DEVICE_INFO = 2,
	REQUEST_KEY_COUNTERS = 3,
	REQUEST_RESET_KEY_COUNTERS = 4,
	REQUEST_REBOOT = 5,
};

enum CommandType {
	COMMAND_BOOTLOADER_VERSION = 0,
	COMMAND_APPLICATION_VERSION = 1,
	COMMAND_FONT_VERSION = 2,
	COMMAND_USER_DB_VERSION = 3,
	COMMAND_STATIC_DB_VERSION = 4,
	COMMAND_UPDATE_IMAGE = 5,
	COMMAND_DEVICE_INFO = 6,
	COMMAND_KEY_COUNTERS = 7,
	COMMAND_RESET_KEY_COUNTERS = 8,
	COMMAND_REBOOT = 9,
	COMMAND_HELP = 10,
};
