/*
 * dsp driver for BeO ASE FEP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"

#define DEVICE_NAME "beo-ase-dsp-ev"
#define SET_CMD 1
#define GET_CMD 0
#define FIFO_SIZE 2048

struct beo_ase_dsp_priv {
	struct platform_device *pdev;
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;

	struct cdev cdev;
	struct device *cdevice;

	int open_count;
	wait_queue_head_t read_wait_queue;

	struct kfifo data;
};

static dev_t dsp_base_dev;
struct class *dsp_class;

static void u32tou8(u32 value, u8 *hw_hb, u8 *hw_lb, u8 *lw_hb, u8 *lw_lb)
{
	u32 le = cpu_to_le32(value);
	*lw_lb = ((u8 *)&le)[0];
	*lw_hb = ((u8 *)&le)[1];
	*hw_lb = ((u8 *)&le)[2];
	*hw_hb = ((u8 *)&le)[3];
}

static struct beo_ase_dsp_priv *device_to_private(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	return platform_get_drvdata(pdev);
}

static int device_open(struct inode *inode, struct file *file)
{
	struct beo_ase_dsp_priv *priv = container_of(inode->i_cdev, struct beo_ase_dsp_priv, cdev);
	if (!priv)
		return -EINVAL;

	file->private_data = priv;

	if (priv->open_count == 0)
		priv->open_count ++;
	else {
		dev_err(priv->dev, "Device is already opened.\n");
		return -EINVAL;
	}

	return nonseekable_open(inode, file);
}

static int device_release(struct inode *inode, struct file *file)
{
	struct beo_ase_dsp_priv *priv = container_of(inode->i_cdev, struct beo_ase_dsp_priv, cdev);
	if (!priv)
		return -EINVAL;

	priv->open_count --;

	return 0;
}

static ssize_t device_read(struct file *file,
   char *buffer,    /* The buffer to fill with data */
   size_t length,   /* The length of the buffer     */
   loff_t *offset)  /* Our offset in the file       */
{
	struct beo_ase_dsp_priv *priv = file->private_data;
	int ret, size;

	//Note that with only one concurrent reader and one concurrent writer, you don't need extra locking to use these macro.
	ret = kfifo_to_user(&priv->data, buffer, length, &size);
	if (ret != 0) {
		return ret;
	}
	return size;
}

static unsigned int device_poll(struct file *file, poll_table *wait)
{
	struct beo_ase_dsp_priv *priv = file->private_data;
	unsigned int mask = 0;

	if (!priv)
		return -EINVAL;

	dev_dbg(priv->dev, "%s\n", __func__);

	poll_wait(file, &priv->read_wait_queue, wait);
	/* we receive ACK or data */
	if (!kfifo_is_empty(&priv->data))
		mask |= POLLIN | POLLRDNORM;

	dev_dbg(priv->dev, "poll return - %d \n", mask);

	return mask;
}

static struct beo_hh1_msg_hdr beo_ase_dsp_create_header(u16 command, u16 len)
{
	struct beo_hh1_msg_hdr hdr;
	hdr.command = cpu_to_le16(command);
	hdr.len = cpu_to_le16(len);
	return hdr;
}

static struct beo_hh1_msg_outlen beo_ase_dsp_create_outlen(void)
{
	struct beo_hh1_msg_outlen outlen;
	outlen.outlen = cpu_to_le16(sizeof(struct beo_ase_dsp_ack));
	outlen.pad = 0;
	return outlen;
}

static int beo_ase_dsp_event_short_cb(void *_priv, u16 event, u16 data)
{
	struct beo_ase_dsp_priv *priv = _priv;
	u32 ev = event;
	int size = sizeof(data);

	if (priv->open_count == 0)
		return 0;

	if (event == BEO_ASE_EV_HW_OVERHEAT) {
#ifdef DEBUG_VERBOSE
		dev_info(priv->dev, "HW overheat: %04x\n", data);
#endif
	}

	if (event == BEO_ASE_EV_DSP_ERRLEVEL) {
#ifdef DEBUG_VERBOSE
		dev_info(priv->dev, "DSP errorLevel: %04x\n", data);
#endif
	}

	kfifo_in(&priv->data, &ev, sizeof(u32));
	kfifo_in(&priv->data, &size, sizeof(u32));
	kfifo_in(&priv->data, &data, size);
	wake_up_interruptible(&priv->read_wait_queue);

	return 0;
}

static int beo_ase_dsp_event_cb(void *_priv, u16 _event, u32 size, u8 *data)
{
	struct beo_ase_dsp_priv *priv = _priv;
	u32 event = _event;

	if(priv->open_count == 0)
		return 0;

	//have to switch byte order
	if (size >= sizeof(struct beo_ase_dsp_cmd_response)) {
		struct beo_ase_dsp_cmd_response *response = (struct beo_ase_dsp_cmd_response *)data;
		response->originCmd = le16_to_cpu(response->originCmd);
	}
	if (event == BEO_ASE_EV_DSP) {
		struct beo_ase_dsp_response *response = (struct beo_ase_dsp_response *)data;
		response->error = le32_to_cpu(response->error);
	}
	if (event == BEO_ASE_EV_DSP_TUNNEL) {
		struct beo_ase_dsp_response_tunnel *response = (struct beo_ase_dsp_response_tunnel *)data;
		response->count = le16_to_cpu(response->count);
	}
	if (event == BEO_ASE_EV_DSP_ERRLOG) {
		struct beo_ase_dsp_rsp_errlog_get *err_get = (struct beo_ase_dsp_rsp_errlog_get *)data;
		err_get->number = le16_to_cpu(err_get->number);
	}
	if (event == BEO_ASE_EV_DSP_AUTOCALIBRATE) {
		struct beo_ase_dsp_rsp_autocalibrate *autocalibrate = (struct beo_ase_dsp_rsp_autocalibrate *)data;
		autocalibrate->result = le16_to_cpu(autocalibrate->result);
	}
	if (event == BEO_ASE_EV_DSP_NTC_VALUES) {
		int i = 0;
		struct beo_ase_dsp_response_ntc_values *response = (struct beo_ase_dsp_response_ntc_values *)data;
		for (i = 0; i < response->count; ++i) {
			response->data[i] = le32_to_cpu(response->data[i]);
		}
	}

	//Note that with only one concurrent reader and one concurrent writer, you don't need extra locking to use these macro.
	kfifo_in(&priv->data, &event, sizeof(u32));
	kfifo_in(&priv->data, &size, sizeof(u32));
	kfifo_in(&priv->data, data, size);
	wake_up_interruptible(&priv->read_wait_queue);

	return 0;
}

static int beo_ase_dsp_log(struct device *dev, struct beo_ase_dsp_ack *ack, const int len, const char* msg)
{
	if (le16_to_cpu(ack->command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "%s No ACK!\n", msg);
		return -ENODEV;
	}

	return len;
}

static ssize_t beo_ase_dsp_initialize(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	int init, volume, source, response_len;

	if (sscanf(buf, "%i;%i;%i", &init, &volume, &source) != 3) {
		dev_err(dev, "Incorrect init command: '%s'", buf);
		return -EINVAL;
	}

	if (init) {
		struct beo_ase_msg_init init_msg;
		init_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_INIT, sizeof(init_msg));
		init_msg.outlen = beo_ase_dsp_create_outlen();
		init_msg.volume = cpu_to_le16(volume);
		init_msg.source = cpu_to_le16(source);
		response_len = priv->fepcore->send(priv->fepcore, sizeof(init_msg), (u8 *)&init_msg, sizeof(ack), (u8 *)&ack);
	} else {
		struct beo_ase_msg_deinit deinit_msg;
		deinit_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_DEINIT, sizeof(deinit_msg));
		deinit_msg.outlen = beo_ase_dsp_create_outlen();
		response_len = priv->fepcore->send(priv->fepcore, sizeof(deinit_msg), (u8 *)&deinit_msg, sizeof(ack), (u8 *)&ack);
	}

	return beo_ase_dsp_log(dev, &ack, len, "initialize");
}

static ssize_t beo_ase_dsp_volume(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_volume volume_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;
	s16 volume = 0;

	if (sscanf(buf, "%hi", &volume) != 1) {
		dev_err(dev, "Incorrect volume value: '%s'", buf);
		return -EINVAL;
	}

	volume_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_VOLUME, sizeof(volume_msg));
	volume_msg.outlen = beo_ase_dsp_create_outlen();
	volume_msg.volume = cpu_to_le16(volume);

	response_len = priv->fepcore->send(priv->fepcore, sizeof(volume_msg), (u8 *)&volume_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "volume");
}

static ssize_t beo_ase_dsp_mute(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_mute mute_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	int mute = 0;
	if ((sscanf(buf, "%i", &mute) != 1) || (mute != 0 && mute != 1)) {
		dev_err(dev, "Incorrect mute value: '%s'", buf);
		return -EINVAL;
	}

	mute_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_MUTE, sizeof(mute_msg));
	mute_msg.outlen = beo_ase_dsp_create_outlen();
	mute_msg.mute = cpu_to_le16(mute);

	response_len = priv->fepcore->send(priv->fepcore, sizeof(mute_msg), (u8 *)&mute_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "mute");
}

static ssize_t beo_ase_dsp_source(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_source source_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	u16 source = 0;
	if (sscanf(buf, "%hu", &source) != 1) {
		dev_err(dev, "Incorrect source value: '%s'", buf);
		return -EINVAL;
	}

	source_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_SOURCE, sizeof(source_msg));
	source_msg.outlen = beo_ase_dsp_create_outlen();
	source_msg.source = cpu_to_le16(source);

	response_len = priv->fepcore->send(priv->fepcore, sizeof(source_msg), (u8 *)&source_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "source");
}

static ssize_t beo_ase_dsp_sound(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_notif_sound sound_msg;
	struct beo_ase_dsp_ack ack;
	char *token, *string_buffer, *string;
	int state = 0;
	unsigned int value;
	int response_len, ret = 1;

	sound_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_NOTIF_SOUND, sizeof(sound_msg));
	sound_msg.outlen = beo_ase_dsp_create_outlen();
	sound_msg.count = 0;

	string_buffer = (char *)kmalloc(len + 1, GFP_KERNEL);
	memcpy(string_buffer, buf, len);
	string_buffer[len] = 0;

	string = string_buffer;
	while (ret && (token = strsep(&string, ";")) != NULL && sound_msg.count < BEO_ASE_NOTIF_SOUND_MAX) {
		ret = (sscanf(token, "%u", &value) == 1);
		if (state == 0) {
			sound_msg.forceInit = value;
			state = 1;
		}
		else if (state == 1) {
			sound_msg.data[sound_msg.count].sound_id = value;
			state = 2;
		}
		else if (state == 2) {
			sound_msg.data[sound_msg.count].time_delay = value;
			state = 1;
			++sound_msg.count;
		}
	}

	if (sound_msg.count == 0 || !ret || state != 1) {
		dev_err(dev, "Incorrect notify sound value: '%s'", buf);
		goto out;
	}

	sound_msg.forceInit = cpu_to_le16(sound_msg.forceInit);
	sound_msg.count = cpu_to_le16(sound_msg.count);

	response_len = priv->fepcore->send(priv->fepcore, sizeof(sound_msg), (u8 *)&sound_msg, sizeof(ack), (u8 *)&ack);
	beo_ase_dsp_log(dev, &ack, response_len, "sound");

out:
	kfree(string_buffer);
	if (!ret) {
		return -EINVAL;
	}
	return len;
}

static ssize_t beo_ase_dsp_version(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_version version_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	version_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_VERSION, sizeof(version_msg));
	version_msg.outlen = beo_ase_dsp_create_outlen();

	response_len = priv->fepcore->send(priv->fepcore, sizeof(version_msg), (u8 *)&version_msg, sizeof(ack), (u8 *)&ack);
	return beo_ase_dsp_log(dev, &ack, len, "version");
}

static ssize_t beo_ase_dsp_layout(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	int response_len = 0;

	u32 type, layout_type_id;

	if (sscanf(buf, "%u;%u", &type, &layout_type_id) != 2) {
		dev_err(dev, "Incorrect layout message");
		return -EINVAL;
	}

	if (type == SET_CMD) {
		struct beo_ase_msg_layout_set layout_set_msg;
		layout_set_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_LAYOUT_SET, sizeof(layout_set_msg));
		layout_set_msg.outlen = beo_ase_dsp_create_outlen();
		layout_set_msg.layout_type_id = (u8)layout_type_id;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(layout_set_msg), (u8 *)&layout_set_msg, sizeof(ack), (u8 *)&ack);
	}
	else {
		struct beo_ase_msg_layout_get layout_get_msg;
		layout_get_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_LAYOUT_GET, sizeof(layout_get_msg));
		layout_get_msg.outlen = beo_ase_dsp_create_outlen();
		response_len = priv->fepcore->send(priv->fepcore, sizeof(layout_get_msg), (u8 *)&layout_get_msg, sizeof(ack), (u8 *)&ack);
	}

	return beo_ase_dsp_log(dev, &ack, len, "layout");
}

static ssize_t beo_ase_dsp_filter(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	int response_len = 0;

	u32 type, speaker_unit_type, gain;
	u8 hw_hb, hw_lb, lw_hb, lw_lb;

	if (sscanf(buf, "%u;%u;%u", &type, &speaker_unit_type, &gain) != 3) {
		dev_err(dev, "Incorrect filter message");
		return -EINVAL;
	}

	if (type == SET_CMD) {
		struct beo_ase_msg_filter_set filter_set_msg;
		u32tou8(gain, &hw_hb, &hw_lb, &lw_hb, &lw_lb);
		filter_set_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_FILTER_SET, sizeof(filter_set_msg));
		filter_set_msg.outlen = beo_ase_dsp_create_outlen();
		filter_set_msg.speaker_unit_type = (u8)speaker_unit_type;
		filter_set_msg.gain.hw_hb = (u8)hw_hb;
		filter_set_msg.gain.hw_lb = (u8)hw_lb;
		filter_set_msg.gain.lw_hb = (u8)lw_hb;
		filter_set_msg.gain.lw_lb = (u8)lw_lb;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(filter_set_msg), (u8 *)&filter_set_msg, sizeof(ack), (u8 *)&ack);
	}
	else {
		struct beo_ase_msg_filter_get filter_get_msg;
		filter_get_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_FILTER_GET, sizeof(filter_get_msg));
		filter_get_msg.outlen = beo_ase_dsp_create_outlen();
		filter_get_msg.speaker_unit_type = speaker_unit_type;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(filter_get_msg), (u8 *)&filter_get_msg, sizeof(ack), (u8 *)&ack);
	}

	return beo_ase_dsp_log(dev, &ack, len, "filter");
}

static ssize_t beo_ase_dsp_gain(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	int response_len = 0;

	u32 type, speaker_unit_type, gain;
	u8 hw_hb, hw_lb, lw_hb, lw_lb;

	if (sscanf(buf, "%u;%u;%u", &type, &speaker_unit_type, &gain) != 3) {
		dev_err(dev, "Incorrect gain message");
		return -EINVAL;
	}

	if (type == SET_CMD) {
		struct beo_ase_msg_gain_set gain_set_msg;
		u32tou8(gain, &hw_hb, &hw_lb, &lw_hb, &lw_lb);
		gain_set_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_GAIN_SET, sizeof(gain_set_msg));
		gain_set_msg.outlen = beo_ase_dsp_create_outlen();
		gain_set_msg.speaker_unit_type = (u8)speaker_unit_type;
		gain_set_msg.gain.hw_hb = (u8)hw_hb;
		gain_set_msg.gain.hw_lb = (u8)hw_lb;
		gain_set_msg.gain.lw_hb = (u8)lw_hb;
		gain_set_msg.gain.lw_lb = (u8)lw_lb;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(gain_set_msg), (u8 *)&gain_set_msg, sizeof(ack), (u8 *)&ack);
	}
	else {
		struct beo_ase_msg_gain_get gain_get_msg;
		gain_get_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_GAIN_GET, sizeof(gain_get_msg));
		gain_get_msg.outlen = beo_ase_dsp_create_outlen();
		gain_get_msg.speaker_unit_type = speaker_unit_type;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(gain_get_msg), (u8 *)&gain_get_msg, sizeof(ack), (u8 *)&ack);
	}

	return beo_ase_dsp_log(dev, &ack, len, "gain");
}


static ssize_t beo_ase_dsp_tunnel(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_tunnel_cmd *tunnel_msg = (struct beo_ase_msg_tunnel_cmd *)
					kmalloc(sizeof(struct beo_ase_msg_tunnel_cmd), GFP_KERNEL);
	struct beo_ase_dsp_ack ack;
	int data_len, response_len;

	int tunnel_data_size = sizeof(tunnel_msg->data);

	tunnel_msg->hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_TUNNEL, sizeof(*tunnel_msg));
	tunnel_msg->outlen = beo_ase_dsp_create_outlen();
	data_len = len > tunnel_data_size ? tunnel_data_size : len;

	memcpy(tunnel_msg->data, buf, data_len);
	tunnel_msg->count = cpu_to_le16(data_len);

	response_len = priv->fepcore->send(priv->fepcore, sizeof(*tunnel_msg), (u8 *)tunnel_msg, sizeof(ack), (u8 *)&ack);
	kfree(tunnel_msg);

	return beo_ase_dsp_log(dev, &ack, len, "tunnel");
}

static ssize_t beo_ase_dsp_calibrate(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_auto_calibrate_cmd tunnel_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	dev_dbg(dev, "Starting autocalibrate");

	tunnel_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_AUTO_CALIBRATE, sizeof(tunnel_msg));
	tunnel_msg.outlen = beo_ase_dsp_create_outlen();

	response_len = priv->fepcore->send(priv->fepcore, sizeof(tunnel_msg), (u8 *)&tunnel_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "calibrate");
}

static ssize_t beo_ase_dsp_reset_counters(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_reset_counters_cmd reset_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	reset_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_RESET_COUNTERS, sizeof(reset_msg));
	reset_msg.outlen = beo_ase_dsp_create_outlen();

	response_len = priv->fepcore->send(priv->fepcore, sizeof(reset_msg), (u8 *)&reset_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "reset_counters");
}

static ssize_t beo_ase_dsp_errlog(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	int response_len;
	u32 cmd, from, to;

	if (sscanf(buf, "%u;%u;%u", &cmd, &from, &to) != 3) {
		dev_err(dev, "Incorrect errlog message");
		return -EINVAL;
	}

	if (cmd == GET_CMD) {
		struct beo_ase_msg_errlog_get_cmd errlog_get;
		errlog_get.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_ERRLOG_GET, sizeof(errlog_get));
		errlog_get.outlen = beo_ase_dsp_create_outlen();
		errlog_get.range_from = cpu_to_le16((u16)from);
		errlog_get.range_to = cpu_to_le16((u16)to);
		response_len = priv->fepcore->send(priv->fepcore, sizeof(errlog_get), (u8 *)&errlog_get, sizeof(ack), (u8 *)&ack);
	}
	else if (cmd == SET_CMD) {
		struct beo_ase_msg_errlog_clr_cmd errlog_clr;
		errlog_clr.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_ERRLOG_CLR, sizeof(errlog_clr));
		errlog_clr.outlen = beo_ase_dsp_create_outlen();
		response_len = priv->fepcore->send(priv->fepcore, sizeof(errlog_clr), (u8 *)&errlog_clr, sizeof(ack), (u8 *)&ack);
	}
	else {
		dev_err(dev, "Incorrect errlog message");
		return -EINVAL;
	}

	return beo_ase_dsp_log(dev, &ack, len, "errlog");
}

static ssize_t beo_ase_dsp_ntc_values(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_ntc_values_cmd ntc_values_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	ntc_values_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_NTC_VALUES, sizeof(ntc_values_msg));
	ntc_values_msg.outlen = beo_ase_dsp_create_outlen();

	response_len = priv->fepcore->send(priv->fepcore, sizeof(ntc_values_msg), (u8 *)&ntc_values_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "ntc_values");
}

static ssize_t beo_ase_dsp_testsignal(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_dsp_ack ack;
	unsigned int response_len, enable, type;

	if (sscanf(buf, "%u;%u", &enable, &type) != 2) {
		dev_err(dev, "Incorrect testsignal message");
		return -EINVAL;
	}

	if (enable) { //get
		struct beo_ase_msg_enable_test_signal_cmd enable_test_signal;
		enable_test_signal.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_ENABLE_TEST_SIGNAL, sizeof(enable_test_signal));
		enable_test_signal.outlen = beo_ase_dsp_create_outlen();
		enable_test_signal.signal_type = type;
		response_len = priv->fepcore->send(priv->fepcore, sizeof(enable_test_signal), (u8 *)&enable_test_signal, sizeof(ack), (u8 *)&ack);
	}
	else {	//clear
		struct beo_ase_msg_disable_test_signal_cmd disable_test_signal;
		disable_test_signal.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_DISABLE_TEST_SIGNAL, sizeof(disable_test_signal));
		disable_test_signal.outlen = beo_ase_dsp_create_outlen();
		response_len = priv->fepcore->send(priv->fepcore, sizeof(disable_test_signal), (u8 *)&disable_test_signal, sizeof(ack), (u8 *)&ack);
	}

	return beo_ase_dsp_log(dev, &ack, len, "testsignal");
}

static ssize_t beo_ase_dsp_sinusconf(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_conf_sin_gen_cmd con_sin_generator;
	struct beo_ase_dsp_ack ack;
	u32 response_len, frequency;
	u8 hw_hb, hw_lb, lw_hb, lw_lb;

	if (sscanf(buf, "%u", &frequency) != 1) {
		dev_err(dev, "Incorrect testsignal message");
		return -EINVAL;
	}

	u32tou8(frequency, &hw_hb, &hw_lb, &lw_hb, &lw_lb);
	con_sin_generator.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_CONF_SIN_GEN, sizeof(con_sin_generator));
	con_sin_generator.outlen = beo_ase_dsp_create_outlen();
	con_sin_generator.setting.generator_id = 0;
	con_sin_generator.setting.mask = FREQUENCY_MASK;
	con_sin_generator.setting.frequency.hw_hb = hw_hb;
	con_sin_generator.setting.frequency.hw_lb = hw_lb;
	con_sin_generator.setting.frequency.lw_hb = lw_hb;
	con_sin_generator.setting.frequency.lw_lb = lw_lb;
	response_len = priv->fepcore->send(priv->fepcore, sizeof(con_sin_generator), (u8 *)&con_sin_generator, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "sinusconf");
}

static ssize_t beo_ase_dsp_mtsinusconf(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_conf_mt_sin_gen_cmd con_sin_generator;
	struct beo_ase_dsp_ack ack;
	u32 response_len, count, frequency, gain, mask, state, i = 0;
	u8 hw_hb, hw_lb, lw_hb, lw_lb;
	char *token, *string_buffer, *string;
	u8 ret = 1;

	string_buffer = (char *)kmalloc(len + 1, GFP_KERNEL);
	memcpy(string_buffer, buf, len);
	string_buffer[len] = 0;

	state = 0;
	string = string_buffer;
	while ((token = strsep(&string, ";")) != NULL) {
		if (state == 0) {
			ret = (sscanf(token, "%u", &count) == 1);
			state = 1;
		}
		else if (state == 1) {
			ret = (sscanf(token, "%u", &mask) == 1);
			state = 2;
		}
		else if (state == 2) {
			ret = (sscanf(token, "%u", &frequency) == 1);
			state = 3;
		}
		else if (state == 3) {
			ret = (sscanf(token, "%u", &gain) == 1);
			con_sin_generator.setting[con_sin_generator.count].generator_id = con_sin_generator.count;
			con_sin_generator.setting[con_sin_generator.count].mask = mask;
			u32tou8(frequency, &hw_hb, &hw_lb, &lw_hb, &lw_lb);
			con_sin_generator.setting[con_sin_generator.count].frequency.hw_hb = hw_hb;
			con_sin_generator.setting[con_sin_generator.count].frequency.hw_lb = hw_lb;
			con_sin_generator.setting[con_sin_generator.count].frequency.lw_hb = lw_hb;
			con_sin_generator.setting[con_sin_generator.count].frequency.lw_lb = lw_lb;
			u32tou8(gain, &hw_hb, &hw_lb, &lw_hb, &lw_lb);
			con_sin_generator.setting[con_sin_generator.count].gain.hw_hb = hw_hb;
			con_sin_generator.setting[con_sin_generator.count].gain.hw_lb = hw_lb;
			con_sin_generator.setting[con_sin_generator.count].gain.lw_hb = lw_hb;
			con_sin_generator.setting[con_sin_generator.count].gain.lw_lb = lw_lb;
			con_sin_generator.count++;
			++i;
			state = 1;
		}
		if (!ret) {
			dev_err(dev, "Incorrect mtsinusconf message");
			goto out;
		}
		if (i >= count) break;
	}
	if (state != 1) {
		dev_err(dev, "Incorrect mtsinusconf message");
		ret = 0;
		goto out;
	}

	con_sin_generator.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_CONF_MT_SIN_GEN, sizeof(con_sin_generator));
	con_sin_generator.outlen = beo_ase_dsp_create_outlen();
	response_len = priv->fepcore->send(priv->fepcore, sizeof(con_sin_generator), (u8 *)&con_sin_generator, sizeof(ack), (u8 *)&ack);

	beo_ase_dsp_log(dev, &ack, response_len, "mtsinusconf");

out:
	kfree(string_buffer);
	if (!ret) {
		return -EINVAL;
	}
	return len;
}

static ssize_t beo_ase_dsp_signalconf(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_conf_signal_cmd conf_signal;
	struct beo_ase_dsp_ack ack;
	u32 response_len, outputAllocation, signalType;

	if (sscanf(buf, "%u;%u", &outputAllocation, &signalType) != 2) {
		dev_err(dev, "Incorrect signalconf message");
		return -EINVAL;
	}

	conf_signal.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_CONF_SIGNAL, sizeof(conf_signal));
	conf_signal.outlen = beo_ase_dsp_create_outlen();
	conf_signal.outputAllocation = outputAllocation;
	conf_signal.signalType = signalType;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(conf_signal), (u8 *)&conf_signal, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "signalconf");
}

static ssize_t beo_ase_dsp_audioswitch(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_set_audio_switch audio_switch;
	struct beo_ase_dsp_ack ack;
	u32 response_len;
	u32 position, type, options;

	if (sscanf(buf, "%u;%u;%u", &position, &type, &options) != 3) {
		dev_err(dev, "Incorrect audio switch message");
		return -EINVAL;
	}

	audio_switch.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_SET_AUDIO_SWITCH, sizeof(audio_switch));
	audio_switch.outlen = beo_ase_dsp_create_outlen();
	audio_switch.speaker_position = (u8)position;
	audio_switch.speaker_type = (u8)type;
	audio_switch.router_options = (u8)options;
	response_len = priv->fepcore->send(priv->fepcore, sizeof(audio_switch), (u8 *)&audio_switch, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "audioswitch");
}

static ssize_t beo_ase_dsp_amplifiers(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_cmd_msg cmd_msg;
	struct beo_ase_dsp_ack ack;
	u32 response_len;
	u32 type;

	if (sscanf(buf, "%u;", &type) != 1) {
		dev_err(dev, "Incorrect audio amplifiers message");
		return -EINVAL;
	}

	if (type == 0) {
		cmd_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_AMPS_SHUTDOWN, sizeof(cmd_msg));
	}
	else {
		cmd_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_AMPS_RESTORE, sizeof(cmd_msg));
	}
	cmd_msg.outlen = beo_ase_dsp_create_outlen();
	response_len = priv->fepcore->send(priv->fepcore, sizeof(cmd_msg), (u8 *)&cmd_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "amplifiers");
}

static ssize_t beo_ase_dsp_swposition(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_dsp_sw_position_cmd sw_pos_msg;
	struct beo_ase_dsp_ack ack;
	int response_len;

	sw_pos_msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_GET_POSITION, sizeof(sw_pos_msg));
	sw_pos_msg.outlen = beo_ase_dsp_create_outlen();

	response_len = priv->fepcore->send(priv->fepcore, sizeof(sw_pos_msg), (u8 *)&sw_pos_msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "swposition");
}

static ssize_t beo_ase_dsp_audioswitch_new(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_cmd_conf_audio_switch_new msg;
	struct beo_ase_dsp_ack ack;
	u32 response_len, col, row, connection_data;

	if (sscanf(buf, "%u;%u;%u", &col, &row, &connection_data) != 3) {
		dev_err(dev, "Incorrect audioswitch_new message");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_CONF_AUDIO_SWITCH, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();

	msg.column = col;
	msg.row = row;
	msg.connection_data = connection_data;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "audioswitch_new");
}

static ssize_t beo_ase_dsp_audiostate(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_cmd_set_audio_state msg;
	struct beo_ase_dsp_ack ack;
	u32 response_len, state;

	if (sscanf(buf, "%u;", &state) != 1) {
		dev_err(dev, "Incorrect  dsp audio state message");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_SET_AUDIO_STATE, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();

	msg.state = state;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "audiostate");
}

static ssize_t beo_ase_dsp_toslinkctrl(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_toslink_cmd msg;
	struct beo_ase_dsp_ack ack;
	int response_len;
	unsigned int mode;

	if (sscanf(buf, "%u;", &mode) != 1) {
		dev_err(dev, "Incorrect toslink mode\n");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_TOSLINKCTRL, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();
	msg.mode = mode;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "toslinkctrl");
}

static ssize_t beo_ase_dsp_bass(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_bass msg;
	struct beo_ase_dsp_ack ack;
	int response_len;
	long int bass;

	if (kstrtol(buf, 10, &bass)) {
		dev_err(dev, "Incorrect bass value\n");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_BASS, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();
	msg.bass = bass;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "bass");
}

static ssize_t beo_ase_dsp_treble(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_treble msg;
	struct beo_ase_dsp_ack ack;
	int response_len;
	long int treble;

	if (kstrtol(buf, 10, &treble)) {
		dev_err(dev, "Incorrect treble value\n");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_TREBLE, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();
	msg.treble = treble;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "treble");
}

static ssize_t beo_ase_dsp_autoloudness(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_dsp_priv *priv = device_to_private(dev);
	struct beo_ase_msg_autoloudness msg;
	struct beo_ase_dsp_ack ack;
	int response_len;
	long int autoloudness;

	if (kstrtol(buf, 10, &autoloudness)) {
		dev_err(dev, "Incorrect loudness value\n");
		return -EINVAL;
	}

	msg.hdr = beo_ase_dsp_create_header(BEO_ASE_CMD_AUTOLOUDNESS, sizeof(msg));
	msg.outlen = beo_ase_dsp_create_outlen();
	msg.autoLoudness = autoloudness;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	return beo_ase_dsp_log(dev, &ack, len, "autoloudness");
}

static const struct file_operations fops = {
	.read = device_read,
	.open = device_open,
	.release = device_release,
	.poll = device_poll,
};

static DEVICE_ATTR(initialize, S_IWUSR, NULL, beo_ase_dsp_initialize);
static DEVICE_ATTR(volume, S_IWUSR, NULL, beo_ase_dsp_volume);
static DEVICE_ATTR(mute, S_IWUSR, NULL, beo_ase_dsp_mute);
static DEVICE_ATTR(source, S_IWUSR, NULL, beo_ase_dsp_source);
static DEVICE_ATTR(sound, S_IWUSR, NULL, beo_ase_dsp_sound);
static DEVICE_ATTR(version, S_IWUSR, NULL, beo_ase_dsp_version);
static DEVICE_ATTR(layout, S_IWUSR, NULL, beo_ase_dsp_layout);
static DEVICE_ATTR(filter, S_IWUSR, NULL, beo_ase_dsp_filter);
static DEVICE_ATTR(gain, S_IWUSR, NULL, beo_ase_dsp_gain);
static DEVICE_ATTR(tunnel, S_IWUSR, NULL, beo_ase_dsp_tunnel);
static DEVICE_ATTR(calibrate, S_IWUSR, NULL, beo_ase_dsp_calibrate);
static DEVICE_ATTR(resetcounters, S_IWUSR, NULL, beo_ase_dsp_reset_counters);
static DEVICE_ATTR(errlog, S_IWUSR, NULL, beo_ase_dsp_errlog);
static DEVICE_ATTR(ntcvalues, S_IWUSR, NULL, beo_ase_dsp_ntc_values);
static DEVICE_ATTR(testsignal, S_IWUSR, NULL, beo_ase_dsp_testsignal);
static DEVICE_ATTR(sinusconf, S_IWUSR, NULL, beo_ase_dsp_sinusconf);
static DEVICE_ATTR(mtsinusconf, S_IWUSR, NULL, beo_ase_dsp_mtsinusconf);
static DEVICE_ATTR(signalconf, S_IWUSR, NULL, beo_ase_dsp_signalconf);
static DEVICE_ATTR(audioswitch, S_IWUSR, NULL, beo_ase_dsp_audioswitch);
static DEVICE_ATTR(amplifiers, S_IWUSR, NULL, beo_ase_dsp_amplifiers);
static DEVICE_ATTR(swposition, S_IWUSR, NULL, beo_ase_dsp_swposition);
static DEVICE_ATTR(audioswitch_new, S_IWUSR, NULL, beo_ase_dsp_audioswitch_new);
static DEVICE_ATTR(audiostate, S_IWUSR, NULL, beo_ase_dsp_audiostate);
static DEVICE_ATTR(toslinkctrl, S_IWUSR, NULL, beo_ase_dsp_toslinkctrl);
static DEVICE_ATTR(bass, S_IWUSR, NULL, beo_ase_dsp_bass);
static DEVICE_ATTR(treble, S_IWUSR, NULL, beo_ase_dsp_treble);
static DEVICE_ATTR(autoloudness, S_IWUSR, NULL, beo_ase_dsp_autoloudness);

static void beo_ase_dsp_unregister_devices(struct device* dev)
{
	device_remove_file(dev, &dev_attr_initialize);
	device_remove_file(dev, &dev_attr_volume);
	device_remove_file(dev, &dev_attr_mute);
	device_remove_file(dev, &dev_attr_source);
	device_remove_file(dev, &dev_attr_sound);
	device_remove_file(dev, &dev_attr_version);
	device_remove_file(dev, &dev_attr_layout);
	device_remove_file(dev, &dev_attr_filter);
	device_remove_file(dev, &dev_attr_gain);
	device_remove_file(dev, &dev_attr_tunnel);
	device_remove_file(dev, &dev_attr_calibrate);
	device_remove_file(dev, &dev_attr_resetcounters);
	device_remove_file(dev, &dev_attr_errlog);
	device_remove_file(dev, &dev_attr_ntcvalues);
	device_remove_file(dev, &dev_attr_testsignal);
	device_remove_file(dev, &dev_attr_sinusconf);
	device_remove_file(dev, &dev_attr_mtsinusconf);
	device_remove_file(dev, &dev_attr_audioswitch);
	device_remove_file(dev, &dev_attr_signalconf);
	device_remove_file(dev, &dev_attr_amplifiers);
	device_remove_file(dev, &dev_attr_swposition);
	device_remove_file(dev, &dev_attr_audioswitch_new);
	device_remove_file(dev, &dev_attr_audiostate);
	device_remove_file(dev, &dev_attr_toslinkctrl);
	device_remove_file(dev, &dev_attr_bass);
	device_remove_file(dev, &dev_attr_treble);
	device_remove_file(dev, &dev_attr_autoloudness);
}

static bool beo_ase_dsp_register_devices(struct device* dev)
{
	int ret = 0;
	ret =  device_create_file(dev, &dev_attr_initialize);
	ret |= device_create_file(dev, &dev_attr_volume);
	ret |= device_create_file(dev, &dev_attr_mute);
	ret |= device_create_file(dev, &dev_attr_source);
	ret |= device_create_file(dev, &dev_attr_sound);
	ret |= device_create_file(dev, &dev_attr_version);
	ret |= device_create_file(dev, &dev_attr_layout);
	ret |= device_create_file(dev, &dev_attr_filter);
	ret |= device_create_file(dev, &dev_attr_gain);
	ret |= device_create_file(dev, &dev_attr_tunnel);
	ret |= device_create_file(dev, &dev_attr_calibrate);
	ret |= device_create_file(dev, &dev_attr_resetcounters);
	ret |= device_create_file(dev, &dev_attr_errlog);
	ret |= device_create_file(dev, &dev_attr_ntcvalues);
	ret |= device_create_file(dev, &dev_attr_testsignal);
	ret |= device_create_file(dev, &dev_attr_sinusconf);
	ret |= device_create_file(dev, &dev_attr_mtsinusconf);
	ret |= device_create_file(dev, &dev_attr_audioswitch);
	ret |= device_create_file(dev, &dev_attr_signalconf);
	ret |= device_create_file(dev, &dev_attr_amplifiers);
	ret |= device_create_file(dev, &dev_attr_swposition);
	ret |= device_create_file(dev, &dev_attr_audioswitch_new);
	ret |= device_create_file(dev, &dev_attr_audiostate);
	ret |= device_create_file(dev, &dev_attr_toslinkctrl);
	ret |= device_create_file(dev, &dev_attr_bass);
	ret |= device_create_file(dev, &dev_attr_treble);
	ret |= device_create_file(dev, &dev_attr_autoloudness);
	return ret;
}

static int beo_ase_dsp_register_device(struct beo_ase_dsp_priv *priv, struct device* dev)
{
	int ret;
	ret = alloc_chrdev_region(&dsp_base_dev, 0, 1, DEVICE_NAME);
	if (ret) {
		dev_err(dev, "alloc_chrdev_region() failed %d\n", ret);
		return ret;
	}

	dsp_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(dsp_class)) {
		ret = PTR_ERR(dsp_class);
		dev_err(dev, "class_create() failed %d\n", ret);
		goto unreg_chardev;
	}

	cdev_init(&priv->cdev, &fops);
	priv->cdev.owner = THIS_MODULE;

	ret = cdev_add(&priv->cdev, dsp_base_dev, 1);
	if (ret) {
		dev_err(dev, "cdev_add() failed %d\n", ret);
		goto clean_class;
	}

	priv->cdevice = device_create(dsp_class, dev, dsp_base_dev, NULL, DEVICE_NAME);
		if (IS_ERR(priv->cdevice)) {
		ret = PTR_ERR(priv->cdevice);
		dev_err(dev, "device_create() failed %d\n", ret);
		goto clean_cdev;
	}

	ret = beo_ase_dsp_register_devices(dev);
	if (ret) {
		goto clean_sysfs;
	}

	return 0;

clean_sysfs:
	beo_ase_dsp_unregister_devices(dev);
clean_cdev:
	cdev_del(&priv->cdev);
clean_class:
	class_destroy(dsp_class);
unreg_chardev:
	unregister_chrdev_region(dsp_base_dev, 1);

	return ret;
}

static int beo_ase_dsp_probe(struct platform_device *pdev)
{
	struct beo_ase_dsp_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	int ret;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_ase_dsp_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->pdev = pdev;
	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	ret = kfifo_alloc(&priv->data, FIFO_SIZE, GFP_KERNEL);
	if (ret) {
		kfree(priv);
		return ret;
	}

	init_waitqueue_head(&priv->read_wait_queue);
	ret = beo_ase_dsp_register_device(priv, &pdev->dev);

	if (!ret) {
		mfd_cell_enable(pdev);

		priv->fepcore->register_events(pdev, BEO_ASE_EV_HW_OVERHEAT, 1, priv, beo_ase_dsp_event_short_cb);
		priv->fepcore->register_events(pdev, BEO_ASE_EV_PL_ON, 1, priv, beo_ase_dsp_event_short_cb);
		priv->fepcore->register_events(pdev, BEO_ASE_EV_PL_OFF, 1, priv, beo_ase_dsp_event_short_cb);
		priv->fepcore->register_events(pdev, BEO_ASE_EV_TOSLINK_UP, 2, priv, beo_ase_dsp_event_short_cb);
		priv->fepcore->register_events(pdev, BEO_ASE_EV_DSP_ERRLEVEL, 1, priv, beo_ase_dsp_event_short_cb);

		priv->fepcore->register_long_events(pdev,
											_BEO_ASE_EV_DSP_BEGIN,
											_BEO_ASE_EV_DSP_END - _BEO_ASE_EV_DSP_BEGIN + 1,
											priv,
											beo_ase_dsp_event_cb);

		platform_set_drvdata(pdev, priv);

		dev_info(&pdev->dev, "Initialized.\n");
		return 0;
	}

	cdev_del(&priv->cdev);
	kfifo_free(&priv->data);

	kfree(priv);
	return ret;
}

static int beo_ase_dsp_remove(struct platform_device *pdev)
{
	struct beo_ase_dsp_priv *priv;

	mfd_cell_disable(pdev);

	priv = platform_get_drvdata(pdev);

	cdev_del(&priv->cdev);
	kfifo_free(&priv->data);
	device_destroy(dsp_class, dsp_base_dev);
	class_destroy(dsp_class);

	unregister_chrdev_region(dsp_base_dev, 1);
	beo_ase_dsp_unregister_devices(&pdev->dev);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static const struct of_device_id of_beo_ase_dsp_match[] = {
	{ .compatible = "beo-ase-dsp", },
	{},
};

static struct platform_driver beo_ase_dsp_driver = {
	.probe		= beo_ase_dsp_probe,
	.remove		= beo_ase_dsp_remove,
	.driver		= {
		.name	= "beo-ase-dsp",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_ase_dsp_match),
	},
};

module_platform_driver(beo_ase_dsp_driver);

MODULE_AUTHOR("Martin Geier <martin.geier@streamunlimited.com>");
MODULE_DESCRIPTION("BeO ASE FEP dsp driver");
MODULE_LICENSE("GPL v2");
