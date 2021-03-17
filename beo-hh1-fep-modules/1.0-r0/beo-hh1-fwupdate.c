/*
 * FEP FW update driver for BeO HH1 FEP
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
#include <linux/leds.h>
#include <linux/completion.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"

// #define DEBUG_COMMANDS

#define FWUPDATE_TIMEOUT_REBOOTING		10
#define FWUPDATE_TIMEOUT_STARTING		15

struct beo_hh1_fwupdate_priv;

enum beo_hh1_fwupdate_state {
	state_boot,
	state_idle,
	state_rebooting,
	state_uploading,
	state_starting,
	state_error,
};

struct beo_hh1_fwupdate_priv {
	struct platform_device *pdev;
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;

	volatile enum beo_hh1_fwupdate_state state;
	volatile int upload_done;
	volatile int upload_pending_ack;
	volatile int upload_pending_seq;
	int upload_size;
	const struct firmware *upload_fw;
	struct delayed_work upload_work;

	int upload_rebooting_cnt;
	int upload_starting_cnt;
	int upload_lost_block_cnt;

	// bldr/app versions
	struct beo_hh1_rsp_version ver_bldr;
	struct beo_hh1_rsp_version ver_app;
	int ver_bldr_valid;
	int ver_app_valid;

	struct beo_hh1_rsp_hw_version ver_hw;
	int ver_hw_valid;

	struct mutex fw_mutex;

	int compl_needed;
	struct completion compl;
	const char *firmware_name;
};

static const struct of_device_id of_beo_hh1_fwupdate_match[] = {
	{ .compatible = "beo-hh1-fwupdate", },
	{},
};


DEFINE_LED_TRIGGER(ledtrig_hh1_fwup);


static int beo_hh1_fwupdate_check_fepapp(struct beo_hh1_fwupdate_priv *priv)
{
	// send is_app to FEP
	struct beo_hh1_msg_is_app *msg;
	struct beo_hh1_rsp_ack ack;

	msg = kzalloc(sizeof(struct beo_hh1_msg_is_app), GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_IS_APP);
		msg->hdr.len = cpu_to_le16(sizeof(*msg));
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;

		priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);
	}
	else {
		return -ENOMEM;
	}

#ifdef DEBUG_COMMANDS
// fake ack
ack.command = cpu_to_le16(BEO_HH1_REP_ACK);
#endif

	if (le16_to_cpu(ack.command) == BEO_HH1_REP_ACK)
		return 0;

	// no ack -> we're not in app mode
	return 1;
}

static int beo_hh1_fwupdate_load_version(struct beo_hh1_fwupdate_priv *priv, u16 cmd, int *valid, struct beo_hh1_rsp_version *ver)
{
	// send get_version to FEP
	struct beo_hh1_msg_version *msg;
	struct beo_hh1_rsp_version ack;
	const char *module;

	msg = kzalloc(sizeof(struct beo_hh1_msg_version), GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(cmd);
		msg->hdr.len = cpu_to_le16(sizeof(*msg));
		msg->outlen.outlen = cpu_to_le16(sizeof(ack));
		msg->outlen.pad = 0;

		priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);
	}
	else {
		return -ENOMEM;
	}

	module = (cmd == BEO_HH1_CMD_GETBOOTLVER ? "BLDR" : "APP");

	if (le16_to_cpu(ack.hdr.command) == BEO_HH1_REP_ACK) {
		*valid = 1;
		memcpy(ver, &ack, sizeof(ack));
		dev_info(priv->dev, "Version of %s: type %d, %d.%d.%d, %s %s\n", module,
					ack.binaryType, ack.major, ack.minor, ack.build, ack.buildDate, ack.buildTime);
		return 0;
	}
	else {
		dev_warn(priv->dev, "Fetching %s version failed\n", module);
		*valid = 0;
		return 1;
	}
}

static int beo_hh1_fwupdate_load_hw_version(struct beo_hh1_fwupdate_priv *priv, u16 cmd, int *valid, struct beo_hh1_rsp_hw_version *ver)
{
	// send get_fwversion to FEP
	struct beo_hh1_msg_version *msg;
	struct beo_hh1_rsp_hw_version ack;

	msg = kzalloc(sizeof(struct beo_hh1_msg_version), GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(cmd);
		msg->hdr.len = cpu_to_le16(sizeof(*msg));
		msg->outlen.outlen = cpu_to_le16(sizeof(ack));
		msg->outlen.pad = 0;

		priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);
	}
	else {
		return -ENOMEM;
	}

	if (le16_to_cpu(ack.hdr.command) == BEO_HH1_REP_ACK) {
		*valid = 1;
		memcpy(ver, &ack, sizeof(ack));
		dev_info(priv->dev, "Version of HW: revision %c, \n", ack.revision);
		dev_info(priv->dev, "Version of HW: pcbVariant %d, \n", ack.pcbVariant);
		dev_info(priv->dev, "Version of HW: productVariant %d, \n", ack.productVariant);
		dev_info(priv->dev, "Version of HW: productId %d, \n", ack.productId);

		return 0;
	}
	else {
		dev_warn(priv->dev, "Fetching HW version failed\n");
		*valid = 0;
		return 1;
	}
}


static int beo_hh1_fwupdate_upload(struct beo_hh1_fwupdate_priv *priv)
{
	struct beo_hh1_msg_fwupdate_start *msg;
	struct beo_hh1_rsp_ack ack;

	if (priv->upload_fw == NULL) {
		if (request_firmware(&priv->upload_fw, priv->firmware_name, priv->dev) == 0) {
			dev_info(priv->dev, "Loaded FW: %d bytes\n", priv->upload_fw->size);
		}
		else {
			dev_err(priv->dev, "Failed to load FW image!\n");
			return -ENODEV;
		}
	}

	dev_info(priv->dev, "Starting FW upload (%d bytes)...\n", priv->upload_fw->size);

	priv->upload_rebooting_cnt = 0;
	priv->upload_starting_cnt = 0;
	priv->upload_size = priv->upload_fw->size;
	priv->upload_done = 0;
	priv->upload_pending_ack = 0;
	priv->upload_pending_seq = 0;
	priv->upload_lost_block_cnt = 0;

	// send start to FEP
	msg = kzalloc(sizeof(struct beo_hh1_msg_fwupdate_start), GFP_KERNEL);
	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_FWUP_START);
		msg->hdr.len = cpu_to_le16(sizeof(*msg));
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;
		msg->imglen = cpu_to_le32(priv->upload_fw->size);

		priv->state = state_rebooting;
		priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);
	}
	else {
		return -ENOMEM;
	}

#ifdef DEBUG_COMMANDS
// fake ack
ack.command = cpu_to_le16(BEO_HH1_REP_ACK);
#endif

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(priv->dev, "Error: FWUP_START not acked\n");
		priv->state = state_error;
		return -ENODEV;
	}

	// poll while FEP reboots into bootloader
	schedule_delayed_work(&priv->upload_work, 0);

	// in a loop:
	//   send block of data
	//   wait for confirmation event

	// ? start the FEP / wait for FEP running

	return 0;
}

static void beo_hh1_fwupdate_work_func(struct work_struct *work)
{
	struct delayed_work *dwork = container_of(work, struct delayed_work, work);
	struct beo_hh1_fwupdate_priv *priv = container_of(dwork, struct beo_hh1_fwupdate_priv, upload_work);

	mutex_lock(&priv->fw_mutex);

#ifdef DEBUG_VERBOSE
	dev_info(priv->dev, "beo_hh1_fwupdate_work_func(state=%d)\n", priv->state);
#endif

	if (priv->state == state_idle) {
		if (priv->ver_app_valid == 0) {
			beo_hh1_fwupdate_load_version(priv, BEO_HH1_CMD_GETAPPVER, &(priv->ver_app_valid), &(priv->ver_app));
			beo_hh1_fwupdate_load_hw_version(priv, BEO_HH1_CMD_GETHWVER, &(priv->ver_hw_valid), &(priv->ver_hw));
		}
		else {
			dev_info(priv->dev, "idle - no work to be done\n");
		}
		goto out_done;
	}

	if (priv->state == state_rebooting) {
		dev_info(priv->dev, "rebooting - waiting for ready-event\n");
		priv->upload_rebooting_cnt++;

		if (priv->upload_rebooting_cnt > FWUPDATE_TIMEOUT_REBOOTING) {
			dev_err(priv->dev, "FEP timeout waiting for bootloader!\n");
			priv->state = state_error;
			goto out_done;
		}

		goto out_reschedule;
	}

	if (priv->state == state_uploading) {
		if (priv->upload_pending_ack != 0) {
			// waiting for block ack
#ifdef DEBUG_VERBOSE
			dev_info(priv->dev, "... wait for data ack\n");
#endif

			priv->upload_lost_block_cnt++;
			if (priv->upload_lost_block_cnt > 1) {
				dev_info(priv->dev, "... re-sending block @%d, %d since not acked\n", priv->upload_done, priv->upload_pending_seq+1);
				priv->upload_pending_ack = 0;
				priv->upload_lost_block_cnt = 0;
			}
			else {
				goto out_reschedule;
			}
		}

		if (priv->upload_done < priv->upload_size) {
#ifdef DEBUG_VERBOSE
			dev_info(priv->dev, "Sending chunk @%d, %d..\n", priv->upload_done, priv->upload_pending_seq+1);
#endif

			struct beo_hh1_msg_fwupdate_data *msg;
			struct beo_hh1_rsp_ack ack;
			int data_len;

			msg = kzalloc(sizeof(struct beo_hh1_msg_fwupdate_data), GFP_KERNEL);
			if (msg) {
				msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_FWUP_DATA);
				msg->hdr.len = cpu_to_le16(sizeof(*msg));
				msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
				msg->outlen.pad = 0;
				msg->imglen = cpu_to_le32(priv->upload_size);
				msg->offset = cpu_to_le32(priv->upload_done);

				priv->upload_pending_seq++;
				msg->seq = cpu_to_le16(priv->upload_pending_seq & 0xffff);

				data_len = priv->upload_size - priv->upload_done;
				if (data_len > sizeof(msg->data))
					data_len = sizeof(msg->data);
				memcpy(msg->data, priv->upload_fw->data + priv->upload_done, data_len);

				msg->crc = cpu_to_le16(priv->fepcore->crc16(msg->data, sizeof(msg->data)));

				priv->upload_pending_ack = priv->upload_done + data_len;

				priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
				kfree(msg);
			}
			else {
				dev_err(priv->dev, "Unable to allocate data buffer\n");
				// TODO: report error to caller
				goto out_error;
			}

#ifdef DEBUG_COMMANDS
// fake ack
ack.command = cpu_to_le16(BEO_HH1_REP_ACK);
#endif

			if (le16_to_cpu(ack.command) == BEO_HH1_REP_ACK) {
				// wait for confirmation event
			}
			else {
				// retry block
				priv->upload_pending_ack = 0;
				dev_err(priv->dev, "FEP rejected data chunk @%d\n", priv->upload_done);
			}
		}
		else {
			dev_info(priv->dev, "FEP FW upload finished\n");
			priv->state = state_starting;

			// wait for fep-application-up event, or timeout
			goto out_reschedule;
		}
	}

	// wait for app-up event
	if (priv->state == state_starting) {
		dev_info(priv->dev, "starting - waiting for ready-event\n");
		priv->upload_starting_cnt++;

		if (priv->upload_starting_cnt > FWUPDATE_TIMEOUT_STARTING) {
			dev_err(priv->dev, "FEP timeout waiting for up after update!\n");
			priv->state = state_error;
			goto out_done;
		}

		goto out_reschedule;
	}


out_reschedule:
	schedule_delayed_work(&priv->upload_work, HZ/1);
	goto out_done;

out_error:
	dev_err(priv->dev, "Error sending FW - cleanup\n");

	// release upload_fw, cleanup state, ..
	release_firmware(priv->upload_fw);

	priv->state = state_error;
	priv->upload_fw = NULL;
	priv->upload_done = 0;
	priv->upload_pending_ack = 0;
	priv->upload_pending_seq = 0;
	priv->upload_size = 0;

out_done:
	mutex_unlock(&priv->fw_mutex);
}

static int beo_hh1_fwupdate_event_cb(void *_priv, u16 event, u16 data)
{
	struct beo_hh1_fwupdate_priv *priv = _priv;

	mutex_lock(&priv->fw_mutex);

#ifdef DEBUG_VERBOSE
	dev_info(priv->dev, "event: %04x %04x\n", event, data);
#endif

	// fep ready in bootloader mode
	if (event == BEO_HH1_EV_FWUP_READY && priv->state == state_rebooting) {
		dev_info(priv->dev, "FEP in bootloader mode\n");
		priv->state = state_uploading;

		cancel_delayed_work(&priv->upload_work);

		led_trigger_event(ledtrig_hh1_fwup, 0x22);
		goto out_reschedule_0;
	}

	// data written ack
	if (event == BEO_HH1_EV_FWUP_ACK) {
		if (priv->upload_pending_ack != 0) {
			if (data == (priv->upload_pending_seq & 0xffff)) {
#ifdef DEBUG_VERBOSE
				dev_info(priv->dev, "Data ack event (seq %d)\n", data);
#endif
				cancel_delayed_work(&priv->upload_work);

				priv->upload_done = priv->upload_pending_ack;
				priv->upload_pending_ack = 0;

				goto out_reschedule_0;
			}
			else {
				dev_err(priv->dev, "Invalid data ack event (seq %d, expecting %d)\n", data, priv->upload_pending_seq);
			}
		}
	}

	if (event == BEO_HH1_EV_APP_READY && priv->state == state_starting) {
		dev_info(priv->dev, "FEP in application mode\n");
		priv->state = state_idle;

		cancel_delayed_work(&priv->upload_work);
		if (priv->compl_needed) {
			priv->compl_needed = 0;
			complete(&priv->compl);
		}

		led_trigger_event(ledtrig_hh1_fwup, 0x22);

		// force version re-fetch
		priv->ver_app_valid = 0;
		goto out_reschedule_0;
	}

	mutex_unlock(&priv->fw_mutex);
	return 0;

out_reschedule_0:
	mutex_unlock(&priv->fw_mutex);
	schedule_delayed_work(&priv->upload_work, 0);
	return 0;
}



static ssize_t beo_hh1_fwupdate_status_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_fwupdate_priv *priv = platform_get_drvdata(pdev);
	int ret = -EINVAL;

	if (len >= 5 && strncmp(buf, "start", 5) == 0) {
		int err;
		dev_info(dev, "Starting FEP FW update...\n");

		mutex_lock(&priv->fw_mutex);

		err = beo_hh1_fwupdate_upload(priv);
		if (err) {
			dev_err(dev, "Error uploading FW! err=%d\n", err);
		}

		mutex_unlock(&priv->fw_mutex);

		ret = len;
	}

#ifdef DEBUG_COMMANDS
// fake bootloader ready
	if (len >= 5 && strncmp(buf, "ready", 5) == 0) {
		beo_hh1_fwupdate_event_cb(priv, BEO_HH1_EV_FWUP_READY, 0x0000);
		ret = len;
	}

// fake data ack
	if (len >= 4 && strncmp(buf, "ack", 3) == 0) {
		int seq = 0;
		sscanf(buf+3, "%d", &seq);
		beo_hh1_fwupdate_event_cb(priv, BEO_HH1_EV_FWUP_ACK, seq);
		ret = len;
	}
#endif

	return ret;
}

static ssize_t beo_hh1_fwupdate_status_show(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_fwupdate_priv *priv = platform_get_drvdata(pdev);
	int ret = 0;

	// first line: fwupdate status
	switch (priv->state) {
	case state_boot:
		ret = snprintf(buf, PAGE_SIZE, "boot\n");
		break;
	case state_idle:
		ret = snprintf(buf, PAGE_SIZE, "idle\n");
		break;
	case state_rebooting:
		ret = snprintf(buf, PAGE_SIZE, "rebooting\n");
		break;
	case state_uploading:
		ret = snprintf(buf, PAGE_SIZE, "uploading %d/%d, awaiting seq %d\n", priv->upload_done, priv->upload_size, priv->upload_pending_seq);
		break;
	case state_starting:
		ret = snprintf(buf, PAGE_SIZE, "starting\n");
		break;
	case state_error:
		ret = snprintf(buf, PAGE_SIZE, "error\n");
		break;
	default:
		ret = snprintf(buf, PAGE_SIZE, "unknown\n");
	}

	// 2nd, 3rd lines: bldr/app versions
	if (priv->ver_bldr_valid)
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "BLDR: %d.%d.%d, %s %s\n",
					priv->ver_bldr.major, priv->ver_bldr.minor, priv->ver_bldr.build, priv->ver_bldr.buildDate, priv->ver_bldr.buildTime);
	else
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "BLDR: unknown\n");

	if (priv->ver_app_valid)
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "APP: %d.%d.%d, %s %s\n",
					priv->ver_app.major, priv->ver_app.minor, priv->ver_app.build, priv->ver_app.buildDate, priv->ver_app.buildTime);
	else
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "APP: unknown\n");

	if (priv->ver_hw_valid) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "HW: %c\n", priv->ver_hw.revision);
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "pcbVariant: %d\n", priv->ver_hw.pcbVariant);
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "productVariant: %d\n", priv->ver_hw.productVariant);
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "productId: %d\n", priv->ver_hw.productId);
	} else
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "HW: unknown\n");

	return ret;
}


static DEVICE_ATTR(status, S_IRUGO | S_IWUSR,
			beo_hh1_fwupdate_status_show,
			beo_hh1_fwupdate_status_store);


static int beo_hh1_fwupdate_probe(struct platform_device *pdev)
{
	struct beo_hh1_fwupdate_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_fwupdate_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->pdev = pdev;
	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	/* get firmware name from DT bindings */
	if (np) {
		priv->firmware_name = of_get_property(np, "fw_name", NULL);
	} else {
		dev_err(&pdev->dev, "Cannot get firmware filename from devicetreee.");
	}

	ret = device_create_file(&pdev->dev,  &dev_attr_status);
	if (ret) {
		goto out_free_priv;
	}


	INIT_DELAYED_WORK(&priv->upload_work, beo_hh1_fwupdate_work_func);
	mutex_init(&priv->fw_mutex);

	mfd_cell_enable(pdev);

	priv->state = state_boot;
	priv->fepcore->register_events(pdev, 0x1000, 0x0010, priv, beo_hh1_fwupdate_event_cb);

	platform_set_drvdata(pdev, priv);
	led_trigger_register_simple("hh1-fwup", &ledtrig_hh1_fwup);

	dev_info(&pdev->dev, "Initialized.\n");

	beo_hh1_fwupdate_load_version(priv, BEO_HH1_CMD_GETBOOTLVER, &(priv->ver_bldr_valid), &(priv->ver_bldr));

	// if FEP is in bootloader mode we need to flash the app right now (and block the boot process)
	if (beo_hh1_fwupdate_check_fepapp(priv) != 0) {
		dev_err(priv->dev, "FEP is not in app mode - force update NOW!\n");

		// init completion structure
		init_completion(&priv->compl);
		priv->compl_needed = 1;

		if (beo_hh1_fwupdate_upload(priv) == 0) {
			// wait until fw update is done
			if (wait_for_completion_killable_timeout(&priv->compl, HZ * 90) == 0) {
				dev_err(priv->dev, "Timeout waiting for FEP update!\n");
			}
			else {
				dev_info(priv->dev, "FEP update finished\n");
				priv->state = state_idle;
			}
		}
		else {
			dev_err(priv->dev, "Unable to start FEP update!\n");
		}
	}
	else {
		beo_hh1_fwupdate_load_version(priv, BEO_HH1_CMD_GETAPPVER, &(priv->ver_app_valid), &(priv->ver_app));
		beo_hh1_fwupdate_load_hw_version(priv, BEO_HH1_CMD_GETHWVER, &(priv->ver_hw_valid), &(priv->ver_hw));
	}

	return 0;

out_free_priv:
	return ret;
}

static int beo_hh1_fwupdate_remove(struct platform_device *pdev)
{
	struct beo_hh1_fwupdate_priv *priv = platform_get_drvdata(pdev);

	mfd_cell_disable(pdev);

	led_trigger_unregister_simple(ledtrig_hh1_fwup);

	cancel_delayed_work(&priv->upload_work);
	device_remove_file(&pdev->dev, &dev_attr_status);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_fwupdate_driver = {
	.probe		= beo_hh1_fwupdate_probe,
	.remove		= beo_hh1_fwupdate_remove,
	.driver		= {
		.name	= "beo-hh1-fwupdate",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_fwupdate_match),
	},
};

module_platform_driver(beo_hh1_fwupdate_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP FW update driver");
MODULE_LICENSE("GPL v2");
