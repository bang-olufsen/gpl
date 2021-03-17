#ifndef _PARTITIONS_H
#define _PARTITIONS_H

/* For 256MiB flash  */

struct partition {
	u32 base;
	u32 size;
	const char *name;
};

enum s800_flash_size {
	FLASH_SIZE_256M = 0,
	FLASH_SIZE_512M,
	FLASH_SIZE_UNKNOWN,
};

struct partition s800_part_256M[] = {
	[0] = { 0x00000000, 0x00020000, "mlo" },
	[1] = { 0x00020000, 0x00020000, "mlo2" },
	[2] = { 0x00040000, 0x00020000, "mlo3" },
	[3] = { 0x00060000, 0x00020000, "mlo4" },
	[4] = { 0x00080000, 0x00200000, "u-boot" },
	/*
	 * when update env size or base update also CONFIG_ENV_RANGE and
	 * CONFIG_UBOOTENV_BASE in stream800.h
	 */
	[5] = { 0x00280000, 0x00080000, "u-boot-env" },
	/*
	 * when update constants size or base  update also CONFIG_CONSTANTS_RANGE
	 * and CONFIG_CONSTANTS_BASE  in stream800.h
	 */
	[6] = { 0x00300000, 0x00080000, "constants" },
	[7] = { 0x00380000, 0x01400000, "settings" },
	[8] = { 0x01780000, 0x00600000, "kernel" },
	[9] = { 0x01D80000, 0x00200000, "dts" },
	[10] = { 0x01F80000, 0x06700000, "rootfs" },
	[11] = { 0x08680000, 0x07980000, "download" }
};

struct partition s800_part_512M[] = {
	[0] = { 0x00000000, 0x00020000, "mlo" },
	[1] = { 0x00020000, 0x00020000, "mlo2" },
	[2] = { 0x00040000, 0x00020000, "mlo3" },
	[3] = { 0x00060000, 0x00020000, "mlo4" },
	[4] = { 0x00080000, 0x00200000, "u-boot" },
	/*
	 * when update env size and or update also CONFIG_ENV_RANGE and
	 * CONFIG_UBOOTENV_BASE in stream800.h
	 */
	[5] = { 0x00280000, 0x00080000, "u-boot-env" },
	/*
	 * when update constants size or base  update also CONFIG_CONSTANTS_RANGE
	 * and CONFIG_CONSTANTS_BASE  in stream800.h
	 */
	[6] = { 0x00300000, 0x00080000, "constants" },
	[7] = { 0x00380000, 0x00a00000, "secure" },
	[8] = { 0x00d80000, 0x0f600000, "settings"},
	[9] = { 0x10380000, 0x00600000, "kernel" },
	[10] = { 0x10980000, 0x00200000, "dts" },
	[11] = { 0x10B80000, 0x07B00000, "rootfs" },
	[12] = { 0x18680000, 0x07980000, "download" }
};

#endif /* _PARTITIONS_H */
