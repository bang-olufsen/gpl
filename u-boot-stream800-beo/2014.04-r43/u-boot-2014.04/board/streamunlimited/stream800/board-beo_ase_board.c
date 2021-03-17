/*
 * board-beo_ase_board.c
 *
 * Copyright (C) 2013, StreamUnlimited Engineering GmbH, http://www.streamunlimited.com/
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

#include "board-beo_ase_board.h"

/*
 * "B&O ASE" - board specific initialization
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <phy.h>
#include <asm/arch-am33xx/phy.h>
#include <asm-generic/gpio.h>
#include <asm/arch/mux.h>
#include <spi.h>
#include <i2c.h>
#include "ase_fep.h"
#include "board.h"

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;
static struct ase_fep *fep_mcu = NULL;
static int update_flag = 0;

#define GPIO_PHY_RESET (60) /* gpio1_28 */

#define GPIO_DEBUG_LED (58) /* gpio1_26 */

static struct module_pin_mux gpio_pin_mux[] = {
	{OFFSET(xdma_event_intr1), MODE(7)},
	{OFFSET(xdma_event_intr0), MODE(7)},
	{OFFSET(gpmc_ad8),  (MODE(7) & (~RXACTIVE))},
	{OFFSET(gpmc_be1n), MODE(7)},
	{OFFSET(gpmc_a10), MODE(7)},
	{-1},
};

void beo_ase__init(const struct Stream800Board *board)
{
	if (!board || board->moduleVersion != MV_BeoASE)
		BUG();

	configure_module_pin_mux(gpio_pin_mux);

	/* enable debug led */
	gpio_direction_output(GPIO_DEBUG_LED, 1);
}

void beo_ase__late_init(const struct Stream800Board *board)
{
	if (!board || board->moduleVersion != MV_BeoASE)
		BUG();

	printf("TODO: 'B&O ASE' board late initialization.\n");
}

#ifdef CONFIG_DRIVER_TI_CPSW
static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,	// CPSW_SL1
		.phy_addr		= 4,
		.phy_if			= PHY_INTERFACE_MODE_RMII
	},
};

void beo_ase__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data)
{
	if (!board || board->moduleVersion != MV_BeoASE)
		BUG();

	/* setup phy reset direction and level (reset active - HIGH) */
	gpio_direction_output(GPIO_PHY_RESET, 0);
	udelay(10000);

	cpsw_data->slaves = 1;
	cpsw_data->slave_data = cpsw_slaves;

	// Advisory 1.0.10
	// "The AM335x device does not support internal delay mode, so RGMII1_IDMODE and RGMII2_IDMODE must be set to 1b."
	writel(RMII1_MODE_ENABLE | MII1_CLK_FROM_CHIP_PIN | RGMII1_NO_INTERNAL_DELAY
		| MII2_MODE_NOT_USED , &cdev->miisel);
}
#endif /* CONFIG_DRIVER_TI_CPSW */

int beo_ase__get_fw_update_button_status(void)
{
	if (!fep_mcu) {
		printf("Fep isn't inicialized yet\n");
		update_flag = 1;
		return update_flag;
	}

	return is_pressed_update_button(fep_mcu);
}

void beo_ase__on_board_state_changed(const struct Stream800Board *board, BoardState state)
{
	printf("Board state changed to '%s'\n", boardStateStrings[state]);

	if (!fep_mcu) {
		printf("Fep isn't inicialized yet\n");
		return;
	}

	switch (state) {
	case BS_DontUnplug:
		set_flashing_on_state_ase_fep(fep_mcu);
		break;
	case BS_BootingKernel:
		/* We power on the led only at boot withouth firmware update */
		if (bootcount_load() <= 1) {
			set_boot_state_ase_fep(fep_mcu, board);
		}
		break;
	case BS_HardFailure:
//		set_flashing_off_state_ase_fep(fep_mcu);
		set_error_state_ase_fep(fep_mcu);
		break;
	default:
		break;
	}
}

int beo_ase__do_usb_update(void)
{
	return (beo_ase__get_fw_update_button_status()) ? 1 : 0 ;	// fw update button -> check update file on USB
}


static struct StreamBoardInterface beo_ase_board_interface = {
	.init = beo_ase__init,
	.late_init = beo_ase__late_init,
	.cpsw_eth_init = beo_ase__cpsw_eth_init,
	.on_board_state_changed = beo_ase__on_board_state_changed,
	.do_usb_update = beo_ase__do_usb_update,

	.has_rmii1 = 1,
};

void beo_ase__set_board_interface(void) {
	set_board_interface(&beo_ase_board_interface);
}

void beo_ase__set_ase_fep(struct ase_fep *fep) {
	fep_mcu = fep;
}
