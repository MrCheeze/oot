/*
 * File: z_en_wonder_item.c
 * Overlay: ovl_En_Wonder_Item
 * Description: Invisible Collectable; Used in MQ to create "Cow" switches
 */

#include "z_en_wonder_item.h"
#include "terminal.h"

#define FLAGS 0

void En_Wonder_Item_actor_ct(Actor* thisx, PlayState* play);
void En_Wonder_Item_actor_dt(Actor* thisx, PlayState* play);
void En_Wonder_Item_actor_move(Actor* thisx, PlayState* play);

void mode_jyozan(EnWonderItem* this, PlayState* play);
void mode_get_item(EnWonderItem* this, PlayState* play);
void mode_attack_item(EnWonderItem* this, PlayState* play);
void mode_pass(EnWonderItem* this, PlayState* play);
void mode_order_check(EnWonderItem* this, PlayState* play);
void mode_ukkari(EnWonderItem* this, PlayState* play);
void mode_rolling_get_item(EnWonderItem* this, PlayState* play);

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 30, 0, { 0, 0, 0 } },
};

ActorProfile En_Wonder_Item_Profile = {
    /**/ ACTOR_EN_WONDER_ITEM,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnWonderItem),
    /**/ En_Wonder_Item_actor_ct,
    /**/ En_Wonder_Item_actor_dt,
    /**/ En_Wonder_Item_actor_move,
    /**/ NULL,
};

#pragma increment_block_number "gc-eu:0 gc-eu-mq:0 gc-jp:0 gc-jp-ce:0 gc-jp-mq:0 gc-us:0 gc-us-mq:0 hiratsu3:0"

static Vec3f point_pos[9];
static Vec3f order_point_pos[9];

void En_Wonder_Item_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWonderItem* this = (EnWonderItem*)thisx;

    if ((this->collider.dim.radius != 0) || (this->collider.dim.height != 0)) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void item_get_check(EnWonderItem* this, PlayState* play, s32 autoCollect) {
    static s16 Item_No_Data[] = {
        ITEM00_NUTS,           ITEM00_HEART_PIECE,  ITEM00_MAGIC_LARGE,   ITEM00_MAGIC_SMALL,
        ITEM00_RECOVERY_HEART, ITEM00_ARROWS_SMALL, ITEM00_ARROWS_MEDIUM, ITEM00_ARROWS_LARGE,
        ITEM00_RUPEE_GREEN,    ITEM00_RUPEE_BLUE,   ITEM00_RUPEE_RED,     ITEM00_FLEXIBLE,
    };
    s32 i;
    s32 randomDrop;

    Na_StartSystemSe_F(NA_SE_SY_GET_ITEM);

    if (this->dropCount == 0) {
        this->dropCount++;
    }
    for (i = this->dropCount; i > 0; i--) {
        if (this->itemDrop < WONDERITEM_DROP_RANDOM) {
            if ((this->itemDrop == WONDERITEM_DROP_FLEXIBLE) || !autoCollect) {
                Item_set0(play, &this->actor.world.pos, Item_No_Data[this->itemDrop]);
            } else {
                Item_set0(play, &this->actor.world.pos, Item_No_Data[this->itemDrop] | 0x8000);
            }
        } else {
            randomDrop = this->itemDrop - WONDERITEM_DROP_RANDOM;
            if (!autoCollect) {
                Item_Set_Std(play, NULL, &this->actor.world.pos, randomDrop);
            } else {
                Item_Set_Std(play, NULL, &this->actor.world.pos, randomDrop | 0x8000);
            }
        }
    }
    if (this->switchFlag >= 0) {
        Actor_Environment_sw_On(play, this->switchFlag);
    }
    Actor_delete(&this->actor);
}

void En_Wonder_Item_actor_ct(Actor* thisx, PlayState* play) {
    static u32 Attack_Weapon_Data[] = {
        DMG_SLASH | DMG_DEKU_STICK,
        DMG_ARROW,
        DMG_HAMMER_SWING,
        DMG_EXPLOSIVE,
        DMG_SLINGSHOT,
        DMG_BOOMERANG,
        DMG_HOOKSHOT,
    };
    s32 pad;
    s16 colTypeIndex;
    EnWonderItem* this = (EnWonderItem*)thisx;
    s16 rotZover10;
    s16 tagIndex;

    PRINTF("\n\n");
    PRINTF(VT_FGCOL(GREEN) T("☆☆☆☆☆ 不思議不思議まか不思議 \t   ☆☆☆☆☆ %x\n",
                             "☆☆☆☆☆ Mysterious mystery, very mysterious \t   ☆☆☆☆☆ %x\n") VT_RST,
           this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    this->wonderMode = PARAMS_GET_U(this->actor.params, 11, 5);
    this->itemDrop = PARAMS_GET_U(this->actor.params, 6, 5);
    this->switchFlag = PARAMS_GET_U(this->actor.params, 0, 6);
    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    if ((this->switchFlag >= 0) && Actor_Environment_sw_Check(play, this->switchFlag)) {
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
        Actor_delete(&this->actor);
        return;
    }
    switch (this->wonderMode) {
        case WONDERITEM_MULTITAG_FREE:
            this->numTagPoints = this->actor.world.rot.z & 0xF;
            rotZover10 = 0;
            if (this->actor.world.rot.z >= 10) {
                rotZover10 = this->actor.world.rot.z / 10;
                this->timerMod = rotZover10 * 20;
            }
            this->numTagPoints = this->actor.world.rot.z - rotZover10 * 10;
            // i.e timerMod = rot.z / 10 seconds, numTagPoints = rot.z % 10
            this->updateFunc = mode_jyozan;
            break;
        case WONDERITEM_TAG_POINT_FREE:
            tagIndex = this->actor.world.rot.z & 0xFF;
            point_pos[tagIndex] = this->actor.world.pos;
            Actor_delete(&this->actor);
            break;
        case WONDERITEM_PROXIMITY_DROP:
            this->dropCount = this->actor.world.rot.z & 0xFF;
            this->updateFunc = mode_get_item;
            break;
        case WONDERITEM_INTERACT_SWITCH:
            colTypeIndex = this->actor.world.rot.z & 0xFF;
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
            this->collider.elem.acDmgInfo.dmgFlags = Attack_Weapon_Data[colTypeIndex];
            this->collider.dim.radius = 20;
            this->collider.dim.height = 30;
            this->updateFunc = mode_attack_item;
            break;
        case WONDERITEM_UNUSED:
            break;
        case WONDERITEM_MULTITAG_ORDERED:
            this->numTagPoints = this->actor.world.rot.z & 0xF;
            rotZover10 = 0;
            if (this->actor.world.rot.z >= 10) {
                rotZover10 = this->actor.world.rot.z / 10;
                this->timerMod = rotZover10 * 20;
            }
            this->numTagPoints = this->actor.world.rot.z - rotZover10 * 10;
            // i.e timerMod = rot.z / 10 seconds, numTagPoints = rot.z % 10
            this->updateFunc = mode_order_check;
            break;
        case WONDERITEM_TAG_POINT_ORDERED:
            tagIndex = this->actor.world.rot.z & 0xFF;
            order_point_pos[tagIndex] = this->actor.world.pos;
            Actor_delete(&this->actor);
            break;
        case WONDERITEM_PROXIMITY_SWITCH:
            this->updateFunc = mode_pass;
            break;
        case WONDERITEM_BOMB_SOLDIER:
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
            this->collider.elem.acDmgInfo.dmgFlags = DMG_SLINGSHOT;
            this->unkPos = this->actor.world.pos;
            this->collider.dim.radius = 35;
            this->collider.dim.height = 75;
            this->updateFunc = mode_ukkari;
            break;
        case WONDERITEM_ROLL_DROP:
            this->dropCount = this->actor.world.rot.z & 0xFF;
            this->updateFunc = mode_rolling_get_item;
            break;
        default:
            Actor_delete(&this->actor);
            break;
    }
}

void mode_jyozan(EnWonderItem* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 prevTagFlags = this->tagFlags;
    s32 i;
    s32 mask;
    f32 dx;
    f32 dy;
    f32 dz;

    for (i = 0, mask = 1; i < this->numTagPoints; i++, mask <<= 1) {
        if (!(prevTagFlags & mask)) {
            dx = player->actor.world.pos.x - point_pos[i].x;
            dy = player->actor.world.pos.y - point_pos[i].y;
            dz = player->actor.world.pos.z - point_pos[i].z;

            if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 50.0f) {
                this->tagFlags |= mask;
                this->tagCount++;
                this->timer = this->timerMod + 81;
                return;
            }

#if DEBUG_FEATURES
            if (BREG(0) != 0) {
                Debug_Display_new(point_pos[i].x, point_pos[i].y, point_pos[i].z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 255, 0, 255, 4, play->state.gfxCtx);
            }
#endif
        }
    }
    if (this->timer == 1) {
        Actor_delete(&this->actor);
        return;
    }
    if (this->tagCount == this->numTagPoints) {
        if (this->switchFlag >= 0) {
            Actor_Environment_sw_On(play, this->switchFlag);
        }
        item_get_check(this, play, true);
    }
}

void mode_get_item(EnWonderItem* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.xzDistToPlayer < 50.0f) && (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        item_get_check(this, play, true);
    }
}

void mode_attack_item(EnWonderItem* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        item_get_check(this, play, false);
    }
}

void mode_pass(EnWonderItem* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.xzDistToPlayer < 50.0f) && (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        if (this->switchFlag >= 0) {
            Actor_Environment_sw_On(play, this->switchFlag);
        }
        Actor_delete(&this->actor);
    }
}

void mode_order_check(EnWonderItem* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 prevTagFlags = this->tagFlags;
    s32 i;
    s32 mask;
    f32 dx;
    f32 dy;
    f32 dz;

    for (i = 0, mask = 1; i < this->numTagPoints; i++, mask <<= 1) {
        if (!(prevTagFlags & mask)) {
            dx = player->actor.world.pos.x - order_point_pos[i].x;
            dy = player->actor.world.pos.y - order_point_pos[i].y;
            dz = player->actor.world.pos.z - order_point_pos[i].z;

            if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 50.0f) {
                if (prevTagFlags & mask) {
                    return;
                } else if (i == this->nextTag) {
                    this->tagFlags |= mask;
                    this->tagCount++;
                    this->nextTag++;
                    this->timer = this->timerMod + 81;
                    return;
                } else {
                    Actor_delete(&this->actor);
                    return;
                }
            }

#if DEBUG_FEATURES
            if (BREG(0) != 0) {
                Debug_Display_new(order_point_pos[i].x, order_point_pos[i].y, order_point_pos[i].z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 0, 255, 255, 4, play->state.gfxCtx);
            }
#endif
        }
    }
    if (this->timer == 1) {
        Actor_delete(&this->actor);
        return;
    }
    if (this->tagCount == this->numTagPoints) {
        item_get_check(this, play, true);
    }
}

void mode_ukkari(EnWonderItem* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_HEISHI2, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, this->actor.yawTowardsPlayer, 0, 9) != NULL) {
            PRINTF(VT_FGCOL(YELLOW) T("☆☆☆☆☆ うっかり兵セット完了 ☆☆☆☆☆ \n", "☆☆☆☆☆ Careless soldier spawned ☆☆☆☆☆ \n")
                       VT_RST);
        }
        if (this->switchFlag >= 0) {
            Actor_Environment_sw_On(play, this->switchFlag);
        }
        Actor_delete(&this->actor);
    }
}

void mode_rolling_get_item(EnWonderItem* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.xzDistToPlayer < 50.0f) && (player->invincibilityTimer < 0) &&
        (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        item_get_check(this, play, true);
    }
}

void En_Wonder_Item_actor_move(Actor* thisx, PlayState* play) {
    static s16 color_data[] = {
        255, 255, 0,   255, 0,   255, 0,   255, 255, 255, 0,   0, 0, 255, 0,   0, 0, 255, 128, 128,
        128, 128, 128, 0,   128, 0,   128, 0,   128, 0,   128, 0, 0, 0,   128, 0, 0, 0,   128,
    }; // These seem to be mistyped. Logically they should be s16[13][3] and be indexed as [colorIndex][i]
    s32 pad;
    EnWonderItem* this = (EnWonderItem*)thisx;
    s32 colorIndex;

    if (this->timer != 0) {
        this->timer--;
    }
    this->updateFunc(this, play);

    if (this->wonderMode == WONDERITEM_UNUSED) {
        Actor_world_to_eye(&this->actor, this->unkHeight);
    }
    if ((this->wonderMode == WONDERITEM_INTERACT_SWITCH) || (this->wonderMode == WONDERITEM_BOMB_SOLDIER)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    colorIndex = this->wonderMode;
    if (this->wonderMode > 12) {
        colorIndex = 0;
    }

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, color_data[colorIndex], color_data[colorIndex + 1],
                               color_data[colorIndex + 2], 255, 4, play->state.gfxCtx);
    }
}
