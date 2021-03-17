/*
 * drivers/misc/mrvl_init.c
 *
 * Driver for marvell chip init (clock and reset gpio).
 *
 * Copyright (C) 2013 StreamuUnlimited.
 *
 * Authors:
 *	Matus Ujhelyi	<matus.ujhelyi@streamunlimited.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <asm/gpio.h>
#include <misc/mrvl_init.h>

#define DRIVER_NAME		"mrvl-init"

#ifdef CONFIG_OF
static const struct of_device_id mrvl_init_of_match[] = {
	{
		.compatible = "mrvl-init",
	},
	{},
};
MODULE_DEVICE_TABLE(of, mrvl_init_of_match);

static int mrvl_init_get_devtree_pdata(struct device *dev,
			struct mrvl_init_platform_data *pdata)
{
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int gpio;

	gpio = of_get_gpio_flags(np, 0, &flags);
	if (gpio < 0) {
		if (gpio != -EPROBE_DEFER)
			dev_err(dev, "Failed to get gpio flags (%d)\n", gpio);
		return gpio;
	}

	pdata->gpio_nr = gpio;
	pdata->gpio_flag = flags;
	/* probe() takes care of clock_name == NULL */
	pdata->clock_name = of_get_property(np, "linux,clock-name", NULL);

	return 0;
}
#else

#define mrvl_init_get_devtree_pdata(dev, pdata)	(-ENOSYS)

#endif

static int mrvl_init_probe(struct platform_device *pdev)
{
	const struct mrvl_init_platform_data *pdata =
					pdev->dev.platform_data;
	int ret = 0;
	struct clk *ck_32;
	int gpio_value = 0;

	if (pdev->dev.of_node) {
		struct mrvl_init_platform_data *dtpdata =
			devm_kzalloc(&pdev->dev, sizeof(*dtpdata), GFP_KERNEL);
		if (!dtpdata)
			return -ENOMEM;
		ret = mrvl_init_get_devtree_pdata(&pdev->dev, dtpdata);
		if (ret)
			return ret;
		pdata = dtpdata;
	}

	if (!pdata) {
		dev_err(&pdev->dev, "Platform data or device tree not defined.\n");
		return -EINVAL;
	}

	if (pdata->gpio_nr < 0) {
		dev_err(&pdev->dev, "Gpio not valid: %d\n", pdata->gpio_nr);
		return -EINVAL;
	}

	if (!pdata->clock_name) {
		dev_err(&pdev->dev, "Clock name empty\n");
		return -EINVAL;
	}

	ret = devm_gpio_request(&pdev->dev, pdata->gpio_nr, "wifi-pdn");
	if (ret < 0)
		return ret;

	gpio_value = pdata->gpio_flag;

	gpio_direction_output(pdata->gpio_nr, gpio_value);

	ck_32 = clk_get(NULL, pdata->clock_name);
        if (IS_ERR(ck_32)) {
		dev_err(&pdev->dev, "Cannot clk_get() err: %lu\n", PTR_ERR(ck_32));
		return PTR_RET(ck_32);
        }

	ret = clk_prepare(ck_32);
	if (ret) {
		goto err_clk;
	}

	ret = clk_enable(ck_32);
	if (ret) {
		goto err_clk;
	}

	/* reset chip */
	udelay(2000);
	gpio_set_value(pdata->gpio_nr, !gpio_value);

	dev_info(&pdev->dev, "Marvel init probed\n");
	return 0;

err_clk:
	clk_put(ck_32);
	return ret;
}

static struct platform_driver mrvl_init_driver = {
	.probe		= mrvl_init_probe,
	.driver		= {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(mrvl_init_of_match),
	},
};

module_platform_driver(mrvl_init_driver);
MODULE_DESCRIPTION("MRVL init");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);
MODULE_AUTHOR("StreamUnlimited");
