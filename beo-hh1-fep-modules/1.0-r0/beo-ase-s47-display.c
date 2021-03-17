/*
 * Display driver for BeO S47 FEP
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
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "beo-hh1-fep.h"

#define DEVICE_NAME "ase_s47_display"

struct beo_ase_s47_display_priv {
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;
	struct cdev cdev;

	int open_count;
};

// sync with app
struct ASE_S47_DisplayCommand
{
	uint32_t command_id;
	uint32_t data_length;
	uint8_t command_data[];
};

// sync with app
struct ASE_S47_DisplayFrameData
{
	uint16_t width;
	uint16_t height;
	uint32_t imageLength;
	uint16_t imageId;
	uint8_t format;
	uint8_t padding;
	uint8_t data[];
};

static dev_t major_number;
static struct device *device_node;
static struct class *display_class;

static struct beo_ase_s47_display_priv *device_to_private(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	return platform_get_drvdata(pdev);
}

static int beo_ase_s47_test_mode(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_display_priv *priv = device_to_private(dev);
	struct beo_ase_s47_msg_display_test_mode msg_data;
	struct beo_hh1_rsp_ack ack;

	int response_len = 0;

	u32 enable;

	if (sscanf(buf, "%u", &enable) != 1) {
		dev_err(dev, "Incorrect input format. Allowed: '0'-disable, '1'-enable");
		return -EINVAL;
	}

	msg_data.hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_TEST_MODE);
	msg_data.hdr.len = cpu_to_le16(sizeof(msg_data));
	msg_data.outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_data.outlen.pad = 0;

	msg_data.enable = enable;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg_data), (u8 *)&msg_data, sizeof(ack), (u8 *)&ack);

	return len;
}

static int beo_ase_s47_test_pattern(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_display_priv *priv = device_to_private(dev);
	struct beo_ase_s47_msg_display_test_pattern msg_data;
	struct beo_hh1_rsp_ack ack;

	int response_len = 0;

	u32 pattern;

	if (sscanf(buf, "%u", &pattern) != 1) {
		dev_err(dev, "Incorrect input format.");
		return -EINVAL;
	}

	msg_data.hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_TEST_PATTERN);
	msg_data.hdr.len = cpu_to_le16(sizeof(msg_data));
	msg_data.outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_data.outlen.pad = 0;

	msg_data.pattern = pattern;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg_data), (u8 *)&msg_data, sizeof(ack), (u8 *)&ack);

	return len;
}

static int beo_ase_s47_test_brightness(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_display_priv *priv = device_to_private(dev);
	struct beo_ase_s47_msg_display_test_brightness msg_data;
	struct beo_hh1_rsp_ack ack;

	int response_len = 0;

	u32 master_current_control;
	u32 contrast_current;

	if (sscanf(buf, "%u;%u", &master_current_control, &contrast_current) != 2) {
		dev_err(dev, "Incorrect input format.");
		return -EINVAL;
	}

	msg_data.hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_TEST_BRIGHTNESS);
	msg_data.hdr.len = cpu_to_le16(sizeof(msg_data));
	msg_data.outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_data.outlen.pad = 0;

	msg_data.master_current_control = master_current_control;
	msg_data.contrast_current = contrast_current;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(msg_data), (u8 *)&msg_data, sizeof(ack), (u8 *)&ack);

	return len;
}

static int beo_ase_s47_command(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct beo_ase_s47_display_priv *priv = device_to_private(dev);
	struct beo_ase_s47_msg_display_command *msg_data;
	struct beo_hh1_rsp_ack ack;

	int response_len = 0;

	struct ASE_S47_DisplayCommand *cmd = (struct ASE_S47_DisplayCommand *)buf;
	if (cmd->data_length + sizeof(*cmd) != len) {
		return -EINVAL;
	}

	msg_data = (struct beo_ase_s47_msg_display_command *)kzalloc(sizeof(*msg_data), GFP_KERNEL);

	msg_data->hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_COMMAND);
	msg_data->hdr.len = cpu_to_le16(sizeof(*msg_data));
	msg_data->outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_data->outlen.pad = 0;

	if (cmd->data_length > 0) {
		memcpy(msg_data->command_data, cmd->command_data, cmd->data_length);
	}
	msg_data->command_id = cmd->command_id;
	msg_data->data_length = cmd->data_length;

	response_len = priv->fepcore->send(priv->fepcore, sizeof(*msg_data), (u8 *)msg_data, sizeof(ack), (u8 *)&ack);
	if (ack.command == BEO_HH1_REP_NACK) {
		dev_err(dev, "NACK");
	}
	kfree(msg_data);
	return len;
}

static DEVICE_ATTR(test_mode, S_IWUSR, NULL, beo_ase_s47_test_mode);
static DEVICE_ATTR(test_pattern, S_IWUSR, NULL, beo_ase_s47_test_pattern);
static DEVICE_ATTR(test_brightness, S_IWUSR, NULL, beo_ase_s47_test_brightness);
static DEVICE_ATTR(command, S_IWUSR, NULL, beo_ase_s47_command);

static bool beo_ase_s47_register_devices(struct device* dev)
{
	int ret = 0;
	ret |= device_create_file(dev, &dev_attr_test_mode);
	ret |= device_create_file(dev, &dev_attr_test_pattern);
	ret |= device_create_file(dev, &dev_attr_test_brightness);
	ret |= device_create_file(dev, &dev_attr_command);
	return ret;
}
static void beo_ase_s47_unregister_devices(struct device* dev)
{
	device_remove_file(dev, &dev_attr_test_mode);
	device_remove_file(dev, &dev_attr_test_pattern);
	device_remove_file(dev, &dev_attr_test_brightness);
	device_remove_file(dev, &dev_attr_command);
}

static int beo_ase_s47_display_open(struct inode *inode, struct file *file)
{
	struct beo_ase_s47_display_priv *priv = container_of(inode->i_cdev, struct beo_ase_s47_display_priv, cdev);
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

static int beo_ase_s47_display_release(struct inode *inode, struct file *file)
{
	struct beo_ase_s47_display_priv *priv = container_of(inode->i_cdev, struct beo_ase_s47_display_priv, cdev);
	if (!priv)
		return -EINVAL;

	priv->open_count--;

	return 0;
}

static int beo_ase_s47_display_write(struct file *file, const char __user *user_buff, size_t count, loff_t *ppos)
{

	struct beo_ase_s47_display_priv *priv = file->private_data;

	int ret, to_send = 0;
	unsigned int data_offset = 0;
	int seq = 0;
	int msg_length;
	struct ASE_S47_DisplayFrameData *frame_data;

	struct beo_ase_s47_msg_display_data_extension *msg_dataex;
	struct beo_ase_s47_msg_display_data *msg_data;

	struct beo_hh1_rsp_ack ack;

	if (count < sizeof(struct ASE_S47_DisplayFrameData)) {
		return 0;
	}

	if (!priv)
		return -EINVAL;


	msg_data = kzalloc(sizeof(*msg_data), GFP_KERNEL);
	msg_dataex = kzalloc(sizeof(*msg_dataex), GFP_KERNEL);

	frame_data = (struct ASE_S47_DisplayFrameData *) kmalloc(count, GFP_KERNEL);

	if (!msg_data)
		return -ENOMEM;

	if (!msg_dataex) {
		ret = -ENOMEM;
		goto free_msg_data;
	}

	if (!frame_data) {
		ret = -ENOMEM;
		goto free_msg_dataex;
	}

	ret = copy_from_user(frame_data, user_buff, count);
	if (ret) {
		dev_err(priv->dev, "copy_from_user failed %d\n", ret);
		ret = -EFAULT;
		goto out;
	}

	//
	// sanity checks
	//
	if (count != sizeof(*frame_data) + frame_data->imageLength) {
		ret = -EINVAL;
		goto out;
	}

	ret = count;
	//
	// Send Data
	//
	if (frame_data->imageLength <= sizeof(msg_data->frame_data)) {
		to_send = frame_data->imageLength;
		// Fix, always send full sized packet - because of fragmentation of memory on fep
		msg_length = sizeof(*msg_data);
	} else {
		to_send = sizeof(msg_data->frame_data);
		msg_length = sizeof(*msg_data);
	}
	msg_data->hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_DATA);
	msg_data->hdr.len = cpu_to_le16(msg_length);
	msg_data->outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_data->outlen.pad = 0;
	msg_data->width = frame_data->width;
	msg_data->height = frame_data->height;
	msg_data->imglen = frame_data->imageLength;
	msg_data->imgid = frame_data->imageId;
	msg_data->format = frame_data->format;

	memcpy(msg_data->frame_data, frame_data->data, to_send);

	// send it to FEP
	priv->fepcore->send(priv->fepcore, msg_length, (u8 *)msg_data, sizeof(ack), (u8 *)&ack);
	// check for ACK
	if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
		dev_err(priv->dev, "No ACK!\n");
		ret = -ENODEV;
		goto out;
	}

	data_offset = sizeof(msg_data->frame_data);

	//
	// Send Data Extension
	//
	msg_dataex->hdr.command = cpu_to_le16(BEO_ASE_S47_CMD_DISPLAY_DATA_EXTENSION);
	msg_dataex->outlen.outlen = cpu_to_le16(sizeof(ack));
	msg_dataex->outlen.pad = 0;

	while (data_offset < msg_data->imglen) {

		// calculate how much to send
		to_send = msg_data->imglen - data_offset;
		if (to_send <= 0 ) {
			dev_err(priv->dev, "error to_send!! %d %d\n", msg_data->imglen, data_offset);
			ret = count;
			goto out;
		}
		if (to_send > sizeof(msg_dataex->frame_data)) {
			to_send = sizeof(msg_dataex->frame_data);
		}
		//// Fix, always send full sized packet - because of fragmentation of memory on fep
		msg_length = sizeof(*msg_dataex);
		msg_dataex->hdr.len = cpu_to_le16(msg_length);

		msg_dataex->offset = data_offset;
		msg_dataex->imgid = msg_data->imgid;
		msg_dataex->seq = seq;
		// copy frame data
		memcpy(msg_dataex->frame_data, frame_data->data + data_offset, to_send);

		// send it to FEP
		priv->fepcore->send(priv->fepcore, msg_length, (u8 *)msg_dataex, sizeof(ack), (u8 *)&ack);

		// check for ACK
		if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
			dev_err(priv->dev, "No ACK!\n");
			ret = -ENODEV;
			goto out;
		}

		data_offset += to_send;
		seq++;
	}

out:
	kfree(frame_data);
free_msg_dataex:
	kfree(msg_dataex);
free_msg_data:
	kfree(msg_data);


	return ret;
}

static const struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.open = beo_ase_s47_display_open,
	.release = beo_ase_s47_display_release,
	.write = beo_ase_s47_display_write
};

static int beo_ase_s47_display_probe(struct platform_device *pdev)
{
	int ret;
	struct beo_ase_s47_display_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;

	if (!fepcore) {
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_ase_s47_display_priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	platform_set_drvdata(pdev, priv);

	// get one free major - minor number pair
	ret = alloc_chrdev_region(&major_number, 0, 1, DEVICE_NAME);
	if (ret) {
		dev_err(priv->dev, "alloc_chrdev_region() failed %d\n", ret);
		return ret;
	}

	// create device class
	display_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(display_class)) {
		ret = PTR_ERR(display_class);
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
	device_node = device_create(display_class, priv->dev, major_number, NULL, DEVICE_NAME);
		if (IS_ERR(device_node)) {
		 ret = PTR_ERR(device_node);
		 dev_err(priv->dev, "device_create() failed %d\n", ret);
		 goto clean_cdev;
	}

	beo_ase_s47_register_devices(&pdev->dev);

	dev_info(priv->dev, "Initialized.\n");
	return 0;

clean_cdev:
	cdev_del(&priv->cdev);
destroy_class:
	class_destroy(display_class);
unregister_chardev:
	unregister_chrdev_region(major_number, 1);

	return ret;
}

static int beo_ase_s47_display_remove(struct platform_device *pdev)
{
	struct beo_ase_s47_display_priv *priv = platform_get_drvdata(pdev);

	device_destroy(display_class, major_number);
	cdev_del(&priv->cdev);
	class_destroy(display_class);
	unregister_chrdev_region(major_number, 1);
	beo_ase_s47_unregister_devices(&pdev->dev);

	return 0;
}

static const struct of_device_id of_beo_ase_s47_display_match[] = {
	{ .compatible = "beo,beo-ase-s47-display" },
	{},
};

static struct platform_driver beo_ase_s47_display_driver = {
	.probe		= beo_ase_s47_display_probe,
	.remove		= beo_ase_s47_display_remove,
	.driver		= {
		.name	= "beo-ase-s47-display",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_ase_s47_display_match),
	},
};

module_platform_driver(beo_ase_s47_display_driver);

MODULE_AUTHOR("Maros Medvec <maros.medvec@streamunlimited.com>");
MODULE_AUTHOR("Amir Hammad <amir.hammad@streamunlimited.com>");
MODULE_DESCRIPTION("BeO S47 FEP display driver");
MODULE_LICENSE("GPL v2");
