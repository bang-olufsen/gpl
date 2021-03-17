/*
 * Display driver for BeO S47 FEP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Driver used for get/set touch calibration and ambient sensor
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/mfd/core.h>


#include "beo-hh1-fep.h"

#define DEVICE_NAME "ase_s47_misc"

#define FIFO_SIZE 100

struct beo_ase_s47_misc_priv {
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;
	struct cdev cdev;

	int open_count;

	struct kfifo data;
	wait_queue_head_t read_wq;
};

static dev_t major_number;
static struct device *device_node;
static struct class *misc_class;

static struct beo_ase_s47_misc_priv *device_to_private(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	return platform_get_drvdata(pdev);
}

static int beo_ase_s47_misc_open(struct inode *inode, struct file *file)
{
	struct beo_ase_s47_misc_priv *priv = container_of(inode->i_cdev, struct beo_ase_s47_misc_priv, cdev);
	if (!priv)
		return -EINVAL;

	file->private_data = priv;

	if (priv->open_count == 0)
		priv->open_count ++;
	else {
		dev_err(priv->dev, "Device is already opened.\n");
		return -EBUSY;
	}

	return nonseekable_open(inode, file);
}

static int beo_ase_s47_misc_release(struct inode *inode, struct file *file)
{
	struct beo_ase_s47_misc_priv *priv = container_of(inode->i_cdev, struct beo_ase_s47_misc_priv, cdev);
	if (!priv)
		return -EINVAL;

	priv->open_count--;

	return 0;
}

static ssize_t beo_ase_s47_misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct beo_ase_s47_misc_priv *priv = file->private_data;
	int ret, size;

	ret = kfifo_to_user(&priv->data, buf, count, &size);
	if (ret != 0)
		return ret;
	
	return size;
}

static unsigned int beo_ase_s47_poll(struct file *file, poll_table *wait)
{
	struct beo_ase_s47_misc_priv *priv = file->private_data;
	unsigned int mask = 0;

	if (!priv)
		return -EINVAL;

	poll_wait(file, &priv->read_wq, wait);
	if (!kfifo_is_empty(&priv->data))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static const struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.open = beo_ase_s47_misc_open,
	.release = beo_ase_s47_misc_release,
	.read = beo_ase_s47_misc_read,
	.poll = beo_ase_s47_poll,
};

static int beo_ase_s47_misc_ev_short_cb(void *_priv, u16 _event, u32 size, u8 *data)
{
	struct beo_ase_s47_misc_priv *priv = _priv;

#ifdef DEBUG_VERBOSE
	dev_info(priv->dev, "Received event:%x\b", _event);
#endif
	kfifo_in(&priv->data, &_event, sizeof(u32));
	kfifo_in(&priv->data, &size, sizeof(u32));
	kfifo_in(&priv->data, data, size);
	wake_up_interruptible(&priv->read_wq);

	return 0;
}

static ssize_t beo_ase_s47_misc_touch_calibrate_zero(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg msg;
	int msglen = sizeof(msg);
	int ret;

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_TOUCH_CALIBRATE_ZERO);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_touch_calibrate_load(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_ase_msg_s47touch_calibrate_with_load msg;
	int msglen = sizeof(msg);
	int ret, weight;

	if (sscanf(buf, "%i", &weight) != 1) {
		dev_err(dev, "Incorrect brate with load command: '%s'", buf);
		return -EINVAL;
	}

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_TOUCH_CALIBRATE_WITHLOAD);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;
	msg.weight = weight;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_touch_calibrate_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg msg;
	int msglen = sizeof(msg);
	int ret;

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_TOUCH_GET_CALIBRATE_DATA);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_touch_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg msg;
	int msglen = sizeof(msg);
	int ret;

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_TOUCH_GET_DATA);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_light_calibrate(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_ase_msg_s47lightsensor_calibrate msg;
	int msglen = sizeof(msg);
	int ret, sensorId, appliedLux;

	if (sscanf(buf, "%i;%i", &sensorId, &appliedLux) != 2) {
		dev_err(dev, "Incorrect sensor and lux: '%s'", buf);
		return -EINVAL;
	}

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_LIGHT_SENSOR_CALIBRATE);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;
	msg.sensorId = sensorId;
	msg.appliedLux = appliedLux;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_light_calibrate_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg msg;
	int msglen = sizeof(msg);
	int ret;

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_LIGHT_SENSOR_CALIBRATE_GET_DATA);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static ssize_t beo_ase_s47_misc_light_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_misc_priv *priv = device_to_private(dev);
	struct beo_hh1_rsp_ack ack;
	struct beo_cmd_msg msg;
	int msglen = sizeof(msg);
	int ret;

	msg.hdr.command = cpu_to_le16(BEO_S47_CMD_LIGHT_SENSOR_GET_DATA);
	msg.hdr.len = cpu_to_le16(msglen);
	msg.outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, msglen, (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	} else {
		ret = len;
	}

out:
	return ret;
}

static DEVICE_ATTR(touch_calibrate_zero, S_IWUSR, NULL, beo_ase_s47_misc_touch_calibrate_zero);
static DEVICE_ATTR(touch_calibrate_load, S_IWUSR, NULL, beo_ase_s47_misc_touch_calibrate_load);
static DEVICE_ATTR(touch_get_calibrate_data, S_IWUSR, NULL, beo_ase_s47_misc_touch_calibrate_data);
static DEVICE_ATTR(touch_get_data, S_IWUSR, NULL, beo_ase_s47_misc_touch_data);

static DEVICE_ATTR(light_calibrate, S_IWUSR, NULL, beo_ase_s47_misc_light_calibrate);
static DEVICE_ATTR(light_get_calibrate_data, S_IWUSR, NULL, beo_ase_s47_misc_light_calibrate_data);
static DEVICE_ATTR(light_get_data, S_IWUSR, NULL, beo_ase_s47_misc_light_data);

static int beo_ase_s47_misc_probe(struct platform_device *pdev)
{
	int ret;
	struct beo_ase_s47_misc_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;

	if (!fepcore) {
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_ase_s47_misc_priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	ret = kfifo_alloc(&priv->data, FIFO_SIZE, GFP_KERNEL);
	if (ret) {
		return ret;
	}

	init_waitqueue_head(&priv->read_wq);

	// register sysfs
	ret =  device_create_file(priv->dev, &dev_attr_touch_calibrate_zero);
	ret |=  device_create_file(priv->dev, &dev_attr_touch_calibrate_load);
	ret |=  device_create_file(priv->dev, &dev_attr_touch_get_calibrate_data);
	ret |=  device_create_file(priv->dev, &dev_attr_touch_get_data);
	ret |=  device_create_file(priv->dev, &dev_attr_light_calibrate);
	ret |=  device_create_file(priv->dev, &dev_attr_light_get_calibrate_data);
	ret |=  device_create_file(priv->dev, &dev_attr_light_get_data);
	
	ret = alloc_chrdev_region(&major_number, 0, 1, DEVICE_NAME);
	if (ret) {
		dev_err(priv->dev, "alloc_chrdev_region() failed %d\n", ret);
		return ret;
	}

	// create device class
	misc_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(misc_class)) {
		ret = PTR_ERR(misc_class);
		dev_err(priv->dev, "class_create() failed %d\n", ret);
		goto unregister_chardev;
	}

	// init char device
	cdev_init(&priv->cdev, &file_ops);
	priv->cdev.owner = THIS_MODULE;

	// add it to the system
	ret = cdev_add(&priv->cdev, major_number, 1);
	if (ret) {
		dev_err(priv->dev, "cdev_add() failed %d\n", ret);
		goto destroy_class;
	}

	// add corresponding device node
	device_node = device_create(misc_class, priv->dev, major_number, NULL, DEVICE_NAME);
	if (IS_ERR(device_node)) {
		 ret = PTR_ERR(device_node);
		 dev_err(priv->dev, "device_create() failed %d\n", ret);
		 goto clean_cdev;
	}

	if (!ret) {
		mfd_cell_enable(pdev);

		priv->fepcore->register_long_events(pdev, BEO_S47_EV_TOUCH_CALIBRATE_STATUS, 7, priv, beo_ase_s47_misc_ev_short_cb);
		platform_set_drvdata(pdev, priv);
	}

	
	dev_info(priv->dev, "Initialized.\n");
	return 0;

clean_cdev:
	cdev_del(&priv->cdev);
destroy_class:
	class_destroy(misc_class);
unregister_chardev:
	unregister_chrdev_region(major_number, 1);

	return ret;
}

static int beo_ase_s47_misc_remove(struct platform_device *pdev)
{
	struct beo_ase_s47_misc_priv *priv = platform_get_drvdata(pdev);

	device_remove_file(priv->dev, &dev_attr_touch_calibrate_zero);
	device_remove_file(priv->dev, &dev_attr_touch_calibrate_load);
	device_remove_file(priv->dev, &dev_attr_touch_get_calibrate_data);
	device_remove_file(priv->dev, &dev_attr_touch_get_data);
	device_remove_file(priv->dev, &dev_attr_light_calibrate);
	device_remove_file(priv->dev, &dev_attr_light_get_calibrate_data);
	device_remove_file(priv->dev, &dev_attr_light_get_data);


	device_destroy(misc_class, major_number);
	cdev_del(&priv->cdev);
	class_destroy(misc_class);
	unregister_chrdev_region(major_number, 1);

	return 0;
}

static const struct of_device_id of_beo_ase_s47_misc_match[] = {
	{ .compatible = "beo,beo-ase-s47-misc" },
	{},
};

static struct platform_driver beo_ase_s47_misc_driver = {
	.probe		= beo_ase_s47_misc_probe,
	.remove		= beo_ase_s47_misc_remove,
	.driver		= {
		.name	= "beo-ase-s47-misc",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_ase_s47_misc_match),
	},
};

module_platform_driver(beo_ase_s47_misc_driver);

MODULE_AUTHOR("Marek Belisko <marek.belisko@streamunlimited.com>");
MODULE_DESCRIPTION("BeO S47 misc driver");
MODULE_LICENSE("GPL v2");
