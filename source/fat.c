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

#include "ffs_calls.h"

#include "fat.h"
#include "ipc.h"
#include "isfs.h"
#include "syscalls.h"
#include "tools.h"
#include "types.h"

/* IOCTL commands */
#define IOCTL_FAT_OPEN		0x01
#define IOCTL_FAT_CLOSE		0x02
#define IOCTL_FAT_READ		0x03
#define IOCTL_FAT_WRITE		0x04
#define IOCTL_FAT_SEEK		0x05
#define IOCTL_FAT_MKDIR		0x06
#define IOCTL_FAT_MKFILE	0x07
#define IOCTL_FAT_READDIR	0x08
#define IOCTL_FAT_DELETE	0x09
#define IOCTL_FAT_DELETEDIR	0x0A
#define IOCTL_FAT_RENAME	0x0B
#define IOCTL_FAT_STAT		0x0C
#define IOCTL_FAT_VFSSTATS	0x0D
#define IOCTL_FAT_FILESTATS	0x0E
#define IOCTL_FAT_MOUNTSD	0xF0
#define IOCTL_FAT_UMOUNTSD	0xF1
#define IOCTL_FAT_MOUNTUSB	0xF2
#define IOCTL_FAT_UMOUNTUSB	0xF3

/* Variables */
static s32 fatFd = 0;

/* FAT buffer */
static struct {
	/* Vector */
	ioctlv vector[4];

	/* Buffers */
	u32 buffer[72];
} *iobuf = NULL;


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

s32 FAT_Open(const char *path, u32 mode)
{
	/* Open file */
	return os_open(path, mode);
}

s32 FAT_Close(s32 fd)
{
	/* Close file */
	return os_close(fd);
}

s32 FAT_Read(s32 fd, void *buffer, u32 len)
{
	/* Read file */
	return os_read(fd, buffer, len);
}

s32 FAT_Write(s32 fd, void *buffer, u32 len)
{
	/* Write file */
	return os_write(fd, buffer, len);
}

s32 FAT_Seek(s32 fd, u32 where, u32 whence)
{
	/* Seek file */
	return os_seek(fd, where, whence);
}

s32 FAT_CreateDir(const char *dirpath)
{
	/* Copy path */
	os_strcpy((char *)iobuf->buffer, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->buffer;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Create directory */
	return os_ioctlv(fatFd, IOCTL_FAT_MKDIR, 1, 0, iobuf->vector);
}

s32 FAT_CreateFile(const char *filepath)
{
	/* Copy path */
	os_strcpy((char *)iobuf->buffer, filepath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->buffer;
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
	os_strcpy((char *)iobuf->buffer, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = &iobuf->buffer[0];
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->buffer[24];
	iobuf->vector[1].len  = 4;

	if (outbuf) {
		/* Input/Output buffers */
		in = io = 2;

		/* Set entries value */
		iobuf->buffer[24] = *entries;

		iobuf->vector[2].data = outbuf;
		iobuf->vector[2].len  = (outbuf) ? (*entries * FAT_MAXPATH) : 0;
		iobuf->vector[3].data = &iobuf->buffer[24];
		iobuf->vector[3].len  = 4;
	}

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Read directory */
	ret = os_ioctlv(fatFd, IOCTL_FAT_READDIR, in, io, iobuf->vector);
	if (ret >= 0)
		*entries = iobuf->buffer[24];

	return ret;
}

s32 FAT_Delete(const char *path)
{
	/* Copy path */
	os_strcpy((char *)iobuf->buffer, path);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->buffer;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Delete */
	return os_ioctlv(fatFd, IOCTL_FAT_DELETE, 1, 0, iobuf->vector);	
}

s32 FAT_DeleteDir(const char *dirpath)
{
	/* Copy path */
	os_strcpy((char *)iobuf->buffer, dirpath);

	/* Setup vector */
	iobuf->vector[0].data = iobuf->buffer;
	iobuf->vector[0].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Delete */
	return os_ioctlv(fatFd, IOCTL_FAT_DELETEDIR, 1, 0, iobuf->vector);	
}

s32 FAT_Rename(const char *oldpath, const char *newpath)
{
	/* Copy paths */
	os_strcpy((char *)&iobuf->buffer[0],  oldpath);
	os_strcpy((char *)&iobuf->buffer[24], newpath);

	/* Setup vector */
	iobuf->vector[0].data = &iobuf->buffer[0];
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->buffer[24];
	iobuf->vector[1].len  = FAT_MAXPATH;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Rename */
	return os_ioctlv(fatFd, IOCTL_FAT_RENAME, 2, 0, iobuf->vector);
}

s32 FAT_Stat(const char *path, struct stat *stats)
{
	s32 ret;

	/* Copy path */
	os_strcpy((char *)iobuf->buffer, path);

	/* Setup vector */
	iobuf->vector[0].data = &iobuf->buffer[0];
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->buffer[24];
	iobuf->vector[1].len  = sizeof(struct stat);

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get stats */
	ret = os_ioctlv(fatFd, IOCTL_FAT_STAT, 1, 1, iobuf->vector);
	if (ret >= 0 && stats)
		os_memcpy(stats, &iobuf->buffer[24], sizeof(struct stat));

	return ret;
}

s32 FAT_GetVfsStats(const char *path, struct statvfs *stats)
{
	s32 ret;

	/* Copy path */
	os_strcpy((char *)iobuf->buffer, path);

	/* Setup vector */
	iobuf->vector[0].data = &iobuf->buffer[0];
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->buffer[24];
	iobuf->vector[1].len  = sizeof(struct statvfs);

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get filestats */
	ret = os_ioctlv(fatFd, IOCTL_FAT_VFSSTATS, 1, 1, iobuf->vector);
	if (ret >= 0)
		os_memcpy(stats, &iobuf->buffer[24], sizeof(struct statvfs));

	return ret;
}

s32 FAT_GetFileStats(s32 fd, fstats *stats)
{
	s32 ret;

	/* Setup vector */
	iobuf->vector[0].data = iobuf->buffer;
	iobuf->vector[0].len  = sizeof(fstats);

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get filestats */
	ret = os_ioctlv(fd, IOCTL_FAT_FILESTATS, 0, 1, iobuf->vector);
	if (ret >= 0)
		os_memcpy(stats, iobuf->buffer, sizeof(fstats));

	return ret;
}

#if 0
s32 FAT_GetUsage(const char *path, u32 *blocks, u32 *inodes)
{
	s32 ret;

	/* Copy path */
	os_strcpy((char *)iobuf->buffer, path);

	/* Setup vector */
	iobuf->vector[0].data = &iobuf->buffer[0];
	iobuf->vector[0].len  = FAT_MAXPATH;
	iobuf->vector[1].data = &iobuf->buffer[24];
	iobuf->vector[1].len  = 8;
	iobuf->vector[2].data = &iobuf->buffer[32];
	iobuf->vector[2].len  = 4;

	os_sync_after_write(iobuf, sizeof(*iobuf));

	/* Get usage */
	ret = os_ioctlv(fatFd, IOCTL_FAT_GETUSAGE, 1, 2, iobuf->vector);
	if (ret >= 0) {
		u64 *size = (u64 *)&iobuf->buffer[24];

		/* Set blocks and inodes */
		*blocks = (u32)(*size / 16384);
		*inodes = iobuf->buffer[32];
	}

	return ret;
}
#endif
