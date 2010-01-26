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


/*
 * FFS functions
 */
	.section .ffs_printf, "x"

	.code 16
	.thumb_func

	.global FFS_printf
FFS_printf:
	bx lr



/*
 * FFS handlers
 */
	.text
	.align 4

	.thumb_func
	.global FFS_HandleOpen
FFS_HandleOpen:
	push	{r4-r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, sp, #4
	ldr	r3, =ffs_handle_open + 1
	bx	r3

	.thumb_func
	.global FFS_HandleClose
FFS_HandleClose:
	push	{r4-r6, lr}
	movs	r6, #0
	ldr	r4, [r0, #8]
	ldr	r3, [r4, #0x20]
	ldr	r2, =ffs_handle_close + 1
	bx	r2

	.thumb_func
	.global FFS_HandleRead
FFS_HandleRead:
	push	{r4-r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	ldr	r3, =ffs_handle_read + 1
	bx	r3

	.thumb_func
	.global FFS_HandleWrite
FFS_HandleWrite:
	push	{r4-r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	ldr	r3, =ffs_handle_write + 1
	bx	r3

	.thumb_func
	.global FFS_HandleSeek
FFS_HandleSeek:
	push	{r4, lr}
	add	r4, r0, #0
	add	r4, #0xC
	ldr	r1, [r0, #8]
	ldr	r3, =ffs_handle_seek + 1
	bx	r3

	.thumb_func
	.global FFS_HandleIoctl
FFS_HandleIoctl:
	push	{r4-r7, lr}
	sub	sp, sp, #0x70
	movs	r4, #0
	add	r5, r0, #0
	ldr	r3, =ffs_handle_ioctl + 1
	bx	r3

	.thumb_func
	.global FFS_HandleIoctlv
FFS_HandleIoctlv:
	push	{r4-r7, lr}
	sub	sp, sp, #4
	add	r4, r0, #0
	add	r4, #0xC
	ldr	r2, =ffs_handle_ioctlv + 1
	bx	r2
