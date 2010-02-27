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

#ifndef _FAT_H_
#define _FAT_H_

#include <sys/stat.h>
#include <sys/statvfs.h>

#include "types.h"

/* Filestats structure */
typedef struct {
	u32 file_length;
	u32 file_pos;
} fstats;

/* Constants */
#define FAT_MAXPATH	256


/* Prototypes */
s32 FAT_Init(void);
s32 FAT_CreateDir(const char *dirpath);
s32 FAT_CreateFile(const char *filepath);
s32 FAT_ReadDir(const char *dirpath, void *outbuf, u32 *entries);
s32 FAT_Delete(const char *path);
s32 FAT_DeleteDir(const char *dirpath);
s32 FAT_Rename(const char *oldpath, const char *newpath);
s32 FAT_Stat(const char *path, struct stat *stats);
s32 FAT_GetVfsStats(const char *path, struct statvfs *stats);
s32 FAT_GetFileStats(s32 fd, fstats *stats);
s32 FAT_GetUsage(const char *path, u32 *blocks, u32 *inodes);
 
#endif
