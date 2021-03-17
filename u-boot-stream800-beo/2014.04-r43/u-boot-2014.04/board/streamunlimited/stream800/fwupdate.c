/*
 * Copyright 2011 Attero Tech, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Firmware Update Functions and Commands
 */

#include "fwupdate.h"
#include <common.h>
#include <cpsw.h>
#include <hush.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include "board.h"

#ifdef CONFIG_CMD_SFU_PARSER
#include "sfu_parser.h"
#endif

static struct rtc_regs *rtcregs = (struct rtc_regs *)RTC_BASE;

/* offset indexes in RTC scratch0 register for each flag */
#define UPDATE_FLAG_INDEX	0
#define FAIL_FLAG_INDEX		2
#define BOOT_COUNT_FLAG_INDEX	3

/*
 * Modify selected flag by its index.
 */
static int flag_write( uint8_t index, uint8_t data)
{
	uint32_t valb = 0;
	uint32_t addr = (uint32_t)&rtcregs->scratch0;

	assert(index <= 3);

	if (index > 3)
		return 1;

	valb = readl(addr);

	/*
	 * Modify selected byte in RTC register.
	 * 0: (uint8*)addr + 0;    1: (uint8*)addr + 1; ..
	 */
	((uint8_t *)&valb)[index] = data;

	writel(valb, addr);

	return 0;
}

/*
 * Read selected flag by its index.
 */
static int flag_read(uint8_t index, uint8_t *data)
{
	uint32_t valb = 0;
	uint32_t addr = (uint32_t)&rtcregs->scratch0;

	assert(index <= 3);

	if (index > 3)
		return 1;

	valb = readl(addr);

	/*
	 * Read selected byte in RTC register.
	 * 0: (uint8*)addr + 0;    1: (uint8*)addr + 1; ..
	 */
	*data = ((uint8_t *)&valb)[index];

	return 0;
}


int fwupdate_init(void)
{
	int status = 0;
	char* bootlimit;

	bootlimit = getenv("bootlimit");
	if (NULL == bootlimit) {
		char buf[16];
		sprintf(buf, "%lu", MAX_BOOT_COUNT);
		setenv("bootlimit", buf);
	}

	return status;
}

int fwupdate_getUpdateFlag(uint8_t *pUpdateFlag)
{
	return flag_read(UPDATE_FLAG_INDEX, pUpdateFlag);
}

int fwupdate_setUpdateFlag(uint8_t updateFlag)
{
	return flag_write(UPDATE_FLAG_INDEX, updateFlag);
}

int fwupdate_getFailFlag(uint8_t* pFailFlag)
{
	return flag_read(FAIL_FLAG_INDEX, pFailFlag);
}

int fwupdate_setFailFlag(uint8_t failFlag)
{
	return flag_write(FAIL_FLAG_INDEX, failFlag);
}

int fwupdate_getBootCount(uint8_t* pBootCnt)
{
	return flag_read(BOOT_COUNT_FLAG_INDEX, pBootCnt);
}

int fwupdate_setBootCount(uint8_t bootCnt)
{
	return flag_write(BOOT_COUNT_FLAG_INDEX, bootCnt);
}

static int fwupdate_getUsbUpdateReq(void)
{
	if (board_interface != NULL && board_interface->do_usb_update != NULL)
		return board_interface->do_usb_update();
	//panic("Not supported board!");
	return 0;
}

static int do_fwup(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char*       cmd = NULL;
	char*       fwupflag = NULL;
	uint8_t     update;
	uint8_t     fail;
	uint8_t     bootcnt;
	char*       bootlimit;
	uint32_t    bootmax;

	/* at least two arguments please */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	/*
	 * Syntax is:
	 *   0    1     2
	 *   fwup clear flag
	 */
	if (strcmp(cmd, "clear") == 0) {
		if (argc != 3)
			goto usage;
		else
			fwupflag = argv[2];

		if (strcmp(fwupflag, "update") == 0) {
			if (0 != fwupdate_setUpdateFlag(0)) {
				return 1;
			}
		} else if (strcmp(fwupflag, "fail") == 0) {
			if (0 != fwupdate_setFailFlag(0)) {
				return 1;
			}
		}
		return 0;
	}

	if (strcmp(cmd, "flags") == 0) {
		uint8_t failFlag = 0;
		uint8_t updateFlag = 0;

		fwupdate_getFailFlag(&failFlag);
		fwupdate_getUpdateFlag(&updateFlag);

		printf("INFO: Update flags: FAIL: %d, UPDATE: %d\n",
		       failFlag, updateFlag);
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1   2
	 *   fwup set flag
	 */
	if (strcmp(cmd, "set") == 0) {
		if (argc != 3)
			goto usage;
		else
			fwupflag = argv[2];

		if (strcmp(fwupflag, "update") == 0) {
			if (0 != fwupdate_setUpdateFlag(1)) {
				return 1;
			}
		} else if (strcmp(fwupflag, "fail") == 0) {
			if (0 != fwupdate_setFailFlag(1)) {
				return 1;
			}
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1
	 *   fwup update
	 */
	if (strcmp(cmd, "update") == 0) {

		if (0 > fwupdate_getUpdateFlag(&update)) {
			return 1;
		} else {
			return (update ? 0 : 1);
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1
	 *   fwup usb_update_req
	 */
	if (strcmp(cmd, "usb_update_req") == 0)
            return fwupdate_getUsbUpdateReq() ? 0 : 1;

	/*
	 * Syntax is:
	 *   0    1
	 *   fwup fail
	 */
	if (strcmp(cmd, "fail") == 0) {

		if (0 > fwupdate_getFailFlag(&fail)) {
			return 1;
		} else {
			return (fail ? 0 : 1);
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1
	 *   fwup incbootcnt
	 */
	if (strcmp(cmd, "incbootcnt") == 0) {

		if (0 > fwupdate_getBootCount(&bootcnt)) {
			return 1;
		} else if (0 > fwupdate_setBootCount(bootcnt+1)) {
			return 1;
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1
	 *   fwup bootcnt
	 */
	if (strcmp(cmd, "bootcnt") == 0) {

		if (0 > fwupdate_getBootCount(&bootcnt)) {
			return 1;
		} else {
			bootlimit = getenv("bootlimit");
			if (NULL == bootlimit) {
				bootmax = MAX_BOOT_COUNT;
			} else {
				bootmax = simple_strtoul(bootlimit, NULL, 10);
			}
		}
		return ((bootcnt < bootmax) ? 0 : 1);
	}

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
		fwup, CONFIG_SYS_MAXARGS, 1, do_fwup,
		"Streamunlimited firmware update",
		"clear flag - clears the user requested flag\n"
		"fwup flags      - print current flags\n"
		"fwup set flag   - sets the user requested flag\n"
		"fwup update     - checks if update flag is set\n"
		"fwup usb_update_req - checks if USB update request active\n"
		"fwup fail       - checks if fail flag is set\n"
		"fwup incbootcnt - increments boot count\n"
		"fwup bootcnt    - checks if boot count is less than maximum allowed"
		);

#ifdef CONFIG_CMD_SFU_PARSER

#define SFU_BUF_LEN     (80)

static int check_sfu_signature(uint32_t addr, uint32_t *sfu_addr, int verify)
{
#ifdef CONFIG_FIT
	const char *fit_uname = "kernel";
	ulong		len;
	const image_header_t *hdr;
	ulong		*data;
	void *buf;
	const void*	fit_hdr;
	int		noffset;
	const void	*fit_data;
	size_t		fit_len;

	*sfu_addr = addr;

	buf = map_sysmem(addr, 0);
	switch (genimg_get_format(buf)) {
	case IMAGE_FORMAT_FIT:
		puts("   FIT image found\n");
		if (fit_uname == NULL) {
			puts ("No FIT subimage unit name\n");
			return 1;
		}

		fit_hdr = buf;
		if (!fit_check_format(fit_hdr)) {
			puts ("Bad FIT image format\n");
			return 1;
		}

		if (verify) {
			fit_print_contents(fit_hdr);
		}

		/* get script component image node offset */
		noffset = fit_image_get_node (fit_hdr, fit_uname);
		if (noffset < 0) {
			printf ("Can't find '%s' FIT subimage\n", fit_uname);
			return 1;
		}

		if (!fit_image_check_type (fit_hdr, noffset, IH_TYPE_KERNEL)) {
			puts ("Not a image image\n");
			return 1;
		}

		/* verify integrity */
		if (verify) {
			puts("Checking signature:");
			if (!fit_image_verify(fit_hdr, noffset)) {
				puts ("Bad Data Hash\n");
				return 1;
			}
			puts("\n");
		}

		/* get script subimage data address and length */
		if (fit_image_get_data(fit_hdr, noffset, &fit_data, &fit_len)) {
			puts ("Could not find subimage data\n");
			return 1;
		}
		*sfu_addr = (uint32_t *)fit_data;
		break;
	default:
		puts ("Wrong image format\n");
		return 1;
	}
#endif
	return 0;
}

static int do_sfu(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char*           cmd;
	uint32_t        addr;
	sfu_cpuver_t    glob_ver;
	uint32_t        num_chunks;
	sfu_cpuhdr_t    chunk_hdr;
	uint32_t        chunk;
	uint32_t        chnklen;
	char            sfubuf[SFU_BUF_LEN];
	int             idx;
	char            hexchar[3];
	int             status = 0;
	uint32_t sfu_addr;
	int verify = 0, ret;

	if (argc == 2) {
		/*
		 * Syntax is:
		 *   0   1
		 *   sfu errstate
		 */
		if (strcmp(argv[1], "errstate") == 0) {
			// Set error state (status LED to solid red, ...)
			set_board_state(BS_HardFailure);
			printf("SERIOUS ERROR\n");

			//}

			// DO NOT RETURN FROM AN ERROR STATE
			SFU_ERR("SFU: ERROR STATE  -");
			while(1) {
				SFU_ERR("\b\\");
				udelay(1 * 1000000);
				SFU_ERR("\b|");
				udelay(1 * 1000000);
				SFU_ERR("\b/");
				udelay(1 * 1000000);
				SFU_ERR("\b-");
				udelay(1 * 1000000);
			}
			return 0;
		} else {
			goto usage;
		}
	}
	/* at least three arguments for the remaining commands please */
	else if (argc < 3)
		goto usage;

	cmd = argv[1];
	addr = simple_strtoul(argv[2], NULL, 16);

	if (strcmp(cmd, "valid") == 0) {
		verify = 1;
	}

	if (strcmp(cmd, "chk_hdr") == 0) {
	/* check FIT header and return size */


		void *buf = map_sysmem(addr, 0);
		int ret;
		char strbuf[16];

		ret = fdt_check_header(buf);
		if (ret) {
			printf("Invalid FIT header! - %d\n", ret);
			return 1;
		}

		printf("FDT total size:0x%8x\n", fdt_totalsize(buf));
		sprintf(strbuf, "0x%08x", fdt_totalsize(buf));
		status = set_hush_var_with_str_value("SFU_TOTAL_LEN", strbuf);
		return 0;
	}

	/* skip FIT bytes to get correct sfu address */
	ret = check_sfu_signature(addr, &sfu_addr, verify);
	if (ret) {
		return ret;
	} else {
		addr = sfu_addr;
	}

	/*
	 * Syntax is:
	 *   0   1     2
	 *   sfu magic addr
	 */
	if (strcmp(cmd, "magic") == 0) {

		if (0 > sfu_MagicValid(addr)) {
			return 1;
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0   1     2
	 *   sfu valid addr
	 */
	if (strcmp(cmd, "valid") == 0) {

		if (0 > sfu_ImageValid(addr)) {
			return 1;
		}
		return 0;
	}

	/*
	 * Syntax is:
	 *   0   1       2
	 *   sfu globver addr
	 */
	if (strcmp(cmd, "globver") == 0) {

		if (0 > sfu_GetGlobalVersion(addr, &glob_ver)) {
			return 1;
		}
		sprintf(sfubuf, "%08x.%08x.%08x.%08x",
				glob_ver.major,
				glob_ver.minor,
				glob_ver.commitcnt,
				glob_ver.githash);
		status |= set_hush_var_with_str_value("SFU_GLOB_VERS", sfubuf);
		SFU_DBG("SFU_GLOB_VERS=%s\n", sfubuf);

		return status;
	}

	/*
	 * Syntax is:
	 *   0   1       2
	 *   sfu chnknum addr
	 */
	if (strcmp(cmd, "chnknum") == 0) {

		if (0 > sfu_GetNumChunks(addr, &num_chunks)) {
			return 1;
		}
		sprintf(sfubuf, "0x%08x", num_chunks);
		status |= set_hush_var_with_str_value("SFU_NUM_CHNKS", sfubuf);
		SFU_DBG("SFU_NUM_CHNKS=%s\n", sfubuf);

		return status;
	}

	/*
	 * Syntax is:
	 *   0   1       2    3
	 *   sfu chnkhdr addr id
	 */
	if (strcmp(cmd, "chnkhdr") == 0) {
		if (argc != 4)
			goto usage;

		if (0 > sfu_GetNumChunks(addr, &num_chunks)) {
			return 1;
		}
		SFU_DBG("num chunks=%d\n", num_chunks);
		SFU_DBG("Searching for \"%s\" chunk...\n", argv[3]);

		for (chunk = 1; num_chunks >= chunk; chunk++) {
			if (0 > sfu_GetChunkHeader(addr, chunk, &chunk_hdr)) {
				return 1;
			}
			SFU_DBG("chunk %d header name(%s) hdr_flag(0x%08x)\n",
					chunk, chunk_hdr.dest, chunk_hdr.hdr_flag);

			if ((SFU_HDR_FLAG_MINIMUM == (chunk_hdr.hdr_flag &
							SFU_HDR_FLAG_MINIMUM)) &&
					(strlen(argv[3]) == strlen(chunk_hdr.dest)) &&
					(0 == strncmp(argv[3], chunk_hdr.dest, strlen(argv[3])))) {
				// chunk found
				SFU_DBG("chunk found\n");
				break;
			}
		}

		if (chunk > num_chunks) {
			return 1;
		} else {
			SFU_DBG("\n\n--------- CHUNK NAME(%s) ---------\n", chunk_hdr.dest);
			sprintf(sfubuf, "0x%08x", (addr + chunk_hdr.data));
			status |= set_hush_var_with_str_value("SFU_CHNK_DATA", sfubuf);
			SFU_DBG("SFU_CHNK_DATA=%s\n", sfubuf);
			sprintf(sfubuf, "%s", chunk_hdr.dest);
			status |= set_hush_var_with_str_value("SFU_CHNK_DEST", sfubuf);
			SFU_DBG("SFU_CHNK_DEST=%s\n", sfubuf);
			sprintf(sfubuf, "0x%08x", chunk_hdr.size);
			status |= set_hush_var_with_str_value("SFU_CHNK_SIZE", sfubuf);
			SFU_DBG("SFU_CHNK_SIZE=%s\n", sfubuf);
			sprintf(sfubuf, "%08x.%08x.%08x.%08x",
					chunk_hdr.version.major,
					chunk_hdr.version.minor,
					chunk_hdr.version.commitcnt,
					chunk_hdr.version.githash);
			status |= set_hush_var_with_str_value("SFU_CHNK_VERS", sfubuf);
			SFU_DBG("SFU_CHNK_VERS=%s\n", sfubuf);
			sprintf(sfubuf, "%08x", chunk_hdr.encryption_method);
			status |= set_hush_var_with_str_value("SFU_CHNK_ENCM", sfubuf);
			SFU_DBG("SFU_CHNK_ENCM=%s\n", sfubuf);
			if (chunk_hdr.hdr_flag & SFU_HDR_FLAG__CR32) {
				sprintf(sfubuf, "%08x", chunk_hdr.crc32);
				status |= set_hush_var_with_str_value("SFU_CHNK_CR32", sfubuf);
				SFU_DBG("SFU_CHNK_CR32=%s\n", sfubuf);
			}
			else
			{
				status |= set_hush_var_with_str_value("SFU_CHNK_CR32", "");
				SFU_DBG("SFU_CHNK_CR32=%s\n", "");
			}
			if (chunk_hdr.hdr_flag & SFU_HDR_FLAG__MD5_) {
				memset(sfubuf, 0x00, SFU_BUF_LEN);
				for (idx = 0; sizeof(chunk_hdr.md5) > idx; idx++) {
					sprintf(hexchar, "%02x", chunk_hdr.md5[idx]);
					strncat(sfubuf, hexchar, (SFU_BUF_LEN-1));
				}
				status |= set_hush_var_with_str_value("SFU_CHNK_MD5_", sfubuf);
				SFU_DBG("SFU_CHNK_MD5_=%s\n", sfubuf);
			}
			else
			{
				status |= set_hush_var_with_str_value("SFU_CHNK_MD5_", "");
				SFU_DBG("SFU_CHNK_MD5_=%s\n", "");
			}
			if (chunk_hdr.hdr_flag & SFU_HDR_FLAG__SHA2) {
				memset(sfubuf, 0x00, SFU_BUF_LEN);
				for (idx = 0; sizeof(chunk_hdr.sha256) > idx; idx++) {
					sprintf(hexchar, "%02x", chunk_hdr.sha256[idx]);
					strncat(sfubuf, hexchar, (SFU_BUF_LEN-1));
				}
				status |= set_hush_var_with_str_value("SFU_CHNK_SHA2", sfubuf);
				SFU_DBG("SFU_CHNK_SHA2=%s\n", sfubuf);
			}
			else
			{
				status |= set_hush_var_with_str_value("SFU_CHNK_SHA2", "");
				SFU_DBG("SFU_CHNK_SHA2=%s\n", "");
			}
			SFU_DBG("--- --- --- --- --- --- ---\n\n");
		}

		return status;
	}

#ifdef CONFIG_BLOWFISH
	/*
	 * Syntax is:
	 *   0   1       2    3
	 *   sfu decrypt addr len
	 */
	if (strcmp(cmd, "decrypt") == 0) {
		if (argc != 4)
			goto usage;

		chnklen = simple_strtoul(argv[3], NULL, 16);
		if (0 > sfu_Decrypt(addr, chnklen)) {
			return 1;
		}

		return 0;
	}
#endif // CONFIG_BLOWFISH

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
		sfu, CONFIG_SYS_MAXARGS, 1, do_sfu,
		"Streamunlimited Firmware Update parser",
		"magic addr           - determine if SFU magic number is valid\n"
		"sfu valid addr       - determine if SFU image is valid\n"
		"sfu globver addr     - get the global version of the SFU image\n"
		"sfu chnknum addr     - get the number of chunks in SFU image\n"
		"sfu chnkhdr addr id  - get chunk header with id\n"
		"sfu errstate         - handle error state condition\n"
#ifdef CONFIG_BLOWFISH
		"sfu decrypt addr len - decrypt ram starting at <addr> and <len> bytes\n"
#endif // CONFIG_BLOWFISH
		);
#endif // CONFIG_CMD_SFU_PARSER

#ifdef CONFIG_HUSH_INIT_VAR
int hush_init_var (void)
{
	int status = 0;

	status |= set_hush_var_with_str_value("UBOOT_RESET", "no");
	status |= set_hush_var_with_str_value("UPDATE_XLOADER", "no");
	status |= set_hush_var_with_str_value("KERNEL_ERASED", "no");
	status |= set_hush_var_with_str_value("UPDATE_ROOTFS", "no");
	status |= set_hush_var_with_str_value("UPDATE_KERNEL", "no");
	status |= set_hush_var_with_str_value("ERROR_STATE", "no");

	status |= set_hush_var_with_str_value("SFU_TOTAL_LEN", "0x0");
	status |= set_hush_var_with_str_value("SFU_GLOB_VERS", "");
	status |= set_hush_var_with_str_value("SFU_NUM_CHNKS", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_DATA", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_SIZE", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_DEST", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_VERS", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_ENCM", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_CR32", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_MD5_", "");
	status |= set_hush_var_with_str_value("SFU_CHNK_SHA2", "");

	return status;
}
#endif
