/*
 * board-sue_first_carrier_board.c
 *
 * Copyright (C) 2012, StreamUnlimited Engineering GmbH, http://www.streamunlimited.com/
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

/*
 * "SUE Carrier Board" - board specific initialization
 */

#include "board-sue_first_carrier_board.h"
#include <common.h>
#include <cpsw.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <phy.h>
#include <asm/arch-am33xx/phy.h>

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

// "SUE Carrier Board" board initialization
void first_carrier__init(const struct Stream800Board *board)
{
	if (!board || board->carrierBoardType != CBT_SueFirstCarrierBoard)
		BUG();

	printf("TODO: 'SUE First Carrier' initialization.\n");
}

// "SUE First Carrier Board" board late initialization
void first_carrier__late_init(const struct Stream800Board *board)
{
	if (!board || board->carrierBoardType != CBT_SueFirstCarrierBoard)
		BUG();

	printf("TODO: 'SUE First Carrier' board late initialization.\n");
}

#ifdef CONFIG_DRIVER_TI_CPSW
static struct cpsw_slave_data cpsw_slaves[] = {
	// TODO: check this configuration for the SUE Carrier Board board
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,	// CPSW_SL1
		.phy_addr		= 4,
		.phy_if			= PHY_INTERFACE_MODE_RMII
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,	// CPSW_SL2
		.phy_addr		= 0,
		.phy_if			= PHY_INTERFACE_MODE_RGMII
	}
};

void first_carrier__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data)
{
	if (!board || board->carrierBoardType != CBT_SueFirstCarrierBoard)
		BUG();

	cpsw_data->slaves = 2;
	cpsw_data->slave_data = cpsw_slaves;

	// Advisory 1.0.10
	// "The AM335x device does not support internal delay mode, so RGMII1_IDMODE and RGMII2_IDMODE must be set to 1b."
	writel(RMII1_MODE_ENABLE | MII1_CLK_FROM_CHIP_PIN | RGMII1_NO_INTERNAL_DELAY
		| RGMII2_MODE_ENABLE | RGMII2_NO_INTERNAL_DELAY , &cdev->miisel);
}
#endif /* CONFIG_DRIVER_TI_CPSW */

void first_carrier__on_board_state_changed(const struct Stream800Board *board, BoardState state)
{
	printf("TODO: Board state changed to '%s'\n", boardStateStrings[state]);
}

int first_carrier__do_usb_update(void)
{
	return 1;
}

static struct StreamBoardInterface carrier_board_interface = {
	.init = first_carrier__init,
	.late_init = first_carrier__late_init,
	.cpsw_eth_init = first_carrier__cpsw_eth_init,
	.on_board_state_changed = first_carrier__on_board_state_changed,
	.do_usb_update = first_carrier__do_usb_update,

	.has_rmii1 = 1,
	.has_rgmii2 = 1,
	.has_mmc1 = 1,
};

void first_carrier__set_board_interface(void) {
	set_board_interface(&carrier_board_interface);
}
