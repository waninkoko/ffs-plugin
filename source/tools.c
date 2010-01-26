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

#include "syscalls.h"
#include "types.h"


void os_memset(void *dst, u8 val, u32 size)
{
	u8 *ptr = (u8 *)dst;
	u32 cnt;

	/* Set bytes */
	for (cnt = 0; cnt < size; cnt++)
		ptr[cnt] = val;

	/* Flush cache */
	os_sync_after_write(dst, size);
}

void os_memcpy(void *dst, const void *src, u32 size)
{
	u8 *s = (u8 *)src;
	u8 *d = (u8 *)dst;

	u32 cnt;

	/* Copy bytes */
	for (cnt = 0; cnt < size; cnt++)
		d[cnt] = s[cnt];

	/* Flush cache */
	os_sync_after_write(dst, size);
}

s32 os_memcmp(const void *p1, const void *p2, u32 len)
{
	const char *c1 = (const char *)p1;
	const char *c2 = (const char *)p2;

	u32 cnt;

	/* Compare bytes */
	for (cnt = 0; cnt < len; cnt++) {
		if (c1[cnt] < c2[cnt])
			return -1;
		if (c1[cnt] > c2[cnt])
			return  1;
	}

	return 0;
}

s32 os_strncmp(const char *s1, const char *s2, u32 len)
{
	u32 cnt;

	/* Compare bytes */
	for (cnt = 0; cnt < len; cnt++) {
		if (s1[cnt] < s2[cnt])
			return -1;
		if (s1[cnt] > s2[cnt])
			return  1;

		/* End of string */
		if (!s1[cnt] || !s2[cnt])
			break;
	}

	return 0;
}

void os_strcpy(char *dst, const char *src)
{
	u32 cnt;

	/* Copy bytes */
	for (cnt = 0; src[cnt]; cnt++)
		dst[cnt] = src[cnt];

	/* End of string */
	dst[cnt] = 0;

	/* Flush cache */
	os_sync_after_write(dst, cnt);
}

void os_strcat(char *dst, const char *src)
{
	u32 cnt;

	/* Find end of string character */
	for (cnt = 0; dst[cnt]; cnt++);

	/* Copy string */
	os_strcpy(dst + cnt, src);
}

u32 os_strlen(const char *str)
{
	u32 cnt;

	/* Count characters */
	for (cnt = 0; str[cnt]; cnt++);

	return cnt;
}
