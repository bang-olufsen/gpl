/*
 * board-spl.c
 *
 * Board functions for TI AM335X based boards
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
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

#include <common.h>
#include <cpsw.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <i2c.h>
#include "adc.h"
#include "board_defines.h"
#include "spl.h"
#include "mux.h"

#ifndef CONFIG_SPL_BUILD
#error This file should be built only for SPL
#endif

DECLARE_GLOBAL_DATA_PTR;

static const struct ddr_data ddr3_ase_data = {
	.datardsratio0 = MT41K256M16HA125E_RD_DQS,
	.datawdsratio0 = MT41K256M16HA125E_WR_DQS,
	.datafwsratio0 = MT41K256M16HA125E_PHY_FIFO_WE,
	.datawrsratio0 = MT41K256M16HA125E_PHY_WR_DATA,
};

static const struct cmd_control ddr3_ase_cmd_ctrl_data = {
	.cmd0csratio = MT41K256M16HA125E_RATIO,
	.cmd0iclkout = MT41K256M16HA125E_INVERT_CLKOUT,

	.cmd1csratio = MT41K256M16HA125E_RATIO,
	.cmd1iclkout = MT41K256M16HA125E_INVERT_CLKOUT,

	.cmd2csratio = MT41K256M16HA125E_RATIO,
	.cmd2iclkout = MT41K256M16HA125E_INVERT_CLKOUT,
};

static struct emif_regs ddr3_ase_emif_reg_data = {
	.sdram_config = MT41K256M16HA125E_EMIF_SDCFG,
	.ref_ctrl = MT41K256M16HA125E_EMIF_SDREF,
	.sdram_tim1 = MT41K256M16HA125E_EMIF_TIM1,
	.sdram_tim2 = MT41K256M16HA125E_EMIF_TIM2,
	.sdram_tim3 = MT41K256M16HA125E_EMIF_TIM3,
	.zq_config = MT41K256M16HA125E_ZQ_CFG,
	.emif_ddr_phy_ctlr_1 = MT41K256M16HA125E_EMIF_READ_LATENCY,
};

const struct ctrl_ioregs ioregs_ase = {
	.cm0ioctl		= MT41K256M16HA125E_IOCTRL_VALUE,
	.cm1ioctl		= MT41K256M16HA125E_IOCTRL_VALUE,
	.cm2ioctl		= MT41K256M16HA125E_IOCTRL_VALUE,
	.dt0ioctl		= MT41K256M16HA125E_IOCTRL_VALUE,
	.dt1ioctl		= MT41K256M16HA125E_IOCTRL_VALUE,
};

/* Definitions for DDR3 */

/* Micron MT41K128M16JT-125E 2Gbit on Stream800DDR3 @ 303MHz */
#define MT41K128M16JT125E_303_EMIF_READ_LATENCY 0x100006
#define MT41K128M16JT125E_303_EMIF_TIM1		0x0888A39B
#define MT41K128M16JT125E_303_EMIF_TIM2		0x26337FDA
#define MT41K128M16JT125E_303_EMIF_TIM3		0x501F830F
#define MT41K128M16JT125E_303_EMIF_SDCFG	0x61C14AB2
#define MT41K128M16JT125E_303_EMIF_SDREF	0x0000093B
#define MT41K128M16JT125E_303_ZQ_CFG		0x50074BE4
#define MT41K128M16JT125E_303_RATIO		0x80
#define MT41K128M16JT125E_303_INVERT_CLKOUT	0x0
#define MT41K128M16JT125E_303_RD_DQS		0x40
#define MT41K128M16JT125E_303_WR_DQS		0x40
#define MT41K128M16JT125E_303_PHY_WR_DATA	0x80
#define MT41K128M16JT125E_303_PHY_FIFO_WE	0x58
#define MT41K128M16JT125E_303_IOCTRL_VALUE	0x37B

/* Micron MT41K128M16JT-125E 2Gbit on Stream800DDR3 @ 400MHz */
#define MT41K128M16JT125E_400_EMIF_READ_LATENCY 0x100007
#define MT41K128M16JT125E_400_EMIF_TIM1		0x0AAAD4DB
#define MT41K128M16JT125E_400_EMIF_TIM2		0x26437FDA
#define MT41K128M16JT125E_400_EMIF_TIM3		0x501F83FF
#define MT41K128M16JT125E_400_EMIF_SDCFG	0x61C152B2
#define MT41K128M16JT125E_400_EMIF_SDREF	0x00000C30
#define MT41K128M16JT125E_400_ZQ_CFG		0x50074BE4
#define MT41K128M16JT125E_400_RATIO		0x80
#define MT41K128M16JT125E_400_INVERT_CLKOUT	0x0
#define MT41K128M16JT125E_400_RD_DQS		0x40
#define MT41K128M16JT125E_400_WR_DQS		0x40
#define MT41K128M16JT125E_400_PHY_WR_DATA	0x80
#define MT41K128M16JT125E_400_PHY_FIFO_WE	0x60
#define MT41K128M16JT125E_400_IOCTRL_VALUE	0x37B

static const struct ddr_data ddr3_data = {
	.datardsratio0	= MT41K128M16JT125E_303_RD_DQS,
	.datawdsratio0	= MT41K128M16JT125E_303_WR_DQS,
	.datafwsratio0	= MT41K128M16JT125E_303_PHY_FIFO_WE,
	.datawrsratio0	= MT41K128M16JT125E_303_PHY_WR_DATA,
};

static const struct cmd_control ddr3_cmd_ctrl_data = {
	.cmd0csratio	= MT41K128M16JT125E_303_RATIO,
	.cmd0iclkout	= MT41K128M16JT125E_303_INVERT_CLKOUT,

	.cmd1csratio	= MT41K128M16JT125E_303_RATIO,
	.cmd1iclkout	= MT41K128M16JT125E_303_INVERT_CLKOUT,

	.cmd2csratio	= MT41K128M16JT125E_303_RATIO,
	.cmd2iclkout	= MT41K128M16JT125E_303_INVERT_CLKOUT,
};

static const struct ctrl_ioregs ioregs_ddr3 = {
	.cm0ioctl	= MT41K128M16JT125E_303_IOCTRL_VALUE,
	.cm1ioctl	= MT41K128M16JT125E_303_IOCTRL_VALUE,
	.cm2ioctl	= MT41K128M16JT125E_303_IOCTRL_VALUE,
	.dt0ioctl	= MT41K128M16JT125E_303_IOCTRL_VALUE,
	.dt1ioctl	= MT41K128M16JT125E_303_IOCTRL_VALUE,
};

////////////////////////////////////////////////////////////////////////////////

void set_mux_conf_regs(void)
{
}

void set_uart_mux_conf(void)
{
#ifdef CONFIG_SERIAL1
	enable_uart0_pin_mux();
#endif /* CONFIG_SERIAL1 */
#ifdef CONFIG_SERIAL2
	enable_uart1_pin_mux();
#endif /* CONFIG_SERIAL2 */
#ifdef CONFIG_SERIAL3
	enable_uart2_pin_mux();
#endif /* CONFIG_SERIAL3 */
#ifdef CONFIG_SERIAL4
	enable_uart3_pin_mux();
#endif /* CONFIG_SERIAL4 */
#ifdef CONFIG_SERIAL5
	enable_uart4_pin_mux();
#endif /* CONFIG_SERIAL5 */
#ifdef CONFIG_SERIAL6
	enable_uart5_pin_mux();
#endif /* CONFIG_SERIAL6 */
}

void sdram_init(void)
{
	config_ddr(400, &ioregs_ase,
			   &ddr3_ase_data,
			   &ddr3_ase_cmd_ctrl_data,
			   &ddr3_ase_emif_reg_data, 0);

}

#define OSC	(V_OSCK/1000000)
const struct dpll_params dpll_ddr = {
		400, OSC-1, 1, -1, -1, -1, -1};

const struct dpll_params *get_dpll_ddr_params(void)
{
	return &dpll_ddr;
}
