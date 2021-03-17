/*
 * (C) Copyright 2011 Attero Tech LLC
 *
 * Written by: Dan Brunswick <dan.brunswick@atterotech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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
 *
 */

#ifndef _SFU_PARSER_H_
#define _SFU_PARSER_H_

#include <common.h>

#ifdef CONFIG_SFU_FILE
#include <stdint.h>
#include <stdbool.h>
#endif // CONFIG_SFU_FILE

#include <errno.h>
#define SFU_ERR(x...) printf(x)

#ifdef SFU_DEBUG
#define SFU_DBG(x...) printf(x)
#else
#define SFU_DBG(x...)
#endif

#define SFU_FILE_HEADER_MAGIC                 "S800_UPD"
#define SFU_DOWNLOAD_MTD_PARTITION      "/dev/mtd7"
#define MAX_SFU_IMG_FMT_VERSION         (0x00000001UL)
#define SFU_GLBL_CHNK_ID                "CHNK"
#define SFU_CHNK_OPT_ID_VERS            "vers"
#define SFU_CHNK_OPT_ID_SIZE            "size"
#define SFU_CHNK_OPT_ID_DATA            "data"
#define SFU_CHNK_OPT_ID_DEST            "dest"
#define SFU_CHNK_OPT_ID_MD5_            "md5_"
#define SFU_CHNK_OPT_ID_SHA2            "sha2"
#define SFU_CHNK_OPT_ID_CR32            "cr32"
#define SFU_CHNK_OPT_ID_ENCM            "encm"

#define IMG_VALIDITY__VALID             (0)
#define IMG_VALIDITY__MAGIC_ERR         (-1)
#define IMG_VALIDITY__TOTLEN_ERR        (-2)
#define IMG_VALIDITY__IMGFMTVER_ERR     (-3)
#define IMG_VALIDITY__CRC32_ERR         (-4)
#define IMG_VALIDITY__FILE_ERR          (-5)
#define IMG_VALIDITY__MD5_ERR           (-6)
#define IMG_VALIDITY__SH256_ERR         (-7)

#define SFU_MAGIC_BYTE_LEN              (8)
#define SFU_COUNT_BYTE_LEN              (4)
#define SFU_CRC32_BYTE_LEN              (4)
#define SFU_TOTAL_LENGTH_BYTE_LEN       (4)
#define SFU_IMG_FMT_VERSION_BYTE_LEN    (4)
#define SFU_VERSION_BYTE_LEN            (4)
#define SFU_OPTION_ID_BYTE_LEN          (4)
#define SFU_OPTION_LENGTH_BYTE_LEN      (4)
#define SFU_CHNK_CNT_BYTE_LEN           (4)
#define SFU_DEST_BYTE_LEN               (32)
#define SFU_MD5_HASH_LEN                (16)
#define SFU_SHA256_HASH_LEN             (32)
#define MAX_SFU_OPTION_DATA_LEN         (512)

#define SFU_HDR_FLAG__VERS      (0x00000001UL)
#define SFU_HDR_FLAG__SIZE      (0x00000002UL)
#define SFU_HDR_FLAG__DATA      (0x00000004UL)
#define SFU_HDR_FLAG__DEST      (0x00000008UL)
#define SFU_HDR_FLAG__MD5_      (0x00000010UL)
#define SFU_HDR_FLAG__SHA2      (0x00000020UL)
#define SFU_HDR_FLAG__CR32      (0x00000040UL)
#define SFU_HDR_FLAG__ENCM      (0x00000080UL)

#define SFU_HDR_FLAG_MINIMUM    (SFU_HDR_FLAG__VERS | \
                                 SFU_HDR_FLAG__SIZE | \
                                 SFU_HDR_FLAG__DATA | \
                                 SFU_HDR_FLAG__DEST | \
                                 SFU_HDR_FLAG__ENCM)

#define SFU_32BIT_VALUE(addr)   ((addr[3] << 24) | \
                                 (addr[2] << 16) | \
                                 (addr[1] << 8)  | \
                                 (addr[0]))

typedef struct
{
    uint32_t major;
    uint32_t minor;
    uint32_t commitcnt;
    uint32_t githash;
} sfu_cpuver_t;

typedef struct
{
    uint8_t major[SFU_VERSION_BYTE_LEN];
    uint8_t minor[SFU_VERSION_BYTE_LEN];
    uint8_t commitcnt[SFU_VERSION_BYTE_LEN];
    uint8_t githash[SFU_VERSION_BYTE_LEN];
} sfu_ver_t;
#define SFU_VER_LEN     sizeof(sfu_ver_t)

typedef struct
{
    uint8_t     magic[SFU_MAGIC_BYTE_LEN];
    uint8_t     img_fmt_version[SFU_IMG_FMT_VERSION_BYTE_LEN];
    uint8_t     total_length[SFU_TOTAL_LENGTH_BYTE_LEN];
    uint8_t     global_crc32[SFU_CRC32_BYTE_LEN];
    sfu_ver_t   global_version;
    uint8_t     global_options_count[SFU_COUNT_BYTE_LEN];
    uint8_t     var_len_data[];
} sfu_global_hdr_w_data_t;
#define GLOBAL_HDR_LEN          (SFU_MAGIC_BYTE_LEN +           \
                                 SFU_IMG_FMT_VERSION_BYTE_LEN + \
                                 SFU_TOTAL_LENGTH_BYTE_LEN +    \
                                 SFU_CRC32_BYTE_LEN +           \
                                 SFU_VER_LEN +                  \
                                 SFU_COUNT_BYTE_LEN)
#define GLOBAL_OPT_CNT_OFFSET   (GLOBAL_HDR_LEN - SFU_COUNT_BYTE_LEN)
#define GLOBAL_CRC_OFFSET       (SFU_MAGIC_BYTE_LEN +           \
                                 SFU_IMG_FMT_VERSION_BYTE_LEN + \
                                 SFU_TOTAL_LENGTH_BYTE_LEN)
#define GLOBAL_CRC_CALC_OFFSET  (SFU_MAGIC_BYTE_LEN +           \
                                 SFU_IMG_FMT_VERSION_BYTE_LEN + \
                                 SFU_TOTAL_LENGTH_BYTE_LEN +    \
                                 SFU_CRC32_BYTE_LEN)

typedef struct
{
    uint8_t id[SFU_OPTION_ID_BYTE_LEN];
    uint8_t length[SFU_OPTION_LENGTH_BYTE_LEN];
} sfu_option_hdr_t;
#define SFU_OPTION_HDR_LEN  sizeof(sfu_option_hdr_t)

typedef struct
{
    sfu_option_hdr_t hdr;
    uint8_t          var_len_data[MAX_SFU_OPTION_DATA_LEN];
} sfu_option_t;

typedef struct
{
    uint32_t        hdr_flag;
    sfu_cpuver_t    version;
    uint32_t        size;
    uint32_t        data;
    char            dest[SFU_DEST_BYTE_LEN];
    uint8_t         md5[SFU_MD5_HASH_LEN];
    uint8_t         sha256[SFU_SHA256_HASH_LEN];
    uint32_t        crc32;
    uint32_t        encryption_method;
} sfu_cpuhdr_t;


#ifdef CONFIG_SFU_FILE
#define FILE_RETRY_COUNT    (10)

int32_t sfu_ImageValid(int fd);
int32_t sfu_GetGlobalVersion(int           fd,
                             sfu_cpuver_t* pGlobalVer);
int32_t sfu_GetNumChunks(int       fd,
                         uint32_t* pNumChunks);
int32_t sfu_GetChunkHeader(int           fd,
                           uint32_t      chunkNum,
                           sfu_cpuhdr_t* pHdr);
#ifdef CONFIG_BLOWFISH
int32_t sfu_Decrypt(int      fd,
                    uint32_t data,
                    uint32_t size
);
#endif // CONFIG_BLOWFISH
#endif // CONFIG_SFU_FILE

#ifdef CONFIG_SFU_RAM
int32_t sfu_MagicValid(uint32_t startAddr);
int32_t sfu_ImageValid(uint32_t startAddr);
int32_t sfu_GetGlobalVersion(uint32_t      startAddr,
                             sfu_cpuver_t* pGlobalVer);
int32_t sfu_GetNumChunks(uint32_t  startAddr,
                         uint32_t* pNumChunks);
int32_t sfu_GetChunkHeader(uint32_t      startAddr,
                           uint32_t      chunkNum,
                           sfu_cpuhdr_t* pHdr);
#ifdef CONFIG_BLOWFISH
int32_t sfu_Decrypt(uint32_t data,
                    uint32_t size);
#endif // CONFIG_BLOWFISH
#endif // CONFIG_SFU_RAM

int set_hush_var_with_str_value(const char *var_name, char *var_value);

#endif /* _SFU_PARSER_H_ */
