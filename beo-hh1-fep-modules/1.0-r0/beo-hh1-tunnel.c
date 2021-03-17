/*
 * FEP console tunnel driver for BeO HH1 FEP
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
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"

struct beo_hh1_tunnel_priv {
	struct platform_device *pdev;
	struct device *dev;
	struct beo_hh1_fep_core *fepcore;
};


static ssize_t beo_hh1_tunnel_sendFEP_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	return -EINVAL;
}

static DEVICE_ATTR(sendFEP, S_IWUSR,
			NULL,
			beo_hh1_tunnel_sendFEP_store);


static int beo_hh1_tunnel_event_cb(void *_priv, u16 event, u32 size, u8 *data)
{
	struct beo_hh1_tunnel_priv *priv = _priv;

	if (event == BEO_HH1_EV_CONSOLE) {
		dev_info(priv->dev, "> %.*s", size, data);
	}

	return 0;
}


static int beo_hh1_tunnel_probe(struct platform_device *pdev)
{
	struct beo_hh1_tunnel_priv *priv;
	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	int ret;

	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_tunnel_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->pdev = pdev;
	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	ret = device_create_file(&pdev->dev,  &dev_attr_sendFEP);
	if (ret) {
		goto out_free_priv;
	}

	mfd_cell_enable(pdev);

	priv->fepcore->register_long_events(pdev, BEO_HH1_EV_CONSOLE, 1, priv, beo_hh1_tunnel_event_cb);
	platform_set_drvdata(pdev, priv);


	dev_info(&pdev->dev, "Initialized.\n");
	return 0;

out_free_priv:
	return ret;
}

static int beo_hh1_tunnel_remove(struct platform_device *pdev)
{
	mfd_cell_disable(pdev);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static const struct of_device_id of_beo_hh1_tunnel_match[] = {
	{ .compatible = "beo-hh1-tunnel", },
	{},
};

static struct platform_driver beo_hh1_tunnel_driver = {
	.probe		= beo_hh1_tunnel_probe,
	.remove		= beo_hh1_tunnel_remove,
	.driver		= {
		.name	= "beo-hh1-tunnel",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_tunnel_match),
	},
};

module_platform_driver(beo_hh1_tunnel_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP tunnel driver");
MODULE_LICENSE("GPL v2");
