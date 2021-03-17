/*
 * adc.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "adc.h"

/*
 * ADC functions for AM33xx touchscreen controller
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>

#define PRCM_MOD_EN             0x2

#define TSC_ADC_SS_REG_BASE		0x44E0D000

// TSC_ADC_SS Registers — Described in TI AM335x Sitara Processes Technical
// Reference Manual, pg 1745.
#define REG_REVISION			(0x00)
#define REG_SYSCONFIG			(0x10)
#define REG_IRQSTATUS_RAW		(0x24)
#define REG_IRQSTATUS			(0x28)
#define REG_IRQENABLE_SET 		(0x2c)
#define REG_IRQENABLE_CLR 		(0x30)
#define REG_IRQWAKEUP			(0x34)
#define REG_DMAENABLE_SET		(0x38)
#define REG_DMAENABLE_CLR		(0x3c)
#define REG_CTRL			(0x40)
#define REG_ADCSTAT			(0x44)
#define REG_ADCRANGE			(0x48)
#define REG_ADC_CLKDIV			(0x4c)
#define REG_ADC_MISC			(0x50)
#define REG_STEPENABLE			(0x54)
#define REG_IDLECONFIG			(0x58)
#define REG_TS_CHARGE_STEPCONFIG	(0x5c)
#define REG_TS_CHARGE_DELAY		(0x60)

#define REG_STEPCONFIG(X)			(0x64 + ((X) * 8))
#define REG_STEPCONFIG_FIFO_SEL			(1 << 26)
#define REG_STEPCONFIG_DIFF_CNTRL		(1 << 25)
#define REG_STEPCONFIG_SEL_RFM_SWC_1_0(X)	(X << 23)
#define REG_STEPCONFIG_SEL_INP_SWC_3_0(X)	(X << 19)
#define REG_STEPCONFIG_SEL_INM_SWC_3_0(X)	(X << 15)
#define REG_STEPCONFIG_SEL_RFP_SWC_2_0(X)	(X << 12)
#define REG_STEPCONFIG_WPNSW_SWC		(1 << 11)
#define REG_STEPCONFIG_YPNSW_SWC		(1 << 10)
#define REG_STEPCONFIG_XNPSW_SWC		(1 << 9)
#define REG_STEPCONFIG_YNNSW_SWC		(1 << 8)
#define REG_STEPCONFIG_YPPSW_SWC		(1 << 7)
#define REG_STEPCONFIG_XNNSW_SWC		(1 << 6)
#define REG_STEPCONFIG_XPPSW_SWC		(1 << 5)
#define REG_STEPCONFIG_AVG_0			(0 << 2)
#define REG_STEPCONFIG_AVG_2			(1 << 2)
#define REG_STEPCONFIG_AVG_4			(2 << 2)
#define REG_STEPCONFIG_AVG_8			(3 << 2)
#define REG_STEPCONFIG_AVG_16			(4 << 2)
#define REG_STEPCONFIG_MODE_SW_ONESHOT		(0)
#define REG_STEPCONFIG_MODE_SW_CONT		(1)
#define REG_STEPCONFIG_MODE_HW_ONESHOT		(2)
#define REG_STEPCONFIG_MODE_HW_CONT		(3)

#define REG_STEPDELAY(X)		(0x68	+ ((X) * 8))
#define REG_FIFOCOUNT(X)		(0xe4 	+ ((X) * 0xc))
#define REG_FIFOTHRESHOLD(X)		(0xe8	+ ((X) * 0xc))
#define REG_DMAREQ(X)			(0xec	+ ((X) * 0xc))
#define REG_FIFODATA(X)			(0x100	+ ((X) * 0x100))

/* Sequencer Status */
#define SEQ_STATUS BIT(5)

//static const struct cm_wkuppll *cmwkup = (struct cm_wkuppll *)CM_WKUP;

static unsigned long int adc_readl(unsigned long reg)
{
	return readl(TSC_ADC_SS_REG_BASE + reg);
}

static void adc_writel(unsigned long val, unsigned long reg)
{
	writel(val, TSC_ADC_SS_REG_BASE + reg);
}

int init_adc(void)
{
	/* enable hardware module */
	writel(PRCM_MOD_EN, &cmwkup->wkup_adctscctrl);
	while (readl(&cmwkup->wkup_adctscctrl) != PRCM_MOD_EN);

	printf("%s(): HW revision 0x%08lx\n", __func__, adc_readl(REG_REVISION));

	return 0;
}

int read_adc_channel(int channel)
{
	/*
	 * Based on comment from the linux driver (ti_am335x_tscadc.h), it
	 * takes 5µs to process one sample.  The question then is how fast can
	 * our loop below run; a guesstimate would be that our processor is
	 * running at a max of 1GHz, and in that case the entire loop body
	 * would have to run in 5 cycles, for the timout to occur inside of
	 * 5µs.  The code in that loop takes a lot longer than 5 cycles to run,
	 * so the timeout of 1000 should be very conservative.  Comment from
	 * Linux driver: ADC runs at 3MHz, and it takes 15 cycles to latch one
	 * data output.  Hence the idle time for ADC to process one sample data
	 * would be around 5 micro seconds.
	 */
	unsigned timeout = 1000;
	/* make step config regs writeable */
	adc_writel(1 << 2, REG_CTRL);
	/* use step #1 only */
	adc_writel(REG_STEPCONFIG_AVG_16 |
		   REG_STEPCONFIG_FIFO_SEL |
		   REG_STEPCONFIG_MODE_SW_ONESHOT |
		   REG_STEPCONFIG_SEL_INP_SWC_3_0(channel),
		   REG_STEPCONFIG(0));
	/* enable charge and step1 */
	adc_writel(1 << 1, REG_STEPENABLE);
	/* set delay time between 16 measurements */
	adc_writel((32 << 24) + 32 , REG_STEPDELAY(0));
	/* GO! */
	adc_writel(1 << 0, REG_CTRL);

	while (timeout-- > 0) {
		int fifo;

		/* Wait for ADC sequencer to complete sampling */
		if (adc_readl(REG_ADCSTAT) & SEQ_STATUS)
			continue;
		for (fifo = 0; fifo < 2; fifo++)
			if ((adc_readl(REG_FIFOCOUNT(fifo)) > 0))
				return adc_readl(REG_FIFODATA(fifo)) & 0xfff;
		//printf("waiting ... state %08lx\n", adc_readl(REG_ADCSTAT));
	}
	printf("Read from ADC channel: %d timed out!\n", channel);
	return -1UL;
}
