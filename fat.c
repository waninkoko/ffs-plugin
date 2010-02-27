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

#include <string.h>

#include "fat.h"
#include "ipc.h"
#include "isfs.h"
#include "syscalls.h"
#include "tools.h"
#include "types.h"

/* IOCTL commands */
#define IOCTL_FAT_FILESTATS	11

/* IOCTLV commands */
#define IOCTL_FAT_MKDIR		0x01
#define IOCTL_FAT_MKFILE	0x02
#define IOCTL_FAT_READDIR	0x03
#define IOCTL_FAT_READDIR_LFN	0x04
#define IOCTL_FAT_DELETE	0x05
#define IOCTL_FAT_DELETEDIR	0x06
#define IOCTL_FAT_RENAME	0x07
#define IOCTL_FAT_STAT		0x08
#define IOCTL_FAT_VFSSTATS	0x09
#define IOCTL_FAT_GETUSAGE	0x0A
#define IOCTL_FAT_MOUNT_SD	0xF0
#define IOCTL_FAT_UMOUNT_SD	0xF1
#define IOCTL_FAT_MOUNT_USB	0xF2
#define IOCTL_FAT_UMOUNT_USB	0xF3

/* FAT structure */
typedef struct {
	ioctlv vector[8];

	union {
		char   filename[FAT_MAXPATH];
		fstats filestats;

		struct {
			char filename[FAT_MAXPATH];
			s32  mode;
		} open;

		struct {
			char filename[FAT_MAXPATH];
			s32  outlen;
		} readDir;

		struct {
			char oldname[FAT_MAXPATH];
			char newname[FAT_MAXPATH];
		} rename;

		struct {
			char filename[FAT_MAXPATH];
			struct stat fstat;
		} stat;

		struct {
			char filename[FAT_MAXPATH];
			struct statvfs vstat;
		} statVfs;
	
		struct {
			char filename[FAT_MAXPATH];
			u32  size;
			u8   padding[28];
			u32  inodes;
		} getUsage;
	};
} ATTRIBUTE_PACKED fatBuf;


/* Variables */
static s32 fatFd = 0;

/* I/O buffer */
static fatBuf *iobuf = NULL;


s32 FAT_Init(void)
{
	/* FAT module already open */
	if (fatFd > 0)
		return 0;

	/* Allocate memory */
	if (!iobuf)
		iobuf = os_heap_alloc_aligned(0, sizeof(*iobuf), 32);

	/* Open FAT module */
	fatFd = os_open("fat", 0);
	if (fatFd < 0)
		return fatFd;

	return 0;
}

s32 FAT_CreateDir(const char *dirpath)
{
	/* Copy path */
	strcpy(iobuf->filename, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->filename;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Create directory */
	return os_ioctlv(fatFd, IOCTL_FAT_MKDIR, 1, 0, iobuf->vector);
}

s32 FAT_CreateFile(const char *filepath)
{
	/* Copy path */
	strcpy(iobuf->filename, filepath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->filename;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Create file */
	return os_ioctlv(fatFd, IOCTL_FAT_MKFILE, 1, 0, iobuf->vector);
}

s32 FAT_ReadDir(const char *dirpath, void *outbuf, u32 *entries)
{
	u32 in = 1, io = 1;
	s32 ret;

	/* Copy path */
	strcpy(iobuf->readDir.filename, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->readDir.filename;
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->readDir.outlen;
	iobuf->vector[1].len  = 4;

	if (outbuf) {
		u32 cnt = *entries;

		/* Input/Output buffers */
		in = io = 2;

		/* Set entries value */
		iobuf->readDir.outlen = cnt;

		iobuf->vector[2].data = outbuf;
		iobuf->vector[2].len  = (FAT_MAXPATH * cnt);
		iobuf->vector[3].data = &iobuf->readDir.outlen;
		iobuf->vector[3].len  = 4;
	}

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Read directory */
	ret = os_ioctlv(fatFd, IOCTL_FAT_READDIR, in, io, iobuf->vector);

	os_sync_before_read(iobuf, sizeof(*iobuf));

	/* Copy results */
	if (ret >= 0)
		*entries = iobuf->readDir.outlen;

	return ret;
}

s32 FAT_Delete(const char *path)
{
	/* Copy path */
	strcpy(iobuf->filename, path);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->filename;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Delete */
	return os_ioctlv(fatFd, IOCTL_FAT_DELETE, 1, 0, iobuf->vector);	
}

s32 FAT_DeleteDir(const char *dirpath)
{
	/* Copy path */
	strcpy(iobuf->filename, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->filename;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Delete */
	return os_ioctlv(fatFd, IOCTL_FAT_DELETEDIR, 1, 0, iobuf->vector);	
}

s32 FAT_Rename(const char *oldpath, const char *newpath)
{
	/* Copy paths */
	strcpy(iobuf->rename.oldname, oldpath);
	strcpy(iobuf->rename.newname, newpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->rename.oldname;
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = iobuf->rename.newname;
	iobuf->vector[1].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Rename */
	return os_ioctlv(fatFd, IOCTL_FAT_RENAME, 2, 0, iobuf->vector);
}

s32 FAT_Stat(const char *path, struct stat *stats)
{
	s32 ret;

	/* Copy path */
	strcpy(iobuf->stat.filename, path);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->stat.filename;
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->stat.fstat;
	iobuf->vector[1].len  = sizeof(struct stat);

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get stats */
	ret = os_ioctlv(fatFd, IOCTL_FAT_STAT, 1, 1, iobuf->vector);

	if (ret >= 0 && stats) {
		/* Copy data */
		memcpy(stats, &iobuf->stat.fstat, sizeof(struct stat));

		/* Flush cache */
		os_sync_after_write(stats, sizeof(struct stat));
	}

	return ret;
}

s32 FAT_GetVfsStats(const char *path, struct statvfs *stats)
{
	s32 ret;

	/* Copy path */
	strcpy(iobuf->statVfs.filename, path);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->statVfs.filename;
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->statVfs.vstat;
	iobuf->vector[1].len  = sizeof(struct statvfs);

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get filestats */
	ret = os_ioctlv(fatFd, IOCTL_FAT_VFSSTATS, 1, 1, iobuf->vector);

	if (ret >= 0) {
		/* Copy data */
		memcpy(stats, &iobuf->statVfs.vstat, sizeof(struct statvfs));

		/* Flush cache */
		os_sync_after_write(stats, sizeof(struct statvfs));
	}

	return ret;
}

s32 FAT_GetFileStats(s32 fd, fstats *stats)
{
	s32 ret;

	/* Get filestats */
	ret = os_ioctl(fd, IOCTL_FAT_FILESTATS, &iobuf->filestats, sizeof(fstats), NULL, 0);

	if (ret >= 0) {
		/* Copy data */
		memcpy(stats, &iobuf->filestats, sizeof(fstats));

		/* Flush cache */
		os_sync_after_write(stats, sizeof(stats));
	}

	return ret;
}

s32 FAT_GetUsage(const char *path, u32 *blocks, u32 *inodes)
{
	s32 ret;

	/* Copy path */
	strcpy(iobuf->getUsage.filename, path);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->getUsage.filename;
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->getUsage.size;
	iobuf->vector[1].len  = 4;
	iobuf->vector[2].data = &iobuf->getUsage.inodes;
	iobuf->vector[2].len  = 4;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get usage */
	ret = os_ioctlv(fatFd, IOCTL_FAT_GETUSAGE, 1, 2, iobuf->vector);

	if (ret >= 0) {
		*blocks = iobuf->getUsage.size;
		*inodes = iobuf->getUsage.inodes;
	}

	return ret;
}
