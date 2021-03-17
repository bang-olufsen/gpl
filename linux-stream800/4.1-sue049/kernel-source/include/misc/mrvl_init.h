/*
 * include/misc/mrvl_init.h
 *
 * Copyright (C) 2013 StreamuUnlimited.
 *
 * Authors:
 *	Matus Ujhelyi	<matus.ujhelyi@streamunlimited.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __MRVL_INIT_H__
#define __MRVL_INIT_H__

struct mrvl_init_platform_data {
	int		gpio_nr;
	int		gpio_flag;
	const char	*clock_name;
};

#endif /* __MRVL_INIT_H__ */

