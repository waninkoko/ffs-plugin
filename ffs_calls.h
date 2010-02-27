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

/* FFS handlers */
s32 _ffs_unk   (void *data);
s32 _ffs_open  (void *data);
s32 _ffs_close (void *data);
s32 _ffs_read  (void *data);
s32 _ffs_write (void *data);
s32 _ffs_seek  (void *data);
s32 _ffs_ioctl (void *data);
s32 _ffs_ioctlv(void *data);

/* Syscall open hook */
s32 _syscall_open(char *path, s32 mode);

#endif
