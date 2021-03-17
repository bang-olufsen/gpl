/*
 * mux.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "mux.h"
#include <common.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mux.h>
#include <asm/io.h>
#include <i2c.h>
#include <cpsw.h>

static struct module_pin_mux uart0_pin_mux[] = {
	{OFFSET(uart0_rxd), (MODE(0) | PULLDOWN_EN | RXACTIVE)},	/* UART0_RXD */
	{OFFSET(uart0_txd), (MODE(0) | PULLUDEN)},		/* UART0_TXD */
	{-1},
};

static struct module_pin_mux uart1_pin_mux[] = {
	{OFFSET(uart1_rxd), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* UART1_RXD */
	{OFFSET(uart1_txd), (MODE(0) | PULLUDEN)},		/* UART1_TXD */
	{-1},
};

static struct module_pin_mux uart2_pin_mux[] = {
	{OFFSET(spi0_sclk), (MODE(1) | PULLUP_EN | RXACTIVE)},	/* UART2_RXD */
	{OFFSET(spi0_d0), (MODE(1) | PULLUDEN)},		/* UART2_TXD */
	{-1},
};

static struct module_pin_mux uart3_pin_mux[] = {
	{OFFSET(spi0_cs1), (MODE(1) | PULLUP_EN | RXACTIVE)},	/* UART3_RXD */
	{OFFSET(ecap0_in_pwm0_out), (MODE(1) | PULLUDEN)},	/* UART3_TXD */
	{-1},
};

static struct module_pin_mux uart4_pin_mux[] = {
	{OFFSET(uart0_ctsn), (MODE(1) | PULLUP_EN | RXACTIVE )},	/* UART4_RXD */
	{OFFSET(uart0_rtsn), (MODE(1) )},				/* UART4_TXD */
	{-1},
};

static struct module_pin_mux uart5_pin_mux[] = {
	{OFFSET(lcd_data9), (MODE(4) | PULLUP_EN | RXACTIVE)},	/* UART5_RXD */
	{OFFSET(lcd_data8), (MODE(4) | PULLUDEN)},		/* UART5_TXD */
	{-1},
};

#ifndef CONFIG_SPL_BUILD
static struct module_pin_mux mmc0_pin_mux[] __attribute__((unused)) = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{OFFSET(mcasp0_aclkr), (MODE(4) | RXACTIVE)},		/* MMC0_WP */
	{OFFSET(spi0_cs1), (MODE(5) | RXACTIVE | PULLUP_EN)},	/* MMC0_CD */
	{-1},
};

static struct module_pin_mux mmc0_no_cd_pin_mux[] __attribute__((unused)) = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{OFFSET(mcasp0_aclkr), (MODE(4) | RXACTIVE)},		/* MMC0_WP */
	{-1},
};

static struct module_pin_mux mmc0_pin_mux_sk_evm[] __attribute__((unused)) = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{OFFSET(spi0_cs1), (MODE(5) | RXACTIVE | PULLUP_EN)},	/* MMC0_CD */
	{-1},
};
#endif

static struct module_pin_mux mmc1_pin_mux[] = {
	{OFFSET(gpmc_ad8),  (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_DAT3 */
	{OFFSET(gpmc_ad9),  (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_DAT2 */
	{OFFSET(gpmc_ad10), (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_DAT1 */
	{OFFSET(gpmc_ad11), (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_DAT0 */
	{OFFSET(gpmc_csn1), (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_CLK */
	{OFFSET(gpmc_csn2), (MODE(2) | RXACTIVE | PULLUP_EN)},  /* MMC1_CMD */
	{OFFSET(gpmc_ad12), (MODE(7) | RXACTIVE | PULLUP_EN)},  /* MMC1_WP */
	{OFFSET(gpmc_ad13), (MODE(7) | RXACTIVE | PULLUP_EN)},  /* MMC1_CD */
	{-1},
};

#ifndef CONFIG_SPL_BUILD
static struct module_pin_mux i2c0_pin_mux[] = {
	{OFFSET(i2c0_sda), (MODE(0) | RXACTIVE |
			PULLUDEN | SLEWCTRL)}, /* I2C_DATA */
	{OFFSET(i2c0_scl), (MODE(0) | RXACTIVE |
			PULLUDEN | SLEWCTRL)}, /* I2C_SCLK */
	{-1},
};

static struct module_pin_mux i2c1_pin_mux[] = {
	{OFFSET(spi0_d1), (MODE(2) | RXACTIVE |
			PULLUDEN | SLEWCTRL)},	/* I2C_DATA */
	{OFFSET(spi0_cs0), (MODE(2) | RXACTIVE |
			PULLUDEN | SLEWCTRL)},	/* I2C_SCLK */
	{-1},
};

static struct module_pin_mux ase_i2c1_pin_mux[] = {
	{OFFSET(uart1_txd), (MODE(3) | RXACTIVE |
			PULLUDEN | SLEWCTRL)},	/* I2C_SCLK */
	{OFFSET(uart1_rxd), (MODE(3) | RXACTIVE |
			PULLUDEN | SLEWCTRL)},	/* I2C_DATA */
	{-1},
};

static struct module_pin_mux spi0_pin_mux[] = {
	{OFFSET(spi0_sclk), (MODE(0) | RXACTIVE | PULLUDEN)},	/* SPI0_SCLK */
	{OFFSET(spi0_d0), (MODE(0) | RXACTIVE |
			PULLUDEN | PULLUP_EN)},			/* SPI0_D0 */
	{OFFSET(spi0_d1), (MODE(0) | RXACTIVE | PULLUDEN)},	/* SPI0_D1 */
	{OFFSET(spi0_cs0), (MODE(0) | RXACTIVE |
			PULLUDEN | PULLUP_EN)},			/* SPI0_CS0 */
	{-1},
};

static struct module_pin_mux gpio0_7_pin_mux[] __attribute__((unused)) = {
	{OFFSET(ecap0_in_pwm0_out), (MODE(7) | PULLUDEN)},	/* GPIO0_7 */
	{-1},
};

static struct module_pin_mux rgmii1_pin_mux[] __attribute__((unused)) = {
	{OFFSET(mii1_txen), MODE(2)},			/* RGMII1_TCTL */
	{OFFSET(mii1_rxdv), MODE(2) | RXACTIVE},	/* RGMII1_RCTL */
	{OFFSET(mii1_txd3), MODE(2)},			/* RGMII1_TD3 */
	{OFFSET(mii1_txd2), MODE(2)},			/* RGMII1_TD2 */
	{OFFSET(mii1_txd1), MODE(2)},			/* RGMII1_TD1 */
	{OFFSET(mii1_txd0), MODE(2)},			/* RGMII1_TD0 */
	{OFFSET(mii1_txclk), MODE(2)},			/* RGMII1_TCLK */
	{OFFSET(mii1_rxclk), MODE(2) | RXACTIVE},	/* RGMII1_RCLK */
	{OFFSET(mii1_rxd3), MODE(2) | RXACTIVE},	/* RGMII1_RD3 */
	{OFFSET(mii1_rxd2), MODE(2) | RXACTIVE},	/* RGMII1_RD2 */
	{OFFSET(mii1_rxd1), MODE(2) | RXACTIVE},	/* RGMII1_RD1 */
	{OFFSET(mii1_rxd0), MODE(2) | RXACTIVE},	/* RGMII1_RD0 */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN},/* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	/* MDIO_CLK */
	{-1},
};

static struct module_pin_mux rmii1_pin_mux[] = {
	{OFFSET(mii1_crs), MODE(1) | RXACTIVE},     /* RMII1_CRS */
	{OFFSET(mii1_rxerr), MODE(1) | RXACTIVE},   /* RMII1_RXERR */
	{OFFSET(mii1_txen), MODE(1)},           /* RMII1_TXEN */
	{OFFSET(mii1_txd1), MODE(1)},           /* RMII1_TXD1 */
	{OFFSET(mii1_txd0), MODE(1)},           /* RMII1_TXD0 */
	{OFFSET(mii1_rxd1), MODE(1) | RXACTIVE},    /* RMII1_RXD1 */
	{OFFSET(mii1_rxd0), MODE(1) | RXACTIVE},    /* RMII1_RXD0 */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN}, /* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},    /* MDIO_CLK */
	{OFFSET(rmii1_refclk), MODE(0) | RXACTIVE}, /* RMII1_REFCLK */
	{-1},
};

static struct module_pin_mux rgmii2_pin_mux[] = {
	{OFFSET(gpmc_a0), MODE(2)},			/* RGMII2_TCTL */
	{OFFSET(gpmc_a1), MODE(2) | RXACTIVE},		/* RGMII2_RCTL */
	{OFFSET(gpmc_a2), MODE(2)},			/* RGMII2_TD3 */
	{OFFSET(gpmc_a3), MODE(2)},			/* RGMII2_TD2 */
	{OFFSET(gpmc_a4), MODE(2)},			/* RGMII2_TD1 */
	{OFFSET(gpmc_a5), MODE(2)},			/* RGMII2_TD0 */
	{OFFSET(gpmc_a6), MODE(2)},			/* RGMII2_TCLK */
	{OFFSET(gpmc_a7), MODE(2) | RXACTIVE},		/* RGMII2_RCLK */
	{OFFSET(gpmc_a8), MODE(2) | RXACTIVE},		/* RGMII2_RD3 */
	{OFFSET(gpmc_a9), MODE(2) | RXACTIVE},		/* RGMII2_RD2 */
	{OFFSET(gpmc_a10), MODE(2) | RXACTIVE},		/* RGMII2_RD1 */
	{OFFSET(gpmc_a11), MODE(2) | RXACTIVE},		/* RGMII2_RD0 */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN}, /* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	/* MDIO_CLK */
	{-1},
};

static struct module_pin_mux suos_asc_phy_reset_pin_mux[] __attribute__((unused)) = {
	{OFFSET(gpmc_ad14), MODE(7) | PULLUP_EN},	/* GPIO1_14 - pull up should be sufficient to keep phy out of reset! */
	{-1},
};

static struct module_pin_mux clk2_pll_pin_mux[] = {
	{OFFSET(xdma_event_intr1), MODE(3) | PULLUP_EN},	/* GPIO1_14 - pull up should be sufficient to keep phy out of reset! */
	{-1},
};

static struct module_pin_mux nand_pin_mux[] = {
	{OFFSET(gpmc_ad0), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD0 */
	{OFFSET(gpmc_ad1), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD1 */
	{OFFSET(gpmc_ad2), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD2 */
	{OFFSET(gpmc_ad3), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD3 */
	{OFFSET(gpmc_ad4), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD4 */
	{OFFSET(gpmc_ad5), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD5 */
	{OFFSET(gpmc_ad6), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD6 */
	{OFFSET(gpmc_ad7), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* NAND AD7 */
	{OFFSET(gpmc_wait0), (MODE(0) | RXACTIVE | PULLUP_EN)}, /* NAND WAIT */
	{OFFSET(gpmc_wpn), (MODE(7) | PULLUP_EN | RXACTIVE)},	/* NAND_WPN */
	{OFFSET(gpmc_csn0), (MODE(0) | PULLUDEN)},	/* NAND_CS0 */
	{OFFSET(gpmc_advn_ale), (MODE(0) | PULLUDEN)}, /* NAND_ADV_ALE */
	{OFFSET(gpmc_oen_ren), (MODE(0) | PULLUDEN)},	/* NAND_OE */
	{OFFSET(gpmc_wen), (MODE(0) | PULLUDEN)},	/* NAND_WEN */
	{OFFSET(gpmc_be0n_cle), (MODE(0) | PULLUDEN)},	/* NAND_BE_CLE */
	{-1},
};
#endif

void enable_uart0_pin_mux(void)
{
	configure_module_pin_mux(uart0_pin_mux);
}

void enable_uart1_pin_mux(void)
{
	configure_module_pin_mux(uart1_pin_mux);
}

void enable_uart2_pin_mux(void)
{
	configure_module_pin_mux(uart2_pin_mux);
}

void enable_uart3_pin_mux(void)
{
	configure_module_pin_mux(uart3_pin_mux);
}

void enable_uart4_pin_mux(void)
{
	configure_module_pin_mux(uart4_pin_mux);
}

void enable_uart5_pin_mux(void)
{
	configure_module_pin_mux(uart5_pin_mux);
}

#ifndef CONFIG_SPL_BUILD

void enable_i2c0_pin_mux(void)
{
	configure_module_pin_mux(i2c0_pin_mux);
}

void enable_board_pin_mux(struct Stream800Board *board)
{
	configure_module_pin_mux(i2c0_pin_mux);
	configure_module_pin_mux(ase_i2c1_pin_mux);
	//ASE DO NOT USE IT AS OUTPUT
	//configure_module_pin_mux(clk2_pll_pin_mux);
	configure_module_pin_mux(nand_pin_mux);

	if (board_interface != NULL) {
#ifndef CONFIG_NO_ETH
		if (board_interface->has_rmii1) {
			printf("pinmux: enabling rmii1\n");
			configure_module_pin_mux(rmii1_pin_mux);
		}

		if (board_interface->has_rgmii2) {
			printf("pinmux: enabling rgmii2\n");
			configure_module_pin_mux(rgmii2_pin_mux);
		}
#endif
		if (board_interface->has_mmc1) {
			printf("pinmux: enabling mmc1\n");
			configure_module_pin_mux(mmc1_pin_mux);
		}
	}

	//configure_module_pin_mux(mmc0_pin_mux);
	//configure_module_pin_mux(mmc1_pin_mux);

}

#else

void enable_mmc1_pin_mux(void)
{
	configure_module_pin_mux(mmc1_pin_mux);
}

#endif
