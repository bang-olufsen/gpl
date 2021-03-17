/*
 * board-sue_factory_test_board.c
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

#include "board-sue_factory_test_board.h"
#include <common.h>
#include <cpsw.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <phy.h>
#include <asm/arch-am33xx/phy.h>

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

// "SUE Factory Test Board" board initialization
void factory_test__init(const struct Stream800Board *board)
{
	if (!board || board->carrierBoardType != CBT_SueFactoryTestBoard)
		BUG();

	printf("TODO: 'SUE Factory Test' board initialization.\n");
}

// "SUE Factory Test" board late initialization
void factory_test__late_init(const struct Stream800Board *board)
{
	if (!board || board->carrierBoardType != CBT_SueFactoryTestBoard)
		BUG();

	printf("TODO: 'SUE Factory Test' board late initialization.\n");
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

void factory_test__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data)
{
	if (!board || board->carrierBoardType != CBT_SueFactoryTestBoard)
		BUG();

	cpsw_data->slaves = 2;
	cpsw_data->slave_data = cpsw_slaves;

	// Advisory 1.0.10
	// "The AM335x device does not support internal delay mode, so RGMII1_IDMODE and RGMII2_IDMODE must be set to 1b."
	writel(RMII1_MODE_ENABLE | MII1_CLK_FROM_CHIP_PIN | RGMII1_NO_INTERNAL_DELAY
		| RGMII2_MODE_ENABLE | RGMII2_NO_INTERNAL_DELAY , &cdev->miisel);
}
#endif /* CONFIG_DRIVER_TI_CPSW */

void factory_test__on_board_state_changed(const struct Stream800Board *board, BoardState state)
{
	printf("TODO: Board state changed to '%s'\n", boardStateStrings[state]);
}

static struct StreamBoardInterface factory_test_board_interface = {
	.init = factory_test__init,
	.late_init = factory_test__late_init,
	.cpsw_eth_init = factory_test__cpsw_eth_init,
	.on_board_state_changed = factory_test__on_board_state_changed,

	.has_rmii1 = 1,
	.has_rgmii2 = 1,
	.has_mmc1 = 1,
};

void factory_test__set_board_interface(void) {
	set_board_interface(&factory_test_board_interface);
}
