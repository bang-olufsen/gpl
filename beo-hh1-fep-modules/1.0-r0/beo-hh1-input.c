/*
 * Input driver for BeO HH1 FEP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/of_platform.h>
#include <linux/mfd/core.h>

#include "beo-hh1-fep.h"

#define MAX_BUTTONS				32

#define EVENT_BUTTONS_CHECK_MASK	0xfe00
#define EVENT_BUTTONS_CHECK_VAL		0x0000
#define EVENT_BUTTONS_MASK			(0xffff - EVENT_BUTTONS_CHECK_MASK)

#define CONTINUOUS_COMMAND_INTERVAL	(HZ * 0.138)

#define IR_BUTTON_STEP_UP		0x1E
#define IR_BUTTON_C_STEP_UP		0x72
#define IR_BUTTON_STEP_DW		0x1F
#define IR_BUTTON_C_STEP_DW		0x73
#define IR_BUTTON_REWIND			0x32
#define IR_BUTTON_C_REWIND		0x70
#define IR_BUTTON_WIND			0x34
#define IR_BUTTON_C_WIND			0x71
#define IR_BUTTON_CNTL_STEP_UP	0xD8
#define IR_BUTTON_CNTL_C_STEP_UP	0x78
#define IR_BUTTON_CNTL_STEP_DW	0xD9
#define IR_BUTTON_CNTL_C_STEP_DW	0x79
#define IR_BUTTON_CNTL_REWIND		0xD5
#define IR_BUTTON_CNTL_C_REWIND	0x76
#define IR_BUTTON_CNTL_WIND		0xD4
#define IR_BUTTON_CNTL_C_WIND		0x77
#define IR_BUTTON_VOL_UP			0x60
#define IR_BUTTON_VOL_DW			0x64
#define IR_BUTTON_CONTINUE		0x75
#define IR_BUTTON_RELEASE		0x7E

enum input_type {
	BUTTON, SWITCH, RELATIVE, ABSOLUTE
};

struct beo_hh1_button_data {
	int fep_addr;
	int linux_code;
	enum input_type type;
};

struct beo_hh1_input_priv {
	struct beo_hh1_fep_core *fepcore;
	struct device *dev;

	struct delayed_work continuous_command_work;
	u32 previous_ir_command[4];

	int num_buttons;
	struct input_dev *buttons_idev;
	struct beo_hh1_button_data buttons[MAX_BUTTONS];
};


/* Code to create from OpenFirmware platform devices */
static int beo_hh1_input_create_of(struct platform_device *pdev, struct beo_hh1_input_priv *priv)
{
	struct device_node *np = pdev->dev.of_node, *child, *buttons;
	int ret = 0;

	buttons = of_get_child_by_name(np, "buttons");
	if (buttons) {
		for_each_child_of_node(buttons, child) {
			const char *label;
			const char *type;
			const __be32 *prop;
			int len;
			int fep_addr;
			int linux_code;

			prop = of_get_property(child, "reg", &len);
			if (!prop || len < sizeof(*prop)) {
				dev_err(&pdev->dev, "%s has no 'reg' property\n",
					child->full_name);
				continue;
			}
			fep_addr = be32_to_cpup(prop);

			prop = of_get_property(child, "linux,code", &len);
			if (!prop || len < sizeof(*prop)) {
				dev_err(&pdev->dev, "%s has no 'linux,code' property\n",
					child->full_name);
				continue;
			}
			linux_code = be32_to_cpup(prop);
			label = of_get_property(child, "label", NULL) ? : child->name;
			type = of_get_property(child, "type", NULL) ? : "";

			if (!strcmp(type, "switch")) {
				priv->buttons[priv->num_buttons].type = SWITCH;
			}
			else if (!strcmp(type, "relative")) {
				priv->buttons[priv->num_buttons].type = RELATIVE;
			}
			else if (!strcmp(type, "absolute")) {
				priv->buttons[priv->num_buttons].type = ABSOLUTE;
			}
			else /*if (!strcmp(type, "button"))*/{
				priv->buttons[priv->num_buttons].type = BUTTON;
			}

			if (linux_code >= 0 && priv->num_buttons < MAX_BUTTONS) {
				dev_info(priv->dev, "Register %i: %s, %04x -> %04x\n", priv->buttons[priv->num_buttons].type, label, fep_addr, linux_code);

				priv->buttons[priv->num_buttons].fep_addr = fep_addr;
				priv->buttons[priv->num_buttons].linux_code = linux_code;

				switch(priv->buttons[priv->num_buttons].type) {
					case SWITCH: __set_bit(linux_code, priv->buttons_idev->swbit); break;
					case RELATIVE: __set_bit(linux_code, priv->buttons_idev->relbit); break;
					case ABSOLUTE: __set_bit(linux_code, priv->buttons_idev->absbit); break;
					case BUTTON: //fall through
					default: __set_bit(linux_code, priv->buttons_idev->keybit); break;
				}

				priv->num_buttons++;
			}

			if (ret < 0) {
				of_node_put(child);
				goto err_free_buttons;
			}
		}

		of_node_put(buttons);
	}

	return 0;

err_free_buttons:

	return -ENODEV;
}


static int beo_hh1_input_report_event(struct beo_hh1_input_priv *priv, u16 event, u16 data, int do_sync)
{
	int i;
	int reported = 0;

	if ((event & EVENT_BUTTONS_CHECK_MASK) == EVENT_BUTTONS_CHECK_VAL) {
		for (i = 0; i < priv->num_buttons; i++) {
			if ((event & EVENT_BUTTONS_MASK) == priv->buttons[i].fep_addr) {
#ifdef DEBUG_VERBOSE
				dev_info(priv->dev, "hh1_input_event: %i %04x, data: %04x\n", priv->buttons[i].type, event, data);
#endif
				switch(priv->buttons[i].type) {
					case SWITCH: input_report_switch(priv->buttons_idev, priv->buttons[i].linux_code, data); break;
					case RELATIVE: input_report_rel(priv->buttons_idev, priv->buttons[i].linux_code, data); break;
					case ABSOLUTE: input_report_abs(priv->buttons_idev, priv->buttons[i].linux_code, data); break;
					case BUTTON: //fall through
					default: input_report_key(priv->buttons_idev, priv->buttons[i].linux_code, data); break;
				}

				reported = 1;
				break;
			}
		}
	}

	if (reported) {
		if (do_sync)
			input_sync(priv->buttons_idev);
	}
	else {
		dev_info(priv->dev, "hh1_input_event: unknown: %04x, data: %04x\n", event, data);
	}

	return 0;
}

static int beo_hh1_input_long_report_event(struct beo_hh1_input_priv *priv, u16 event, u32 evlen, u8 *data, int do_sync)
{
	int i;
	int reported = 0;
	u32 *data_to_send;

	if (event == BEO_HH1_EV_IR) {
		memset(&priv->previous_ir_command, 0, sizeof(priv->previous_ir_command));
		memcpy(&priv->previous_ir_command, data, evlen);

		data_to_send = priv->previous_ir_command;

#ifdef DEBUG_VERBOSE
		dev_info(priv->dev, "IR-press: %08x %08x %08x %08x\n", data_to_send[0], data_to_send[1], data_to_send[2], data_to_send[3]);
#endif

		for (i = 0; i < 4; i++)
			input_event(priv->buttons_idev, EV_MSC, MSC_RAW, data_to_send[i]);
		reported = 1;

		// generate 'continuous command' events on our side
		schedule_delayed_work(&priv->continuous_command_work, CONTINUOUS_COMMAND_INTERVAL);
	}
	else if (event == BEO_HH1_EV_IRRELEASETIME && evlen == sizeof(struct beo_hh1_msg_ir_release)) {
		struct beo_hh1_msg_ir_release msg;
		u8 *button_id;
		memcpy(&msg, data, sizeof(msg));

		// generate 'release' command
		data_to_send = priv->previous_ir_command;
		button_id = (u8*) &data_to_send[1];
		*button_id = IR_BUTTON_RELEASE;

		for (i = 0; i < 4; i++)
			input_event(priv->buttons_idev, EV_MSC, MSC_RAW, data_to_send[i]);
		reported = 1;

		// release event received - stop continuous commands
		cancel_delayed_work_sync(&priv->continuous_command_work);
#ifdef DEBUG_VERBOSE
		dev_info(priv->dev, "IR-release duration: %d\n", msg.duration);
#endif
	}

	if (reported) {
		if (do_sync)
			input_sync(priv->buttons_idev);
	}
	else {
#ifdef DEBUG_VERBOSE
		dev_info(priv->dev, "hh1_input_event_long: unknown: %04x, data_len: %04x\n", event, evlen);
		for (i = 0; i < evlen; i++)
			dev_info(priv->dev, "hh1_input_event_long: data: %02x\n", data[i]);
#endif
	}

	return 0;
}

static int beo_hh1_input_event_cb(void *_priv, u16 event, u16 data)
{
	return beo_hh1_input_report_event((struct beo_hh1_input_priv *)_priv, event, data, 1);
}

static int beo_hh1_long_input_event_cb(void *_priv, u16 event, u32 evlen, u8 *data)
{
	return beo_hh1_input_long_report_event((struct beo_hh1_input_priv *)_priv, event, evlen, data, 1);
}


static void beo_hh1_input_report_initial(struct beo_hh1_input_priv *priv)
{
	struct beo_hh1_msg_buttons msg;
	struct beo_hh1_rsp_buttons ack;

	memset(&ack, 0, sizeof(ack));

	msg.hdr.command = cpu_to_le16(BEO_HH1_CMD_BUTTONS);
	msg.hdr.len = cpu_to_le16(sizeof(msg));
	msg.outlen.outlen = cpu_to_le16(sizeof(ack));
	msg.outlen.pad = 0;

	priv->fepcore->send(priv->fepcore, sizeof(msg), (u8 *)&msg, sizeof(ack), (u8 *)&ack);

	if (le16_to_cpu(ack.hdr.command) == BEO_HH1_REP_ACK) {
		int len = le16_to_cpu(ack.hdr.len);
		int i;
		struct beo_hh1_event ev;

		if (len > sizeof(ack)) {
			dev_err(priv->dev, "Invalid length of buttons response: %d\n", len);
			return;
		}

		for (i = 0; i < (len - sizeof(ack.hdr)) / sizeof(ev); i++) {
			memcpy(&ev, ack.events + i, sizeof(ev));
			ev.id = le16_to_cpu(ev.id);
			ev.value = le16_to_cpu(ev.value);

			// no event - stop parsing
			if (ev.id == 0)
				break;

			dev_info(priv->dev, "  button %d state: %04x: %04x\n", i, ev.id, ev.value);

			beo_hh1_input_report_event(priv, ev.id, ev.value, false);
		}

		input_sync(priv->buttons_idev);
	}
	else {
		dev_err(priv->dev, "buttons cmd without ack\n");
	}
}

static void beo_hh1_continuous_command_work_func(struct work_struct *work)
{
	struct delayed_work *dwork = container_of(work, struct delayed_work, work);
	struct beo_hh1_input_priv *priv = container_of(dwork, struct beo_hh1_input_priv, continuous_command_work);

	int i;
	u32 data_to_send[4];
	u8 *newRemote = (u8*) &data_to_send[0];
	u8 *button_id = (u8*) &data_to_send[1];

	memset(&data_to_send, 0, sizeof(data_to_send));
	memcpy(&data_to_send, priv->previous_ir_command, sizeof(data_to_send));

	// fix continuous commands for old remotes
	if (*newRemote == 0) {
		switch (*button_id) {
		case IR_BUTTON_STEP_UP:
			*button_id = IR_BUTTON_C_STEP_UP;
			break;
		case IR_BUTTON_STEP_DW:
			*button_id = IR_BUTTON_C_STEP_DW;
			break;
		case IR_BUTTON_REWIND:
			*button_id = IR_BUTTON_C_REWIND;
			break;
		case IR_BUTTON_WIND:
			*button_id = IR_BUTTON_C_WIND;
			break;
		case IR_BUTTON_CNTL_STEP_UP:	// blue
			*button_id = IR_BUTTON_CNTL_C_STEP_UP;
			break;
		case IR_BUTTON_CNTL_STEP_DW:	// red
			*button_id = IR_BUTTON_CNTL_C_STEP_DW;
			break;
		case IR_BUTTON_CNTL_REWIND:	// green
			*button_id = IR_BUTTON_CNTL_C_REWIND;
			break;
		case IR_BUTTON_CNTL_WIND:	// yellow
			*button_id = IR_BUTTON_CNTL_C_WIND;
			break;
		case IR_BUTTON_VOL_UP:
		case IR_BUTTON_VOL_DW:
			// no fixing for volume up/down
			break;
		default:
			// all other buttons use general continue command
			*button_id = IR_BUTTON_CONTINUE;
			break;
		}
	}
	else {
		switch (*button_id) {
		case IR_BUTTON_VOL_UP:
		case IR_BUTTON_VOL_DW:
			// no fixing for volume up/down
			break;
		default:
			// all other buttons use general continue command
			*button_id = IR_BUTTON_CONTINUE;
			break;
		}
	}

#ifdef DEBUG_VERBOSE
	dev_info(priv->dev, "IR-continuous-press: %08x %08x %08x %08x\n", data_to_send[0], data_to_send[1], data_to_send[2], data_to_send[3]);
#endif

	for (i = 0; i < 4; i++)
		input_event(priv->buttons_idev, EV_MSC, MSC_RAW, data_to_send[i]);
	input_sync(priv->buttons_idev);

	// reschedule until stopped
	schedule_delayed_work(&priv->continuous_command_work, CONTINUOUS_COMMAND_INTERVAL);
}

static int beo_hh1_input_probe(struct platform_device *pdev)
{
	struct beo_hh1_input_priv *priv;
	int ret = 0;

	struct beo_hh1_fep_core **fepcore = pdev->dev.platform_data;
	if (!fepcore) {
		dev_err(&pdev->dev, "No platform data.\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(struct beo_hh1_input_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "Unable to allocate private data\n");
		return -ENOMEM;
	}

	priv->dev = &pdev->dev;
	priv->fepcore = *fepcore;

	priv->buttons_idev = input_allocate_device();
	if (!priv->buttons_idev) {
		dev_err(priv->dev, "Unable to allocate input device\n");
		ret = -ENOMEM;
		goto out_free;
	}

	priv->buttons_idev->name = pdev->name;
	priv->buttons_idev->phys = "beo-hh1/input0";
	priv->buttons_idev->id.bustype = BUS_HOST;
	priv->buttons_idev->dev.parent = priv->dev;

	__set_bit(EV_KEY, priv->buttons_idev->evbit);
	__set_bit(EV_SW, priv->buttons_idev->evbit);
	__set_bit(EV_MSC, priv->buttons_idev->evbit);
	__set_bit(MSC_RAW, priv->buttons_idev->mscbit);
	__set_bit(EV_REL, priv->buttons_idev->evbit);

	if (beo_hh1_input_create_of(pdev, priv) != 0) {
		ret = -ENODEV;
		goto out_free_idev;
	}

	if (input_register_device(priv->buttons_idev)) {
		ret = -ENODEV;
		goto out_free_idev;
	}

	mfd_cell_enable(pdev);

	platform_set_drvdata(pdev, priv);
	INIT_DELAYED_WORK(&priv->continuous_command_work, beo_hh1_continuous_command_work_func);

	// report initial switch states
	beo_hh1_input_report_initial(priv);

	priv->fepcore->register_events(pdev, 0x0000, 0x0200, priv, beo_hh1_input_event_cb);

	priv->fepcore->register_long_events(pdev, BEO_HH1_EV_IR, 0x1, priv, beo_hh1_long_input_event_cb);
	priv->fepcore->register_long_events(pdev, BEO_HH1_EV_IRRELEASETIME, 0x1, priv, beo_hh1_long_input_event_cb);

	dev_info(priv->dev, "Initialized.\n");
	return 0;

out_free_idev:
	input_free_device(priv->buttons_idev);

out_free:
	return ret;
}

static int beo_hh1_input_remove(struct platform_device *pdev)
{
	struct beo_hh1_input_priv *priv = platform_get_drvdata(pdev);

	mfd_cell_disable(pdev);

	cancel_delayed_work_sync(&priv->continuous_command_work);
	input_unregister_device(priv->buttons_idev);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static const struct of_device_id of_beo_hh1_input_match[] = {
	{ .compatible = "beo-hh1-input", },
	{},
};

static struct platform_driver beo_hh1_input_driver = {
	.probe		= beo_hh1_input_probe,
	.remove		= beo_hh1_input_remove,
	.driver		= {
		.name	= "beo-hh1-input",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_beo_hh1_input_match),
	},
};

module_platform_driver(beo_hh1_input_driver);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 FEP Input driver");
MODULE_LICENSE("GPL v2");
