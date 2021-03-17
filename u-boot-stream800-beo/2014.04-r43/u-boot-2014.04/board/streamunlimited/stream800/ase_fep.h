/*
 * ase_fep.h
 *
 * Copyright (C) 2014, StreamUnlimited Engineering GmbH, http://www.streamunlimited.com/
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

#ifndef __ASE_FEP_H
#define __ASE_FEP_H

#include <common.h>
#include "board.h"

struct ase_fep_version {
	u16 rsp;
	u16 len;
	u16 revision;
	u16 pcbVariant;
	u16 productVariant;
	u16 productId;
};

enum ase_beo_productId {
	unknown = 0,
	a29mk2_version_a = 1,
	a29mk2 = 2,
	ez2mk2 = 3,
	s47 = 4,
	ca6 = 5,
	unknownWithIR = 10,
	noProductAttached = 12,
};

struct ase_fep {
	int fep_uart;
};

struct ase_fep *init_ase_fep(void);

struct ase_fep_version version_ase_fep(struct ase_fep *fep);

bool is_pressed_update_button(struct ase_fep *fep);

void set_boot_state_ase_fep(struct ase_fep *fep, const struct Stream800Board *board);

void set_error_state_ase_fep(struct ase_fep *fep);

void set_flashing_on_state_ase_fep(struct ase_fep *fep);

void set_flashing_off_state_ase_fep(struct ase_fep *fep);

#endif /*__ASE_FEP_H*/
