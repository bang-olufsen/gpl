/*
 * board.c
 *
 * Board functions for TI AM335X based boards
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
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

#include "board.h"
#include <common.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mux.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <i2c.h>
#include <miiphy.h>
#include <cpsw.h>
#include <os.h>
#include <nand.h>
#include <malloc.h>
#include <linux/err.h>
#include <power/tps65217.h>
#include "board.h"
#include "mux.h"
#include "board-sue_demo_client_board.h"
#include "board-sue_first_carrier_board.h"
#include "board-sue_factory_test_board.h"
#include "board-beo_ase_board.h"
#include "adc.h"
#include "fwupdate.h"
#include "si5351x.h"
#include "partitions.h"
#include <linux/err.h>
#include "ase_fep.h"

#ifdef CONFIG_SPL_BUILD
#error This file should not be built for SPL
#endif

#ifndef STREAM_UBOOT_VERSION
#define STREAM_UBOOT_VERSION "00000000.00000000.00000000.00000000"
#warning Using default value for u-boot version: "00000000.00000000.00000000.00000000"
#endif // STREAM_UBOOT_VERSION

#define DB_PART_NAME            "settings"
#define DB_PART_VID_HDR_OFFSET  "2048"
#define DB_VOLUME_NAME          "stream800-settings"

/* GPIO3_7 - eth phy reset gpio */
#define ETH_PHY_RESET_GPIO	(32 + 32 + 32 + 7)

static struct module_pin_mux eth_phy_reset_pin[] = {
		{OFFSET(emu0), (MODE(7) | PULLUP_EN)},	/* Reset button for eth phy */
		{-1},
	};


extern int ubi_part(const char* part_name, const char* vid_header_offset);
extern int ubi_create(char* vol_name);
extern int ubi_remove(char* vol_name);

DECLARE_GLOBAL_DATA_PTR;

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

static struct Stream800Board __attribute__((section (".data"))) board = {moduleVersion : MV_unknown, carrierBoardType : CBT_unknown};

static BoardState boardState = BS_Off;

struct StreamBoardInterface *board_interface = NULL;

static void init_si5351x(void);

Stream800ModuleVersion get_module_version(void)
{
	return board.moduleVersion;
}

Stream800CarrierBoardType get_carrier_board_type(void)
{
	return board.carrierBoardType;
}

static void boost_cpu_speed(void)
{
	/* Get the frequency */
	if (i2c_probe(TPS65217_CHIP_PM))
		return;

	dpll_mpu_opp100.m = MPUPLL_M_720;

	/* Set DCDC3 (CORE) voltage to 1.125V */
	if (tps65217_voltage_update(TPS65217_DEFDCDC3,
								TPS65217_DCDC_VOLT_SEL_1125MV)) {
		puts("tps65217_voltage_update failure\n");
		return;
	}

	/* Set CORE Frequencies to OPP100 */
	do_setup_dpll(&dpll_core_regs, &dpll_core_opp100);

	/* Set DCDC2 (MPU) voltage */
	if (tps65217_voltage_update(TPS65217_DEFDCDC2,
								TPS65217_DCDC_VOLT_SEL_1275MV)) {
		puts("tps65217_voltage_update failure\n");
		return;
	}

	if (tps65217_reg_write(TPS65217_PROT_LEVEL_2,
						   TPS65217_DEFLS2,
						   TPS65217_LDO_VOLTAGE_OUT_3_3,
						   TPS65217_LDO_MASK))
		puts("tps65217_reg_write failure\n");

	/* Set MPU Frequency to what we detected now that voltages are set */
	do_setup_dpll(&dpll_mpu_regs, &dpll_mpu_opp100);
}

void set_board_interface(struct StreamBoardInterface * interface) {
	board_interface = interface;
}

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	//init_adc();
	board_detect(&board);
	board_printf_info(&board);

	switch (get_carrier_board_type()) {
		case CBT_SueDemoClientBoard:
		case CBT_SueHighendDemoClientBoard:
			demo_client__set_board_interface();
			break;
		case CBT_SueFirstCarrierBoard:
			first_carrier__set_board_interface();
			break;
		case CBT_SueFactoryTestBoard:
			factory_test__set_board_interface();
			break;
		case CBT_BeoASE_A9MK2:
		case CBT_BeoASE_EZ2MK2:
		case CBT_BeoASE_CA6:
		case CBT_BeoASE_S47:
			beo_ase__set_board_interface();
			beo_ase__set_ase_fep(fep_instance());
			break;

		// CUSTOMIZE: Add your board __set_board_interface() call here

		default:
			printf("board_late_init: Not supported board - assuming DemoClient!\n");
			demo_client__set_board_interface();
			//panic("Not supported board!");
	}

	enable_board_pin_mux(&board);

	boost_cpu_speed();

	/* This is temporarily (after reset during FW update we sometime detect unwanted external warm reset) */
	{
		uint32_t rstst = readl(PRM_RSTST);
		uint32_t rstctrl = readl(PRM_RSTCTRL);
		printf("rstst: 0x%X, rstctrl: 0x%X\n", rstst, rstctrl);
	}


	if ((board_interface != NULL) && (board_interface->init != NULL))
		board_interface->init(&board);

	/* ASE- disabled
	configure_module_pin_mux(eth_phy_reset_pin);
	if (gpio_request(ETH_PHY_RESET_GPIO, "Eth phy"))
		printf("error: cannot request GPIO %d\n", ETH_PHY_RESET_GPIO);
	gpio_direction_output(ETH_PHY_RESET_GPIO, 0);

	udelay(2000);
	gpio_set_value(ETH_PHY_RESET_GPIO, 1);
	*/
	set_board_state(BS_Normal);

	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	gd->bd->bi_arch_number = MACH_TYPE_TIAM335EVM;

	gpmc_init();

	{
		gpmc_cfg = (struct gpmc *)GPMC_BASE;
		printf("CONFIG 1: 0x%X\n", readl(&gpmc_cfg->cs[0].config1));
		printf("CONFIG 2: 0x%X\n", readl(&gpmc_cfg->cs[0].config2));
		printf("CONFIG 3: 0x%X\n", readl(&gpmc_cfg->cs[0].config3));
		printf("CONFIG 4: 0x%X\n", readl(&gpmc_cfg->cs[0].config4));
		printf("CONFIG 5: 0x%X\n", readl(&gpmc_cfg->cs[0].config5));
		printf("CONFIG 6: 0x%X\n", readl(&gpmc_cfg->cs[0].config6));
		printf("CONFIG 7: 0x%X\n", readl(&gpmc_cfg->cs[0].config7));
	}

	return 0;
}

#ifdef CONFIG_BOOTCOUNT_LIMIT
void bootcount_store (ulong a)
{
	int status = fwupdate_setBootCount((uint32_t)a);

	if (0 != status)
		printf("ERROR: fwupdate_setBootCount() failed!\n");
	else
		printf("BOOTCOUNT is %ld\n", a);
}

ulong bootcount_load (void)
{
	uint8_t bootcount = 0xFF;

	int status = fwupdate_getBootCount(&bootcount);

	if (0 != status)
		printf("ERROR: getBootCount() failed!\n");

	return bootcount;
}
#endif /* CONFIG_BOOTCOUNT_LIMIT */

static void init_si5351x(void)
{
	int i;
	for (i = 0; i < Si5351x_NUMREGS; ++i) {
		if (i2c_write(Si5351x_ADDRR, i, 1, (uint8_t*)&si5351x_default_clock_table[i], 1)){
			printf("Si5351x cannot be initialized\n");
			return;
		}
	}
	printf("Si5351x correctly initialized\n");
}

/* Two unused functions, uncomment when factory_reset is implemented */
/* static int erase_database(void) */
/* { */
/* 	puts("Erasing Nand Settings Partition...\n"); */
/* 	return run_command("nand erase.part settings", 0); */
/* } */

/* static void factory_reset(void){ */
/* 	// erase database partition in NAND */
/* 	if (erase_database()) { */
/* 		printf("ERROR: Database erase failed!\n"); */
/* 	} else if (0 == ubi_part(DB_PART_NAME, DB_PART_VID_HDR_OFFSET)) { */
/* 		// partition is good/valid */
/* 		// create new database volumel */
/* 		if (0 != ubi_create(DB_VOLUME_NAME)) { */
/* 			printf("ERROR: Database UBI create volume failed!\n"); */
/* 		} */
/* 	} else { */
/* 		printf("ERROR: Database UBI partition invalid!\n"); */
/* 	} */
/* 	udelay(2000000); */
/* } */

static int create_mtd_part(char *buf, struct partition part)
{
	return sprintf(buf, "%dk(%s)", (part.size / 1024), part.name);
}

/*
 * Detect flash size and setup mtdparts variable
 * accordingly from defined partition table
 */
static int detect_flash(void)
{
	struct mtd_info *mtd = get_mtd_device_nm("nand0");
	enum s800_flash_size size = FLASH_SIZE_UNKNOWN;
	int ret = 0;

	if (IS_ERR(mtd)) {
		printf("Device %s not found!\n", "nand0");
		return -ENODEV;
	}

	if (mtd->size == 0x10000000) {
		/* 256MB flash */
		size = FLASH_SIZE_256M;
	} else if (mtd->size == 0x20000000) {
		/* 512MB flash */
		size = FLASH_SIZE_512M;
	}

	if (size != FLASH_SIZE_UNKNOWN) {
		printf("Setting partitions for %sMB flash.\n", size == FLASH_SIZE_256M ? "256" : "512");

		struct partition *s800_parts;
		int part_size;

		if (size == FLASH_SIZE_256M) {
			s800_parts = s800_part_256M;
			part_size = ARRAY_SIZE(s800_part_256M);
		} else if (size == FLASH_SIZE_512M) {
			s800_parts = s800_part_512M;
			part_size = ARRAY_SIZE(s800_part_512M);
		}

		/* prepare buffer for mtdparts env*/
		const char *mtdparts_prefix = "mtdparts=omap2-nand.0:";
		char *mtdparts = malloc(strlen(mtdparts_prefix) + (part_size * 30));
		int j = strlen(mtdparts_prefix);
		int i;

		strcpy(mtdparts, mtdparts_prefix);

		for (i = 0; i < part_size; i++) {
			char buf[30];
			create_mtd_part(buf, s800_parts[i]);
			if (i != (part_size - 1)) {
				int len = strlen(buf);
				buf[len] = ',';
				buf[len + 1] = '\0';
			}

			strncpy(&mtdparts[j], buf, strlen(buf));
			j += strlen(buf);
		}

		mtdparts[j] = '\0';
		printf("Setting mtdparts: %s\n", mtdparts);

		setenv("mtdparts", mtdparts);

		for (i = 0; i < part_size; i++) {
			if (!strcmp(s800_parts[i].name, "download")) {
				char buf[30];
				sprintf(buf, "%lu", s800_parts[i].size / 1024);
				setenv("mtd_download_size", buf);
				break;
			}
		}

	} else {
		printf("Unknown flash size!!!!\n");
		ret = -ENODEV;
	}

	return ret;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#if ASE_BUILD
	switch (get_carrier_board_type()) {
		case CBT_BeoASE_A9MK2:
			setenv("board_conf", "#conf@1");
			break;
		case CBT_BeoASE_EZ2MK2:
			setenv("board_conf", "#conf@2");
			break;
		case CBT_BeoASE_CA6:
			setenv("board_conf", "#conf@3");
			break;
		case CBT_BeoASE_S47:
			setenv("board_conf", "#conf@4");
			break;
		default:
			setenv("board_conf", "");
			break;
	}

#endif /* ASE_BUILD */

	/*
	 * DTBs files are stored in cramfs image, path in cramfs filesystem to the correct file is: /${cramfsdir}/${board_name}_${board_rev}.dtb
	 * Loading of DTB is located in include/configs/stream800.h
	 * Change any variable of board_name, cramfsdir, board_rev can corrupt board boot !!
	 */
	switch (get_module_version()) {
		case MV_L0:
			setenv("board_name", "stream800");
			setenv("board_rev", "l0");
			break;
		case MV_L1:
			setenv("board_name", "stream800");
			setenv("board_rev", "l1");
			break;
		case MV_L2:
			setenv("board_name", "stream800");
			setenv("board_rev", "l2");
			break;
		case MV_L0_DDR3:
			/* The DDR3 modules are effectively L2 modules with DDR3 from a
			 * device tree POV. */
			setenv("board_name", "stream800");
			setenv("board_rev", "l2");
			break;
		case MV_L1_DDR3_HE:
			/* Stream820 (HighEnd module) contains faster CPU with graphical accelrator */
			setenv("board_name", "stream820");
			setenv("board_rev", "l1");
			break;
		case MV_BeoASE:
			setenv("board_name", "stream800");
			setenv("board_rev", "BeoASE");
			break;
		case MV_unknown:
		default:
			setenv("board_name", "unknown");
			setenv("board_rev", "unknown");
			setenv("bootcmd", "");
			break;
	}

	switch (get_carrier_board_type()) {
		case CBT_SueDemoClientBoard:
			setenv("cramfsdir", "sue_demo_client_board");
			break;
		case CBT_SueHighendDemoClientBoard:
			setenv("cramfsdir", "sue_highend_demo_client_board");
			break;
		case CBT_SueFirstCarrierBoard:
			setenv("cramfsdir", "sue_first_carrier_board");
			break;
		case CBT_SueFactoryTestBoard:
			setenv("cramfsdir", "sue_factory_test_board");
			break;
		case CBT_BeoASE_A9MK2:
		case CBT_BeoASE_EZ2MK2:
		case CBT_BeoASE_CA6:
		case CBT_BeoASE_S47:
			break;

		// CUSTOMIZE: Add your board here

		default:
			printf("board_late_init: Not supported board!\n");
			setenv("cramfsdir", "");
			//panic("Not supported board!");
	}

	if ((board_interface != NULL) && (board_interface->late_init != NULL))
		board_interface->late_init(&board);

    setenv("buildtime_uboot_version", STREAM_UBOOT_VERSION);

#ifdef CONFIG_CMD_SFU_PARSER

	if (0 > fwupdate_init()) {
		printf("ERROR: fwupdate_init() call failed!\n");
	}
#endif // CONFIG_CMD_SFU_PARSER

	/* TODO Check on gpio to go to factory defaults*/
	/* factory_reset(); */

	/* enable hardware ecc module */
	omap_nand_switch_ecc(1, 8);

	/* detect flash size and setup mtdparts variable */
	detect_flash();

	return 0;
}
#endif

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 0,			// will be set later, depends on each carrier board type
	.slave_data		= NULL,			// ditto
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};
#endif

#if defined(CONFIG_DRIVER_TI_CPSW) || \
	(defined(CONFIG_USB_ETHER) && defined(CONFIG_MUSB_GADGET))

int board_eth_init(bd_t *bis)
{
	int rv, n = 0;
#ifdef CONFIG_DRIVER_TI_CPSW
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;

	if (!eth_getenv_enetaddr("ethaddr", mac_addr)) {
		/* try reading mac address from efuse */
		mac_lo = readl(&cdev->macid0l);
		mac_hi = readl(&cdev->macid0h);
		mac_addr[0] = mac_hi & 0xFF;
		mac_addr[1] = (mac_hi & 0xFF00) >> 8;
		mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
		mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
		mac_addr[4] = mac_lo & 0xFF;
		mac_addr[5] = (mac_lo & 0xFF00) >> 8;

		if (is_valid_ether_addr(mac_addr)) {
			eth_setenv_enetaddr("ethaddr", mac_addr);
			printf("board_eth_init: found mac_addr: '%02x:%02x:%02x:%02x:%02x:%02x'\n",
				   mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
		} else {
			puts("board_eth_init: failed to get valid MAC addr, trying USB eth ...\n");
			goto try_usbether;
		}
	}

	if ((board_interface != NULL) && (board_interface->cpsw_eth_init != NULL))
		board_interface->cpsw_eth_init(&board, &cpsw_data);
	else
		goto try_usbether;

	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);
	else
		n += rv;
#endif	// CONFIG_DRIVER_TI_CPSW

try_usbether:

#if defined(CONFIG_USB_ETHER)
	debug("board_eth_init: configuring USB eth ...\n");
	rv = usb_eth_initialize(bis);
	if (rv < 0)
		printf("Error %d registering USB_ETHER\n", rv);
	else
		n += rv;
#else
	debug("board_eth_init: USB eth not enabled\n");
#endif

	return n;
}
#endif	// CONFIG_DRIVER_TI_CPSW) || (CONFIG_USB_ETHER && CONFIG_MUSB_GADGET)

static int do_wreset(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	writel(0x1, PRM_RSTCTRL);
	return 0;
}

U_BOOT_CMD(
	wreset, 1, 1,  do_wreset,
	"do warm reset",
	""
);

static int do_creset(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	writel(0x2, PRM_RSTCTRL);
	return 0;
}

U_BOOT_CMD(
	creset, 1, 1,  do_creset,
	"do cold reset",
	""
);

/* Handling board state change.
 * Should be handled individually for each board (LED indication, ...)
 */
void on_board_state_changed(BoardState state)
{
	if ((board_interface != NULL) && (board_interface->on_board_state_changed != NULL))
		board_interface->on_board_state_changed(&board, state);
}

void set_board_state(BoardState state)
{
	if (state == get_board_state() && state != 111)
		return;

	boardState = state;
	on_board_state_changed(state);
}

BoardState get_board_state(void)
{
	return boardState;
}

/* change/get board status */
static int do_bstate(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 1 || argc > 2) {
		printf ("Wrong arguments.\nUsage: %s\n", cmdtp->help);
		return 1;
	}

	if (argc == 1) {
		printf("Current board state: '%s'\n", boardStateStrings[get_board_state()]);


		return 0;
	}

	if (strncmp(argv[1], "normal", 5) == 0) {
		set_board_state(BS_Normal);
	} else if (strncmp(argv[1], "dontunplug", 10) == 0) {
		set_board_state(BS_DontUnplug);
	} else if (strncmp(argv[1], "hardfailure", 11) == 0) {
		set_board_state(BS_HardFailure);
	} else if (strncmp(argv[1], "booting", 7) == 0) {
		set_board_state(BS_BootingKernel);
	} else {
		printf ("Invalid parameter: '%s'\n", argv[1]);
		printf ("Usage: %s\n", cmdtp->help);
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	bstate, 2, 1,  do_bstate,
	"set/get board state",
	"[state]\n" \
	"    state: board state to set\n"\
	"            normal - initial state, running\n"\
	"        dontunplug - do not power off or unplug device (FW update, ...)\n"\
	"       hardfailure - non recoverable error state\n"\
	"           booting - booting kernel\n"
);
