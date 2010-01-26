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

#include "fat.h"
#include "ffs_calls.h"
#include "ioctl.h"
#include "ipc.h"
#include "isfs.h"
#include "plugin.h"
#include "syscalls.h"
#include "tools.h"
#include "types.h"

/* Global config */
struct ffsConfig config = { 0 };


void __FFS_GeneratePath(const char *oldpath, char *newpath)
{
	/* Set path prefix */
	switch (config.mode) {
	case MODE_SD:
		os_strcpy(newpath, "sd:");
		break;
	case MODE_USB:
		os_strcpy(newpath, "usb:");
		break;
	}

	/* Generate path */
	os_strcat(newpath, oldpath);
}


s32 FFS_Open(ipcmessage *message)
{
	char *devpath = message->open.device;
	s32   ret;

	FFS_printf("FFS_Open(): %s\n", devpath);

	/* Open file */
	if (config.mode && os_strncmp(devpath, "/dev/", 5)) {
		char path[FAT_MAXPATH];

		/* Generate path */
		__FFS_GeneratePath(devpath, path);

		/* Open file */
		ret = FAT_Open(path, 3);
	} else
		ret = FFS_HandleOpen(message);

	return ret;
}

s32 FFS_Close(ipcmessage *message)
{
	s32 fd = message->fd;
	s32 ret;

	FFS_printf("FFS_Close(): %d\n", fd);

	/* Close file */
	if (fd >= 0 && fd <= 16)
		ret = FAT_Close(fd);
	else
		ret = FFS_HandleClose(message);

	return ret;
}

s32 FFS_Read(ipcmessage *message)
{
	void *buffer = message->read.data;
	u32   len    = message->read.length;

	s32 fd = message->fd;
	s32 ret;

	FFS_printf("FFS_Read(): %d (buffer: 0x%08x, len: %d\n", fd, (u32)buffer, len);

	/* Read file */
	if (fd >= 0 && fd <= 16)
		ret = FAT_Read(fd, buffer, len);
	else
		ret = FFS_HandleRead(message);

	return ret;
}

s32 FFS_Write(ipcmessage *message)
{
	void *buffer = message->write.data;
	u32   len    = message->write.length;

	s32 fd = message->fd;
	s32 ret;

	FFS_printf("FFS_Write(): %d (buffer: 0x%08x, len: %d)\n", fd, (u32)buffer, len);

	/* Write file */
	if (fd >= 0 && fd <= 16)
		ret = FAT_Write(fd, buffer, len);
	else
		ret = FFS_HandleWrite(message);

	return ret;
}

s32 FFS_Seek(ipcmessage *message)
{
	u32 where  = message->seek.offset;
	u32 whence = message->seek.origin;

	s32 fd = message->fd;
	s32 ret;

	FFS_printf("FFS_Seek(): %d (where: %d, whence: %d)\n", fd, where, whence);

	/* Seek file */
	if (fd >= 0 && fd <= 16)
		ret = FAT_Seek(fd, where, whence);
	else
		ret = FFS_HandleSeek(message);

	return ret;
}

s32 FFS_Ioctl(ipcmessage *message)
{
	u32 *inbuf = message->ioctl.buffer_in;
	u32 *iobuf = message->ioctl.buffer_io;
	u32  iolen = message->ioctl.length_io;

	u32  cmd = message->ioctl.command;
	s32  ret;

	/* Parse comamnd */
	switch (cmd) {
	/** Create directory **/
	case IOCTL_ISFS_CREATEDIR: {
		fsattr *attr = (fsattr *)inbuf;

		FFS_printf("FFS_CreateDir(): %s\n", attr->filepath);

		/* Device mode */
		if (config.mode) {
			char newpath[FAT_MAXPATH];

			/* Genereate path */
			__FFS_GeneratePath(attr->filepath, newpath);

			/* Create directory */
			return FAT_CreateDir(newpath);
		}

		break;
	}

	/** Create file **/
	case IOCTL_ISFS_CREATEFILE: {
		fsattr *attr = (fsattr *)inbuf;

		FFS_printf("FFS_CreateFile(): %s\n", attr->filepath);

		/* Device mode */
		if (config.mode) {
			char newpath[FAT_MAXPATH];

			/* Genereate path */
			__FFS_GeneratePath(attr->filepath, newpath);

			/* Create file */
			return FAT_CreateFile(newpath); 
		}

		break;
	}

	/** Delete object **/
	case IOCTL_ISFS_DELETE: {
		char *filepath = (char *)inbuf;

		FFS_printf("FFS_Delete(): %s\n", filepath);

		/* Device mode */
		if (config.mode) {
			char newpath[FAT_MAXPATH];

			/* Genereate path */
			__FFS_GeneratePath(filepath, newpath);

			/* Delete */
			return FAT_Delete(newpath); 
		}

		break;
	}

	/** Rename object **/
	case IOCTL_ISFS_RENAME: {
		fsrename *rename = (fsrename *)inbuf;

		FFS_printf("FFS_Rename(): %s -> %s\n", rename->filepathOld, rename->filepathNew);

		/* Device mode */
		if (config.mode) {
			char oldpath[FAT_MAXPATH];
			char newpath[FAT_MAXPATH];

			struct stat filestat;

			/* Generate paths */
			__FFS_GeneratePath(rename->filepathOld, oldpath);
			__FFS_GeneratePath(rename->filepathNew, newpath);

			/* Get stats */
			ret = FAT_Stat(newpath, &filestat);
			if (ret >= 0) {
				/* Delete directory contents */
				if (filestat.st_mode & S_IFDIR)
					FAT_DeleteDir(newpath);

				/* Delete */
				FAT_Delete(newpath);
			}

			/* Rename */
			return FAT_Rename(oldpath, newpath); 
		}

		break;
	}

	/** Get device stats **/
	case IOCTL_ISFS_GETSTATS: {
		FFS_printf("FFS_GetStats():\n");

		/* Device mode */
		if (config.mode) {
			fsstats *stats = (fsstats *)iobuf;

			char   fatpath[FAT_MAXPATH];
			struct statvfs vfs;

			/* Generate path */
			__FFS_GeneratePath("/", fatpath);

			/* Get filesystem stats */
			ret = FAT_GetVfsStats(fatpath, &vfs);
			if (ret < 0)
				return ret;

			/* Invalidate cache */
			os_memset(iobuf, 0, iolen);

			/* Fill stats */
			stats->block_size  = vfs.f_bsize;
			stats->free_blocks = vfs.f_bfree;
			stats->used_blocks = vfs.f_blocks - vfs.f_bfree;
			stats->free_inodes = vfs.f_ffree;

			/* Flush cache */
			os_sync_after_write(iobuf, iolen);

			return 0;
		}

		break;
	}

	/** Get file stats **/
	case IOCTL_ISFS_GETFILESTATS: {
		s32 fd = message->fd;

		FFS_printf("FFS_GetFileStats(): %d\n", fd);

		/* Device mode */
		if (config.mode) {
			fstats *stats = (fstats *)iobuf;

			/* Invalid descriptor */
			if (fd < 0 || fd > 16)
				break;

			/* Get file stats */
			ret = FAT_GetFileStats(fd, stats);

			return ret;
		}

		break;
	}

	/** Get attributes **/
	case IOCTL_ISFS_GETATTR: {
		char *path = (char *)inbuf;

		FFS_printf("FFS_GetAttributes(): %s\n", path);

		/* Device mode */
		if (config.mode) {
			fsattr *attr = (fsattr *)iobuf;
			char    newpath[FAT_MAXPATH];

			/* Generate path */
			__FFS_GeneratePath(path, newpath);

			/* Check path */
			ret = FAT_Stat(newpath, NULL);
			if (ret < 0)
				return ret;

			/* Fake attributes */
			attr->owner_id   = 0;
			attr->group_id   = 0;
			attr->ownerperm  = ISFS_OPEN_RW;
			attr->groupperm  = ISFS_OPEN_RW;
			attr->otherperm  = ISFS_OPEN_RW;
			attr->attributes = 0;

			/* Flush cache */
			os_sync_after_write(iobuf, iolen);

			return 0;
		}

		break;
	}

	/** Set attributes **/
	case IOCTL_ISFS_SETATTR: {
		fsattr *attr = (fsattr *)inbuf;

		FFS_printf("FFS_SetAttributes(): %s\n", attr->filepath);

		/* Device mode */
		if (config.mode) {
			char newpath[FAT_MAXPATH];

			/* Generate path */
			__FFS_GeneratePath(attr->filepath, newpath);

			/* Check path */
			return FAT_Stat(newpath, NULL);
		}

		break;
	}

	/** Format **/
	case IOCTL_ISFS_FORMAT: {
		FFS_printf("FFS_Format():\n");

		/* Device mode */
		if (config.mode)
			return 0;

		break;
	}

	/** Set FFS mode **/
	case IOCTL_ISFS_SETMODE: {
		u32 val = inbuf[0];

		/* FAT mode enabled */
		if (val) {
			char fatpath[FAT_MAXPATH];

			/* Generate path */
			__FFS_GeneratePath("/tmp", fatpath);

			/* Initialize FAT */
			ret = FAT_Init();
			if (ret < 0)
				return ret;

			/* Delete "/tmp" */
			FAT_DeleteDir(fatpath);
		}

		/* Set FFS mode */
		config.mode = val;

		return 0;
	}

	default:
		break;
	}

	/* FFS IOCTL handler */
	return FFS_HandleIoctl(message);
}

s32 FFS_Ioctlv(ipcmessage *message)
{
	ioctlv *vector = message->ioctlv.vector;
	u32     iolen  = message->ioctlv.num_io;

	u32 cmd = message->ioctlv.command;
	s32 ret;

	/* Parse comamnd */
	switch (cmd) {
	/** Read directory **/
	case IOCTL_ISFS_READDIR: {
		char *dirpath = (char *)vector[0].data;

		FFS_printf("FFS_Readir(): %s\n", dirpath);

		/* Device mode */
		if (config.mode) {
			char *outbuf = NULL;
			u32  *outlen = NULL;
			u32   buflen = 0;

			char newpath[FAT_MAXPATH];
			u32  entries;

			/* Set pointers/values */
			if (iolen > 1) {
				entries = *(u32 *)vector[1].data;
				outbuf  = (char *)vector[2].data;
				outlen  =  (u32 *)vector[3].data;
				buflen  =         vector[2].len;
			} else
				outlen  =  (u32 *)vector[1].data;

			/* Generate path */
			__FFS_GeneratePath(dirpath, newpath);

			/* Read directory */
			ret = FAT_ReadDir(newpath, outbuf, &entries);
			if (ret >= 0)
				*outlen = entries;

			return ret;
		}

		break;
	}

	/** Get device usage **/
	case IOCTL_ISFS_GETUSAGE: {
		char *dirpath = (char *)vector[0].data;

		FFS_printf("FFS_GetUsage(): %s\n", dirpath);

		/* Device mode */
		if (config.mode) {
			u32 *blocks = (u32 *)vector[1].data;
			u32 *inodes = (u32 *)vector[2].data;

			/* Set fake values */
			*blocks = 1;
			*inodes = 1;

			/* Flush cache */
			os_sync_after_write(blocks, sizeof(u32));
			os_sync_after_write(inodes, sizeof(u32));

			return 0;
		}

		break;
	}

	default:
		break;
	}

	/* FFS IOCTLV handler */
	return FFS_HandleIoctlv(message);
}


s32 FFS_EmulateCmd(ipcmessage *message)
{
	s32 ret = 0;

	/* Parse IPC command */
	switch (message->command) {
	case IOS_OPEN: {
		/* Filesystem open */
		ret = FFS_Open(message);
		break;
	}

	case IOS_CLOSE: {
		/* Filesystem close */
		ret = FFS_Close(message);
		break;
	}

	case IOS_READ: {
		/* Filesystem read */
		ret = FFS_Read(message);
		break;
	}

	case IOS_WRITE: {
		/* Filesystem write */
		ret = FFS_Write(message);
		break;
	}

	case IOS_SEEK: {
		/* Filesystem seek */
		ret = FFS_Seek(message);
		break;
	}

	case IOS_IOCTL: {
		/* Filesystem IOCTL */
		ret = FFS_Ioctl(message);
		break;
	}

	case IOS_IOCTLV: {
		/* Filesystem IOCTLV */
		ret = FFS_Ioctlv(message);
		break;
	}

	default:
		ret = IPC_EINVAL;
	}

	FFS_printf("FFS Return: %d\n", ret);

	return ret;
}
