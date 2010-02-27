/*
 * FFS plugin for Custom IOS.
 *
 * Copyright (C) 2008-2010 Waninkoko, WiiGator.
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
#include "tools.h"

/* Macros */
#define Write8(addr, val)	\
	*(u8 *)addr = val;	\
	DCFlushRange((void *)addr, sizeof(u8));

#define Write16(addr, val)	\
	*(u16 *)addr = val;	\
	DCFlushRange((void *)addr, sizeof(u16));

#define Write32(addr, val)	\
	*(u32 *)addr = val;	\
	DCFlushRange((void *)addr, sizeof(u32));


/* FFS jump table */
static u32 ffs_jmpTable[8]=
{
	(u32)_ffs_unk,
	(u32)_ffs_open,
	(u32)_ffs_close,
	(u32)_ffs_read,
	(u32)_ffs_write,
	(u32)_ffs_seek,
	(u32)_ffs_ioctl,
	(u32)_ffs_ioctlv,
};

/* Addresses */
u32 addr_callOpen = 0;
u32 addr_jmpTable = 0;
u32 addr_reentry  = 0;


void Patch_FfsModule(u32 version)
{
	switch (version) {
	/** 12/24/08 13:48:17 **/
	case 0x49523DA1:
		/* Set addresses */
		addr_jmpTable = *(u32 *)0x20005F38;
		addr_reentry  = 0x20005F0A;

		/* Patch command handler */
		Write32(0x20005F38, (u32)ffs_jmpTable);

		break;

	/** 12/23/08 17:26:21 **/
	case 0x49511F3D:
		/* Set addresses */
		addr_jmpTable = *(u32 *)0x200021D0;
		addr_reentry  = 0x2000219C;

		/* Patch command handler */
		Write32(0x200021D0, (u32)ffs_jmpTable);

		break;

	/** 11/24/08 15:36:10 **/
	case 0x492AC9EA:
		/* Set addresses */
		addr_jmpTable = *(u32 *)0x200061B8;
		addr_reentry  = 0x2000618A;

		/* Patch command handler */
		Write32(0x200061B8, (u32)ffs_jmpTable);

		break;
	}
}

void Patch_IopModule(u32 version)
{
	switch (version) {
	/** 07/11/08 14:34:29 **/
	case 0x48776F75:
		/* Set addresses */
		addr_callOpen = 0xFFFF2E5C;

		/* Patch syscall open */
		Write32(0xFFFF2E50, 0x477846C0);
		Write32(0xFFFF2E54, 0xE51FF004);
		Write32(0xFFFF2E58, (u32)_syscall_open);

		break;

	/** 12/23/08 17:28:32 **/
	case 0x49511FC0:
		/* Set addresses */
		addr_callOpen = 0xFFFF2D4C;

		/* Patch syscall open */
		Write32(0xFFFF2D40, 0x477846C0);
		Write32(0xFFFF2D44, 0xE51FF004);
		Write32(0xFFFF2D48, (u32)_syscall_open);

		break;

	/** 11/24/08 15:39:12 **/
	case 0x492ACAA0:
		/* Set addresses */
		addr_callOpen = 0xFFFF302C;

		/* Patch syscall open */
		Write32(0xFFFF3020, 0x477846C0);
		Write32(0xFFFF3024, 0xE51FF004);
		Write32(0xFFFF3028, (u32)_syscall_open);

		break;
	}
}
