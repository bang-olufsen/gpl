/*
 * Low Speed Link driver for BeO HH1 FEP
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

#define DEVICE_NAME "hh1_lsl"

extern int (*beo_pl_mute_enable_func)(unsigned int);
static struct beo_hh1_lsl_priv *beo_pl_mute_enable_func_priv = NULL;

struct beo_hh1_lsl_priv {
	struct beo_hh1_fep_core *fepcore;
	struct device *dev;
	struct delayed_work pl_unmute_work;
};

static ssize_t beo_hh1_lsl_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_lsl_priv *priv = platform_get_drvdata(pdev);
	struct beo_hh1_rsp_ack ack;
	int msglen = sizeof(struct beo_hh1_msg_pl_data);
	struct beo_hh1_msg_pl_data *msg;
	/* we send 2 more bytes as subcommand to update powerlink pin */
	int tlg_size = LSL_TELEGRAM_SIZE + 2;
	int ret = -EINVAL;

	if (len != tlg_size) {
		dev_err(dev, "Incorrect lsl telegram size: %d\n", len);
		ret = -EINVAL;
		goto out;
	}

	// send command/data
	msg = kzalloc(msglen, GFP_KERNEL);

	if (msg) {
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_PL_DATA);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;
		memcpy(&msg->plcmd, buf, 2);

		memcpy(&msg->data, buf + 2, len - 2);

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

static ssize_t beo_hh1_lsl_show(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	int ret = 0;
	return ret;
}

static int beo_pl_send_mute(struct beo_hh1_lsl_priv *priv, int mute)
{
	struct beo_hh1_rsp_ack ack;
	struct beo_ase_msg_mute msg;
	int msglen = sizeof(struct beo_ase_msg_mute);
	int ret = -EINVAL;

	msg.hdr.command = cpu_to_le16(BEO_HH1_CMD_PL_MUTE);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;
	msg.mute = cpu_to_le16(mute);

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(priv->dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	}
	ret = msglen;

out:
	return ret;
}

static int beo_pl_mute_enable(unsigned int duration_ms)
{
	if (duration_ms && beo_pl_mute_enable_func_priv) {
		beo_pl_send_mute(beo_pl_mute_enable_func_priv, 1);
		schedule_delayed_work(&beo_pl_mute_enable_func_priv->pl_unmute_work, msecs_to_jiffies(duration_ms));
		return 1;
	}
	return 0;
}

static void beo_hh1_lsl_pl_unmute_work_func(struct work_struct *work)
{
	if (beo_pl_mute_enable_func_priv)
		beo_pl_send_mute(beo_pl_mute_enable_func_priv, 0);
}

static ssize_t beo_pl_mute_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_lsl_priv *priv = platform_get_drvdata(pdev);
	int mute;

	if ((sscanf(buf, "%i", &mute) != 1) || (mute != 0 && mute != 1)) {
		dev_err(dev, "Incorrect mute value: '%s'", buf);
		return -EINVAL;
	}

	return beo_pl_send_mute(priv, mute);
}

static DEVICE_ATTR(lsl, S_IRUGO | S_IWUSR,
			beo_hh1_lsl_show,
			beo_hh1_lsl_store);

static DEVICE_ATTR(pl_mute, S_IWUSR,
			NULL,
			beo_pl_mute_store);

static const struct of_device_id of_beo_hh1_lsl_match[] = {
	{ .compatible = "beo-ase-lsl", },
	{},
};

static int beo_hh1_lsl_probe(struct platform_device *pdev)
{
	struct beo_hh1_lsl_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	int ret = 0;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_lsl_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	beo_pl_mute_enable_func_priv = priv;
	beo_pl_mute_enable_func = &beo_pl_mute_enable;

	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	INIT_DELAYED_WORK(&priv->pl_unmute_work, beo_hh1_lsl_pl_unmute_work_func);

	ret = device_create_file(&pdev->dev,  &dev_attr_lsl);
	ret |= device_create_file(&pdev->dev,  &dev_attr_pl_mute);
	if (ret)
		goto out_free_priv;

	mfd_cell_enable(pdev);

	platform_set_drvdata(pdev, priv);

	dev_info(&pdev->dev, "Initialized.\n");

	return 0;

out_free_priv:

	return ret;
}

static int beo_hh1_lsl_remove(struct platform_device *pdev)
{
	beo_pl_mute_enable_func_priv = NULL;
	beo_pl_mute_enable_func = NULL;

	device_remove_file(&pdev->dev, &dev_attr_lsl);

	mfd_cell_disable(pdev);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_lsl_driver = {
	.probe		= beo_hh1_lsl_probe,
	.remove		= beo_hh1_lsl_remove,
	.driver		= {
		.name	= "beo-ase-lsl",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_lsl_match),
	},
};

module_platform_driver(beo_hh1_lsl_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP Low Speed Link driver");
MODULE_LICENSE("GPL v2");
