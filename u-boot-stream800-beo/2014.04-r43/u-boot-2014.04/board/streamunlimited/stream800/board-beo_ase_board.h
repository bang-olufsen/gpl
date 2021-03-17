/*
 * board-beo_ase_board.h
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

#ifndef __BOARD_BEO_ASE_BOARD_H
#define __BOARD_BEO_ASE_BOARD_H

#include <common.h>
#include "board.h"
#include "ase_fep.h"
#include <cpsw.h>

void beo_ase__set_board_interface(void);

void beo_ase__init(const struct Stream800Board *board);

void beo_ase__late_init(const struct Stream800Board *board);

#ifdef CONFIG_DRIVER_TI_CPSW
void beo_ase__cpsw_eth_init(const struct Stream800Board *board, struct cpsw_platform_data *cpsw_data);
#endif

/* Handling board state change (LED indication, ...) */
void beo_ase__on_board_state_changed(const struct Stream800Board *board, BoardState state);

void beo_ase__set_ase_fep(struct ase_fep *fep);

#endif	// __BOARD_BEO_ASE_BOARD_H
