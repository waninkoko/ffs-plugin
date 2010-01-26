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

#ifndef _IPC_H_
#define _IPC_H_

#include "types.h"

/* Error codes */
#define IPC_EINVAL		 -1
#define IPC_ENOENT		 -6
#define IPC_ENOMEM		-22
#define IPC_EIO			  2
#define IPC_EINCMD		128

/* IOS calls */
#define IOS_OPEN		0x01
#define IOS_CLOSE		0x02
#define IOS_READ		0x03
#define IOS_WRITE		0x04
#define IOS_SEEK		0x05
#define IOS_IOCTL		0x06
#define IOS_IOCTLV		0x07

/* IOCTL vector */
typedef struct iovec {
	void *data;
	u32   len;
} ioctlv;

/* IPC message */
typedef struct ipcmessage {
	u32 command;
	u32 result;
	u32 fd;

	union 
	{
		struct
		{
			char *device;
			u32   mode;
			u32   resultfd;
		} open;
	
		struct 
		{
			void *data;
			u32   length;
		} read, write;
		
		struct 
		{
			s32 offset;
			s32 origin;
		} seek;
		
		struct 
		{
			u32 command;

			u32 *buffer_in;
			u32  length_in;
			u32 *buffer_io;
			u32  length_io;
		} ioctl;

		struct 
		{
			u32 command;

			u32 num_in;
			u32 num_io;
			ioctlv *vector;
		} ioctlv;
	};

	void *callback;
	void *usrdata;
	u32   relaunch;
	void *syncqueue;
	u32   magic;
	u8    padding[12];
} __attribute__((packed)) ipcmessage;

#endif
