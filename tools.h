#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "types.h"

/* Prototypes */
void DCInvalidateRange(void* ptr, int size);
void DCFlushRange(void* ptr, int size);
void ICInvalidate(void);
u32  Perms_Read(void);
void Perms_Write(u32 flags);
s32  Swi_MLoad(u32 arg0, u32 arg1, u32 arg2, u32 arg3);

#endif

