/*
 * Copyright (C) 2014 StreamUnlimited
 *
 * License Terms: GNU General Public License v2
 * Author: Martin Geier <martin.geier@streamunlimited.com>
 */

#include "beo-hh1-fep.h"
#include "beo-common-fep.c"

#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/mfd/core.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/termios.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#define CHAR_TIMEOUT		1000000		/*microseconds*/
#define ENABLE_GPIO_INTERUPT

struct beo_ase_fep {
	struct mutex send_lock;
	struct file *tty;

	struct beo_common_fep common;
};

static struct mfd_cell beo_ase_fep_devs[] = {
	{
		.name = "beo-ase-s47-display",
		.of_compatible = "beo,beo-ase-s47-display",
	},
	{
		.name = "beo-ase-s47-misc",
		.of_compatible = "beo,beo-ase-s47-misc",
	},

	{
		.name = "beo-hh1-leds",
		.of_compatible = "beo,beo-hh1-leds",
	},
	{
		.name = "beo-hh1-input",
		.of_compatible = "beo,beo-hh1-input",
	},
	{
		.name = "beo-ase-dsp",
		.of_compatible = "beo,beo-ase-dsp",
	},
	{
		.name = "beo-hh1-fwupdate",
		.of_compatible = "beo,beo-hh1-fwupdate",
	},
	{
		.name = "beo-hh1-power",
		.of_compatible = "beo,beo-hh1-power",
	},
	{
		.name = "beo-hh1-misc",
		.of_compatible = "beo,beo-hh1-misc",
	},
	{
		.name = "beo-ase-fephw",
		.of_compatible = "beo,beo-ase-fephw",
	},
	{
		.name = "beo-hh1-tunnel",
		.of_compatible = "beo,beo-hh1-tunnel",
	},
	{
		.name = "beo-hh1-rtc",
		.of_compatible = "beo,beo-hh1-rtc",
	},
	{
		.name = "beo-ase-lsl",
		.of_compatible = "beo,beo-ase-lsl",
	},
};

static irqreturn_t beo_ase_fep_isr(int irq, void *dev_id)
{
	struct beo_ase_fep *fep_ase = dev_id;
	struct beo_common_fep *fep = &fep_ase->common;
	BUG_ON(irq != fep->irq);

	#ifdef DEBUG_VERBOSE
		dev_info(fep->dev, "ping from FEP\n");
	#endif

	disable_irq_nosync(fep->irq);

	schedule_work(&fep->work);

	return IRQ_HANDLED;
}

//copy from comedy driver - serial2002.c
static long tty_ioctl(struct file *tty, unsigned op, unsigned long param)
{
	if (tty->f_op->unlocked_ioctl)
		return tty->f_op->unlocked_ioctl(tty, op, param);

	return -ENOSYS;
}

static int tty_write(struct file *tty, unsigned char *buf, int count)
{
	int result;
	mm_segment_t oldfs;
	oldfs = get_fs();

	set_fs(KERNEL_DS);
	tty->f_pos = 0;
	result = tty->f_op->write(tty, buf, count, &tty->f_pos);
	set_fs(oldfs);
	return result;
}

//copy from comedy driver - serial2002.c
static int tty_read(struct file *tty, long timeout, unsigned char *buf, int count)
{
	int result;

	result = -1;
	if (!IS_ERR(tty)) {
		mm_segment_t oldfs;

		oldfs = get_fs();
		set_fs(KERNEL_DS);
		if (tty->f_op->poll) {
			struct poll_wqueues table;
			struct timeval start, now;

			do_gettimeofday(&start);
			poll_initwait(&table);
			while (1) {
				long elapsed;
				int mask;
				long next_timeout;

				mask = tty->f_op->poll(tty, &table.pt);
				if (mask & (POLLRDNORM | POLLRDBAND | POLLIN |
					    POLLHUP | POLLERR)) {
					break;
				}
				do_gettimeofday(&now);
				elapsed =
				    (1000000 * (now.tv_sec - start.tv_sec) +
				     now.tv_usec - start.tv_usec);

				if (elapsed >= timeout)
					break;
				set_current_state(TASK_INTERRUPTIBLE);
				next_timeout = ((timeout - elapsed) * HZ) / 1000000;
				if(next_timeout < 0)
					break;
				schedule_timeout(next_timeout);
			}
			poll_freewait(&table);
			{
				result = tty->f_op->read(tty, buf, count, &tty->f_pos);
			}
		} else {
			/* Device does not support poll, busy wait */
			int retries = 0;
			while (1) {
				retries++;
				if (retries*100 >= timeout)
					break;

				tty->f_pos = 0;
				result = tty->f_op->read(tty, buf, count, &tty->f_pos);
				if (result > 0)
					break;
				udelay(100);
			}
		}
		set_fs(oldfs);
	}
	return result;
}

static int tty_read_count(struct file *tty, unsigned char *buf, int count)
{
	int value, len = 0;
	while (len != count) {
		value = tty_read(tty, CHAR_TIMEOUT, buf + len, count - len);
		if (value <= 0){
			break;
		}
		len += value;
	}

	if (value < 0) 
		return value;
	return len;
}


static int tty_open(struct file **tty, const char const *interface)
{
	int result;
	char port[20];

	sprintf(port, "/dev/%s", interface);
	(*tty) = filp_open(port, O_RDWR, 0);

	if ((*tty) < 0) {
		printk("Cannot open: '%s'\n", port);
		return result = 1;
	} else {
		mm_segment_t oldfs;
		struct termios settings;
		//bzero(&settings, sizeof(settings));

		oldfs = get_fs();
		set_fs(KERNEL_DS);

		{
			tty_ioctl((*tty), TCGETS, (unsigned long)&settings);
			settings.c_iflag = 0;
			settings.c_oflag = 0;
			settings.c_lflag = 0;
			settings.c_cflag = CLOCAL | CS8 | CREAD | B1000000;	//1MHz
			settings.c_cc[VMIN] = 0;
			settings.c_cc[VTIME] = 0;
			tty_ioctl((*tty), TCSETS, (unsigned long)&settings);
		}

		set_fs(oldfs);
	}
	return 0;
}

static void tty_close(struct file *tty)
{
	  if (!IS_ERR(tty) && tty)
		  filp_close(tty, NULL);
}

static int beo_ase_fep_api_send_private(struct beo_hh1_fep_core *fepcore, size_t len, u8 *data, size_t outlen, u8 *outdata)
{
	struct beo_common_fep *fep = (struct beo_common_fep *)fepcore->fep_priv;
	struct beo_ase_fep *fep_ase = container_of(fep, struct beo_ase_fep, common);
	ssize_t status = 0;

#ifdef DEBUG_VERBOSE
	{
		int i;
		u8 dump[3*32 + 1] = {0};
		int ldump = (len <= 32 ? len : 32);
		for (i = 0; i < ldump; i++)
			sprintf(&dump[3*i], "%02x ", data[i]);
		dev_info(fep->dev, "send %d bytes: %s\n", len, dump);
	}
#endif

	status = tty_write(fep_ase->tty, data, len);
	if(status != len)
		return status;

	memset(outdata, 0, outlen);

	do {
		status = tty_read_count(fep_ase->tty, outdata, 1);
	} while (status == 1 && outdata[0] == 0);

	status += tty_read_count(fep_ase->tty, outdata + 1, outlen - 1);

#ifdef DEBUG_VERBOSE
	{
		int i;
		u8 dump[3*32 + 1] = {0};
		// final received data
		for (i = 0; i < (status <= 32 ? status : 32); i++)
			sprintf(&dump[3*i], "%02x ", outdata[i]);
		dev_info(fep->dev, "received: %d expected: %u bytes: %s\n", status, outlen, dump);
	}
#endif

	return status;
}

static int beo_ase_fep_api_send(struct beo_hh1_fep_core *fepcore, size_t len, u8 *data, size_t outlen, u8 *outdata)
{
	struct beo_common_fep *fep = (struct beo_common_fep *)fepcore->fep_priv;
	struct beo_ase_fep *fep_ase = container_of(fep, struct beo_ase_fep, common);

	int result = 0;

	mutex_lock(&fep_ase->send_lock);
	result = beo_ase_fep_api_send_private(fepcore, len, data, outlen, outdata);
	mutex_unlock(&fep_ase->send_lock);

	return result;
}

static int beo_ase_fep_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct beo_ase_fep *fep_ase;
	struct beo_common_fep *fep;
	int i, error, irqflags;
	enum of_gpio_flags flags;
	char const* interface;

	printk(KERN_ERR "beo_ase_fep_probe()\n");

	fep_ase = kzalloc(sizeof *fep_ase, GFP_KERNEL);
	if (!fep_ase)
		return -ENOMEM;
	fep = &fep_ase->common;
	fep->core = kzalloc(sizeof *fep->core, GFP_KERNEL);
	if (!fep->core)
		return -ENOMEM;

	fep->core->fep_priv = fep;
	fep->dev = &pdev->dev;

	// initialize events
	mutex_init(&fep->events_lock);


	// fill in cells API calls
	fep->core->send = beo_ase_fep_api_send;
	fep->core->register_events = beo_fep_api_register_events;
	fep->core->register_long_events = beo_fep_api_register_long_events;
	fep->core->crc16 = beo_fep_api_crc16;

	mutex_init(&fep_ase->send_lock);
	interface = of_get_property(fep->dev->of_node, "interface", NULL);
	if(!interface){
		ret = -1;
		goto fep_free;
	}

	ret = tty_open(&fep_ase->tty, interface);
	if(ret){
		dev_err(fep->dev, "Cannot open interface /dev/%s\n", interface);
		goto fep_free;
	}

#ifdef ENABLE_GPIO_INTERUPT
	fep->irq_gpio = of_get_named_gpio_flags(fep->dev->of_node, "irq_gpios", 0, &flags);
	fep->irq_gpio_active_low = flags & OF_GPIO_ACTIVE_LOW;

	if (gpio_is_valid(fep->irq_gpio)) {
		error = gpio_request(fep->irq_gpio, "ASE FEP");
		if (error < 0) {
			dev_err(fep->dev, "Failed to request GPIO %d, error %d\n", fep->irq_gpio, error);
			ret = -ENODEV;
			goto out_free_fep_core;
		}

		error = gpio_direction_input(fep->irq_gpio);
		if (error < 0) {
			dev_err(fep->dev, "Failed to configure direction for GPIO %d, error %d\n", fep->irq_gpio, error);
			ret = -ENODEV;
			goto out_free_gpio;
		}

		fep->irq = gpio_to_irq(fep->irq_gpio);
		if (fep->irq < 0) {
			dev_err(fep->dev, "Unable to get irq number for GPIO %d, error %d\n",
					fep->irq_gpio, fep->irq);
			fep->irq = -1;
			ret = -ENODEV;
			goto out_free_gpio;
		}

		dev_info(fep->dev, "Claimed irq %d on gpio %d\n", fep->irq, fep->irq_gpio);
	}
	else {
		dev_err(fep->dev, "No irq gpio specified\n");
		ret = -ENODEV;
		goto out_free_fep_core;
	}

	INIT_WORK(&fep->work, beo_fep_work_func);

	irqflags = IRQF_TRIGGER_LOW;
	error = request_any_context_irq(fep->irq, beo_ase_fep_isr, irqflags, "ASE FEP", fep_ase);
	if (error < 0) {
		dev_err(fep->dev, "Unable to claim irq %d; error %d\n",
			fep->irq, error);
		ret = -ENODEV;
		goto out_free_gpio;
	}
#endif /*ENABLE_GPIO_INTERUPT*/

	// fill in platform_data for all cells
	for (i = 0; i < ARRAY_SIZE(beo_ase_fep_devs); i++) {
		struct mfd_cell *cell = &beo_ase_fep_devs[i];
		cell->platform_data = &(fep->core);
		cell->pdata_size = sizeof(fep->core);
		cell->enable = beo_fep_mfd_enable_callback;
		cell->disable = beo_fep_mfd_disable_callback;
	}

	// register all cells
	ret = mfd_add_devices(fep->dev, 0, beo_ase_fep_devs,
			ARRAY_SIZE(beo_ase_fep_devs), NULL, 0, NULL);
	if (ret)
		goto fep_free;

	dev_set_drvdata(fep->dev, fep_ase);
	dev_info(fep->dev, "detected chip\n");

	return ret;


fep_free:
	mutex_destroy(&fep_ase->send_lock);
#ifdef ENABLE_GPIO_INTERUPT
out_free_gpio:
	if (gpio_is_valid(fep->irq_gpio))
		gpio_free(fep->irq_gpio);
#endif /*ENABLE_GPIO_INTERUPT*/

out_free_fep_core:
	kfree(fep->core);
	kfree(fep_ase);

	return ret;
}

static int beo_ase_fep_remove(struct platform_device *pdev)
{
	struct beo_ase_fep *fep_ase = dev_get_drvdata(&pdev->dev);
	struct beo_common_fep *fep = &fep_ase->common;
	printk(KERN_ERR "beo_ase_fep_remove()\n");

	if(fep_ase){
		mfd_remove_devices(fep->dev);
		tty_close(fep_ase->tty);

		free_irq(fep->irq, fep_ase);
		mutex_destroy(&fep_ase->send_lock);

		mutex_destroy(&fep->events_lock);

		if (gpio_is_valid(fep->irq_gpio))
			gpio_free(fep->irq_gpio);

		kfree(fep->core);
		kfree(fep_ase);
	}
	return 0;
}


static const struct of_device_id my_of_ids[] = {
	{ .compatible = "beo,beo-ase-fep", },
	{ }
};

static struct platform_driver beo_ase_fep_core_driver = {
	.driver = {
		.name = "beo-ase-fep",
		.owner = THIS_MODULE,
		.of_match_table = my_of_ids,
	},
	.probe	= beo_ase_fep_probe,
	.remove	= beo_ase_fep_remove,
};

MODULE_DEVICE_TABLE(of, my_of_ids);

static int __init beo_ase_fep_core_init(void)
{
	printk("beo_ase_fep_core_init()\n");
	return platform_driver_register(&beo_ase_fep_core_driver);
}

static void __exit beo_ase_fep_core_exit(void)
{
	printk("beo_ase_fep_core_exit()\n");
	platform_driver_unregister(&beo_ase_fep_core_driver);
}

subsys_initcall(beo_ase_fep_core_init);
module_exit(beo_ase_fep_core_exit);

MODULE_AUTHOR("Martin Geier <martin.geier@streamunlimited.com>");
MODULE_DESCRIPTION("BeO ASE FEP MFD core");
MODULE_LICENSE("GPL v2");
