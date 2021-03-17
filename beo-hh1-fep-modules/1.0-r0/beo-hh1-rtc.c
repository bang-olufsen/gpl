/*
 * RTC driver for BeO HH1 FEP
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
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"


struct beo_hh1_rtc_priv {
	struct beo_hh1_fep_core *fepcore;
	struct device *dev;
	struct rtc_device *rtc;

	struct beo_hh1_data_rtc rtc_data;
	int rtc_fetched;

	struct completion compl;
};

/* Code to create from OpenFirmware platform devices */
static struct beo_hh1_rtc_priv * __init beo_hh1_rtc_create_of(struct platform_device *pdev)
{
	struct beo_hh1_rtc_priv *priv;

	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data.\n");
		return NULL;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_rtc_priv),
			GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->fepcore = *fepcore;
	return priv;
}


static const struct of_device_id of_beo_hh1_rtc_match[] = {
	{ .compatible = "beo-hh1-rtc", },
	{},
};

static int beo_hh1_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_rtc_priv *priv = platform_get_drvdata(pdev);
	int ret = 0;

	struct beo_hh1_rsp_ack ack;
	struct beo_hh1_msg_rtcget *msg;
	int msglen;

	// re-init completion structure
	init_completion(&priv->compl);

	// send command
	msglen = sizeof(struct beo_hh1_msg_rtcget);
	msg = kzalloc(msglen, GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_GETRTC);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;

		priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);

		if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
			dev_err(dev, "No ACK!\n");
			return -ENODEV;
		}

		ret = 0;
	}
	else {
		ret = -ENOMEM;
	}

	if (wait_for_completion_killable_timeout(&priv->compl, HZ * 1) > 0) {
		tm->tm_year = priv->rtc_data.year - 1900;
		tm->tm_mon = priv->rtc_data.month - 1;
		tm->tm_mday = priv->rtc_data.day;
		tm->tm_hour = priv->rtc_data.hour;
		tm->tm_min = priv->rtc_data.min;
		tm->tm_sec = priv->rtc_data.sec;
		return 0;
	}
	else {
		dev_err(priv->dev, "Timeout reading RTC\n");
		return -ENODEV;
	}
}

static int beo_hh1_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_rtc_priv *priv = platform_get_drvdata(pdev);
	int ret = 0;

	struct beo_hh1_rsp_ack ack;
	struct beo_hh1_msg_rtcset *msg;
	int msglen;

	// send command
	msglen = sizeof(struct beo_hh1_msg_rtcset);
	msg = kzalloc(msglen, GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_SETRTC);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;

		msg->time.year = cpu_to_le16(tm->tm_year + 1900);
		msg->time.month = tm->tm_mon + 1;
		msg->time.day = tm->tm_mday;
		msg->time.hour = tm->tm_hour;
		msg->time.min = tm->tm_min;
		msg->time.sec = tm->tm_sec;

		priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);

		if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
			dev_err(dev, "No ACK!\n");
			return -ENODEV;
		}

		ret = 0;
	}
	else {
		ret = -ENOMEM;
	}

	return ret;
}

static const struct rtc_class_ops beo_hh1_rtc_ops = {
	.read_time = beo_hh1_rtc_read_time,
	.set_time = beo_hh1_rtc_set_time,
};

static int beo_hh1_rtc_event_cb(void *_priv, u16 event, u32 size, u8 *data)
{
	struct beo_hh1_rtc_priv *priv = _priv;
	// dev_info(priv->dev, "long_event_cb(0x%04x, %d bytes)\n", event, size);

	if (event == BEO_HH1_EV_RTC && size == sizeof(struct beo_hh1_data_rtc)) {
		memcpy(&priv->rtc_data, data, size);
		priv->rtc_data.year = le16_to_cpu(priv->rtc_data.year);
		complete_all(&priv->compl);
	}

	return 0;
}

static int beo_hh1_rtc_probe(struct platform_device *pdev)
{
	struct beo_hh1_rtc_priv *priv;
	priv = beo_hh1_rtc_create_of(pdev);
	if (!priv)
		return -ENODEV;

	platform_set_drvdata(pdev, priv);
	priv->dev = &pdev->dev;

	init_completion(&priv->compl);

	mfd_cell_enable(pdev);

	priv->fepcore->register_long_events(pdev, BEO_HH1_EV_RTC, 1, priv, beo_hh1_rtc_event_cb);

	// register rtc; this will cause .read_time to be called!
	priv->rtc = rtc_device_register("hh1", &pdev->dev,
									&beo_hh1_rtc_ops, THIS_MODULE);

	if (priv->rtc == NULL) {
		return -ENODEV;
	}

	return 0;
}

static int beo_hh1_rtc_remove(struct platform_device *pdev)
{
	struct beo_hh1_rtc_priv *priv = platform_get_drvdata(pdev);

	mfd_cell_disable(pdev);

	rtc_device_unregister(priv->rtc);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_rtc_driver = {
	.probe		= beo_hh1_rtc_probe,
	.remove		= beo_hh1_rtc_remove,
	.driver		= {
		.name	= "beo-hh1-rtc",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_rtc_match),
	},
};

module_platform_driver(beo_hh1_rtc_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP RTC driver");
MODULE_LICENSE("GPL v2");
