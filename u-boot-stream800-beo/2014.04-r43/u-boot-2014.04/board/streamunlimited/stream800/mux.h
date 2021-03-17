/*
 * mux.h
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

#ifndef __MUX_H
#define __MUX_H

#include <common.h>
#include "board.h"

void enable_board_pin_mux(struct Stream800Board *board);

void enable_uart0_pin_mux(void);
void enable_uart1_pin_mux(void);
void enable_uart2_pin_mux(void);
void enable_uart3_pin_mux(void);
void enable_uart4_pin_mux(void);
void enable_uart5_pin_mux(void);

void enable_i2c0_pin_mux(void);

#ifdef CONFIG_SPL_BUILD
void enable_mmc1_pin_mux(void);
#endif

#endif	/* __MUX_H */
