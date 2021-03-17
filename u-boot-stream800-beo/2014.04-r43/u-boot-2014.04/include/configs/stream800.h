/*
 * stream800.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Stream800 board based on am335x TI CPU.
 */

#ifndef __CONFIG_STREAM800_H
#define __CONFIG_STREAM800_H

#define CONFIG_AM33XX
#define CONFIG_NAND

#include <configs/ti_am335x_common.h>

#undef CONFIG_SPL_OS_BOOT

#define ASE_BUILD 1

#define CONFIG_DMA_COHERENT
#define CONFIG_DMA_COHERENT_SIZE	(1 << 20)

#define CONFIG_ENV_SIZE			(128 << 10) /* 128 KiB (+384 KiB for spare blocks) */
#define CONFIG_CONST_ENV_COMMON
#define CONFIG_CONSTANTS_SIZE		(128 << 10) /* 128 KiB (+384 KiB for spare blocks) */

#if (defined CONFIG_SYS_MALLOC_LEN)
#undef CONFIG_SYS_MALLOC_LEN
#define CONFIG_SYS_MALLOC_LEN       (CONFIG_ENV_SIZE + (512 << 10) + \
		CONFIG_CONSTANTS_SIZE + (1024 << 10) + (10 << 10))
#endif

#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_SYS_NO_FLASH
#define MACH_TYPE_TIAM335EVM		3589	/* Until the next sync */
#define CONFIG_MACH_TYPE		MACH_TYPE_TIAM335EVM

#define CONFIG_OF_LIBFDT
#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#ifdef CONFIG_SPL_BUILD
#define CONFIG_PANIC_HANG		/* Do halt instead of reseting when calling 'panic' for SPL build */
#else
/* enable devicetree */
#define CONFIG_OF_CONTROL
#define CONFIG_OF_SEPARATE
#define CONFIG_FIT
#define CONFIG_FIT_SIGNATURE
#define CONFIG_RSA
#define CONFIG_DEFAULT_DEVICE_TREE am33xx-beo-ase-0
#endif

/* commands to include */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_VERSION_VARIABLE
#define CONFIG_CMD_MTDPARTS

#define MTDIDS_DEFAULT          "nand0=omap2-nand.0"
/* MTDPARTS_DEFAULT are set dynamically based on flash size */
#define MTDPARTS_DEFAULT		""

#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_NAND_TRIMFFS
#define CONFIG_CMD_MKFS_UBIFS

#if (defined CONFIG_MTD_UBI_BEB_RESERVE)
#undef CONFIG_MTD_UBI_BEB_RESERVE
#define CONFIG_MTD_UBI_BEB_RESERVE  (2)
#endif

#define CONFIG_CMD_SFU_PARSER
#define CONFIG_SFU_RAM
#undef SFU_DEBUG
#undef CRC32_DEBUG
#undef MD5_DEBUG
#undef SHA256_DEBUG

/* Environment information */
#ifdef CONFIG_BOOTDELAY	/* defined in ti_armv7_common.h */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	-2 /* autoboot with no delay and not check for abort */
#endif /* CONFIG_BOOTDELAY */
#define CONFIG_BOOTCOUNT_LIMIT  6

#define _stra(s) #s
#define _str(s) _stra(s)

/*
 * Must be same as env and constants size
 * in board/streamunlimited/stream800/partitions.h
 */
#define CONFIG_UBOOTENV_BASE    0x00280000
#define CONFIG_ENV_RANGE        0x00080000
#define CONFIG_CONSTANTS_BASE   0x00300000
#define CONFIG_CONSTANTS_RANGE  0x00080000

#define CONFIG_ENV_VARS_UBOOT_CONFIG


/*
 * UPDATE PROCESS is fully described at: https://extern.streamunlimited.com:8443/display/Stream800/Firmware+Update+Architecture
 *
 * Update methods:
 *
 * 	USB RECOVERY:
 *
 * 		Following conditions has to be fulfilled to start USB recovery in u-boot:
 * 			board specific tests on USB recovery passes
 * 			=> this test usually check press of button connected to GPIO1_9
 * 			USB stick is plugged in during boot
 * 			USB stick contains valid file called sfupdate on first partition, which has to be fat32
 *
 * 		Following steps will be performed if conditions are fulfilled:
 * 			U-boot copy sfupdate to download partition
 * 			U-boot set FAIL flag and reset the board
 * 			Restarted U-boot loads sfupdate from download partition into RAM
 * 			U-boot verifies checksums and versions of all parts of sfupdate inside RAM
 * 			U-boot flashes MLO, u-boot and u-boot-env if version changed  In this case it also reset the board
 * 				If reset was performed than, u-boot recognize that FAIL flag is set and continues usb recovery
 * 				loads sfupdate from download partition into RAM again
 * 				verifies the checksums and versions and it will find out that MLO, u-boot and u-boot-env are up to date
 * 			FAIL flag is still set so u-boot erase the kernel
 * 			U-boot automatically flashes dts without version check.
 * 			U-boot automatically flashes rootfs without version check.
 * 			U-boot automatically flashes the kernel.
 * 			U-boot boots the kernel.
 * 			Userspace after correct boot should clear FAIL flag.
 *
 * 	NETWORK UPDATE:
 *
 * 		Steps happening in the userspace:
 * 			update image is downloaded and flashed to download partition
 * 			board is restarted with UPDATE flag set
 *
 * 		Following conditions has to be fullfilled to start net update in u-boot:
 * 			UPDATE flag is set
 *
 * 		Following steps will be performed if conditions are fulfilled:
 * 			Restarted U-boot loads sfupdate from download partition into RAM
 * 			U-boot verifies checksums and versions of all parts of sfupdate inside RAM
 * 			U-boot flashes MLO, u-boot and u-boot-env if version changed. In this case it also reset the board
 * 				If reset was performed than, u-boot recognize that UPDATE flag is set and continues with network update
 * 				loads sfupdate from download partition into RAM again
 * 				verifies the checksums and versions and it will find out that MLO, u-boot and u-boot-env are up to date
 * 			If rootfs or kernel version changed, kernel is erased
 * 			If dts version changed, dts is erased and flashed
 * 			If rootfs version changed, rootfs is erased and flashed
 * 			If rootfs or kernel version changed, kernel is flashed
 * 			U-boot clear UPDATE flag
 * 			U-boot set FAIL flag
 * 			U-boot boots the kernel
 * 			Userspace after correct boot should clear FAIL flag.
 *
 * METHODS:
 *		readDtb                         - loads DTB file for board from cramfs image, path in cramfs: /${cramfsdir}/${board_name}_${board_rev}.dtb
 *		readUimage                      - loads uImage to RAM
 *		handle_encm                     - decrypts chunk configured by last "sfu chnkhdr", if it was encrypted
 *		update_download_from_usb        - loads sfupdate file from usb and saves it into download partition
 * 		check_bootloaders_need_flashing - check MLO, u-boot, u-boot-env partition in image, set enviroments to signalize that flashing them is needed
 * 		flash_bootloaders_as_needed     - flash MLO, u-boot, u-boot-env partition as needed, decision is based on enviroment set by check_bootloaders_need_flashing
 * 		check_system_need_flashing      - check DTS, rootfs, kernel partition in image, set enviroments to signalize that flashing them is needed
 * 		flash_system_as_needed          - flash DTS, rootfs, kernel partition in image as needed, decision is based on enviroment set by check_system_need_flashing
 *		sfu_boot                        - main method
 *
 * VARIABLES:
 * 		bootcount                       - actual count of uncorrect reboots
 * 		bootlimit                       - limit of uncorrcet reboots, if reached, board does not boot anymore
 * 		sfu_load_addr                   - base RAM address for u-boot operations
 * 		mlo_vers                        - version of MLO stored in NAND
 * 		uboot_vers                      - version of u-boot stored in NAND
 * 		ubootenv_vers                   - version of u-bootenv stored in NAND
 * 		dts_vers                        - version of DTS partition stored in NAND
 * 		kernel_vers                     - version of kernel stored in NAND
 * 		rootfs_vers                     - version of rootfs stored in NAND
 * 		cramfsdir                       - directory in cramfs image where dtb file for this board should be located
 * 		board_name                      - name of the board, used for locating DTB file in cramfs image
 * 		board_rev                       - rev of the board, used for locating DTB file in cramfs image
 * 		cramfsdir                       - directory in cramfs image where dtb file for this board should be located
 * 		SFU_CHNK_ENCM                   - actual chunk encryption flag in sfupdate file, set by call sfu chnkdr command
 * 		SFU_CHNK_DATA                   - actual chunk data start address, set by call sfu chnkdr command
 * 		SFU_CHNK_SIZE                   - actual chunk data size, set by call sfu chnkdr command
 * 		SFU_CHNK_VERS                   - actual chunk version in sfupdate file, set by call sfu chnkdr command
 * 		SFU_DECRYPT_MLO_CHNK_SIZE       - size of MLO chunk after decryption, set by call sfu decrypt command
 * 		SFU_DECRYPT_UBOOT_CHNK_SIZE     - size of u-boot chunk after decryption, set by call sfu decrypt command
 * 		SFU_DECRYPT_UBOOTENV_CHNK_SIZE  - size of u-boot chunk after decryption, set by call sfu decrypt command
 * 		SFU_DECRYPT_DTS_CHNK_SIZE       - size dts chunk after decryption, set by call sfu decrypt command
 * 		SFU_DECRYPT_ROOTFS_CHNK_SIZE    - size rootfs chunk after decryption, set by call sfu decrypt command
 * 		SFU_DECRYPT_KERNEL_CHNK_SIZE    - size of kernel chunk after decryption, set by call sfu decrypt command
 * 		MLO_NEEDS_FLASHING              - flag for later MLO flashing
 * 		UBOOT_NEEDS_FLASHING            - flag for later u-boot flashing
 * 		UBOOTENV_NEEDS_FLASHING         - flag for later u-boot-env flashing
 * 		DTS_NEEDS_FLASHING              - flag for later dts flashing
 * 		ROOTFS_NEEDS_FLASHING           - flag for later rootfs flashing
 * 		KERNEL_NEEDS_FLASHING           - flag for later kernel flashing
 */

#define CONFIG_EXTRA_ENV_SETTINGS \
    "loadaddr=0x80200000\0" \
    "fitaddr=0x80F80000\0" \
    "fdt_high=0xffffffff\0" \
    "rdaddr=0x81000000\0" \
    "bootfile=uImage\0" \
    "fdtfile=\0" \
    "console=ttyO0,115200n8\0" \
    "optargs=\0" \
    "sfu_load_addr=0x80200000\0" \
    "cramfsaddr=0x80200000\0" \
    "dtb_load_addr=0x80800000\0" \
    "wdtargs=omap_wdt.timer_margin=300 omap_wdt.early_disable=0\0" \
	"bootargs_defaults=setenv bootargs " \
		"console=${console} " \
		"${wdtargs} " \
		"optargs=${optargs}\0" \
    "eraseSettings=" \
        "if nand erase.part settings; " \
            "then " \
            "echo \"INFO: nand erase successful\"; " \
        "else " \
            "echo \"ERROR: nand erase failed\"; " \
            "sfu errstate; " \
        "fi; \0" \
    "mtdparts=" MTDPARTS_DEFAULT "\0" \
    "readuImage=" \
        "if nboot ${fitaddr} kernel; " \
            "then " \
            "echo \"INFO: kernel partition load successful\"; " \
        "else " \
            "echo \"ERROR: cannot load kernel image from nand\"; " \
            "reset; " \
        "fi;\0" \
\
\
    "readDtb=" \
        "if nand read ${sfu_load_addr} dts; " \
            "then " \
            "if cramfsload ${dtb_load_addr} /${cramfsdir}/${board_name}_${board_rev}.dtb; " \
                "then " \
                "echo \"INFO: dts file load successful\"; " \
                "if fdt addr ${dtb_load_addr} ; " \
                    "then " \
                    "echo \"INFO: fdt addr set successful\"; " \
                    "if fdt resize; " \
                        "then " \
                        "echo \"INFO: fdt resize successful\"; " \
                    "else " \
                        "echo \"ERROR: cannot resize fdt\"; " \
                        "reset; " \
                    "fi; " \
                "else " \
                    "echo \"ERROR: cannot set fdt addr\"; " \
                    "reset; " \
                "fi; " \
            "else " \
                "echo \"ERROR: cannot load DTB file from cramfs\"; " \
                "reset; " \
            "fi; " \
        "else " \
            "echo \"ERROR: cannot load DTS image from nand\"; " \
            "reset; " \
        "fi;\0" \
\
\
    "nandroot=ubi0:stream800-rootfs ubi.mtd=11,2048 rw\0" \
    "nandrootfstype=ubifs rootwait=1\0" \
    "nandargs=setenv bootargs console=${console} " \
        "root=${nandroot} noinitrd ${wdtargs} " \
        "rootfstype=${nandrootfstype} ${mtdparts}\0" \
    "nand_boot=echo \"Booting from nand ...\"; " \
        "run nandargs; " \
        "echo \"INFO: loading kernel image into RAM...\"; " \
        "bstate booting; " \
        "run readuImage; " \
        "echo \"INFO: booting kernel image...\"; " \
        "bootm ${fitaddr}${board_conf}; " \
        "echo \"INFO: kernel boot failed...\"; " \
        "echo \"INFO: resetting...\"; " \
        "reset;\0" \
    "panicargs=panic=10 mem=127M\0" \
    "usbmount=usb reset\0" \
 \
 \
    "handle_encm=" \
        "if test ${SFU_CHNK_ENCM} = 00000001; " \
            "then " \
            "if sfu decrypt ${SFU_CHNK_DATA} ${SFU_CHNK_SIZE}; " \
                "then " \
                "echo \"INFO: chunk decrypt successful\"; " \
            "else " \
                "echo \"ERROR: chunk decrypt failed\"; " \
                "sfu errstate; " \
            "fi; " \
        "elif test ${SFU_CHNK_ENCM} = 00000000; " \
            "then " \
             "echo \"INFO: chunk not encrypted\"; " \
        "else " \
            "echo \"ERROR: encryption method invalid (${SFU_CHNK_ENCM})\"; " \
            "sfu errstate; " \
        "fi;\0" \
 \
 \
    "verify_sfu_image="\
	"if sfu valid ${sfu_load_addr}; " \
           "then " \
           "echo \"INFO: SFU image valid\"; " \
           "echo \"INFO: download partition being erased\"; " \
           "nand erase.part download; " \
           "echo \"INFO: writing sfupdate image to flash...\"; " \
           "if nand write ${sfu_load_addr} download ${filesize}; " \
               "then " \
               "echo \"INFO: nand write successful\"; " \
           "else " \
               "echo \"ERROR: nand write failed\"; " \
               "sfu errstate; " \
           "fi; " \
           "echo \"INFO: setting fail flag...\"; " \
           "fwup set fail; " \
           "SFU_IMAGE_LOAD_VALID=yes;" \
           "echo \"INFO: SFU image load and valid\"; " \
        "else echo \"INFO: SFU image invalid\"; " \
        "sfu errstate; " \
        "fi;\0" \
 \
 \
    "check_factory_state=" \
	"echo \"INFO: Checking settings,kernel,dts,rootfs,download partitions if are empty.\"; " \
	"setenv target_addr ${sfu_load_addr}; " \
	"setenv factory 1; " \
	"mw ${sfu_load_addr} 0xffffffff; " \
	"setexpr target_addr ${target_addr} + 4; " \
	"for part in settings kernel dts rootfs download; " \
	    "do; " \
	    "nand read ${target_addr} $part 4; " \
	    "cmp.l ${sfu_load_addr} ${target_addr} 1; " \
		"if test $? -eq 1; " \
		    "then; " \
		    "setenv factory 0; " \
		    "echo \"INFO: partition $part is not empty.\"; " \
		"fi; " \
	"done; " \
	"if test ${factory} -eq 0; " \
	    "then " \
	    "echo \"Board is NOT in factory state.\"; " \
	"else " \
	    "echo \"Board is in factory state.\"; " \
	"fi;\0" \
 \
 \
    "update_download_from_usb=" \
        "echo \"INFO: usb thumbdrive sfupdate check...\"; " \
        "if fatload usb ${usbdev} ${sfu_load_addr} sfupdate; " \
            "then " \
            "echo \"INFO: SFU update image found on usb thumbdrive\"; " \
	    "run verify_sfu_image;" \
        "else echo \"INFO: SFU image USB load failed\"; " \
        "fi; " \
        "echo \"INFO: usb thumbdrive sfupdate check complete\";\0" \
 \
 \
    "check_bootloaders_need_flashing=" \
        "echo \"INFO: Start check_bootloaders_need_flashing...\"; " \
        "if sfu chnkhdr ${sfu_load_addr} mlo; " \
            "then " \
            "echo \"INFO: mlo in download partition SFU update image\"; " \
            "if test -z \\\\'${mlo_vers}\\\\' || test ${mlo_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "if test -z \\\\'${mlo_vers}\\\\'; then echo \"INFO: mlo_vers is missing\"; fi; " \
                "if test ${mlo_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: mlo_vers(${mlo_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\"; fi; " \
                "echo \"INFO: mlo needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_MLO_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "MLO_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: mlo not in download partition SFU update image\"; " \
        "fi; " \
        "if sfu chnkhdr ${sfu_load_addr} u-boot; " \
            "then " \
            "echo \"INFO: uboot in download partition SFU update image\"; " \
            "if test -z \\\\'${uboot_vers}\\\\' || test ${uboot_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "if test -z \\\\'${uboot_vers}\\\\'; then echo \"INFO: uboot_vers is missing\"; fi; " \
                "if test ${uboot_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: uboot_vers(${uboot_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\"; fi; " \
                "echo \"INFO: u-boot needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_UBOOT_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "UBOOT_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: u-boot not in download partition SFU update image\"; " \
        "fi; " \
        "if sfu chnkhdr ${sfu_load_addr} u-boot-env; " \
            "then " \
            "echo \"INFO: uboot environment in download partition SFU update image\"; " \
            "if test -z \\\\'${ubootenv_vers}\\\\' || test ${ubootenv_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "if test -z \\\\'${ubootenv_vers}\\\\'; then echo \"INFO: ubootenv_vers is missing\"; fi; " \
                "if test ${ubootenv_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: ubootenv_vers(${ubootenv_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\"; fi; " \
                "echo \"INFO: u-boot-env needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_UBOOTENV_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "UBOOTENV_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: u-boot-env not in download partition SFU update image\"; " \
        "fi; " \
        "echo \"INFO: Completed check_bootloaders_need_flashing\";\0" \
 \
 \
    "flash_bootloaders_as_needed=" \
        "echo \"INFO: Starting flash_bootloaders_as_needed..\"; " \
        "TMP_MLO_VERS=${mlo_vers}; " \
        "TMP_UBOOT_VERS=${uboot_vers}; " \
        "TMP_UBOOTENV_VERS=${ubootenv_vers}; " \
        "TMP_DTS_VERS=${dts_vers}; " \
        "TMP_ROOTFS_VERS=${rootfs_vers}; " \
        "TMP_KERNEL_VERS=${kernel_vers}; " \
        "if test ${MLO_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} mlo; " \
                "then " \
                "echo \"INFO: mlo in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "echo \"INFO: mlo partition being erased\"; " \
                "nand erase.part ${SFU_CHNK_DEST}; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_MLO_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: writing mlo to flash...\"; " \
                "if nand write ${SFU_CHNK_DATA} ${SFU_CHNK_DEST} ${SFU_CHNK_SIZE}; " \
                    "then " \
                    "echo \"INFO: nand write successful\"; " \
                "else " \
                    "echo \"ERROR: nand write failed\"; " \
                    "sfu errstate; " \
                "fi; " \
                "TMP_MLO_VERS=${SFU_CHNK_VERS}; " \
                "UBOOT_RESET=yes; " \
            "else " \
                "echo \"ERROR: mlo not in download partition SFU update image\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${UBOOT_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} u-boot; " \
                "then " \
                "echo \"INFO: uboot in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "echo \"INFO: u-boot partition being erased\"; " \
                "nand erase.part ${SFU_CHNK_DEST}; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_UBOOT_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: writing uboot to flash...\"; " \
                "if nand write ${SFU_CHNK_DATA} ${SFU_CHNK_DEST} ${SFU_CHNK_SIZE}; " \
                    "then " \
                    "echo \"INFO: nand write successful\"; " \
                "else " \
                    "echo \"ERROR: nand write failed\"; " \
                    "sfu errstate; " \
                "fi; " \
                "TMP_UBOOT_VERS=${SFU_CHNK_VERS}; " \
                "UBOOT_RESET=yes; " \
            "else " \
                "echo \"ERROR: u-boot not in download partition SFU update image\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${UBOOTENV_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} u-boot-env; " \
                "then " \
                "echo \"INFO: uboot environment in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "echo \"INFO: u-boot-env partition being erased\"; " \
                "nand erase.part ${SFU_CHNK_DEST}; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_UBOOTENV_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: writing uboot-env to flash...\"; " \
                "if nand write ${SFU_CHNK_DATA} ${SFU_CHNK_DEST} ${SFU_CHNK_SIZE}; " \
                    "then " \
                    "echo \"INFO: nand write successful\"; " \
                "else " \
                    "echo \"ERROR: nand write failed\"; " \
                    "sfu errstate; " \
                "fi; " \
                "echo \"INFO: reloading uboot-env from flash to internal memory so new environment is used...\"; " \
                "env reload;" \
                "TMP_UBOOTENV_VERS=${SFU_CHNK_VERS};" \
                "UBOOT_RESET=yes; " \
            "else " \
                "echo \"ERROR: u-boot-env not in download partition SFU update image\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${MLO_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: setting mlo_vers in environment...\"; " \
            "setenv mlo_vers ${TMP_MLO_VERS};" \
        "fi; " \
        "if test ${UBOOT_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: setting uboot_vers in environment...\"; " \
            "setenv uboot_vers ${TMP_UBOOT_VERS};" \
        "fi; " \
        "if test ${UBOOTENV_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: setting mlo in environment...\"; " \
            "setenv mlo_vers ${TMP_MLO_VERS};" \
            "echo \"INFO: setting uboot_vers in environment...\"; " \
            "setenv uboot_vers ${TMP_UBOOT_VERS};" \
            "echo \"INFO: setting ubootenv_vers in environment...\"; " \
            "setenv ubootenv_vers ${TMP_UBOOTENV_VERS};" \
            "echo \"INFO: restoring dts_vers in environment...\"; " \
            "setenv dts_vers ${TMP_DTS_VERS};" \
            "echo \"INFO: restoring rootfs_vers in environment...\"; " \
            "setenv rootfs_vers ${TMP_ROOTFS_VERS};" \
            "echo \"INFO: restoring kernel_vers in environment...\"; " \
            "setenv kernel_vers ${TMP_KERNEL_VERS};" \
        "fi; " \
        "if test ${MLO_NEEDS_FLASHING} = yes || test ${UBOOT_NEEDS_FLASHING} = yes || test ${UBOOTENV_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: saving environment...\"; " \
            "saveenv;" \
        "fi; " \
        "echo \"INFO: Completed flash_bootloaders_as_needed.\";\0" \
 \
 \
    "check_system_need_flashing=" \
        "echo \"INFO: Start check_system_need_flashing...\"; " \
        "if sfu chnkhdr ${sfu_load_addr} dts; " \
            "then " \
            "echo \"INFO: dts in download partition SFU update image\"; " \
            "if fwup fail; then echo \"INFO: Fail flag is set\"; fi; " \
            "if test -z \\\\'${dts_vers}\\\\'; then echo \"INFO: dts_vers is missing\"; fi; " \
            "if test ${dts_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: dts_vers(${dts_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\";  fi; " \
            "if fwup fail ||  test -z \\\\'${dts_vers}\\\\' || test ${dts_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "echo \"INFO: dts needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_DTS_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "DTS_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: dts not in download partition SFU update image\"; " \
        "fi; " \
        "if sfu chnkhdr ${sfu_load_addr} rootfs; " \
            "then " \
            "echo \"INFO: rootfs in download partition SFU update image\"; " \
            "if fwup fail; then echo \"INFO: Fail flag is set\"; fi; " \
            "if test -z \\\\'${rootfs_vers}\\\\'; then echo \"INFO: rootfs_vers is missing\"; fi; " \
            "if test ${rootfs_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: rootfs_vers(${rootfs_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\";  fi; " \
            "if fwup fail ||  test -z \\\\'${rootfs_vers}\\\\' || test ${rootfs_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "echo \"INFO: rootfs needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_ROOTFS_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "ROOTFS_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: rootfs not in download partition SFU update image\"; " \
        "fi; " \
        "if sfu chnkhdr ${sfu_load_addr} kernel; " \
            "then " \
            "echo \"INFO: kernel in download partition SFU update image\"; " \
            "if fwup fail; then echo \"INFO: Fail flag is set\"; fi; " \
            "if test -z \\\\'${kernel_vers}\\\\'; then echo \"INFO: kernel_vers is missing\"; fi; " \
            "if test ${kernel_vers} != ${SFU_CHNK_VERS}; then echo \"INFO: kernel_vers(${kernel_vers}) not equal SFU_CHNK_VERS(${SFU_CHNK_VERS})\";  fi; " \
            "if fwup fail ||  test -z \\\\'${kernel_vers}\\\\' || test ${kernel_vers} != ${SFU_CHNK_VERS}; " \
                "then " \
                "echo \"INFO: kernel needs flashing...\"; " \
                "run handle_encm; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_DECRYPT_KERNEL_CHNK_SIZE=${SFU_CHNK_SIZE}; " \
                "fi; " \
                "KERNEL_NEEDS_FLASHING=yes; " \
            "fi; " \
        "else " \
            "echo \"INFO: kernel not in download partition SFU update image\"; " \
        "fi; " \
        "echo \"INFO: Completed check_system_need_flashing\";\0" \
 \
 \
    "flash_system_as_needed=" \
        "echo \"INFO: Starting flash_system_as_needed...\"; " \
        "if test ${DTS_NEEDS_FLASHING} = yes || test ${ROOTFS_NEEDS_FLASHING} = yes || test ${KERNEL_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: kernel partition being erased\"; " \
            "nand erase.part kernel; " \
            "KERNEL_NEEDS_FLASHING=yes; " \
        "fi; " \
        "if test ${DTS_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} dts; " \
                "then " \
                "echo \"INFO: dts in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "echo \"INFO: dts partition being erased\"; " \
                "nand erase.part ${SFU_CHNK_DEST}; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_DTS_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: writing dts to flash...\"; " \
                "if nand write ${SFU_CHNK_DATA} ${SFU_CHNK_DEST} ${SFU_CHNK_SIZE}; " \
                    "then " \
                    "echo \"INFO: nand write successful\"; " \
                "else " \
                    "echo \"ERROR: nand write failed\"; " \
                    "sfu errstate; " \
                "fi; " \
                "setenv dts_vers ${SFU_CHNK_VERS};" \
            "fi; " \
        "fi; " \
        "if test ${ROOTFS_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} rootfs; " \
                "then " \
                "echo \"INFO: rootfs in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "echo \"INFO: erasing rootfs partition\"; " \
                "nand erase.part rootfs; " \
                "if ubi part rootfs 2048; " \
                    "then " \
                    "echo \"INFO: rootfs is a valid ubi partition\"; " \
                "fi; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_ROOTFS_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: need to create ubi stream800-rootfs volume first...\"; " \
                "if ubi create stream800-rootfs; " \
                    "then " \
                    "echo \"INFO: successfully created volume, stream800-rootfs...\"; " \
                    "echo \"INFO: writing ubi rootfs to flash...\"; " \
                    "if ubi write ${SFU_CHNK_DATA} stream800-rootfs ${SFU_CHNK_SIZE}; " \
                        "then " \
                        "echo \"INFO: successfully wrote rootfs to flash after volume, stream800-rootfs, was created...\"; " \
                    "else " \
                        "echo \"ERROR: failed to write rootfs to flash after volume, stream800-rootfs, was created\"; " \
                        "sfu errstate; " \
                    "fi; " \
                "else " \
                    "echo \"ERROR: failed to create volume, stream800-rootfs\"; " \
                    "sfu errstate; " \
                "fi; " \
                "setenv rootfs_vers ${SFU_CHNK_VERS};" \
            "else " \
                "echo \"ERROR: rootfs not in download partition SFU update image\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${KERNEL_NEEDS_FLASHING} = yes; " \
            "then " \
            "if sfu chnkhdr ${sfu_load_addr} kernel; " \
                "then " \
                "echo \"INFO: kernel in download partition SFU update image\"; " \
                "bstate dontunplug; " \
                "if test ${SFU_CHNK_ENCM} = 00000001; " \
                    "then " \
                    "SFU_CHNK_SIZE=${SFU_DECRYPT_KERNEL_CHNK_SIZE}; " \
                "fi; " \
                "echo \"INFO: writing kernel to flash...\"; " \
                "if nand write ${SFU_CHNK_DATA} ${SFU_CHNK_DEST} ${SFU_CHNK_SIZE}; " \
                    "then " \
                    "echo \"INFO: nand write successful\"; " \
                "else " \
                    "echo \"ERROR: nand write failed\"; " \
                    "sfu errstate; " \
                "fi; " \
                "setenv kernel_vers ${SFU_CHNK_VERS};" \
            "else " \
                "echo \"ERROR: kernel not in download partition SFU update image\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${DTS_NEEDS_FLASHING} = yes || test ${ROOTFS_NEEDS_FLASHING} = yes || test ${KERNEL_NEEDS_FLASHING} = yes; " \
            "then " \
            "echo \"INFO: saving environment...\"; " \
            "saveenv;" \
        "fi; " \
        "echo \"INFO: Completed flash_system_as_needed.\";\0" \
 \
 \
    "sfu_boot=" \
        "echo \"INFO: SFU firmware update process started...\"; " \
        "SFU_IMAGE_LOAD_VALID=no; " \
        "if fwup fail; " \
            "then " \
            "if test ${bootcount} -eq ${bootlimit}; " \
                "then " \
                "echo \"INFO: bootcount(${bootcount}) equals bootlimit(${bootlimit})\"; " \
                "echo \"INFO: erasing settings partition\"; " \
                "run eraseSettings; " \
                "if ubi part settings 2048; " \
                    "then " \
                    "echo \"INFO: settings is a valid ubi partition\"; " \
                    "echo \"INFO: creating volume settings...\"; " \
                    "if ubi create stream800-settings; " \
                        "then " \
                        "echo \"INFO: successfully created volume, settings...\"; " \
                    "else " \
                        "echo \"ERROR: failed to create volume, settings\"; " \
                        "sfu errstate; " \
                    "fi; " \
                "fi; " \
            "fi; " \
            "if test ${bootcount} -gt ${bootlimit}; " \
                "then " \
                "echo \"INFO: bootcount(${bootcount}) greater than bootlimit(${bootlimit})\"; " \
                "bstate hardfailure; " \
                "echo \"ERROR: Maximum boot count reached!\"; " \
                "sfu errstate; " \
            "fi; " \
        "fi; " \
        "if test ${bootcount} -eq 1; " \
            "then " \
	    "run check_factory_state; " \
            "if fwup usb_update_req || test ${factory} -eq 1; " \
                "then " \
                "echo \"INFO: USB update request active, checking USB for update file ...\"; " \
                "run usbmount; " \
                "usbdev=-1; " \
                "if usb storage; " \
                    "then " \
                    "if fatfind usb 0 / sfupdate; " \
                        "then " \
                        "echo \"INFO: sfupdate image found on USB-0 thumbdrive\"; " \
                        "bstate dontunplug; " \
                        "usbdev=0; " \
                        "run update_download_from_usb; " \
                    "else " \
			 "if fatfind usb 1 / sfupdate; "\
			    "then " \
			    "echo \"INFO: sfupdate image found on USB-1 thumbdrive\"; " \
			    "bstate dontunplug; " \
			    "usbdev=1; " \
			    "run update_download_from_usb; " \
			"else " \
                            "echo \"ERROR: sfupdate image not found on any USB thumbdrive\"; " \
                            "sfu errstate; " \
			"fi;" \
		    "fi; " \
		    "else "\
		    "setenv autoload no; " \
		    "setenv autostart no; " \
		    "echo \"INFO: starting tftp update process.\"; " \
		    "if stftpup data; " \
			"then "\
			"if test -z \\\\'${serverip}\\\\'; then " \
			    "stftpup use_local_ip; " \
			    "echo \"INFO: Serverip unknown. Using local ip with last digit .10\"; " \
			"fi; " \
		    "else " \
			"echo \"DHCP failed, use static settings\"; " \
			"stftpup use_static_data; " \
		    "fi; " \
			\
		    "stftpup check_modify_bootfile; " \
			\
		    "if tftp ${sfu_load_addr} ${rootpath}${bootfile}; " \
			"then " \
			"run verify_sfu_image; " \
		    "else " \
			"echo \"INFO: Cannot fetch update file\"; " \
		    "fi; " \
                "fi; " \
            "else " \
                "echo \"INFO: USB update request is not active, not checking USB for FW update file\" ;" \
            "fi; " \
        "else " \
	        "echo \"INFO: Bootcount != 1, not checking USB for FW update file\" ;" \
        "fi; " \
        "if fwup fail || fwup update || test -z \\\\'${mlo_vers}\\\\' || test -z \\\\'${uboot_vers}\\\\' || test -z \\\\'${ubootenv_vers}\\\\' || test -z \\\\'${dts_vers}\\\\' || test -z \\\\'${rootfs_vers}\\\\' || test -z \\\\'${kernel_vers}\\\\'; " \
            "then " \
            "if fwup fail; then echo \"INFO: Fail flag is set\"; fi; " \
            "if fwup update; then echo \"INFO: Update flag is set\"; fi; " \
            "if test -z \\\\'${mlo_vers}\\\\'; then echo \"INFO: mlo_vers is missing\"; fi; " \
            "if test -z \\\\'${uboot_vers}\\\\'; then echo \"INFO: uboot_vers is missing\"; fi; " \
            "if test -z \\\\'${ubootenv_vers}\\\\'; then echo \"INFO: ubootenv_vers is missing\"; fi; " \
            "if test -z \\\\'${dts_vers}\\\\'; then echo \"INFO: dts_vers is missing\"; fi; " \
            "if test -z \\\\'${kernel_vers}\\\\'; then echo \"INFO: kernel_vers is missing\"; fi; " \
            "if test -z \\\\'${rootfs_vers}\\\\'; then echo \"INFO: rootfs_vers is missing\"; fi; " \
            "bstate dontunplug; " \
            "VALID_IMAGE=no; " \
            "if test ${SFU_IMAGE_LOAD_VALID} = yes; " \
                "then " \
                "VALID_IMAGE=yes; " \
            "else " \
                "echo \"INFO: Need to read the sfupdate from download partition...\"; " \
                "if nand read ${sfu_load_addr} download 0x64; " \
                    "then " \
 		    "if sfu chk_hdr ${sfu_load_addr}; " \
 			"then "\
			    "echo \"INFO: sfupdate image total length is ${SFU_TOTAL_LEN}\"; " \
			    "if nand read ${sfu_load_addr} download ${SFU_TOTAL_LEN}; " \
			        "then " \
				    "if sfu valid ${sfu_load_addr}; " \
					"then " \
					"echo \"INFO: SFU image in download partition is valid\"; " \
					"VALID_IMAGE=yes; " \
				    "else " \
					"echo \"ERROR: SFU image in download partition is invalid\"; " \
				    "fi; " \
				"else " \
				    "echo \"ERROR: nand read failed\"; " \
				"fi; " \
			    "else "\
				"echo \"ERROR: SFU image in download partition is invalid\"; " \
 			"fi; " \
                "else " \
                    "echo \"ERROR: nand read failed\"; " \
                "fi; " \
            "fi; " \
            "if test ${VALID_IMAGE} = yes; " \
                "then " \
                "UBOOT_NEEDS_FLASHING=no; " \
                "UBOOTENV_NEEDS_FLASHING=no; " \
                "MLO_NEEDS_FLASHING=no; " \
                "DTS_NEEDS_FLASHING=no; " \
                "ROOTFS_NEEDS_FLASHING=no; " \
                "KERNEL_NEEDS_FLASHING=no; " \
                "UBOOT_RESET=no; " \
                "run check_bootloaders_need_flashing; " \
                "run flash_bootloaders_as_needed; " \
                "if test ${UBOOT_RESET} = yes; " \
                "then " \
                    "echo \"INFO: Resetting ...\"; " \
                    "reset; " \
                "fi;" \
                "run check_system_need_flashing; " \
                "run flash_system_as_needed; " \
             "else " \
                "if fwup fail || test -z \\\\'${mlo_vers}\\\\' || test -z \\\\'${uboot_vers}\\\\' || test -z \\\\'${ubootenv_vers}\\\\' || test -z \\\\'${dts_vers}\\\\' || test -z \\\\'${rootfs_vers}\\\\' || test -z \\\\'${kernel_vers}\\\\'; " \
                "then " \
                    "sfu errstate; " \
                "fi; " \
             "fi; " \
        "fi; " \
        "echo \"INFO: SFU firmware update process completed\"; " \
        "echo \"INFO: clearing update flag...\"; " \
        "fwup clear update; " \
        "echo \"INFO: setting fail flag...\"; " \
        "fwup set fail; " \
        "bstate normal; " \
        "run nand_boot;\0" \

#define CONFIG_BOOTCOMMAND \
	"echo \"INFO: attempting SFU boot...\"; " \
	"echo \"INFO: MLO version: ${mlo_vers}\"; " \
	"echo \"INFO: U-boot version: ${uboot_vers}\"; " \
	"echo \"INFO: UImage version: ${kernel_vers}\"; " \
	"echo \"INFO: Rootfs version: ${rootfs_vers}\"; " \
	"fwup flags; "\
	"run sfu_boot;" \

#define CONFIG_ALTBOOTCOMMAND \
	"echo \"ERROR: Maximum boot count reached!\"; sfu errstate;"

/* Clock Defines */
#define V_OSCK				25000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

#define CONFIG_CMD_ECHO

/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		512

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/*
 * memtest works on 8 MB in DRAM after skipping 32MB from
 * start addr of ram disk
 */
#define CONFIG_SYS_MEMTEST_START	(PHYS_DRAM_1 + (64 * 1024 * 1024))
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START \
					+ (8 * 1024 * 1024))

#if (defined CONFIG_SYS_LOAD_ADDR)
#undef CONFIG_SYS_LOAD_ADDR
#define CONFIG_SYS_LOAD_ADDR		0x81000000 /* Default load address */
#endif

#define CONFIG_SYS_HZ			1000 /* 1ms clock */

#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_OMAP_HSMMC
#define CONFIG_CMD_MMC
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2

#define CONFIG_SPI
#define CONFIG_OMAP3_SPI
#define CONFIG_MTD_DEVICE
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED		(24000000)

#define CONFIG_SYS_INIT_SP_ADDR         (NON_SECURE_SRAM_END - \
						GENERATED_GBL_DATA_SIZE)
 /* Platform/Board specific defs */
#define CONFIG_SYS_TIMERBASE		0x48040000	/* Use Timer2 */
#define CONFIG_SYS_PTV			2	/* Divisor: 2^(PTV+1) => 8 */
#define CONFIG_SYS_HZ			1000

/* NS16550 Configuration */
#define CONFIG_SERIAL_MULTI
#define CONFIG_SYS_NS16550_COM1		0x44e09000	/* Base EVM has UART0 */
#define CONFIG_SYS_NS16550_COM2		0x48022000	/* UART1 */
#define CONFIG_SYS_NS16550_COM3		0x48024000	/* UART2 */
#define CONFIG_SYS_NS16550_COM4		0x481a6000	/* UART3 */
#define CONFIG_SYS_NS16550_COM5		0x481a8000	/* UART4 */
#define CONFIG_SYS_NS16550_COM6		0x481aa000	/* UART5 */

/* PMIC support */
#define CONFIG_POWER_TPS65217
#define CONFIG_POWER_TPS65910

/* I2C Configuration */
#define CONFIG_I2C
#define CONFIG_CMD_I2C
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SLAVE		1
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_DRIVER_OMAP24XX_I2C
#define CONFIG_CMD_EEPROM
#define CONFIG_ENV_EEPROM_IS_ON_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50	/* Main EEPROM */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2
#define CONFIG_SYS_I2C_MULTI_EEPROMS

#define CONFIG_OMAP_GPIO

#define CONFIG_BAUDRATE		115200

#define CONFIG_SYS_CONSOLE_INFO_QUIET

#define CONFIG_ENV_IS_NOWHERE

/* Defines for SPL */
#define CONFIG_SPL
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_STACK		CONFIG_SYS_INIT_SP_ADDR

#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR	0x300 /* address 0x60000 */
#define CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS	0x200 /* 256 KB */
#define CONFIG_SYS_MMC_SD_FAT_BOOT_PARTITION	1
#define CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME	"u-boot.img"
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_I2C_SUPPORT

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_GPIO_SUPPORT
#define CONFIG_SPL_YMODEM_SUPPORT
/*#define CONFIG_SPL_NET_SUPPORT*/
/*#define CONFIG_SPL_NET_VCI_STRING	"AM335x U-Boot SPL"*/
/*#define CONFIG_SPL_ETH_SUPPORT*/
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS		0
#define CONFIG_SPL_SPI_CS		0
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x20000
#define CONFIG_SYS_SPI_U_BOOT_SIZE	0x40000
#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/omap-common/u-boot-spl.lds"

#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SPL_NAND_AM33XX_BCH
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_NAND_ECC
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / \
					 CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128*1024)
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS		{ 2, 3, 4, 5, 6, 7, 8, 9, \
					 10, 11, 12, 13, 14, 15, 16, 17, \
					 18, 19, 20, 21, 22, 23, 24, 25, \
					 26, 27, 28, 29, 30, 31, 32, 33, \
					 34, 35, 36, 37, 38, 39, 40, 41, \
					 42, 43, 44, 45, 46, 47, 48, 49, \
					 50, 51, 52, 53, 54, 55, 56, 57, }

#define CONFIG_SYS_NAND_ECCSIZE		512
#define CONFIG_SYS_NAND_ECCBYTES	14

#define CONFIG_SYS_NAND_ECCSTEPS	4
#define	CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * \
						CONFIG_SYS_NAND_ECCSTEPS)

#define CONFIG_NAND_OMAP_ECCSCHEME	OMAP_ECC_BCH8_CODE_HW
#define CONFIG_NAND_OMAP_ELM
#define	CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x80000

/*
 * 1MB into the SDRAM to allow for SPL's bss at the beginning of SDRAM
 * 64 bytes before this address should be set aside for u-boot.img's
 * header. That is 0x800FFFC0--0x80100000 should not be used for any
 * other needs.
 */
#define CONFIG_SYS_TEXT_BASE		0x80800000

/*
 * USB configuration
 */
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_USB_MUSB_DSPS
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_MUSB_GADGET
#define CONFIG_MUSB_PIO_ONLY
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_MUSB_HOST
#define CONFIG_AM335X_USB0
#define CONFIG_AM335X_USB0_MODE	MUSB_HOST
#define CONFIG_AM335X_USB1
#define CONFIG_AM335X_USB1_MODE MUSB_HOST

#ifdef CONFIG_MUSB_HOST
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#endif

#ifdef CONFIG_MUSB_GADGET
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#endif /* CONFIG_MUSB_GADGET */

/* Unsupported features */
#undef CONFIG_USE_IRQ

#define CONFIG_CMD_NET
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PING
#define CONFIG_DRIVER_TI_CPSW
#define CONFIG_MII
#define CONFIG_CMD_MII
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_SERVER_IP
#define CONFIG_BOOTP_MAY_FAIL
#define CONFIG_BOOTP_BOOTILE
#define CONFIG_NET_RETRY_COUNT         10
#define CONFIG_NET_MULTI
#define CONFIG_PHY_GIGE
#define CONFIG_PHYLIB
#define CONFIG_PHY_SMSC

/* NAND support */
#ifdef CONFIG_NAND
#define CONFIG_CMD_NAND
#define CONFIG_NAND_OMAP_GPMC
#define GPMC_NAND_ECC_LP_x8_LAYOUT	1

#define CONFIG_CMD_CRAMFS
#define CONFIG_CRAMFS_CMDLINE
#define CONFIG_CMD_FLASH

							/* to access nand at */
							/* CS0 */
#define CONFIG_SYS_MAX_NAND_DEVICE	1		/* Max number of NAND
							   devices */
#undef CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_IS_IN_NAND 1
#define CONFIG_ENV_OFFSET		CONFIG_UBOOTENV_BASE /* environment starts here */
#define ONENAND_ENV_OFFSET		CONFIG_UBOOTENV_BASE /* environment starts here */
#define ONENAND_END_SIZE        CONFIG_UBOOTENV_SIZE
#define CONFIG_SYS_ENV_SECT_SIZE	(128 << 10)	/* 128 KiB */

#endif

#define CONFIG_CONSTANTS_OFFSET     CONFIG_CONSTANTS_BASE
#define CONFIG_CONST_OFFSET_REDUND  (CONFIG_CONSTANTS_BASE + CONFIG_CONSTANTS_SIZE)

/*  allow overwrite in the constants partition */
#define CONFIG_CONST_OVERWRITE

#define CONFIG_CONST_ECC_AUTOCORRECT

/* watchdog timeout set for 5 minutes */
#define CONFIG_WATCHDOG_TIMEOUT_MSECS 300000

#define CONFIG_HUSH_INIT_VAR
#define CONFIG_MD5
#define CONFIG_SHA256
#define CONFIG_CMD_MD5SUM
#define CONFIG_CMD_SHA256SUM
#define CONFIG_BLOWFISH
#define CONFIG_CMD_SETEXPR

#undef CONFIG_USB_ETHER

#endif	/* ! __CONFIG_STREAM800_H */
