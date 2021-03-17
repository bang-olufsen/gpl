/*
 * LEDs driver for BeO HH1 FEP
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
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"


struct beo_hh1_leds_priv;

struct beo_hh1_led_data {
	struct led_classdev cdev;
	struct beo_hh1_leds_priv *priv;

	int fep_addr;
	u8 active_low;
	u8 blinking;
/*
	struct work_struct work;
	u8 new_level;
	int (*platform_gpio_blink_set)(unsigned gpio, int state,
			unsigned long *delay_on, unsigned long *delay_off);
*/
};

struct beo_hh1_leds_priv {
	struct beo_hh1_fep_core *fepcore;

	// keep last:
	int num_leds;
	struct beo_hh1_led_data leds[];
};



static inline int sizeof_beo_hh1_leds_priv(int num_leds)
{
	return sizeof(struct beo_hh1_leds_priv) +
		(sizeof(struct beo_hh1_led_data) * num_leds);
}


static void beo_hh1_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	struct beo_hh1_led_data *led_dat =
		container_of(led_cdev, struct beo_hh1_led_data, cdev);
	struct beo_hh1_rsp_ack ack;
	struct beo_hh1_msg_leds *msg;
	int msglen;

	dev_dbg(led_cdev->dev, "led_set(%d, %08x)", led_dat->fep_addr, value);

	msglen = sizeof(struct beo_hh1_msg_leds) + sizeof(struct beo_hh1_msg_led) * 1;
	msg = kzalloc(msglen, GFP_KERNEL);
	if (msg) {
		u16 repeat = 0;
		u16 pattern = 0;
		msg->hdr.command = cpu_to_le16(BEO_HH1_CMD_LEDS);
		msg->hdr.len = cpu_to_le16(msglen);
		msg->outlen.outlen = cpu_to_le16(0 + sizeof(struct beo_hh1_rsp_ack));
		msg->outlen.pad = 0;
		msg->leds[0].id = cpu_to_le16(led_dat->fep_addr);

		// "brigthness" is 32bit number - 16bit: repeat; 16bit: blink pattern
		repeat = (value >> 16) & 0xffff;
		pattern = value & 0xffff;

		// value == 0 -> this must be led-off, and pattern == 0x0000 is not valid anyway
		if (pattern == 0 && repeat == 0)
			pattern = 0x0010;

		msg->leds[0].mode = 1;
		msg->leds[0].val = repeat;
		msg->leds[0].setup = (0 << 16) | cpu_to_le16(pattern);

		led_dat->priv->fepcore->send(led_dat->priv->fepcore, msglen, (u8 *)msg, sizeof(ack), (u8 *)&ack);
		kfree(msg);
	}

}

#if 0
static int beo_hh1_blink_set(struct led_classdev *led_cdev,
	unsigned long *delay_on, unsigned long *delay_off)
{
	struct gpio_led_data *led_dat =
		container_of(led_cdev, struct gpio_led_data, cdev);

	led_dat->blinking = 1;
	return led_dat->platform_gpio_blink_set(led_dat->gpio, GPIO_LED_BLINK,
						delay_on, delay_off);
	return 0;
}
#endif

static int __init create_led(const struct gpio_led *template,
	struct beo_hh1_led_data *led_dat, struct device *parent,
	struct beo_hh1_leds_priv *priv,
	int (*blink_set)(unsigned, int, unsigned long *, unsigned long *))
{
	int ret, state;

	led_dat->priv = priv;
	led_dat->cdev.name = template->name;
	led_dat->cdev.default_trigger = template->default_trigger;
	led_dat->cdev.max_brightness = 0xffffffff;
	led_dat->fep_addr = template->gpio;
	led_dat->active_low = template->active_low;
	led_dat->blinking = 0;

	if (blink_set) {
		dev_err(parent, "Blink_set LEDs not yet supported\n");
		/*
		led_dat->platform_gpio_blink_set = blink_set;
		led_dat->cdev.blink_set = beo_hh1_blink_set;
		*/
	}

	led_dat->cdev.brightness_set = beo_hh1_led_set;

	if (template->default_state == LEDS_GPIO_DEFSTATE_KEEP) {
		dev_err(parent, "Keep-state LEDs not yet supported\n");
		state = 0;
	}
	else {
		state = (template->default_state == LEDS_GPIO_DEFSTATE_ON);
	}

	led_dat->cdev.brightness = state ? LED_FULL : LED_OFF;
	if (!template->retain_state_suspended)
		led_dat->cdev.flags |= LED_CORE_SUSPENDRESUME;

//	INIT_WORK(&led_dat->work, beo_hh1_led_work);

	dev_info(parent, "Creating LED: addr=%d, name=%s\n", led_dat->fep_addr, led_dat->cdev.name);

	ret = led_classdev_register(parent, &led_dat->cdev);
	if (ret < 0)
		goto err;

	return 0;

err:
	return ret;
}


static void delete_led(struct beo_hh1_led_data *led)
{
	led_classdev_unregister(&led->cdev);
	// cancel_work_sync(&led->work);
}



/* Code to create from OpenFirmware platform devices */
static struct beo_hh1_leds_priv * __init beo_hh1_leds_create_of(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	struct beo_hh1_leds_priv *priv;
	int count, ret;

	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data.\n");
		return NULL;
	}

	/* count LEDs in this device, so we know how much to allocate */
	count = of_get_child_count(np);
	if (!count)
		return NULL;

	priv = devm_kzalloc(&pdev->dev, sizeof_beo_hh1_leds_priv(count),
			GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->fepcore = *fepcore;

	for_each_child_of_node(np, child) {
		// we re-use gpio_led struct here..
		struct gpio_led led = {};
		const char *state;
		const __be32 *prop;
		int len;

		prop = of_get_property(child, "reg", &len);
		if (!prop || len < sizeof(*prop)) {
			dev_err(&pdev->dev, "%s has no 'reg' property\n",
				child->full_name);
			continue;
		}
		led.gpio = be32_to_cpup(prop);

		if (of_find_property(child, "active-low", NULL))
			led.active_low = 1;


		led.name = of_get_property(child, "label", NULL) ? : child->name;
		led.default_trigger =
			of_get_property(child, "linux,default-trigger", NULL);
		state = of_get_property(child, "default-state", NULL);
		if (state) {
			if (!strcmp(state, "keep"))
				led.default_state = LEDS_GPIO_DEFSTATE_KEEP;
			else if (!strcmp(state, "on"))
				led.default_state = LEDS_GPIO_DEFSTATE_ON;
			else
				led.default_state = LEDS_GPIO_DEFSTATE_OFF;
		}

		ret = create_led(&led, &priv->leds[priv->num_leds++],
				      &pdev->dev, priv, NULL);
		if (ret < 0) {
			of_node_put(child);
			goto err_free_leds;
		}
	}

	return priv;

err_free_leds:
	for (count = priv->num_leds - 2; count >= 0; count--)
		delete_led(&priv->leds[count]);

	return NULL;
}

static const struct of_device_id of_beo_hh1_leds_match[] = {
	{ .compatible = "beo-hh1-leds", },
	{},
};


static int beo_hh1_led_probe(struct platform_device *pdev)
{
	struct beo_hh1_leds_priv *priv;

	priv = beo_hh1_leds_create_of(pdev);
	if (!priv)
		return -ENODEV;

	mfd_cell_enable(pdev);

	platform_set_drvdata(pdev, priv);
	return 0;
}

static int beo_hh1_led_remove(struct platform_device *pdev)
{
	struct beo_hh1_leds_priv *priv = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < priv->num_leds; i++)
		delete_led(&priv->leds[i]);

	mfd_cell_disable(pdev);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver beo_hh1_led_driver = {
	.probe		= beo_hh1_led_probe,
	.remove		= beo_hh1_led_remove,
	.driver		= {
		.name	= "beo-hh1-leds",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_leds_match),
	},
};

module_platform_driver(beo_hh1_led_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP LED driver");
MODULE_LICENSE("GPL v2");
