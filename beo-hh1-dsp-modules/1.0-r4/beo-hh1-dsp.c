/*
 * Copyright (C) 2013 StreamUnlimited
 *
 * License Terms: GNU General Public License v2
 * Author: Vladimir Koutny <vladimir.koutny@streamunlimited.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/spi/spi.h>

#include <linux/mtd/mtd.h>

#include "beo-hh1-dsp.h"

struct beo_hh1_dsp {
	struct device	*dev;
	struct spi_device *spi_dev;

	// irq gpio
	int irq_gpio;
	int irq_gpio_active_low;
	// translated irq number
	int irq;

	int reset_gpio;
	int reset_gpio_active_low;
	int flash_gpio;
	int flash_gpio_active_low;

//	struct work_struct work;

	struct mutex flash_lock;
	struct mtd_info flash_mtd;
	u8 flash_txbuf[16];
	u32 flash_erasesize;
	u32 flash_erasecount;
	u32 flash_pagesize;
	int flash_enable;

	u32 dsp_lastdata;
};

static irqreturn_t beo_hh1_dsp_isr(int irq, void *dev_id)
{
	struct beo_hh1_dsp *dsp = dev_id;
	BUG_ON(irq != dsp->irq);

//	schedule_work(&dsp->work);
//	disable_irq_nosync(dsp->irq);

	return IRQ_HANDLED;
}

static int beo_hh1_dsp_send(struct beo_hh1_dsp *dsp, size_t len, u8 *data, size_t outlen, u8 *outdata)
{
	u8 dump[3*32 + 1] = {0};

	int ldump = (len <= 32 ? len : 32);
	int i;
	ssize_t status;
	struct spi_message message;
	struct spi_transfer x[2];

	u8 *d;
	size_t l;


	for (i = 0; i < ldump; i++)
		sprintf(&dump[3*i], "%02x ", data[i]);
#ifdef DEBUG_VERBOSE
	dev_info(dsp->dev, "send %d bytes: %s\n", len, dump);
#endif

//	spi_write_then_read can't handle >32 bytes (only since 3.8) so do it manually
	memset(outdata, 0, outlen);

	spi_bus_lock(dsp->spi_dev->master);

	l = len;
	d = data;
	while (l >= 4) {
		spi_message_init(&message);
		memset(x, 0, sizeof x);
		x[0].len = 4;
		x[0].tx_buf = d;
		spi_message_add_tail(&x[0], &message);
		status = spi_sync_locked(dsp->spi_dev, &message);

		l -= 4;
		d += 4;
	}

	l = outlen;
	d = outdata;
	while (l >= 4) {
		spi_message_init(&message);
		memset(x, 0, sizeof x);
		x[0].len = 4;
		x[0].rx_buf = d;
		spi_message_add_tail(&x[0], &message);
		status = spi_sync_locked(dsp->spi_dev, &message);

		l -= 4;
		d += 4;
	}
	spi_bus_unlock(dsp->spi_dev->master);

#ifdef DEBUG_VERBOSE
	for (i = 0; i < (outlen <= 32 ? outlen : 32); i++)
		sprintf(&dump[3*i], "%02x ", outdata[i]);
	dev_info(dsp->dev, "status=%d, received %d bytes: %s\n", status, outlen, dump);
#endif

	return status;
}

static int setup_gpio_out(struct device *dev, int gpio, const char *name, int active_low, int value)
{
	int error;

	error = gpio_request(gpio, name);
	if (error < 0) {
		dev_err(dev, "Failed to request GPIO %d, error %d\n",
				gpio, error);
		return -ENODEV;
	}

	error = gpio_direction_output(gpio, value ^ active_low);
	if (error < 0) {
		dev_err(dev,
				"Failed to configure direction for GPIO %d, error %d\n",
				gpio, error);
		return -ENODEV;
	}

	return 0;
}


// flash access code inspired by m25p80 driver (simplified for our use-case only)
static int flash_wait(struct beo_hh1_dsp *dsp)
{
	unsigned long deadline = jiffies + 3*HZ;		// 3sec max time for sector erase

	do {
		u8 code = 0x05;
		u8 v = 0;
		u8 ret = spi_write_then_read(dsp->spi_dev, &code, 1, &v, 1);

		if (ret < 0) {
			dev_err(dsp->dev, "error reading status register: %d\n", ret);
			return ret;
		}

		if ((v & 0x01) == 0)
			return 0;

		cond_resched();
	} while (!time_after_eq(jiffies, deadline));

	return 1;
}

static int flash_writeen(struct beo_hh1_dsp *dsp)
{
	u8 code = 0x06;
	return spi_write(dsp->spi_dev, &code, 1);
}

static int beo_hh1_dspflash_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct beo_hh1_dsp *dsp = container_of(mtd, struct beo_hh1_dsp, flash_mtd);
//	dev_info(dsp->dev, "flash read: from=%x len=%zd\n", (u32)from, len);

	struct spi_transfer t[2];
	struct spi_message m;
	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	mutex_lock(&dsp->flash_lock);

	dsp->flash_txbuf[0] = 0x03;
	dsp->flash_txbuf[1] = (from >> 16) & 0xff;
	dsp->flash_txbuf[2] = (from >> 8) & 0xff;
	dsp->flash_txbuf[3] = (from) & 0xff;

	t[0].tx_buf = dsp->flash_txbuf;
	t[0].len = 1+3;
	spi_message_add_tail(&t[0], &m);

	t[1].rx_buf = buf;
	t[1].len = len;
	spi_message_add_tail(&t[1], &m);

	spi_sync(dsp->spi_dev, &m);
	*retlen = m.actual_length - (1+3);

	mutex_unlock(&dsp->flash_lock);

	return 0;
}

static int flash_writepage(struct beo_hh1_dsp *dsp, u32 addr, const u_char *buf, u32 len)
{
	struct spi_transfer t[2];
	struct spi_message m;

	dsp->flash_txbuf[0] = 0x02;
	dsp->flash_txbuf[1] = (addr >> 16) & 0xff;
	dsp->flash_txbuf[2] = (addr >> 8) & 0xff;
	dsp->flash_txbuf[3] = (addr) & 0xff;

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = dsp->flash_txbuf;
	t[0].len = 1+3;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = buf;
	t[1].len = len;
	spi_message_add_tail(&t[1], &m);

	spi_sync(dsp->spi_dev, &m);

	return m.actual_length - (1+3);
}

static int beo_hh1_dspflash_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct beo_hh1_dsp *dsp = container_of(mtd, struct beo_hh1_dsp, flash_mtd);
//	dev_info(dsp->dev, "flash program: to=%x len=%zd\n", (u32)to, len);

	if ((to & (dsp->flash_pagesize - 1)) != 0) {
		dev_err(dsp->dev, "Non-aligned writes not yet supported!\n");
		return -EIO;
	}

	*retlen = 0;
	mutex_lock(&dsp->flash_lock);
	while (len) {
		u32 l = len;
		if (l > dsp->flash_pagesize)
			l = dsp->flash_pagesize;

		flash_wait(dsp);
		flash_writeen(dsp);

		if ((l = flash_writepage(dsp, to, buf, l)) < 0) {
			mutex_unlock(&dsp->flash_lock);
			return -EIO;
		}

		len -= l;
		to += l;
		buf += l;
		*retlen = *retlen + l;
	}
	mutex_unlock(&dsp->flash_lock);

	return 0;
}

static int flash_erasesect(struct beo_hh1_dsp *dsp, u32 addr)
{
	dsp->flash_txbuf[0] = 0xd8;
	dsp->flash_txbuf[1] = (addr >> 16) & 0xff;
	dsp->flash_txbuf[2] = (addr >> 8) & 0xff;
	dsp->flash_txbuf[3] = (addr) & 0xff;

	spi_write(dsp->spi_dev, dsp->flash_txbuf, 1 + 3);

	return 0;
}

static int beo_hh1_dspflash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct beo_hh1_dsp *dsp = container_of(mtd, struct beo_hh1_dsp, flash_mtd);
	u32 addr = instr->addr;
	u32 len = instr->len;

//	dev_info(dsp->dev, "flash erase: addr=%x len=%d\n", addr, len);

	mutex_lock(&dsp->flash_lock);
	while (len) {
		flash_wait(dsp);
		flash_writeen(dsp);
		if (flash_erasesect(dsp, addr)) {
			instr->state = MTD_ERASE_FAILED;
			mutex_unlock(&dsp->flash_lock);
			return -EIO;
		}
		addr += dsp->flash_erasesize;
		len -= dsp->flash_erasesize;
	}
	mutex_unlock(&dsp->flash_lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}


static int beo_hh1_dsp_create_mtd(struct beo_hh1_dsp *dsp)
{
	// TODO: specify these in device tree?
	dsp->flash_erasesize = 64*1024;		// sector size
	dsp->flash_erasecount = 16;
	dsp->flash_pagesize = 256;

	memset(&dsp->flash_mtd, 0, sizeof(dsp->flash_mtd));

	dsp->flash_mtd.name = "HH1 DSP flash";

	dsp->flash_mtd.type = MTD_NORFLASH;
	dsp->flash_mtd.flags = MTD_CAP_NORFLASH;

	dsp->flash_mtd.size = dsp->flash_erasesize * dsp->flash_erasecount;
	dsp->flash_mtd.writesize = 1;
	dsp->flash_mtd.erasesize = dsp->flash_erasesize;
	dsp->flash_mtd.writebufsize = dsp->flash_pagesize;

	dsp->flash_mtd._erase = beo_hh1_dspflash_erase;
	dsp->flash_mtd._read = beo_hh1_dspflash_read;
	dsp->flash_mtd._write = beo_hh1_dspflash_write;

	dsp->flash_mtd.dev.parent = dsp->dev;


	return mtd_device_register(&dsp->flash_mtd, NULL, 0);
}


// dsp access
static ssize_t beo_hh1_dsp_data_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(dev);
	int ret = -EINVAL;
	struct spi_transfer t[2];
	struct spi_message m;

	if (dsp->flash_enable)
		return ret;

//	dev_info(dev, "DSP data: %d bytes\n", len);

	if (len != 4) {
		dev_err(dsp->dev, "Invalid data size: %d\n", len);
		return ret;
	}

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = buf;
	t[0].rx_buf = &dsp->dsp_lastdata;
	t[0].len = len;
	spi_message_add_tail(&t[0], &m);

	spi_sync(dsp->spi_dev, &m);
	ret = len;

	return ret;
}

static ssize_t beo_hh1_dsp_data_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(dev);

	if (dsp->flash_enable)
		return -EINVAL;

	memcpy(buf, &dsp->dsp_lastdata, 4);
	return 4;
}


static DEVICE_ATTR(dsp_data, S_IRUGO | S_IWUSR,
			beo_hh1_dsp_data_show,
			beo_hh1_dsp_data_store);



static ssize_t beo_hh1_dsp_endtransaction_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(dev);
	int ret = -EINVAL;
	int bytes, l;
	u32 data = 0;
	u32 tmp[256];		// 1024 bytes

	if (dsp->flash_enable)
		return ret;

	sscanf(buf, "%d %d", &bytes, &data);
	dev_info(dev, "DSP endtransaction: send %d bytes\n", bytes);
	memset(tmp, data & 0xff, sizeof(tmp));

	while (bytes > 0) {
		l = (bytes > sizeof(tmp) ? sizeof(tmp) : bytes);
		spi_write(dsp->spi_dev, tmp, l);
		bytes -= l;
	}

	ret = len;
	return ret;
}

static DEVICE_ATTR(dsp_endtransaction, S_IWUSR,
			NULL,
			beo_hh1_dsp_endtransaction_store);




// dsp/flash switching
static ssize_t beo_hh1_flash_enable_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(dev);
	int ret = -EINVAL;

	if (len > 0 && strncmp(buf, "1", 1) == 0) {
		if (dsp->flash_enable == 0) {
			dev_info(dsp->dev, "Switching to FLASH mode...\n");
			// switch to flash
			if (gpio_is_valid(dsp->reset_gpio))
				gpio_set_value(dsp->reset_gpio, 1 ^ dsp->reset_gpio_active_low);
			if (gpio_is_valid(dsp->flash_gpio))
				gpio_set_value(dsp->flash_gpio, 1 ^ dsp->flash_gpio_active_low);

			// report mtd device
			beo_hh1_dsp_create_mtd(dsp);

			dsp->flash_enable = 1;
		}
		ret = len;
	}

	if (len > 0 && strncmp(buf, "0", 1) == 0) {
		if (dsp->flash_enable == 1) {
			dev_info(dsp->dev, "Switching to DSP mode...\n");
			// unregister mtd device
			mtd_device_unregister(&dsp->flash_mtd);

			// switch to dsp
			if (gpio_is_valid(dsp->flash_gpio))
				gpio_set_value(dsp->flash_gpio, 0 ^ dsp->flash_gpio_active_low);
			if (gpio_is_valid(dsp->reset_gpio))
				gpio_set_value(dsp->reset_gpio, 0 ^ dsp->reset_gpio_active_low);

			dsp->flash_enable = 0;
		}
		ret = len;
	}

	return ret;
}

static ssize_t beo_hh1_flash_enable_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(dev);
	int ret = 0;

	ret = snprintf(buf, PAGE_SIZE, "%d\n", dsp->flash_enable);

	return ret;
}

static DEVICE_ATTR(flash_enable, S_IRUGO | S_IWUSR,
			beo_hh1_flash_enable_show,
			beo_hh1_flash_enable_store);




static int beo_hh1_dsp_probe(struct spi_device *spi)
{
	struct beo_hh1_dsp *dsp;
	struct pinctrl *pinctrl;
	int ret = 0;
	int error, irqflags;
	enum of_gpio_flags flags;

	printk(KERN_ERR "beo_hh1_dsp_probe()\n");

	pinctrl = devm_pinctrl_get_select_default(&spi->dev);
	if (IS_ERR(pinctrl))
		dev_warn(&spi->dev,
			"pins are not configured from the driver\n");


	dsp = kzalloc(sizeof *dsp, GFP_KERNEL);
	if (!dsp)
		return -ENOMEM;

	dsp->dev = &spi->dev;
	dsp->spi_dev = spi;



	dsp->irq_gpio = of_get_named_gpio_flags(dsp->dev->of_node, "irq_gpios", 0, &flags);
	if (gpio_is_valid(dsp->irq_gpio)) {
		dsp->irq_gpio_active_low = flags & OF_GPIO_ACTIVE_LOW;

		error = gpio_request(dsp->irq_gpio, "HH1 DSP");
		if (error < 0) {
			dev_err(dsp->dev, "Failed to request GPIO %d, error %d\n",
				dsp->irq_gpio, error);
			ret = -ENODEV;
			goto out_free_dsp;
		}

		error = gpio_direction_input(dsp->irq_gpio);
		if (error < 0) {
			dev_err(dsp->dev,
				"Failed to configure direction for GPIO %d, error %d\n",
				dsp->irq_gpio, error);
			ret = -ENODEV;
			goto out_free_gpio;
		}

		dsp->irq = gpio_to_irq(dsp->irq_gpio);
		if (dsp->irq < 0) {
			dev_err(dsp->dev, "Unable to get irq number for GPIO %d, error %d\n",
					dsp->irq_gpio, dsp->irq);
			dsp->irq = -1;
			ret = -ENODEV;
			goto out_free_gpio;
		}

		irqflags = IRQF_TRIGGER_LOW;
		error = request_any_context_irq(dsp->irq, beo_hh1_dsp_isr, irqflags, "HH1 DSP irq", dsp);
		if (error < 0) {
			dev_err(dsp->dev, "Unable to claim irq %d; error %d\n",
				dsp->irq, error);
			ret = -ENODEV;
			goto out_free_gpio;
		}

		dev_info(dsp->dev, "Claimed irq %d on gpio %d\n", dsp->irq, dsp->irq_gpio);
	}
	else {
		dev_info(dsp->dev, "No irq gpio specified\n");
	}


	dsp->reset_gpio = of_get_named_gpio_flags(dsp->dev->of_node, "reset_gpios", 0, &flags);
	if (gpio_is_valid(dsp->reset_gpio)) {
		dsp->reset_gpio_active_low = flags & OF_GPIO_ACTIVE_LOW;
		if (setup_gpio_out(dsp->dev, dsp->reset_gpio, "HH1 DSP reset", dsp->reset_gpio_active_low, 0) == 0) {
			dev_info(dsp->dev, "Using gpio %d for reset\n", dsp->reset_gpio);
		}
		else {
			dev_err(dsp->dev, "Failed to claim gpio %d for reset\n", dsp->reset_gpio);
			goto out_free_irq;
		}
	}
	else {
		dev_info(dsp->dev, "No reset gpio specified\n");
	}

	dsp->flash_gpio = of_get_named_gpio_flags(dsp->dev->of_node, "flash_gpios", 0, &flags);
	if (gpio_is_valid(dsp->flash_gpio)) {
		dsp->flash_gpio_active_low = flags & OF_GPIO_ACTIVE_LOW;
		if (setup_gpio_out(dsp->dev, dsp->flash_gpio, "HH1 DSP flash", dsp->flash_gpio_active_low, 0) == 0) {
			dev_info(dsp->dev, "Using gpio %d for flash\n", dsp->flash_gpio);
		}
		else {
			dev_err(dsp->dev, "Failed to claim gpio %d for flash\n", dsp->flash_gpio);
			goto out_free_irq;
		}
	}
	else {
		dev_info(dsp->dev, "No flash gpio specified\n");
	}


	dev_set_drvdata(&spi->dev, dsp);
	dev_info(dsp->dev, "detected chip\n");


	mutex_init(&dsp->flash_lock);


	ret = device_create_file(&spi->dev, &dev_attr_flash_enable);
	ret = device_create_file(&spi->dev, &dev_attr_dsp_data);
	ret = device_create_file(&spi->dev, &dev_attr_dsp_endtransaction);



	return ret;

out_free_irq:
	free_irq(dsp->irq, dsp);

out_free_gpio:
	if (gpio_is_valid(dsp->irq_gpio))
		gpio_free(dsp->irq_gpio);

out_free_dsp:
	kfree(dsp);

	return ret;
}

static int beo_hh1_dsp_remove(struct spi_device *spi)
{
	struct beo_hh1_dsp *dsp = dev_get_drvdata(&spi->dev);
	printk(KERN_ERR "beo_hh1_dsp_remove()\n");

	device_remove_file(&spi->dev, &dev_attr_flash_enable);
	device_remove_file(&spi->dev, &dev_attr_dsp_data);
	device_remove_file(&spi->dev, &dev_attr_dsp_endtransaction);


	if (dsp->flash_enable)
		mtd_device_unregister(&dsp->flash_mtd);


	if (dsp->irq > 0)
		free_irq(dsp->irq, dsp);

	if (gpio_is_valid(dsp->irq_gpio))
		gpio_free(dsp->irq_gpio);

	if (gpio_is_valid(dsp->reset_gpio))
		gpio_free(dsp->reset_gpio);

	if (gpio_is_valid(dsp->flash_gpio))
		gpio_free(dsp->flash_gpio);

	kfree(dsp);

	return 0;
}


static const struct of_device_id my_of_ids[] = {
	{ .compatible = "beo,beo-hh1-dsp", },
	{ }
};

static struct spi_driver beo_hh1_dsp_driver = {
	.driver = {
		.name = "beo-hh1-dsp",
		.owner = THIS_MODULE,
		.of_match_table = my_of_ids,
	},
	.probe	= beo_hh1_dsp_probe,
	.remove	= beo_hh1_dsp_remove,
};

MODULE_DEVICE_TABLE(of, my_of_ids);

static int __init beo_hh1_dsp_init(void)
{
	printk(KERN_ERR "beo_hh1_dsp_init()\n");
	return spi_register_driver(&beo_hh1_dsp_driver);
}

static void __exit beo_hh1_dsp_exit(void)
{
	printk(KERN_ERR "beo_hh1_dsp_exit()\n");
	spi_unregister_driver(&beo_hh1_dsp_driver);
}


subsys_initcall(beo_hh1_dsp_init);
module_exit(beo_hh1_dsp_exit);

MODULE_AUTHOR("Vladimir Koutny <vladimir.koutny@streamunlimited.com>");
MODULE_DESCRIPTION("BeO HH1 DSP/flash driver");
MODULE_LICENSE("GPL v2");
