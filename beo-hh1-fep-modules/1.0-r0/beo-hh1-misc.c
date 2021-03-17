/*
 * Driver for miscellaneous commands for BeO HH1 FEP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"

#define DEVICE_NAME "hh1_misc"

struct beo_hh1_misc_priv {
	struct beo_hh1_fep_core *fepcore;
	struct device *dev;
};

static int beo_hh1_misc_short_cb(void *_priv, u16 event, u16 data)
{
	if (event == BEO_EV_WDT_TICK) {
#ifdef DEBUG_VERBOSE
		struct beo_hh1_misc_priv *priv = _priv;
		dev_info(priv->dev, "Watchdog tick\n");
#endif
	}
	return 0;
}

static ssize_t beo_hh1_linein_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_rsp_ack ack;
	int msglen = sizeof(struct beo_hh1_msg_lineinsense);
	struct beo_hh1_msg_lineinsense *msg;
	int ret = -EINVAL;

	if(len != sizeof(struct beo_hh1_msg_lineinsetting))
	{
		dev_err(dev, "Incorrect input size: %d\n", len);
		ret = -EINVAL;
		goto out;
	}

	// send command/data
	msg = kzalloc(msglen, GFP_KERNEL);

	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_LINESENSE);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;
		memcpy(&msg->setting, buf, len);

		priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);

		//ack.command = BEO_HH1_REP_ACK;
		if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
			dev_err(dev, "No ACK!\n");
			ret = -ENODEV;
			goto out;
		}
		ret = len;
	}
	else {
		ret = -ENOMEM;
	}
out:
return ret;
}

static ssize_t beo_hh1_linein_show(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	int ret = 0;
	return ret;
}

static DEVICE_ATTR(linein, S_IRUGO | S_IWUSR,
			beo_hh1_linein_show,
			beo_hh1_linein_store);


static ssize_t beo_ase_critical_error_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg *msg;
	int msglen = sizeof(*msg);
	int ret = -EINVAL;

	// buf argument is ignored
	// send command/data
	msg = kzalloc(msglen, GFP_KERNEL);

	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_ASE_CMD_CRITICALERR);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;

		priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);

		//ack.command = BEO_HH1_REP_ACK;
		if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
			dev_err(dev, "No ACK!\n");
			ret = -ENODEV;
			goto out;
		}
		ret = len;
	}
	else {
		ret = -ENOMEM;
	}
out:
return ret;
}

static DEVICE_ATTR(critical_error, S_IWUSR,
			NULL,
			beo_ase_critical_error_store);

static ssize_t beo_hh1_fepshell_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_rsp_ack ack;
	int msglen = sizeof(struct beo_hh1_msg_fep_shell_cmd);
	struct beo_hh1_msg_fep_shell_cmd *msg;
	int ret = -EINVAL;

	if (len >= FEP_SHELL_MAX_CMD_SIZE) {
		dev_err(dev, "Too long shell command, ignoring\n");
		return -EINVAL;
	}

	// send command/data
	msg = kzalloc(msglen, GFP_KERNEL);
	if (!msg) {
		dev_err(dev, "Cannot allocate buffer\n");
		return -ENOMEM;
	}

	msg->hdr.command = cpu_to_le16(BEO_ASE_CMD_FEP_SHELL_CMD);
	msg->hdr.len = cpu_to_le16(msglen);
	msg->outlen.outlen = cpu_to_le16(sizeof(struct beo_hh1_rsp_ack));
	msg->outlen.pad = 0;
	memcpy(&msg->cmd_str, buf, len);
	msg->cmd_str[len-1] = 0; // replacing newline with 0 (if coming from echo)

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
	kfree(msg);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	}
	ret = len;
out:
	return ret;
}

static DEVICE_ATTR(fepshell, S_IWUSR, NULL, beo_hh1_fepshell_store);

static ssize_t beo_hh1_low_power_time_show(struct device *dev,
						struct device_attribute *attr,
						char *buf)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_msg_low_power_time *msg;
	struct beo_hh1_rsp_lowpowertm ack;

	msg = kzalloc(sizeof(struct beo_hh1_msg_low_power_time), GFP_KERNEL);
	if (!msg) {
		return -ENOMEM;
	}

	msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_GETLOPWRTM);
	msg->hdr.len = cpu_to_le16(sizeof(*msg));
	msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_lowpowertm));
	msg->outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
	kfree(msg);

	if (le16_to_cpu(ack.hdr.command) != BEO_HH1_REP_ACK) {
		dev_warn(dev, "No ACK!\n");
		return -ENODEV;
	}

	return snprintf(buf, PAGE_SIZE, "%u\n", le32_to_cpu(ack.minutes));
}

static DEVICE_ATTR(low_power_time, S_IRUGO, beo_hh1_low_power_time_show, NULL);

static ssize_t beo_hh1_boot_reason_show(struct device *dev,
								   struct device_attribute *attr,
								   char *buf)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_msg_boot_reason *msg;
	struct beo_hh1_rsp_bootreason ack;

	msg = kzalloc(sizeof(struct beo_hh1_msg_boot_reason), GFP_KERNEL);
	if (!msg) {
		return -ENOMEM;
	}

	msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_GETBOOTREASON);
	msg->hdr.len = cpu_to_le16(sizeof(*msg));
	msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_bootreason));
	msg->outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, sizeof(*msg), (u8 *)msg, sizeof(ack), (u8 *)&ack);
	kfree(msg);

	if (le16_to_cpu(ack.hdr.command) != BEO_HH1_REP_ACK) {
		dev_warn(dev, "No ACK!\n");
		return -ENODEV;
	}

	switch (le32_to_cpu(ack.bootReason)) {
	case 0:
		return snprintf(buf, PAGE_SIZE, "PowerFail\n");
	case 1:
		return snprintf(buf, PAGE_SIZE, "PowerOn\n");
	case 2:
		return snprintf(buf, PAGE_SIZE, "AfterFWUpdate\n");
	case 3:
		return snprintf(buf, PAGE_SIZE, "FEPCrash\n");
	default:
		return snprintf(buf, PAGE_SIZE, "Unknown\n");
	}

	return 0;
}

static DEVICE_ATTR(boot_reason, S_IRUGO,
				   beo_hh1_boot_reason_show,
				   NULL);

static ssize_t beo_ase_speaker_position_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_misc_priv *priv = platform_get_drvdata(pdev);
	struct beo_ase_msg_speaker_position position_msg;
	struct beo_ase_dsp_ack ack;
	u8 position = 0;
	if (sscanf(buf, "%hhu", &position) != 1 || position > 2) {
		dev_err(dev, "Incorrect position value: '%s'", buf);
		return -EINVAL;
	}

	position_msg.hdr.command = cpu_to_le16(BEO_ASE_CMD_SPEAKER_POSITION);
	position_msg.hdr.len = cpu_to_le16(sizeof(position_msg));
	position_msg.outlen.outlen = cpu_to_le16(sizeof(struct beo_hh1_rsp_ack));
	position_msg.outlen.pad = 0;
	position_msg.speaker_position = position;

	priv->fepcore->send(priv->fepcore, sizeof(position_msg), (u8 *)&position_msg, sizeof(ack), (u8 *)&ack);
	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		return -ENODEV;
	}
	return len;
};

static DEVICE_ATTR(speaker_position, S_IWUSR,
					NULL,
					beo_ase_speaker_position_store);

static const struct of_device_id of_beo_hh1_misc_match[] = {
	{ .compatible = "beo-hh1-misc", },
	{},
};


static int beo_hh1_misc_probe(struct platform_device *pdev)
{
	struct beo_hh1_misc_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	int ret = 0;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_misc_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	ret = device_create_file(&pdev->dev,  &dev_attr_linein);
	ret |= device_create_file(&pdev->dev,  &dev_attr_critical_error);
	ret |= device_create_file(&pdev->dev,  &dev_attr_boot_reason);
	ret |= device_create_file(&pdev->dev,  &dev_attr_fepshell);
	ret |= device_create_file(&pdev->dev,  &dev_attr_speaker_position);
	ret |= device_create_file(&pdev->dev,  &dev_attr_low_power_time);

	if (ret) {
		dev_err(&pdev->dev, "Can not create misc files\n");
		device_remove_file(&pdev->dev, &dev_attr_linein);
		device_remove_file(&pdev->dev, &dev_attr_critical_error);
		device_remove_file(&pdev->dev, &dev_attr_boot_reason);
		device_remove_file(&pdev->dev, &dev_attr_fepshell);
		device_remove_file(&pdev->dev, &dev_attr_speaker_position);
		device_remove_file(&pdev->dev, &dev_attr_low_power_time);
		goto out_free_priv;
	}

	mfd_cell_enable(pdev);

	priv->fepcore->register_events(pdev, BEO_EV_WDT_TICK, 1, priv, beo_hh1_misc_short_cb);

	platform_set_drvdata(pdev, priv);

	dev_info(&pdev->dev, "Initialized.\n");

	return 0;

out_free_priv:

	return ret;
}

static int beo_hh1_misc_remove(struct platform_device *pdev)
{
	device_remove_file(&pdev->dev, &dev_attr_linein);
	device_remove_file(&pdev->dev, &dev_attr_critical_error);
	device_remove_file(&pdev->dev, &dev_attr_boot_reason);
	device_remove_file(&pdev->dev, &dev_attr_fepshell);
	device_remove_file(&pdev->dev, &dev_attr_speaker_position);
	device_remove_file(&pdev->dev, &dev_attr_low_power_time);

	mfd_cell_disable(pdev);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_misc_driver = {
	.probe		= beo_hh1_misc_probe,
	.remove		= beo_hh1_misc_remove,
	.driver		= {
		.name	= "beo-hh1-misc",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_misc_match),
	},
};

module_platform_driver(beo_hh1_misc_driver);

MODULE_AUTHOR("Marek Kuban <marek.kuban@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP Miscellaneous commands driver");
MODULE_LICENSE("GPL v2");
