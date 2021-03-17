/*
 * board-sue_demo_client_board.c
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
 * "SUE demo client" - board specific initialization
 */

#include "board-sue_demo_client_board.h"
#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <phy.h>
#include <asm/arch-am33xx/phy.h>
#include <asm-generic/gpio.h>
#include <asm/arch/mux.h>
#include <cpsw.h>

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

/* GPIO1_9 - reset button status - check sfupdate file on USB if pressed during the boot-up */
#define FW_UPDATE_BUTTON_STATUS_GPIO	(32 + 9)

static struct module_pin_mux fw_update_button_pin_mux[] = {
		{OFFSET(uart0_rtsn), (MODE(7) | PULLUP_EN | RXACTIVE)},	/* Reset button status gpio1_9, conflict: uart0_pin_mux rts */
		{-1},
	};

static bool demo_client__check_board(const struct Stream800Board *board)
{
	if (!board)
		return false;

	switch (board->carrierBoardType) {
	case CBT_SueDemoClientBoard:
	case CBT_SueHighendDemoClientBoard:
	case CBT_unknown:
		return true;
	default:
		return false;
	}
}

// "SUE demo client" board initialization
void demo_client__init(const struct Stream800Board *board)
{
	if (!demo_client__check_board(board))
		BUG();

	configure_module_pin_mux(fw_update_button_pin_mux);

	if (gpio_request(FW_UPDATE_BUTTON_STATUS_GPIO, "Reset status"))
		printf("error: cannot request GPIO %d\n", FW_UPDATE_BUTTON_STATUS_GPIO);
	gpio_direction_input(FW_UPDATE_BUTTON_STATUS_GPIO);
}

// "SUE demo client" board late initialization
void demo_client__late_init(const struct Stream800Board *board)
{
	if (!demo_client__check_board(board))
		BUG();

	printf("TODO: 'SUE demo client' board late initialization.\n");
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

void demo_client__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data)
{
	if (!demo_client__check_board(board))
		BUG();

	cpsw_data->slaves = 1;
	cpsw_data->slave_data = cpsw_slaves;

	// Advisory 1.0.10
	// "The AM335x device does not support internal delay mode, so RGMII1_IDMODE and RGMII2_IDMODE must be set to 1b."
	writel(RMII1_MODE_ENABLE | MII1_CLK_FROM_CHIP_PIN | RGMII1_NO_INTERNAL_DELAY
		| MII2_MODE_NOT_USED , &cdev->miisel);
}
#endif /* CONFIG_DRIVER_TI_CPSW */

int demo_client__get_fw_update_button_status(void)
{
	return (gpio_get_value(FW_UPDATE_BUTTON_STATUS_GPIO) == 0) ? 1 : 0;
}

void demo_client__on_board_state_changed(const struct Stream800Board *board, BoardState state)
{
	printf("TODO: Board state changed to '%s'\n", boardStateStrings[state]);
}

int demo_client__do_usb_update(void)
{
	return (demo_client__get_fw_update_button_status()) ? 1 : 0 ;	// fw update button -> check update file on USB
}


static struct StreamBoardInterface demo_client_board_interface = {
	.init = demo_client__init,
	.late_init = demo_client__late_init,
	.cpsw_eth_init = demo_client__cpsw_eth_init,
	.on_board_state_changed = demo_client__on_board_state_changed,
	.do_usb_update = demo_client__do_usb_update,

	.has_rmii1 = 1,
};

void demo_client__set_board_interface(void) {
	set_board_interface(&demo_client_board_interface);
}
