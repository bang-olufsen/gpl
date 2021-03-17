/*
 * phy.h
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

#ifndef _ARCH_AM33XX_PHY_H
#define _ARCH_AM33XX_PHY_H

/* MII mode defines
 * See CONTROL_MODULE Registers: gmii_sel Register (offset = 650h)
 */
// gmii1_sel (bites 0-1)
#define MII1_MODE_ENABLE		0x0
#define RMII1_MODE_ENABLE		0x1
#define RGMII1_MODE_ENABLE		0x2
#define MII1_MODE_NOT_USED		0x3
// gmii2_sel (bites 2-3)
#define MII2_MODE_ENABLE		0x0
#define RMII2_MODE_ENABLE		0x4
#define RGMII2_MODE_ENABLE		0x8
#define MII2_MODE_NOT_USED		0xC
// rgmii1_idmode (bite 4)
#define RGMII1_NO_INTERNAL_DELAY 0x10
// rgmii2_idmode (bite 5)
#define RGMII2_NO_INTERNAL_DELAY 0x20
// rmii1_io_clk_en (bite 6)
#define MII1_CLK_FROM_CHIP_PIN	0x40
#define MII1_CLK_FROM_PLL		0x0
// rmii1_io_clk_en (bite 7)
#define MII2_CLK_FROM_CHIP_PIN	0x80
#define MII2_CLK_FROM_PLL		0x0

#endif