#ifndef BEOREMOTE_ONE_H
#define BEOREMOTE_ONE_H

#include <glib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#ifdef BUILDTYPE_PC
#include <boost/thread.hpp>
#endif

#include "beoremote_one_types.h"

#ifndef BUILDTYPE_PC
#define BEOREMOTE_ONE_PLUGIN_VERBOSE
#endif

typedef struct {
    uint16_t handle;
    uint8_t value[BEOREMOTE_ONE_ATTRIB_MAX_SIZE];
} beoremote_one_data_t;

extern int beoremote_one_socket;
extern GMutex beoremote_one_mutex;
extern volatile gboolean beoremote_one_thread_stop;
extern GThread *beoremote_one_thread;
#ifdef BUILDTYPE_PC // Only used for testing,
extern volatile gboolean socket_created_test;
extern GCond beoremote_one_attrib_updated_condition;
// Will be set to TRUE when the socket server has accepted
// an incoming connection.
extern volatile gboolean beoremote_one_thread_accepted_connection;
extern char beoremote_one_socket_path[256];
#endif
extern beoremote_one_data_t beoremote_one_data[];

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
extern int lock_count;
#endif

#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
#define ATTRIB_DATA_AS_STRING_BUFFER_SIZE 1024
#endif

int attrib_changed(enum beoremote_one_attrib, size_t);

#define BA_ADDRESS_STRING_BUFFER_SIZE 18 // Buffer size of MAC address string 11:22:33:44:55:66
static int device_address_or_local_to_str(struct btd_device *, char *);

void update_connected_device_address(struct btd_device *);

static int beoremote_one_socket_create(char *path)
{
    int sock, ret;
    struct sockaddr_un addr;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        int err = errno;
        error("%s: Socket error(%d): \"%s\"\n", __func__, err, strerror(err));
        return sock;
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (unlink(path) < 0) {
        int err = errno;
        if (err != ENOENT) {
            error("%s: Unlink error(%d): \"%s\"\n", __func__, err, strerror(err));
        }
    }

    if ((ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        int err = errno;
        error("%s: Bind error(%d): \"%s\"\n", __func__, err, strerror(err));
        return ret;
    }

    if ((ret = listen(sock, 1)) < 0) {
        int err = errno;
        error("%s: Listen error(%d): \"%s\"\n", __func__, err, strerror(err));
        return ret;
    }

    return sock;
}

static int beoremote_one_socket_write(const uint8_t *data, uint16_t size)
{
    int bytes = -1;

    if (beoremote_one_socket > 0) {
        const uint8_t *data_end = data + size;
        while (data < data_end) {
            bytes = write(beoremote_one_socket, data, (data_end - data));
            if (bytes < 0) {
                int err = errno;
                error("%s: Write error(%d): \"%s\"\n", __func__, err, strerror(err));
                return bytes;
            }
            data += bytes;
        }
    }

    return bytes;
}

static void aqquire_lock()
{
#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    lock_count++;
    DBG("-> %p %d", g_thread_self(), lock_count);
#endif
    g_mutex_lock(&beoremote_one_mutex);
}

static void release_lock()
{
    g_mutex_unlock(&beoremote_one_mutex);
#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    lock_count--;
    DBG("<- %p %d", g_thread_self(), lock_count);
#endif
}

static int beoremote_one_socket_recv(int socket, void *buffer, size_t size)
{
    int bytes = recv(socket, buffer, size, 0);
    if (bytes == 0) {
#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
        DBG("Socket(=%d) disconnected", socket);
#endif
        return -1;
    } else if (bytes < 0) {
        int err = errno;
        switch (err) {
        case EWOULDBLOCK:
            return 0;
        default:
            error("%s: Failed to read message from socket(=%d), error [%s(%d)]\n", __func__, socket, strerror(err), err);
            return -1;
        }
    }
    return bytes;
}

static int beoremote_one_socket_read(int sock)
{
    int bytes = -1;
    gboolean valid_msg_type;
    static uint8_t msg_type, data[BEOREMOTE_ONE_ATTRIB_MAX_SIZE + BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE];
    static uint16_t msg_bytes = 0, data_bytes = 0, remaining_msg_bytes = 0;

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("-> %p", g_thread_self());
#endif

    while (data_bytes < BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE) {
        bytes = beoremote_one_socket_recv(sock, &data[data_bytes], BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE - data_bytes);
        if (bytes < 0 || g_atomic_int_get(&beoremote_one_thread_stop)) {
            msg_bytes = data_bytes = 0;
            return -1;
        }
        data_bytes += bytes;
    }

    msg_type = data[0];
    msg_bytes = ntohs(data[1] | (data[2] << 8));
    valid_msg_type = msg_type > 0 && msg_type < BEOREMOTE_ONE_ATTRIB_MAX ? TRUE : FALSE;

    if (valid_msg_type) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
        DBG("Received new value for attribute (0x%04x)%s with length %d", beoremote_one_data[msg_type].handle, beoremote_one_attrib_to_string((enum beoremote_one_attrib)msg_type), msg_bytes);
#endif
    } else {
        warn("Invalid msg_type %d, msg_bytes %d", msg_type, msg_bytes);
    }
    if (msg_bytes > BEOREMOTE_ONE_ATTRIB_MAX_SIZE) {
        msg_bytes = BEOREMOTE_ONE_ATTRIB_MAX_SIZE;
    }

    remaining_msg_bytes = msg_bytes;
    while (data_bytes != (BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE + msg_bytes)) {
        bytes = beoremote_one_socket_recv(sock, &data[data_bytes], remaining_msg_bytes);
        if (bytes < 0 || g_atomic_int_get(&beoremote_one_thread_stop)) {
            msg_bytes = data_bytes = 0;
            return -1;
        }
        data_bytes += bytes;
        remaining_msg_bytes -= bytes;
    }

    if (valid_msg_type) {
        aqquire_lock();
        memcpy(beoremote_one_data[msg_type].value, &data[BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE], msg_bytes);
        if (beoremote_one_attrib_size((enum beoremote_one_attrib)msg_type) == BEOREMOTE_ONE_ATTRIB_MAX_SIZE)
            beoremote_one_data[msg_type].value[msg_bytes < BEOREMOTE_ONE_ATTRIB_MAX_SIZE ? msg_bytes : BEOREMOTE_ONE_ATTRIB_MAX_SIZE - 1] = '\0';

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
        char string_buffer[msg_bytes + 1];
        DBG("New content of attribute (0x%04x)%s is [%s]",
            beoremote_one_data[msg_type].handle,
            beoremote_one_attrib_to_string((enum beoremote_one_attrib)msg_type),
            beoremote_one_attrib_data_to_string(
                (enum beoremote_one_attrib)msg_type,
                beoremote_one_data[msg_type].value,
                msg_bytes,
                &string_buffer,
                sizeof(string_buffer)));
#endif
        release_lock();
        attrib_changed((enum beoremote_one_attrib)msg_type, msg_bytes);

#ifdef BUILDTYPE_PC
        g_cond_signal(&beoremote_one_attrib_updated_condition);
#endif
    }

    msg_bytes = data_bytes = 0;

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("<- %p (bytes=%d)", g_thread_self(), bytes);
#endif
    return bytes;
}

static gpointer beoremote_one_socket_thread(gpointer data)
{
    (void)data;
    fd_set active_fd_set;
    fd_set read_fd_set;
    struct timeval timeout;
    int i;
#ifndef BUILDTYPE_PC
    int sock = beoremote_one_socket_create(BEOREMOTE_ONE_SOCKET_PATH);
#else
    int sock = beoremote_one_socket_create(beoremote_one_socket_path);
    g_atomic_int_set(&socket_created_test, TRUE);
#endif

    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);

    while (!g_atomic_int_get(&beoremote_one_thread_stop)) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 100 * 1000;
        read_fd_set = active_fd_set;

        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout) < 0) {
            int err = errno;
            error("%s: select error %s(%d)\n", __func__, strerror(err), err);
            break;
        }

        for (i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == sock) {
                    int new_sock = accept(i, NULL, NULL);
                    if (new_sock > 0) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                        DBG("Accepting new socket %d, %d", sock, new_sock);
#endif
                        fcntl(new_sock, F_SETFL, O_NONBLOCK);
                        FD_SET(new_sock, &active_fd_set);
                        beoremote_one_socket = new_sock;
#ifdef BUILDTYPE_PC
                        g_atomic_int_set(&beoremote_one_thread_accepted_connection, TRUE);
#endif
                    }
                } else if (beoremote_one_socket_read(i) < 0) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
                    DBG("Closing socket: %d", i);
#endif
                    close(i);
                    FD_CLR(i, &active_fd_set);
#ifdef BUILDTYPE_PC
                    g_atomic_int_set(&beoremote_one_thread_accepted_connection, FALSE);
#endif
                }
            }
        }
    }

    return NULL;
}

static uint8_t beoremote_one_attrib_read(struct attribute *attrib, struct btd_device *device, gpointer user_data)
{
    struct btd_adapter *adapter = (struct btd_adapter *)user_data;
    int attrib_type;
    size_t attrib_size;
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
    char device_address_str[BA_ADDRESS_STRING_BUFFER_SIZE] = { 0 };
    char print_content_buffer[ATTRIB_DATA_AS_STRING_BUFFER_SIZE];
#endif

    if (!attrib) {
        return 0;
    }

    for (attrib_type = 0; attrib_type < BEOREMOTE_ONE_ATTRIB_MAX; attrib_type++) {
        if (attrib->handle == beoremote_one_data[attrib_type].handle) {
            aqquire_lock();
            attrib_size = beoremote_one_attrib_size((enum beoremote_one_attrib)attrib_type);
            /* For strings only send the length of the string and not the real size */
            if (attrib_size == BEOREMOTE_ONE_ATTRIB_MAX_SIZE) {
                attrib_size = strlen((char *)beoremote_one_data[attrib_type].value);
            }
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
            device_address_or_local_to_str(device, &device_address_str[0]);
            DBG("Read attribute from device %s (0x%04x)%s:[%s]",
                device_address_str,
                attrib->handle,
                beoremote_one_attrib_to_string((enum beoremote_one_attrib)attrib_type),
                beoremote_one_attrib_data_to_string(
                    (enum beoremote_one_attrib)attrib_type,
                    beoremote_one_data[attrib_type].value,
                    attrib_size,
                    &print_content_buffer,
                    sizeof(print_content_buffer)));
#endif
            attrib_db_update(adapter, attrib->handle, NULL, beoremote_one_data[attrib_type].value, attrib_size, NULL);
            release_lock();
            break;
        }
    }

    update_connected_device_address(device);

    return 0;
}

static uint8_t beoremote_one_attrib_write(struct attribute *attrib, struct btd_device *device, gpointer user_data)
{
    (void)device;
    (void)user_data;

    int attrib_type, attrib_size;
    uint8_t data[BEOREMOTE_ONE_ATTRIB_MAX_SIZE + BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE];
    size_t max_len;

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("-> %p", g_thread_self());
#endif

    if (!attrib) {
        return 0;
    }

    for (attrib_type = 0; attrib_type < BEOREMOTE_ONE_ATTRIB_MAX; attrib_type++) {
        if (attrib->handle == beoremote_one_data[attrib_type].handle) {
#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
            DBG("Write for attribute (0x%04x)%s with length %d", attrib->handle, beoremote_one_attrib_to_string((enum beoremote_one_attrib)attrib_type), (int)attrib->len);
#endif
            attrib_size = attrib->len;
            max_len = beoremote_one_attrib_size((enum beoremote_one_attrib)attrib_type);
            if ((size_t)attrib_size > max_len)
                attrib_size = max_len;

            aqquire_lock();
            memcpy(beoremote_one_data[attrib_type].value, attrib->data, attrib_size);
            if (attrib_size == BEOREMOTE_ONE_ATTRIB_MAX_SIZE) {
                beoremote_one_data[attrib_type].value[attrib_size - 1] = '\0';
            }
            release_lock();

            data[0] = attrib_type;
            data[1] = (htons(attrib_size) >> 0) & 0xFF;
            data[2] = (htons(attrib_size) >> 8) & 0xFF;

            memcpy(&data[BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE], attrib->data, attrib_size);
            beoremote_one_socket_write(data, attrib_size + BEOREMOTE_ONE_SOCKET_MSG_HEADER_SIZE);

#ifdef BEOREMOTE_ONE_PLUGIN_VERBOSE
            DBG("Sent new value for attribute (0x%04x)%s with length %d", attrib->handle, beoremote_one_attrib_to_string((enum beoremote_one_attrib)attrib_type), attrib_size);
#endif
            break;
        }
    }

#ifdef BEOREMOTE_ONE_PLUGIN_TRACE
    DBG("<- %p", g_thread_self());
#endif
    return 0;
}

static void beoremote_one_init(void)
{
    g_mutex_init(&beoremote_one_mutex);
#ifdef BUILDTYPE_PC
    g_cond_init(&beoremote_one_attrib_updated_condition);
    g_atomic_int_set(&socket_created_test, FALSE);
#endif
    beoremote_one_thread = g_thread_new(NULL, beoremote_one_socket_thread, NULL);
#ifdef BUILDTYPE_PC
    while (g_atomic_int_get(&socket_created_test) != TRUE) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(25));
    }
#endif
}

static void beoremote_one_exit(void)
{
    g_atomic_int_set(&beoremote_one_thread_stop, TRUE);
    g_thread_join(beoremote_one_thread);
    beoremote_one_thread = NULL;
#ifdef BUILDTYPE_PC
    g_cond_clear(&beoremote_one_attrib_updated_condition);
#endif
    g_mutex_clear(&beoremote_one_mutex);
}

#endif
