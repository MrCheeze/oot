#include "global.h"
#include "terminal.h"

#include "z64frame_advance.h"

EffectSsInfo EffectSS2Info = { 0 }; // "EffectSS2Info"

void EffectSoftSprite_ct(PlayState* play, s32 tableSize) {
    u32 i;
    EffectSs* effectSs;
    EffectSsOverlay* overlay;

#if DEBUG_FEATURES
    for (i = 0; i < ARRAY_COUNT(effect_ss2_dlftbls); i++) {
        overlay = &effect_ss2_dlftbls[i];
        PRINTF("effect index %3d:size=%6dbyte romsize=%6dbyte\n", i,
               (uintptr_t)overlay->vramEnd - (uintptr_t)overlay->vramStart,
               overlay->file.vromEnd - overlay->file.vromStart);
    }
#endif

    EffectSS2Info.table =
        GAME_STATE_ALLOC(&play->state, tableSize * sizeof(EffectSs), "../z_effect_soft_sprite.c", 289);
    ASSERT(EffectSS2Info.table != NULL, "EffectSS2Info.data_table != NULL", "../z_effect_soft_sprite.c", 290);

    EffectSS2Info.searchStartIndex = 0;
    EffectSS2Info.tableSize = tableSize;

    for (effectSs = &EffectSS2Info.table[0]; effectSs < &EffectSS2Info.table[EffectSS2Info.tableSize]; effectSs++) {
        EffectSoftSprite2_ElementClear(effectSs);
    }

    overlay = &effect_ss2_dlftbls[0];
    for (i = 0; i < ARRAY_COUNT(effect_ss2_dlftbls); i++) {
        overlay->loadedRamAddr = NULL;
        overlay++;
    }
}

void EffectSoftSprite_dt(PlayState* play) {
    u32 i;
    EffectSs* effectSs;
    EffectSsOverlay* overlay;
    void* addr;

    EffectSS2Info.table = NULL;
    EffectSS2Info.searchStartIndex = 0;
    EffectSS2Info.tableSize = 0;

    // This code doesn't actually work, since table was just set to NULL and tableSize to 0
    for (effectSs = &EffectSS2Info.table[0]; effectSs < &EffectSS2Info.table[EffectSS2Info.tableSize]; effectSs++) {
        EffectSoftSprite2_ElementDestructClear(effectSs);
    }

    overlay = &effect_ss2_dlftbls[0];
    for (i = 0; i < ARRAY_COUNT(effect_ss2_dlftbls); i++) {
        addr = overlay->loadedRamAddr;

        if (addr != NULL) {
            ZELDA_ARENA_FREE(addr, "../z_effect_soft_sprite.c", 337);
        }

        overlay->loadedRamAddr = NULL;
        overlay++;
    }
}

void EffectSoftSprite2_ElementDestructClear(EffectSs* effectSs) {
    if (effectSs->flags & 2) {
        Nai_StopAllObjFx(&effectSs->pos);
    }

    if (effectSs->flags & 4) {
        Nai_StopAllObjFx(&effectSs->vec);
    }

    EffectSoftSprite2_ElementClear(effectSs);
}

void EffectSoftSprite2_ElementClear(EffectSs* effectSs) {
    u32 i;

    effectSs->type = EFFECT_SS_TYPE_MAX;
    effectSs->accel.x = effectSs->accel.y = effectSs->accel.z = 0;
    effectSs->velocity.x = effectSs->velocity.y = effectSs->velocity.z = 0;
    effectSs->vec.x = effectSs->vec.y = effectSs->vec.z = 0;
    effectSs->pos.x = effectSs->pos.y = effectSs->pos.z = 0;
    effectSs->life = -1;
    effectSs->flags = 0;
    effectSs->priority = 128;
    effectSs->draw = NULL;
    effectSs->update = NULL;
    effectSs->gfx = NULL;
    effectSs->actor = NULL;

    for (i = 0; i < ARRAY_COUNT(effectSs->regs); i++) {
        effectSs->regs[i] = 0;
    }
}

s32 EffectSoftSprite2_SearchEmptyIndex(s32 priority, s32* pIndex) {
    s32 foundFree;
    s32 i;

    if (EffectSS2Info.searchStartIndex >= EffectSS2Info.tableSize) {
        EffectSS2Info.searchStartIndex = 0;
    }

    // Search for a free slot
    i = EffectSS2Info.searchStartIndex;
    foundFree = false;
    while (true) {
        if (EffectSS2Info.table[i].life == -1) {
            foundFree = true;
            break;
        }

        i++;

        if (i >= EffectSS2Info.tableSize) {
            i = 0; // Loop around the whole table
        }

        // After a full loop, break out
        if (i == EffectSS2Info.searchStartIndex) {
            break;
        }
    }

    if (foundFree == true) {
        *pIndex = i;
        return 0;
    }

    // If all slots are in use, search for a slot with a lower priority
    // Note that a lower priority is representend by a higher value
    i = EffectSS2Info.searchStartIndex;
    while (true) {
        // Equal priority should only be considered "lower" if flag 0 is set
        if ((priority <= EffectSS2Info.table[i].priority) &&
            !((priority == EffectSS2Info.table[i].priority) && (EffectSS2Info.table[i].flags & 1))) {
            break;
        }

        i++;

        if (i >= EffectSS2Info.tableSize) {
            i = 0; // Loop around the whole table
        }

        // After a full loop, return 1 to indicate that we failed to find a suitable slot
        if (i == EffectSS2Info.searchStartIndex) {
            return 1;
        }
    }

    *pIndex = i;
    return 0;
}

void EffectSoftSprite2_add(PlayState* play, EffectSs* effectSs) {
    s32 index;

    if (_Game_play_isPause(play) != true) {
        if (EffectSoftSprite2_SearchEmptyIndex(effectSs->priority, &index) == 0) {
            EffectSS2Info.searchStartIndex = index + 1;
            EffectSS2Info.table[index] = *effectSs;
        }
    }
}

// original name: "EffectSoftSprite2_makeEffect"
void EffectSoftSprite2_makeEffect(PlayState* play, s32 type, s32 priority, void* initParams) {
    s32 index;
    u32 overlaySize;
    EffectSsOverlay* overlayEntry;
    EffectSsProfile* profile;

    overlayEntry = &effect_ss2_dlftbls[type];

    ASSERT(type < EFFECT_SS_TYPE_MAX, "type < EFFECT_SS2_TYPE_LAST_LABEL", "../z_effect_soft_sprite.c", 556);

    if (EffectSoftSprite2_SearchEmptyIndex(priority, &index) != 0) {
        // Abort because we couldn't find a suitable slot to add this effect in
        return;
    }

    EffectSS2Info.searchStartIndex = index + 1;
    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    if (overlayEntry->vramStart == NULL) {
        PRINTF(T("EffectSoftSprite2_makeEffect():オーバーレイではありません。\n",
                 "EffectSoftSprite2_makeEffect(): Not an overlay.\n"));
        profile = overlayEntry->profile;
    } else {
        if (overlayEntry->loadedRamAddr == NULL) {
            overlayEntry->loadedRamAddr = ZELDA_ARENA_MALLOC_R(overlaySize, "../z_effect_soft_sprite.c", 585);

            if (overlayEntry->loadedRamAddr == NULL) {
                PRINTF_COLOR_RED();
                PRINTF(T("EffectSoftSprite2_makeEffect():zelda_malloc_r()により,%dbyteのメモリ確保ができま\n"
                         "せん。そのため、プログラムのロードも\n"
                         "出来ません。ただいま危険な状態です！\n"
                         "もちろん,エフェクトも出ません。\n",
                         "EffectSoftSprite2_makeEffect():zelda_malloc_r() The memory of %d byte cannot be\n"
                         "secured. Therefore, the program\n"
                         "cannot be loaded. What a dangerous situation!\n"
                         "Naturally, effects will not be produced either.\n"),
                       overlaySize);
                PRINTF_RST();
                return;
            }

            LoadFragmentFix2(overlayEntry->file.vromStart, overlayEntry->file.vromEnd, overlayEntry->vramStart,
                         overlayEntry->vramEnd, overlayEntry->loadedRamAddr);

            PRINTF_COLOR_GREEN();
            PRINTF("EFFECT SS OVL:SegRom %08x %08x, Seg %08x %08x, RamStart %08x, type: %d\n",
                   overlayEntry->file.vromStart, overlayEntry->file.vromEnd, overlayEntry->vramStart,
                   overlayEntry->vramEnd, overlayEntry->loadedRamAddr, type);
            PRINTF_RST();
        }

        profile = (void*)(uintptr_t)((overlayEntry->profile != NULL)
                                         ? (void*)((uintptr_t)overlayEntry->profile -
                                                   (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                              (uintptr_t)overlayEntry->loadedRamAddr))
                                         : NULL);
    }

    if (profile->init == NULL) {
        PRINTF(T("EffectSoftSprite2_makeEffect():すでにエフェクトはロード済みで\n"
                 "すが,コンストラクターがNULLなので追加をやめます。\n"
                 "直してください。（メモリーの無駄) %08x %d\n",
                 "EffectSoftSprite2_makeEffect(): Effects have already been loaded,\n"
                 "but the constructor is NULL so the addition will not occur.\n"
                 "Please fix this. (Waste of memory) %08x %d\n"),
               profile, type);
        return;
    }

    // Delete the previous effect in the slot, in case the slot wasn't free
    EffectSoftSprite2_ElementDestructClear(&EffectSS2Info.table[index]);

    EffectSS2Info.table[index].type = type;
    EffectSS2Info.table[index].priority = priority;

    if (profile->init(play, index, &EffectSS2Info.table[index], initParams) == 0) {
        PRINTF_COLOR_GREEN();
        PRINTF(T("EffectSoftSprite2_makeEffect():"
                 "何らかの理由でコンストラクト失敗。コンストラクターがエラーを返しました。"
                 "エフェクトの追加を中止します。\n",
                 "EffectSoftSprite2_makeEffect(): "
                 "Construction failed for some reason. The constructor returned an error. "
                 "Ceasing effect addition.\n"));
        PRINTF_RST();
        EffectSoftSprite2_ElementClear(&EffectSS2Info.table[index]);
    }
}

void EffectSoftSprite2_proc_calc(PlayState* play, s32 index) {
    EffectSs* effectSs = &EffectSS2Info.table[index];

    if (effectSs->update != NULL) {
        effectSs->velocity.x += effectSs->accel.x;
        effectSs->velocity.y += effectSs->accel.y;
        effectSs->velocity.z += effectSs->accel.z;

        effectSs->pos.x += effectSs->velocity.x;
        effectSs->pos.y += effectSs->velocity.y;
        effectSs->pos.z += effectSs->velocity.z;

        effectSs->update(play, index, effectSs);
    }
}

void EffectSoftSprite_proc(PlayState* play) {
    s32 i;

    for (i = 0; i < EffectSS2Info.tableSize; i++) {
        if (EffectSS2Info.table[i].life > -1) {
            EffectSS2Info.table[i].life--;

            if (EffectSS2Info.table[i].life < 0) {
                EffectSoftSprite2_ElementDestructClear(&EffectSS2Info.table[i]);
            }
        }

        if (EffectSS2Info.table[i].life > -1) {
            EffectSoftSprite2_proc_calc(play, i);
        }
    }
}

void EffectSoftSprite2_ElementDisp(PlayState* play, s32 index) {
    EffectSs* effectSs = &EffectSS2Info.table[index];

    if (effectSs->draw != NULL) {
        effectSs->draw(play, index, effectSs);
    }
}

// original name: "EffectSoftSprite2_disp"
void EffectSoftSprite_disp(PlayState* play) {
    Lights* lights = Global_light_read(&play->lightCtx, play->state.gfxCtx);
    s32 i;

    LightsN_list_check(lights, play->lightCtx.listHead, NULL);
    LightsN_disp(lights, play->state.gfxCtx);

    for (i = 0; i < EffectSS2Info.tableSize; i++) {
        if (EffectSS2Info.table[i].life > -1) {
            if ((EffectSS2Info.table[i].pos.x > 32000.0f) || (EffectSS2Info.table[i].pos.x < -32000.0f) ||
                (EffectSS2Info.table[i].pos.y > 32000.0f) || (EffectSS2Info.table[i].pos.y < -32000.0f) ||
                (EffectSS2Info.table[i].pos.z > 32000.0f) || (EffectSS2Info.table[i].pos.z < -32000.0f)) {
                PRINTF_COLOR_RED();
                PRINTF(T("EffectSoftSprite2_disp():位置が領域外のため "
                         "削除します。エフェクトラベルNo.%d:プログラムの方で対応をお願いします。ここです ==> "
                         "pos(%f, %f, %f)で、ラベルはz_effect_soft_sprite_dlftbls.declにあります。\n",
                         "EffectSoftSprite2_disp(): Since the position is outside the area, "
                         "delete it. Effect label No. %d: Please respond by the program. Here is ==> "
                         "pos(%f, %f, %f) and the label is in z_effect_soft_sprite_dlftbls.decl.\n"),
                       EffectSS2Info.table[i].type, EffectSS2Info.table[i].pos.x, EffectSS2Info.table[i].pos.y,
                       EffectSS2Info.table[i].pos.z);
                PRINTF_COLOR_GREEN();
                PRINTF(T("もし、posを別のことに使っている場合相談に応じます。\n",
                         "If you are using pos for something else, consult me.\n"));
                PRINTF_RST();

                EffectSoftSprite2_ElementDestructClear(&EffectSS2Info.table[i]);
            } else {
                EffectSoftSprite2_ElementDisp(play, i);
            }
        }
    }
}

/**
 * Lerp from `a` (weightInv == inf) to `b` (weightInv == 1 or 0).
 */
s16 Effect_SS_Uty_short_interpolation(s16 a, s16 b, s32 weightInv) {
    s16 ret = (weightInv == 0) ? b : (a + (s32)((b - a) / (f32)weightInv));

    return ret;
}

/**
 * Lerp from `a` (weight == 0) to `b` (weight == 1).
 */
s16 Effect_SS_Uty_short_interpolation_t01(s16 a, s16 b, f32 weight) {
    return (b - a) * weight + a;
}

/**
 * Lerp from `a` (weight == 0) to `b` (weight == 1).
 */
u8 Effect_SS_Uty_uc_interpolation_t01(u8 a, u8 b, f32 weight) {
    return weight * ((f32)b - (f32)a) + a;
}
