/*
 * FFS plugin for Custom IOS.
 *
 * Copyright (C) 2009-2010 Waninkoko.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "types.h"

/* Mode codes */
#define MODE_NONE	0x0
#define MODE_SD		0x1
#define MODE_USB	0x2
#define MODE_FULL	0x100


/* Config structure */
struct ffsConfig {
	/* Mode */
	u32 mode;

	/* FS path */
	char path[FAT_MAXPATH];
};

/* Extern */
extern struct ffsConfig config;
 
#endif
