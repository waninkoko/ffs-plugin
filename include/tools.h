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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "types.h"


/* Prototypes */
void os_memset(void *dst, u8 val, u32 size);
void os_memcpy(void *dst, const void *src, u32 size);
s32  os_memcmp(const void *p1, const void *p2, u32 len);
s32  os_strncmp(const char *s1, const char *s2, u32 len);
void os_strcpy(char *dst, const char *src);
void os_strcat(char *dst, const char *src);
u32  os_strlen(const char *str);

#endif
