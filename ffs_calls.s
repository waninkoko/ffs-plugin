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
 * FFS handlers
 */
	.text

	.align 4
	.code 16
	.thumb
	.global _ffs_unk
_ffs_unk:
	ldr     r0, =addr_jmpTable
	ldr	r0, [r0]
	nop
	ldr	r0, [r0, #0]
	nop
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_open
_ffs_open:
	add	r0, r4, #0
	push	{r1-r7}
	bl	FFS_Open
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_close
_ffs_close:
	add	r0, r4, #0
	push	{r1-r7}
	bl	FFS_Close
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #8]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_read
_ffs_read:
	add	r0, r4, #0
	push	{r1-r7}
	bl	FFS_Read
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #12]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_write
_ffs_write:
	add	r0, r4, #0
	push	{r1-r7}
	bl	FFS_Write	
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #16]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_seek
_ffs_seek:
	add	r0, r4, #0
	push	{r1-r7}
	bl	FFS_Seek
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #20]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_ioctl
_ffs_ioctl:
	add	r0, r4, #0
	push	{r1-r7}
	push	{r0}
	mov	r1, sp
	bl	FFS_Ioctl
	pop	{r3}
	cmp	r3, #0
	bne	_ffs_exit	
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #24]
	pop	{r1-r7}
	mov	pc, r0

	.align 4
	.code 16
	.thumb
	.global _ffs_ioctlv
_ffs_ioctlv:
	add	r0, r4, #0
	push	{r1-r7}
	push	{r0}
	mov	r1, sp
	bl	FFS_Ioctlv
	pop	{r3}
	cmp	r3, #0
	bne	_ffs_exit	
	cmp	r0, #0
	bge	_ffs_exit
	ldr	r0, =addr_jmpTable
	ldr	r3, [r0]
	ldr	r0, [r3, #28]
	pop	{r1-r7}
	mov	pc, r0

_ffs_exit:
	pop	{r1-r7}
	add	r1, r0, #0
	ldr	r0, =addr_reentry
	ldr	r0, [r0]
	mov	pc, r0


/*
 * Syscall open hook
 */
	.align 4
	.code 32
	.arm

	.global _syscall_open
_syscall_open:
	stmfd	sp!, {r4-r7, lr}
	stmfd	sp!, {r2-r3}
	nop
	ldr	r4, =(_syscall_open_thumb + 1)
	bx	r4

	.align 4
	.code 16
	.thumb
_syscall_open_thumb:
	bl	__FFS_SyscallOpen
	ldr	r2, =openMode
	ldr	r1, [r2]
	pop	{r2-r3}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	mov	r4, r8
	push	{r4-r7}
	ldr	r4, =addr_callOpen
	ldr	r4, [r4]
	nop
	mov	pc, r4
