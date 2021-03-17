/*
 * board-sue_first_carrier_board.h
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

#ifndef __BOARD_SUE_FIRST_CARRIER_BOARD_H
#define __BOARD_SUE_FIRST_CARRIER_BOARD_H

#include <common.h>
#include "board.h"

void first_carrier__set_board_interface(void);

// "SUE Carrier Board" board initialization
void first_carrier__init(const struct Stream800Board *board);

// "SUE Carrier Board" board late initialization
void first_carrier__late_init(const struct Stream800Board *board);

#ifdef CONFIG_DRIVER_TI_CPSW
void first_carrier__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data);
#endif

/* Handling board state change (LED indication, ...) */
void first_carrier__on_board_state_changed(const struct Stream800Board *board, BoardState state);

#endif	// __BOARD_SUE_FIRST_CARRIER_BOARD_H
