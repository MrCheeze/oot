#ifndef LETTERBOX_H
#define LETTERBOX_H

#include "ultra64.h"

void shrink_window_setval(s32 target);
u32 shrink_window_getval(void);
void shrink_window_setnowval(s32 size);
u32 shrink_window_getnowval(void);
void shrink_window_init(void);
void shrink_window_cleanup(void);
void shrink_window_move(s32 updateRate);

#endif
