/*
 * fephw driver for BeO ASE FEP
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
#include <linux/kfifo.h>
#include <linux/sched.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"
#define DEVICE_NAME "beo-ase-fephw"

#define TOUCHRAW_LEN 12
#define TOUCHAVER_LEN 12
#define TOUCHLEVEL_LEN 4
#define FIFO_SIZE 256

struct beo_ase_fephw_priv {
	struct platform_device *pdev;
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;

	wait_queue_head_t read_wait_queue;
	struct kfifo data;
};

static struct beo_ase_fephw_priv *device_to_private(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	return platform_get_drvdata(pdev);
}

static struct beo_hh1_msg_hdr beo_ase_fephw_create_header(u16 command, u16 len)
{
	struct beo_hh1_msg_hdr hdr;
	hdr.command = cpu_to_le16(command);
	hdr.len = cpu_to_le16(len);
	return hdr;
}

static struct beo_hh1_msg_outlen beo_ase_fephw_create_outlen(ssize_t size)
{
	struct beo_hh1_msg_outlen outlen;
	outlen.outlen = cpu_to_le16(size);
	outlen.pad = 0;
	return outlen;
}

static int beo_ase_fephw_event_cb(void *_priv, u16 _event, u32 size, u8 *data)
{
	struct beo_ase_fephw_priv *priv = _priv;

	kfifo_in(&priv->data, "1", 1);
	wake_up_interruptible(&priv->read_wait_queue);

	return 0;
}

static ssize_t beo_ase_fephw_touchaver(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct beo_ase_fephw_priv *priv = device_to_private(dev);
	struct beo_ase_msg_touch touch_msg;
	struct beo_ase_rsp_touchaver touchaver_rsp;
	int response_len;

	touch_msg.hdr = beo_ase_fephw_create_header(BEO_ASE_CMD_TOUCHAVER, sizeof(touch_msg));
	touch_msg.outlen = beo_ase_fephw_create_outlen(sizeof(touchaver_rsp));
	response_len = priv->fepcore->send(priv->fepcore, sizeof(touch_msg), (u8 *)&touch_msg, sizeof(touchaver_rsp), (u8 *)&touchaver_rsp);

	if (response_len == sizeof(touchaver_rsp)) {
		int i;
		for (i = 0; i < TOUCHAVER_LEN; i++) {
			touchaver_rsp.values[i] = le16_to_cpu(touchaver_rsp.values[i]);
		}
	}
	else {
		memset(&touchaver_rsp, 0, sizeof(touchaver_rsp));
	}
	memcpy(buf, touchaver_rsp.values, sizeof(u16) * TOUCHAVER_LEN);

	return sizeof(u16) * TOUCHAVER_LEN;
}

static ssize_t beo_ase_fephw_touchraw(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct beo_ase_fephw_priv *priv = device_to_private(dev);
	struct beo_ase_msg_touch touch_msg;
	struct beo_ase_rsp_touchraw touchraw_rsp;
	int response_len;

	touch_msg.hdr = beo_ase_fephw_create_header(BEO_ASE_CMD_TOUCHRAW, sizeof(touch_msg));
	touch_msg.outlen = beo_ase_fephw_create_outlen(sizeof(touchraw_rsp));

	response_len = priv->fepcore->send(priv->fepcore, sizeof(touch_msg), (u8 *)&touch_msg, sizeof(touchraw_rsp), (u8 *)&touchraw_rsp);

	if (response_len == sizeof(touchraw_rsp)) {
		int i;
		for (i = 0; i < TOUCHRAW_LEN; i++) {
			touchraw_rsp.values[i] = le16_to_cpu(touchraw_rsp.values[i]);
		}
	}
	else {
		memset(&touchraw_rsp, 0, sizeof(touchraw_rsp));
	}

	memcpy(buf, touchraw_rsp.values, sizeof(u16) * TOUCHRAW_LEN);
	return sizeof(u16) * TOUCHRAW_LEN;
}

static ssize_t beo_ase_fephw_touchlevel_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct beo_ase_fephw_priv *priv = device_to_private(dev);
	struct beo_ase_msg_touch touch_msg;
	struct beo_ase_rsp_touchlevel touchlevel_rsp;
	int response_len;

	touch_msg.hdr = beo_ase_fephw_create_header(BEO_ASE_CMD_TOUCHLEVEL, sizeof(touch_msg));
	touch_msg.outlen = beo_ase_fephw_create_outlen(sizeof(touchlevel_rsp));

	response_len = priv->fepcore->send(priv->fepcore, sizeof(touch_msg), (u8 *)&touch_msg, sizeof(touchlevel_rsp), (u8 *)&touchlevel_rsp);

	if (response_len == sizeof(touchlevel_rsp)) {
		int i;
		for (i = 0; i < TOUCHLEVEL_LEN; i++) {
			touchlevel_rsp.values[i] = le16_to_cpu(touchlevel_rsp.values[i]);
		}
	}
	else {
		memset(&touchlevel_rsp, 0, sizeof(touchlevel_rsp));
	}
	memcpy(buf, touchlevel_rsp.values, sizeof(u16) * TOUCHLEVEL_LEN);

	return sizeof(u16) * TOUCHLEVEL_LEN;
}

static ssize_t beo_ase_fephw_touchlevel_write(struct device *dev,struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_fephw_priv *priv = device_to_private(dev);
	struct beo_ase_msg_touchlevel touchlevel_msg;
	struct beo_hh1_rsp_ack ack;
	int i;

	if (len != sizeof(u16) * TOUCHLEVEL_LEN) {
		dev_err(dev, "Invalid data size: '%i'", len);
		return -EINVAL;
	}

	touchlevel_msg.hdr = beo_ase_fephw_create_header(BEO_ASE_CMD_TOUCHLEVEL, sizeof(touchlevel_msg));
	touchlevel_msg.outlen = beo_ase_fephw_create_outlen(sizeof(ack));

	for (i = 0; i < TOUCHLEVEL_LEN; i++) {
		touchlevel_msg.values[i] = cpu_to_le16(((u16 *)buf)[i]);
	}
	priv->fepcore->send(priv->fepcore, sizeof(touchlevel_msg), (u8 *)&touchlevel_msg, sizeof(ack), (u8 *)&ack);
	return len;
}

static ssize_t beo_ase_fephw_touchraw_block_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct beo_ase_fephw_priv *priv = device_to_private(dev);
	struct beo_ase_msg_sync_touch_cmd sync_touch;
	struct beo_hh1_rsp_ack ack;
	char item;
	int ret;

	sync_touch.hdr = beo_ase_fephw_create_header(BEO_ASE_CMD_TOUCHSYNC, sizeof(sync_touch));
	sync_touch.outlen = beo_ase_fephw_create_outlen(sizeof(ack));

	priv->fepcore->send(priv->fepcore, sizeof(sync_touch), (u8 *)&sync_touch, sizeof(ack), (u8 *)&ack);

	wait_event_interruptible(priv->read_wait_queue, !kfifo_is_empty(&priv->data));
	ret = kfifo_out(&priv->data, &item, 1);

	if (!ret) {
		return 0;
	}

	return beo_ase_fephw_touchraw(dev, attr, buf);
}

static DEVICE_ATTR(touchaver, S_IRUSR, beo_ase_fephw_touchaver, NULL);
static DEVICE_ATTR(touchraw, S_IRUSR, beo_ase_fephw_touchraw, NULL);
static DEVICE_ATTR(touchlevel, S_IRUSR | S_IWUSR, beo_ase_fephw_touchlevel_read, beo_ase_fephw_touchlevel_write);
static DEVICE_ATTR(touchrawblock, S_IRUSR, beo_ase_fephw_touchraw_block_read, NULL);

static int beo_ase_fephw_probe(struct platform_device *pdev)
{
	struct beo_ase_fephw_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	int ret;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_ase_fephw_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->pdev = pdev;
	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	ret = kfifo_alloc(&priv->data, FIFO_SIZE, GFP_KERNEL);
	if(ret){
		kfree(priv);
		return ret;
	}

	init_waitqueue_head(&priv->read_wait_queue);

	ret =  device_create_file(priv->dev, &dev_attr_touchaver);
	ret |= device_create_file(priv->dev, &dev_attr_touchraw);
	ret |= device_create_file(priv->dev, &dev_attr_touchlevel);
	ret |= device_create_file(priv->dev, &dev_attr_touchrawblock);

	if (!ret) {
		mfd_cell_enable(pdev);
		priv->fepcore->register_long_events(pdev, BEO_ASE_EV_DSP_TOUCHSYNC, 1, priv, beo_ase_fephw_event_cb);
		platform_set_drvdata(pdev, priv);

		dev_info(&pdev->dev, "Initialized.\n");
	}
	else {
		device_remove_file(priv->dev, &dev_attr_touchaver);
		device_remove_file(priv->dev, &dev_attr_touchraw);
		device_remove_file(priv->dev, &dev_attr_touchlevel);
		device_remove_file(priv->dev, &dev_attr_touchrawblock);

		kfifo_free(&priv->data);
		kfree(priv);
	}
	return ret;
}

static int beo_ase_fephw_remove(struct platform_device *pdev)
{
	struct beo_ase_fephw_priv *priv;

	priv = platform_get_drvdata(pdev);

	mfd_cell_disable(pdev);

	kfifo_free(&priv->data);
	device_remove_file(&pdev->dev, &dev_attr_touchaver);
	device_remove_file(&pdev->dev, &dev_attr_touchraw);
	device_remove_file(&pdev->dev, &dev_attr_touchlevel);
	device_remove_file(&pdev->dev, &dev_attr_touchrawblock);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static const struct of_device_id of_beo_ase_fephw_match[] = {
	{ .compatible = "beo-ase-fephw", },
	{},
};

static struct platform_driver beo_ase_fephw_driver = {
	.probe		= beo_ase_fephw_probe,
	.remove		= beo_ase_fephw_remove,
	.driver		= {
		.name	= "beo-ase-fephw",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_ase_fephw_match),
	},
};

module_platform_driver(beo_ase_fephw_driver);

MODULE_AUTHOR("Martin Geier <martin.geier@streamunlimited.com>");
MODULE_DESCRIPTION("BeO ASE FEP hardware info driver");
MODULE_LICENSE("GPL v2");
