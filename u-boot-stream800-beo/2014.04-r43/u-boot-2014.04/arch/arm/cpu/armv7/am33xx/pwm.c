/*
 * Copyright (C) 2013 StreamUnlimited Engineering GmbH
 *
 * Marek Belisko <marek.belisko@streamunlimited.com>
 *
 * Based on kernel ehrpwm driver:
 * Copyright (C) 2012 Texas Instruments, Inc. - http://www.ti.com/
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <errno.h>
#include <pwm.h>
#include <div64.h>
#include <asm/io.h>
#include <asm/arch/pwm.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>

#define PERIOD_MAX 0xFFFF
#define NSEC_PER_SEC 1000000000L
#define CLK_RATE (100000000)

/* We have 3 pwm all have 2 channels */
#define PWM_CHANNEL_MAX 6

#define VALID_PWM(id) ((id >= 0) && (id < PWM_CHANNEL_MAX))

/* hold platform data for every ehrpwm module */
struct ehrpwm_chip pwm_chip[PWM_CHANNEL_MAX];

/* tbclk clock enable count */
int tbclk[PWM_CHANNEL_MAX / 2] = { 0, 0, 0 };

static struct am33xx_pwm *get_base(int pwm_id)
{
	int index = pwm_id / 2;

	if (index == 0)
		return (struct am33xx_pwm *)EHRPWM0_BASE_ADDR;
	else if (index == 1)
		return (struct am33xx_pwm *)EHRPWM1_BASE_ADDR;
	else if (index == 2)
		return (struct am33xx_pwm *)EHRPWM2_BASE_ADDR;
	else
		return NULL;
}

static u16 ehrpwm_read(void *base, int offset)
{
	u16 val;

	val = readw(base + offset);

	return val;
}

static void ehrpwm_write(void *base, int offset, unsigned int val)
{
	writew(val & 0xFFFF, base + offset);
}

static void ehrpwm_modify(void *base, int offset,
                unsigned short mask, unsigned short val)
{
	unsigned short regval;

	regval = ehrpwm_read(base, offset);
	regval &= ~mask;
	regval |= val & mask;
	ehrpwm_write(base, offset, regval);
}

static void pwmss_ctrl(int pwm_id, int enable)
{
	int val = readl(CTRL_BASE + 0x664);
	int index = pwm_id / 2;

	if (enable) {
		val |= (1 << index);
		tbclk[index]++;
	} else {
		val &= ~(1 << index);
		if (tbclk[index] == 0)
			return;

		if (--tbclk[index] > 0)
			return;
	}

	writel(val, CTRL_BASE + 0x664);
}

static void configure_polarity(int pwm_id)
{
	struct am33xx_pwm *pwm = get_base(pwm_id);
	int aqctl_reg;
	unsigned short aqctl_val, aqctl_mask;

	/*
	 * Configure PWM output to HIGH/LOW level on counter
	 * reaches compare register value and LOW/HIGH level
	 * on counter value reaches period register value and
	 * zero value on counter
	 */
	if (pwm_id % 2) {
		aqctl_reg = AQCTLB;
		aqctl_mask = AQCTL_CBU_MASK;

		if (pwm_chip[pwm_id].polarity == 1)
			aqctl_val = AQCTL_CHANB_POLINVERSED;
		else
			aqctl_val = AQCTL_CHANB_POLNORMAL;
	} else {
		aqctl_reg = AQCTLA;
		aqctl_mask = AQCTL_CAU_MASK;

		if (pwm_chip[pwm_id].polarity == 1)
			aqctl_val = AQCTL_CHANA_POLINVERSED;
		else
			aqctl_val = AQCTL_CHANA_POLNORMAL;
	}

	aqctl_mask |= AQCTL_PRD_MASK | AQCTL_ZRO_MASK;
	ehrpwm_modify(pwm, aqctl_reg, aqctl_mask, aqctl_val);
}

int pwm_enable(int pwm_id)
{
	struct am33xx_pwm *pwm = get_base(pwm_id);
	unsigned short aqcsfrc_val, aqcsfrc_mask;

	if (!VALID_PWM(pwm_id)) {
		printf("Allowed pwm id is out of boundary (max 5).\n");
		return -1;
	}

	printf("%s - id:%d\n", __func__, pwm_id);

	/* Disabling Action Qualifier on PWM output */
	if (pwm_id % 2) {
		aqcsfrc_val = AQCSFRC_CSFB_FRCDIS;
		aqcsfrc_mask = AQCSFRC_CSFB_MASK;
	} else {
		aqcsfrc_val = AQCSFRC_CSFA_FRCDIS;
		aqcsfrc_mask = AQCSFRC_CSFA_MASK;
	}

	/* Changes to shadow mode */
	ehrpwm_modify(pwm, AQSFRC, AQSFRC_RLDCSF_MASK,
			AQSFRC_RLDCSF_ZRO);

	ehrpwm_modify(pwm, AQCSFRC, aqcsfrc_mask, aqcsfrc_val);

	/* Channels polarity can be configured from action qualifier module */
	configure_polarity(pwm_id);

	/* enable clocks */
	pwmss_ctrl(pwm_id, 1);

	/* Enable time counter for free_run */
	ehrpwm_modify(pwm, TBCTL, TBCTL_RUN_MASK, TBCTL_FREE_RUN);

	return 0;
}

void pwm_disable(int pwm_id)
{
	struct am33xx_pwm *pwm = get_base(pwm_id);
	unsigned short aqcsfrc_val, aqcsfrc_mask;

	if (!VALID_PWM(pwm_id)) {
		printf("Allowed pwm id is out of boundary (max 5).\n");
		return;
	}

	printf("%s - id:%d\n", __func__, pwm_id);

	/* Action Qualifier puts PWM output low forcefully */
	if (pwm_id % 2) {
		aqcsfrc_val = AQCSFRC_CSFB_FRCLOW;
		aqcsfrc_mask = AQCSFRC_CSFB_MASK;
	} else {
		aqcsfrc_val = AQCSFRC_CSFA_FRCLOW;
		aqcsfrc_mask = AQCSFRC_CSFA_MASK;
	}

	/*
	 * Changes to immediate action on Action Qualifier. This puts
	 * Action Qualifier control on PWM output from next TBCLK
	 */
	ehrpwm_modify(pwm, AQSFRC, AQSFRC_RLDCSF_MASK,
			AQSFRC_RLDCSF_IMDT);

	ehrpwm_modify(pwm, AQCSFRC, aqcsfrc_mask, aqcsfrc_val);

	/* Clear to avoid enabling both channels when only one is requested */
	if (pwm_id % 2)
		ehrpwm_write(pwm, AQCTLB, 0);
	else
		ehrpwm_write(pwm, AQCTLA, 0);

	/* Disable clock */
	pwmss_ctrl(pwm_id, 0);

	/* Stop Time base counter */
	ehrpwm_modify(pwm, TBCTL, TBCTL_RUN_MASK, TBCTL_STOP_NEXT);
}

/**
 * set_prescale_div -   Set up the prescaler divider function
 * @rqst_prescaler:     prescaler value min
 * @prescale_div:       prescaler value set
 * @tb_clk_div:         Time Base Control prescaler bits
 */
static int set_prescale_div(unsigned long rqst_prescaler,
		unsigned short *prescale_div, unsigned short *tb_clk_div)
{
	unsigned int clkdiv, hspclkdiv;

	for (clkdiv = 0; clkdiv <= CLKDIV_MAX; clkdiv++) {
		for (hspclkdiv = 0; hspclkdiv <= HSPCLKDIV_MAX; hspclkdiv++) {

			/*
			 * calculations for prescaler value :
			 * prescale_div = HSPCLKDIVIDER * CLKDIVIDER.
			 * HSPCLKDIVIDER =  2 ** hspclkdiv
			 * CLKDIVIDER = (1),            if clkdiv == 0 *OR*
			 *              (2 * clkdiv),   if clkdiv != 0
			 *
			 * Configure prescale_div value such that period
			 * register value is less than 65535.
			 */

			*prescale_div = (1 << clkdiv) *
					(hspclkdiv ? (hspclkdiv * 2) : 1);
			if (*prescale_div > rqst_prescaler) {
				*tb_clk_div = (clkdiv << TBCTL_CLKDIV_SHIFT) |
					(hspclkdiv << TBCTL_HSPCLKDIV_SHIFT);
				return 0;
			}
		}
	}
	return 1;
}

int pwm_config(int pwm_id, int duty_ns, int period_ns)
{
	struct am33xx_pwm *pwm = get_base(pwm_id);
	unsigned long long c;
	unsigned long period_cycles, duty_cycles;
	unsigned short ps_divval, tb_divval;
	int cmp_reg;

	if (!VALID_PWM(pwm_id)) {
		printf("Allowed pwm id is out of boundary (max 5).\n");
		return -1;
	}

	c = CLK_RATE;
	c = c * period_ns;
	do_div(c, NSEC_PER_SEC);
	period_cycles = (unsigned long)c;

	if (period_cycles < 1) {
		period_cycles = 1;
		duty_cycles = 1;
	} else {
		c = CLK_RATE;
		c = c * duty_ns;
		do_div(c, NSEC_PER_SEC);
		duty_cycles = (unsigned long)c;
	}

	if (set_prescale_div(period_cycles/PERIOD_MAX, &ps_divval,
				&tb_divval)) {
		printf("Unsupported values\n");
		return -EINVAL;
	}

	/* Update clock prescaler values */
	ehrpwm_modify(pwm, TBCTL, TBCTL_CLKDIV_MASK, tb_divval);
	/* Update period & duty cycle with presacler division */
	period_cycles = period_cycles / ps_divval;
	duty_cycles = duty_cycles / ps_divval;

	/* Configure shadow loading on Period register */
	ehrpwm_modify(pwm, TBCTL, TBCTL_PRDLD_MASK, TBCTL_PRDLD_SHDW);

	ehrpwm_write(pwm, TBPRD, period_cycles);

	/* Configure ehrpwm counter for up-count mode */
	ehrpwm_modify(pwm, TBCTL, TBCTL_CTRMODE_MASK,
		TBCTL_CTRMODE_UP);

	if (pwm_id % 2)
		/* Channel 1 configured with compare B register */
		cmp_reg = CMPB;
	else
		/* Channel 0 configured with compare A register */
		cmp_reg = CMPA;

	ehrpwm_write(pwm, cmp_reg, duty_cycles);

	printf("%s - duty_ns:%d, period_ns:%d\n", __func__, duty_ns, period_ns);

	return 0;
}

int pwm_init(int pwm_id, int div, int invert)
{
	if (!VALID_PWM(pwm_id)) {
		printf("Allowed pwm id is out of boundary (max 5).\n");
		return -1;
	}

	pwm_chip[pwm_id].polarity = invert;
	pwm_chip[pwm_id].div = div;

	return 0;
}
