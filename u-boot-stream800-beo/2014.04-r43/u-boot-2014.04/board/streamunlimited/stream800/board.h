/*
 * board.h
 *
 * Stream800 boards information header. Based on TI AM335x.
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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <common.h>
#include <cpsw.h>
#include "board_defines.h"

struct StreamBoardInterface {

	void (*init) (const struct Stream800Board *board);
	void (*late_init) (const struct Stream800Board *board);
	void (*cpsw_eth_init) (const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data);
	void (*on_board_state_changed) (const struct Stream800Board *board, BoardState state);
	/* return 0, usb update should not start, return 1, usb update should start immediately  */
	int (*do_usb_update) (void);

	/* flags used during pinmux setup - fill in according to peripherals used */
	int has_rmii1;
	int has_rgmii2;
	int has_mmc1;
};

void set_board_interface(struct StreamBoardInterface * interface);
void set_board_state(BoardState state);
BoardState get_board_state(void);

/* Handling board state change.
 * Should be handled individually for each board (LED indication, ...)
 */
void on_board_state_changed(BoardState state);

extern struct StreamBoardInterface *board_interface;


#endif	// _BOARD_H_
