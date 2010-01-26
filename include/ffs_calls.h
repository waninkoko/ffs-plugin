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

#ifndef _FFS_CALLS_H_
#define _FFS_CALLS_H_

#include "types.h"

/* Debug */
#ifdef DEBUG
void FFS_printf(const char *fmt, ...);
#else
# define FFS_printf(fmt, ...)
#endif

/* FFS command handlers */
s32 FFS_HandleOpen  (void *data);
s32 FFS_HandleClose (void *data);
s32 FFS_HandleRead  (void *data);
s32 FFS_HandleWrite (void *data);
s32 FFS_HandleSeek  (void *data);
s32 FFS_HandleIoctl (void *data);
s32 FFS_HandleIoctlv(void *data);

#endif
