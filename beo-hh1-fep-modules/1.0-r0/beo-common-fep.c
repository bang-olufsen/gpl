/*
 * Copyright (C) 2013 StreamUnlimited
 *
 * License Terms: GNU General Public License v2
 * Author: Vladimir Koutny <vladimir.koutny@streamunlimited.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/spi/spi.h>

#include "beo-hh1-fep.h"

#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>

// public API for cells:

typedef int (*BEO_HH1_EVENT_CB)(void *priv, u16 event, u16 data);
typedef int (*BEO_HH1_LONGEVENT_CB)(void *priv, u16 event, u32 size, u8 *data);

// forward declaration
struct platform_device;
struct device;

struct event_range {
	int first;
	int last;
	BEO_HH1_EVENT_CB cb;
	BEO_HH1_LONGEVENT_CB longcb;
	const struct platform_device *pdev;
	void *priv;
};

#define IRQ_QUERY_SIZE		(8 * 4)
#define EVENT_RANGES		32
#define DATA_FETCH_MAX_CHUNK	256
#define DATA_FETCH_MAX_SIZE		128*1024

struct beo_common_fep {
	struct device	*dev;

	struct beo_hh1_fep_core *core;

	// irq gpio
	int irq_gpio;
	int irq_gpio_active_low;

	// translated irq number
	int irq;
	struct work_struct work;

	struct event_range events[EVENT_RANGES];
	struct mutex events_lock;

	// data_fetch handling
	u16 data_fetch_id;
	u8 *data_fetch_buf;
	u16 data_fetch_chunksize;
	u32 data_fetch_size;
	u32 data_fetch_done;
};

static const unsigned short gs_XMODEM_CRC_tab[256] =
{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

// this is the XMODEM_CRC_computeCRC16 fep is using
static u16 beo_fep_api_crc16(const u8 *data, u16 size)
{
	u16 crc = 0;
	u16 us;
	for (us = 0; us < size; us++)
		crc = (crc<<8) ^ gs_XMODEM_CRC_tab[ ( (crc>>8) ^ *data++ ) & 0x00FF ];
	return crc;
}

static void deliver_short_event(struct beo_common_fep *fep, u16 evid, u16 evdata)
{
	int delivered = 0;
	int j;

	for (j = 0; j < EVENT_RANGES; j++) {
		if (fep->events[j].cb) {
			if (evid >= fep->events[j].first && evid <= fep->events[j].last) {
				fep->events[j].cb(fep->events[j].priv, evid, evdata);
				delivered = 1;
				break;
			}
		}
	}

	if (!delivered) {
		dev_err(fep->dev, "Undelivered event: %04x,%04x\n", evid, evdata);
	}
}

static void deliver_long_event(struct beo_common_fep *fep, u16 evid, u32 evlen, u8 *data)
{
	int delivered = 0;
	int j;

	for (j = 0; j < EVENT_RANGES; j++) {
		if (fep->events[j].longcb) {
			if (evid >= fep->events[j].first && evid <= fep->events[j].last) {
				fep->events[j].longcb(fep->events[j].priv, evid, evlen, data);
				delivered = 1;
				break;
			}
		}
	}

	if (!delivered) {
		dev_err(fep->dev, "Undelivered long event: %04x,%d bytes\n", evid, evlen);
	}
}

static int beo_fep_api_register_events(const struct platform_device *pdev, u16 events_base, u16 events_size, void *priv, BEO_HH1_EVENT_CB cb)
{
	struct beo_common_fep *fep = container_of(pdev->mfd_cell->platform_data, struct beo_common_fep, core);
	int index = -1;
	int j;
	u16 events_last = events_base + events_size - 1;

	dev_info(fep->dev, "Registering events %04x..%04x.. for module %s\n", events_base, events_last, pdev->name);

	if (cb == NULL) {
		return 0;
	}

	mutex_lock(&fep->events_lock);

	for (j = 0; j < EVENT_RANGES; j++) {
		if (!fep->events[j].cb && !fep->events[j].longcb) {
			index = j;
			break;
		}
	}

	if (index != -1) {
		fep->events[index].first = events_base;
		fep->events[index].last = events_last;
		fep->events[index].cb = cb;
		fep->events[index].longcb = NULL;
		fep->events[index].priv = priv;
		fep->events[index].pdev = pdev;
	}

	mutex_unlock(&fep->events_lock);

	return 0;
}

static int beo_fep_api_register_long_events(const struct platform_device *pdev, u16 events_base, u16 events_size, void *priv, BEO_HH1_LONGEVENT_CB cb)
{
	struct beo_common_fep *fep = container_of(pdev->mfd_cell->platform_data, struct beo_common_fep, core);
	int index = -1;
	int j;
	u16 events_last = events_base + events_size - 1;

	dev_info(fep->dev, "Registering long events %04x..%04x.. for module %s\n", events_base, events_last, pdev->name);

	if (cb == NULL) {
		return 0;
	}

	mutex_lock(&fep->events_lock);

	for (j = 0; j < EVENT_RANGES; j++) {
		if (!fep->events[j].cb && !fep->events[j].longcb) {
			index = j;
			break;
		}
	}

	if (index != -1) {
		fep->events[index].first = events_base;
		fep->events[index].last = events_last;
		fep->events[index].cb = NULL;
		fep->events[index].longcb = cb;
		fep->events[index].priv = priv;
		fep->events[index].pdev = pdev;
	}

	mutex_unlock(&fep->events_lock);

	return 0;
}

static int beo_fep_mfd_enable_callback(struct platform_device *pdev)
{
	return 0;
}

static int beo_fep_mfd_disable_callback(struct platform_device *pdev)
{
	struct beo_common_fep *fep = container_of(pdev->mfd_cell->platform_data, struct beo_common_fep, core);

	unsigned int j;

	mutex_lock(&fep->events_lock);

	for (j = 0; j < EVENT_RANGES; j++) {
		if (fep->events[j].cb != NULL || fep->events[j].longcb != NULL) {
			if (strcmp(pdev->name, fep->events[j].pdev->name) == 0) { // ?comparison of pointers?
				dev_info(fep->dev, "Deregistering events %04x..%04x.. for module %s\n", fep->events[j].first, fep->events[j].last, fep->events[j].pdev->name);
				memset(&fep->events[j], 0, sizeof(fep->events[0]));
			}
		}
	}

	mutex_unlock(&fep->events_lock);

	return 0;
}

static void data_fetch_clear(struct beo_common_fep *fep, bool warn)
{
	if (fep->data_fetch_buf != NULL) {
		if (warn) {
			dev_warn(fep->dev, "data_fetch received incomplete transfer - discard (id %04x, done %d of %d)\n",
				fep->data_fetch_id, fep->data_fetch_done, fep->data_fetch_size);
		}
		kfree(fep->data_fetch_buf);
		fep->data_fetch_buf = NULL;
	}

	fep->data_fetch_id = 0;
	fep->data_fetch_size = 0;
	fep->data_fetch_chunksize = 0;
	fep->data_fetch_done = 0;

}

static void handle_data_fetch_event(struct beo_common_fep *fep, struct beo_hh1_ev_data_fetch data_fetch)
{
	struct beo_hh1_msg_data_fetch msg;
	struct beo_hh1_rsp_data_fetch ack;
	u8 outbuf[sizeof(struct beo_hh1_rsp_data_fetch) + DATA_FETCH_MAX_CHUNK];
	int outlen;

#ifdef DEBUG_VERBOSE
	dev_info(fep->dev, "data_fetch request received: ev %04x, chunk %d, size %d\n", data_fetch.id, data_fetch.chunksize, data_fetch.size);
#endif

	// safety checks
	if (data_fetch.chunksize > DATA_FETCH_MAX_CHUNK) {
		dev_err(fep->dev, "data_fetch error: requested chunksize %d too big! (max %d)\n", data_fetch.chunksize, DATA_FETCH_MAX_CHUNK);
		return;
	}
	if (data_fetch.size > DATA_FETCH_MAX_SIZE) {
		dev_err(fep->dev, "data_fetch error: requested size %d too big! (max %d)\n", data_fetch.size, DATA_FETCH_MAX_SIZE);
		return;
	}

	// prepare output buffer if needed
	if (data_fetch.id != fep->data_fetch_id) {
#ifdef DEBUG_VERBOSE
		dev_info(fep->dev, "preparing data_fetch buffer size: %d\n", data_fetch.size);
#endif

		data_fetch_clear(fep, true);
		fep->data_fetch_buf = kzalloc(data_fetch.size, GFP_KERNEL);

		if (fep->data_fetch_buf == NULL) {
			dev_err(fep->dev, "Unable to allocate data_fetch buffer\n");
			return;
		}

		fep->data_fetch_id = data_fetch.id;
		fep->data_fetch_size = data_fetch.size;
		fep->data_fetch_chunksize = data_fetch.chunksize;
	}

	outlen = sizeof(struct beo_hh1_rsp_data_fetch) + data_fetch.chunksize;
	memset(outbuf, 0, outlen);

	msg.hdr.command = cpu_to_le16(BEO_HH1_CMD_DATA_FETCH);
	msg.hdr.len = cpu_to_le16(sizeof(msg));
	msg.outlen.outlen = cpu_to_le16(outlen);
	msg.outlen.pad = 0;
	msg.id = cpu_to_le16(data_fetch.id);
	msg.pad = 0;

	fep->core->send(fep->core, sizeof(msg), (u8 *)&msg, outlen, outbuf);

/*
// HACKs:
static u32 ooo = 0;
u32 ooo2 = cpu_to_le32(ooo);
memcpy(outbuf, "\x01\x80\x2c\x00"  "\x10\x80\x00\x00" "\x00\x00\x00\x00" "0123456789ABCDEFGHIJKLMNOPQRSTUV", 4 + 8 + 32);
memcpy(outbuf+8, &ooo2, 4);
ooo += data_fetch.chunksize;
*/

	memcpy(&ack, outbuf, outlen);
	if (le16_to_cpu(ack.hdr.command) == BEO_HH1_REP_ACK) {
		int chunklen;
		int len = le16_to_cpu(ack.hdr.len);
		if (len > outlen || len < sizeof(struct beo_hh1_rsp_data_fetch)) {
			dev_err(fep->dev, "data_fetch invalid length of response: %d\n", len);
			data_fetch_clear(fep, false);
			return;
		}

		ack.id = le16_to_cpu(ack.id);
		ack.offset = le32_to_cpu(ack.offset);
#ifdef DEBUG_VERBOSE
		dev_info(fep->dev, "data_fetch received: id %04x, offset %d\n", ack.id, ack.offset);
#endif

		if (ack.id != fep->data_fetch_id) {
			dev_err(fep->dev, "data_fetch invalid chunk id: %04x, expected %04x\n", ack.id, fep->data_fetch_id);
			data_fetch_clear(fep, false);
			return;
		}

		chunklen = fep->data_fetch_chunksize;
		if (fep->data_fetch_size - fep->data_fetch_done < chunklen)
			chunklen = fep->data_fetch_size - fep->data_fetch_done;

		if (len - sizeof(struct beo_hh1_rsp_data_fetch) < chunklen) {
			dev_err(fep->dev, "data_fetch chunk len too short: %d\n", len - sizeof(struct beo_hh1_rsp_data_fetch));
			data_fetch_clear(fep, false);
			return;
		}

		if (ack.offset != fep->data_fetch_done) {
			dev_err(fep->dev, "data_fetch invalid chunk offset: %d, expected %d\n", ack.offset, fep->data_fetch_done);
			data_fetch_clear(fep, false);
			return;
		}

		memcpy(fep->data_fetch_buf + fep->data_fetch_done, ack.data, chunklen);
		fep->data_fetch_done += chunklen;

		if (fep->data_fetch_done == fep->data_fetch_size) {
#ifdef DEBUG_VERBOSE
			dev_info(fep->dev, "data_fetch transfer of %04x done, %d bytes\n", fep->data_fetch_id, fep->data_fetch_done);
#endif

			deliver_long_event(fep, fep->data_fetch_id, fep->data_fetch_done, fep->data_fetch_buf);
			data_fetch_clear(fep, false);
		}
	}
	else {
		dev_err(fep->dev, "data_fetch without ack!\n");
	}

}

static void beo_fep_work_func(struct work_struct *work)
{
	struct beo_common_fep *fep = container_of(work, struct beo_common_fep, work);
	struct beo_hh1_msg_irq_query msg;
	struct beo_hh1_rsp_ack ack;
	u8 outbuf[sizeof(ack) + IRQ_QUERY_SIZE];
	int outlen = sizeof(outbuf);

#ifdef DEBUG_VERBOSE
	dev_dbg(fep->dev, "beo_ase_fep_work_func()\n");
#endif

	memset(outbuf, 0, outlen);

	msg.hdr.command = cpu_to_le16(BEO_HH1_CMD_IRQ_QUERY);
	msg.hdr.len = cpu_to_le16(sizeof(msg));
	msg.outlen.outlen = cpu_to_le16(IRQ_QUERY_SIZE + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	fep->core->send(fep->core, sizeof(msg), (u8 *)&msg, outlen, outbuf);

	memcpy(&ack, outbuf, sizeof(ack));
	if (le16_to_cpu(ack.command) == BEO_HH1_REP_ACK) {
		int len = le16_to_cpu(ack.len);
		int i;
		struct beo_hh1_event ev;

		if (len > outlen) {
			dev_err(fep->dev, "Invalid length of irq_query response: %d\n", len);
			goto done;
		}

		memcpy(&ev, outbuf + sizeof(ack), sizeof(ev));
		ev.id = le16_to_cpu(ev.id);

		if (ev.id & BEO_HH1_EV_LONG_MASK) {
			int evlen = len - sizeof(ack) - sizeof(ev.id);
			// dev_info(fep->dev, "Long event received: %04x, %d bytes\n", ev.id, evlen);

			// data_fetch long event is handled internally
			if (ev.id == BEO_HH1_EV_DATA_FETCH) {
				struct beo_hh1_ev_data_fetch data_fetch;
				if (evlen >= sizeof(data_fetch)) {
					memcpy(&data_fetch, outbuf + sizeof(ack) + sizeof(ev.id), evlen);
					data_fetch.id = le16_to_cpu(data_fetch.id);
					data_fetch.chunksize = le16_to_cpu(data_fetch.chunksize);
					data_fetch.size = le32_to_cpu(data_fetch.size);

					handle_data_fetch_event(fep, data_fetch);
				}
				else {
					dev_err(fep->dev, "Invalid data_fetch event size: %d\n", evlen);
				}
			}
			else {
				deliver_long_event(fep, ev.id, evlen, outbuf + sizeof(ack) + sizeof(ev.id));
			}
		}
		else {
			// process short events
			for (i = 0; i < (len - sizeof(ack)) / sizeof(ev); i++) {
				memcpy(&ev, outbuf + sizeof(ack) + sizeof(ev)*i, sizeof(ev));
				ev.id = le16_to_cpu(ev.id);
				ev.value = le16_to_cpu(ev.value);

				// no event - stop parsing
				if (ev.id == 0)
					break;

				deliver_short_event(fep, ev.id, ev.value);
			}
		}
	}
	else {
		int i;
		u8 dump[3*32 + 1] = {0};
		dev_err(fep->dev, "irq_query without ack:");
		for (i = 0; i < (outlen <= 32 ? outlen : 32); i++)
			sprintf(&dump[3*i], "%02x ", outbuf[i]);
		dev_err(fep->dev, "received %d bytes: %s\n", outlen, dump);
	}

done:
	enable_irq(fep->irq);
}
