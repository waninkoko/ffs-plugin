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
#include "ffs_calls.h"
#include "ioctl.h"
#include "ipc.h"
#include "isfs.h"
#include "plugin.h"
#include "syscalls.h"
#include "types.h"

/* Global config */
struct ffsConfig config = { 0 };

/* Variables */
static char openPath[FAT_MAXPATH] ATTRIBUTE_ALIGN(32);
s32         openMode;


u16 __FFS_GetUID(void)
{
	/* Return user ID */
	return 0x1001;
}

u16 __FFS_GetGID(void)
{
	/* Return title ID */
	return *(u16 *)0x00000004;
}

void __FFS_AppendPath(char *dst, const char *src)
{
	u32 cnt;

	/* Move to end */
	dst += strlen(dst);

	/* Copy path */
	for (cnt = 0; src[cnt]; cnt++) {
		char c = src[cnt];

		/* Check character */
		switch (c) {
		case '"':
		case '*':
		case ':':
		case '<':
		case '>':
		case '?':
		case '|':
			/* Replace character */
			c = '_';
			break;
		}

		/* Copy character */
		dst[cnt] = c;
	}

	/* End of string */
	dst[cnt] = 0;
}

void __FFS_GeneratePath(const char *oldpath, char *newpath)
{
	/* Set prefix */
	if (config.mode & MODE_SD)
		strcpy(newpath, "sd:");
	if (config.mode & MODE_USB)
		strcpy(newpath, "usb:");

	/* Generate path */
	__FFS_AppendPath(newpath, config.path);
	__FFS_AppendPath(newpath, oldpath);
}

u32 __FFS_CheckPath(const char *path)
{
	/* Ignore 'launch.sys' */
	if (!strncmp(path, "/tmp/launch.sys", 15)) return 1;

	/* Check path */
	if (config.mode & MODE_FULL) {
		if (!strncmp(path, "/dev", 4))              return 1;
		if (!strncmp(path, "/", 1))                 return 0;
	} else {
		if (!strncmp(path, "/ticket/00010001", 16)) return 0;
		if (!strncmp(path, "/ticket/00010005", 16)) return 0;
		if (!strncmp(path, "/title/00010000",  15)) return 0;
		if (!strncmp(path, "/title/00010001",  15)) return 0;
		if (!strncmp(path, "/title/00010004",  15)) return 0;
		if (!strncmp(path, "/title/00010005",  15)) return 0;
		if (!strncmp(path, "/tmp", 4))              return 0;
		if (!strncmp(path, "/sys/disc.sys", 13))    return 0;
		if (!strncmp(path, "/sys/uid.sys",  12))    return 0;
	}

	return 1;
}

char *__FFS_SyscallOpen(char *path, s32 mode)
{
	/* Set mode */
	openMode = mode;

	/* Emulation mode */
	if (config.mode & 0xFF) {
		u32 ret;

		/* SD mode */
		if (config.mode & MODE_SD) {
			/* Disable '/dev/sdio' */
			if (!strncmp(path, "/dev/sdio", 9)) {
				strcpy(openPath, "/dev/null");

				/* Return path */
				return openPath;
			}
		}

		/* Check path */
		ret = __FFS_CheckPath(path);
		if (!ret) {
			/* Generate path */
			__FFS_GeneratePath(path, openPath);

			/* Return path */
			return openPath;
		}
	}

	/* Return path */
	return path;
}


s32 FFS_Open(ipcmessage *message)
{
	FFS_printf("FFS_Open(): %s\n", devpath);
	
	return -6;
}

s32 FFS_Close(ipcmessage *message)
{
	FFS_printf("FFS_Close(): %d\n", fd);

	return -6;
}

s32 FFS_Read(ipcmessage *message)
{
	FFS_printf("FFS_Read(): %d (buffer: 0x%08x, len: %d\n", fd, (u32)buffer, len);

	return -6;
}

s32 FFS_Write(ipcmessage *message)
{
	FFS_printf("FFS_Write(): %d (buffer: 0x%08x, len: %d)\n", fd, (u32)buffer, len);

	return -6;
}

s32 FFS_Seek(ipcmessage *message)
{
	FFS_printf("FFS_Seek(): %d (where: %d, whence: %d)\n", fd, where, whence);
	
	return -6;
}

s32 FFS_Ioctl(ipcmessage *message, u32 *flag)
{
	u32 *inbuf = message->ioctl.buffer_in;
	u32 *iobuf = message->ioctl.buffer_io;
	u32  iolen = message->ioctl.length_io;
	u32  cmd   = message->ioctl.command;

	s32 ret;

	/* Set flag */
	*flag = config.mode;

	/* Parse comamnd */
	switch (cmd) {
	/** Create directory **/
	case IOCTL_ISFS_CREATEDIR: {
		fsattr *attr = (fsattr *)inbuf;

		FFS_printf("FFS_CreateDir(): %s\n", attr->filepath);

		/* Check path */
		ret = __FFS_CheckPath(attr->filepath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			char newpath[FAT_MAXPATH];

			/* Generate path */
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

		/* Check path */
		ret = __FFS_CheckPath(attr->filepath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			char newpath[FAT_MAXPATH];

			/* Generate path */
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

		/* Check path */
		ret = __FFS_CheckPath(filepath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			char newpath[FAT_MAXPATH];

			/* Generate path */
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

		/* Check path */
		ret = __FFS_CheckPath(rename->filepathOld);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			char oldpath[FAT_MAXPATH];
			char newpath[FAT_MAXPATH];

			struct stat filestat;

			/* Generate paths */
			__FFS_GeneratePath(rename->filepathOld, oldpath);
			__FFS_GeneratePath(rename->filepathNew, newpath);

			/* Compare paths */
			if (!strcmp(oldpath, newpath))
				return FAT_Stat(oldpath, NULL);

			/* Check new path */
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
		if (config.mode & 0xFF) {
			fsstats *stats = (fsstats *)iobuf;

			char   fatpath[FAT_MAXPATH];
			struct statvfs vfs;

			/* Generate path */
			__FFS_GeneratePath("/", fatpath);

			/* Get filesystem stats */
			ret = FAT_GetVfsStats(fatpath, &vfs);
			if (ret < 0)
				return ret;

			/* Clear buffer */
			memset(iobuf, 0, iolen);

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
		FFS_printf("FFS_GetFileStats(): %d\n", fd);

		/* Disable flag */
		*flag = 0;

		break;
	}

	/** Get attributes **/
	case IOCTL_ISFS_GETATTR: {
		char *path = (char *)inbuf;

		FFS_printf("FFS_GetAttributes(): %s\n", path);

		/* Check path */
		ret = __FFS_CheckPath(path);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			fsattr *attr = (fsattr *)iobuf;
			char    newpath[FAT_MAXPATH];

			/* Generate path */
			__FFS_GeneratePath(path, newpath);

			/* Check path */
			ret = FAT_Stat(newpath, NULL);
			if (ret < 0)
				return ret;

			/* Fake attributes */
			attr->owner_id   = __FFS_GetUID();
			attr->group_id   = __FFS_GetGID();
			attr->ownerperm  = ISFS_OPEN_RW;
			attr->groupperm  = ISFS_OPEN_RW;
			attr->otherperm  = ISFS_OPEN_RW;
			attr->attributes = 0;

			/* Copy filepath */
			memcpy(attr->filepath, path, ISFS_MAXPATH);

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

		/* Check path */
		ret = __FFS_CheckPath(attr->filepath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
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
		if (config.mode & 0xFF)
			return 0;

		break;
	}

	/** Set FFS mode **/
	case IOCTL_ISFS_SETMODE: {
		u32 val = inbuf[0];
	
		/* Set flag */
		*flag = 1;

		/* FAT mode enabled */
		if (val) {
			char fatpath[FAT_MAXPATH];

			/* Set fs path */
			strcpy(config.path, "");

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

	/* Call handler */
	return -6;
}

s32 FFS_Ioctlv(ipcmessage *message, u32 *flag)
{
	ioctlv *vector = message->ioctlv.vector;
	u32     inlen  = message->ioctlv.num_in;
	u32     iolen  = message->ioctlv.num_io;
	u32     cmd    = message->ioctlv.command;

	s32 ret;

	/* Set flag */
	*flag = config.mode;

	/* Parse comamnd */
	switch (cmd) {
	/** Read directory **/
	case IOCTL_ISFS_READDIR: {
		char *dirpath = (char *)vector[0].data;

		FFS_printf("FFS_Readir(): %s\n", dirpath);

		/* Check path */
		ret = __FFS_CheckPath(dirpath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
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
			if (ret >= 0) {
				*outlen = entries;
				os_sync_after_write(outlen, sizeof(u32));
			}

			/* Flush cache */
			if (outbuf)
				os_sync_after_write(outbuf, buflen);

			return ret;
		}

		break;
	}

	/** Get device usage **/
	case IOCTL_ISFS_GETUSAGE: {
		char *dirpath = (char *)vector[0].data;

		FFS_printf("FFS_GetUsage(): %s\n", dirpath);

		/* Check path */
		ret = __FFS_CheckPath(dirpath);
		if (ret) {
			*flag = 0;
			break;
		}

		/* Device mode */
		if (config.mode & 0xFF) {
			char newpath[FAT_MAXPATH];

			u32 *blocks = (u32 *)vector[1].data;
			u32 *inodes = (u32 *)vector[2].data;

			/* Generate path */
			__FFS_GeneratePath(dirpath, newpath);

			/* Get usage */
			ret = FAT_GetUsage(newpath, blocks, inodes);

			/* Flush cache */
			os_sync_after_write(blocks, sizeof(u32));
			os_sync_after_write(inodes, sizeof(u32));

			return ret;
		}

		break;
	}

	/** Set FFS mode **/
	case IOCTL_ISFS_SETMODE: {
		u32   val  = *(u32 *)vector[0].data;
		char *path = "";
	
		/* Set flag */
		*flag = 1;

		/* Get path */
		if (inlen > 1)
			path = (char *)vector[1].data;

		/* FAT mode enabled */
		if (val) {
			char fatpath[FAT_MAXPATH];

			/* Set fs path */
			strcpy(config.path, path);

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

	/* Call handler */
	return -6;
}
