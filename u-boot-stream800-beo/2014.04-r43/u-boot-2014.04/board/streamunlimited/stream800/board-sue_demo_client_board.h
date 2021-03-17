/*
 * board-sue_demo_client_board.h
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

#ifndef __BOARD_SUE_DEMO_CLIENT_BOARD_H
#define __BOARD_SUE_DEMO_CLIENT_BOARD_H

#include <common.h>
#include "board.h"

void demo_client__set_board_interface(void);

// "SUE demo client" board initialization
void demo_client__init(const struct Stream800Board *board);

// "SUE demo client" board late initialization
void demo_client__late_init(const struct Stream800Board *board);

#ifdef CONFIG_DRIVER_TI_CPSW
void demo_client__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data);
#endif

/* Handling board state change (LED indication, ...) */
void demo_client__on_board_state_changed(const struct Stream800Board *board, BoardState state);

#endif	// __BOARD_SUE_DEMO_CLIENT_BOARD_H
