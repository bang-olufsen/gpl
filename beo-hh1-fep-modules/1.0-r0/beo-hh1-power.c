/*
 * PowerMgmt driver for BeO HH1 FEP
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
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"


struct beo_hh1_power_data {
	int id;		// FEP id
	int state;	// logical state of this power supply
	char *name;
	struct device_attribute attr;
};

struct beo_hh1_power_priv {
	struct beo_hh1_fep_core *fepcore;

	// keep last:
	int num_supplies;
	struct beo_hh1_power_data supplies[];
};



static inline int sizeof_beo_hh1_power_priv(int num_supplies)
{
	return sizeof(struct beo_hh1_power_priv) +
		(sizeof(struct beo_hh1_power_data) * num_supplies);
}


static ssize_t beo_hh1_power_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct beo_hh1_power_priv *priv = platform_get_drvdata(pdev);
	int ret = -EINVAL;
	int data;
	int i;
	struct beo_hh1_rsp_ack ack;
	struct beo_hh1_msg_pwrsupplies *msg;
	int msglen;

	if (sscanf(buf, "%d", &data) == 1) {
		for (i = 0; i < priv->num_supplies; i++)
			if (priv->supplies[i].name == attr->attr.name) {
#ifdef DEBUG_VERBOSE
				dev_info(dev, "%s -> %d\n", attr->attr.name, data);
#endif /* DEBUG_VERBOSE */

				// send command
				msglen = sizeof(struct beo_hh1_msg_pwrsupplies);
				msg = kzalloc(msglen, GFP_KERNEL);
				if (msg) {
					msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_POWERSUPPLY);
					msg->hdr.len = cpu_to_le16(msglen);
					msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
					msg->outlen.pad = 0;
					msg->pwrSuppliesSetting.pwrSupp = (1 << priv->supplies[i].id);
					msg->pwrSuppliesSetting.pwrState = ((data ? 1 : 0) << priv->supplies[i].id);

					priv->fepcore->send(priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
					kfree(msg);

					if (le16_to_cpu(ack.command) != BEO_HH1_REP_ACK) {
						dev_err(dev, "No ACK!\n");
						return -ENODEV;
					}

					ret = len;
				}
				else {
					ret = -ENOMEM;
				}
			}
	}

	return ret;
}

/* Code to create from OpenFirmware platform devices */
static struct beo_hh1_power_priv * __init beo_hh1_power_create_of(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	struct beo_hh1_power_priv *priv;
	int count;
	int cnt = 0;

	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data.\n");
		return NULL;
	}

	/* count supplies in this device, so we know how much to allocate */
	count = of_get_child_count(np);
	if (!count)
		return NULL;

	priv = devm_kzalloc(&pdev->dev, sizeof_beo_hh1_power_priv(count),
			GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->fepcore = *fepcore;

	for_each_child_of_node(np, child) {
		const __be32 *prop;
		int len;

		prop = of_get_property(child, "reg", &len);
		if (!prop || len < sizeof(*prop)) {
			dev_err(&pdev->dev, "%s has no 'reg' property\n",
				child->full_name);
			continue;
		}

		priv->supplies[cnt].id = be32_to_cpup(prop);
		priv->supplies[cnt].name = of_get_property(child, "label", NULL) ? : child->name;
		priv->supplies[cnt].state = -1;

		sysfs_attr_init(&priv->supplies[cnt].attr.attr);
		priv->supplies[cnt].attr.attr.name = priv->supplies[cnt].name;
		priv->supplies[cnt].attr.attr.mode = S_IWUSR;
		priv->supplies[cnt].attr.show = NULL;
		priv->supplies[cnt].attr.store = beo_hh1_power_store;
		device_create_file(&pdev->dev, &priv->supplies[cnt].attr);

		dev_info(&pdev->dev, "Power supply: %s, id %d\n", priv->supplies[cnt].name, priv->supplies[cnt].id);

		cnt++;
		priv->num_supplies = cnt;
	}

	return priv;
}


static const struct of_device_id of_beo_hh1_power_match[] = {
	{ .compatible = "beo-hh1-power", },
	{},
};


static int beo_hh1_power_probe(struct platform_device *pdev)
{
	struct beo_hh1_power_priv *priv;
	priv = beo_hh1_power_create_of(pdev);
	if (!priv)
		return -ENODEV;

	mfd_cell_enable(pdev);

	platform_set_drvdata(pdev, priv);
	return 0;
}

static int beo_hh1_power_remove(struct platform_device *pdev)
{
	struct beo_hh1_power_priv *priv = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < priv->num_supplies; i++)
		device_remove_file(&pdev->dev, &priv->supplies[i].attr);

	mfd_cell_disable(pdev);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_power_driver = {
	.probe		= beo_hh1_power_probe,
	.remove		= beo_hh1_power_remove,
	.driver		= {
		.name	= "beo-hh1-power",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_power_match),
	},
};

module_platform_driver(beo_hh1_power_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP PwrMgmt driver");
MODULE_LICENSE("GPL v2");
