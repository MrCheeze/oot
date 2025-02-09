/*
 * File: z_en_po_field.c
 * Overlay: ovl_En_Po_Field
 * Description: Field Poe
 */

#include "z_en_po_field.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_po_field/object_po_field.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_IGNORE_QUAKE)

void En_Po_Field_actor_ct(Actor* thisx, PlayState* play);
void En_Po_Field_actor_dt(Actor* thisx, PlayState* play);
void En_Po_Field_actor_move(Actor* thisx, PlayState* play);
void En_Po_Field_actor_draw(Actor* thisx, PlayState* play);

void En_Po_Field_actor_after_move(Actor* thisx, PlayState* play);
void En_Po_Field_actor_after_draw(Actor* thisx, PlayState* play);

static void mode_dummy_init(EnPoField* this, PlayState* play);
static void mode_dummy(EnPoField* this, PlayState* play);
static void mode_start(EnPoField* this, PlayState* play);
static void mode_rotate(EnPoField* this, PlayState* play);
static void mode_damage(EnPoField* this, PlayState* play);
static void mode_escape(EnPoField* this, PlayState* play);
static void mode_down(EnPoField* this, PlayState* play);
static void mode_disappear(EnPoField* this, PlayState* play);
static void mode_kantera_drop(EnPoField* this, PlayState* play);
static void mode_soul_appear(EnPoField* this, PlayState* play);
static void mode_soul(EnPoField* this, PlayState* play);
static void mode_soul_delete(EnPoField* this, PlayState* play);
static void mode_soul_get(EnPoField* this, PlayState* play);
void po_field_fire_ct(EnPoField* this);

ActorProfile En_Po_Field_Profile = {
    /**/ ACTOR_EN_PO_FIELD,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PO_FIELD,
    /**/ sizeof(EnPoField),
    /**/ En_Po_Field_actor_ct,
    /**/ En_Po_Field_actor_dt,
    /**/ En_Po_Field_actor_move,
    /**/ En_Po_Field_actor_draw,
};

static ColliderCylinderInit PoFieldAcOcPipeData = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 50, 20, { 0, 0, 0 } },
};

static ColliderCylinderInit PoFieldAtPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit PoFieldStatusData = { 4, 25, 50, 40 };

static DamageTable PoFieldBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 set_cnt = 0;

static Vec3f map_center_pos = { -1000.0f, 0.0f, 6500.0f };

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f pvec = { 0.0f, 3.0f, 0.0f };

static Vec3f pacc = { 0.0f, 0.0f, 0.0f };

static EnPoFieldInfo soul_status_data[2] = {
    { { 255, 170, 255 }, { 100, 0, 150 }, { 255, 85, 0 }, 248, gPoeFieldSoulTex },
    { { 255, 255, 170 }, { 255, 200, 0 }, { 160, 0, 255 }, 241, gBigPoeSoulTex },
};

static Vec3f local_light_pos = { 0.0f, 1400.0f, 0.0f };

#pragma increment_block_number "gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128 ntsc-1.2:128 pal-1.0:128" \
                               "pal-1.1:128 hiratsu3:128"

static Vec3s set_pos[10];
static u8 sw_num[10];
static MtxF kantera_mtx;

void En_Po_Field_actor_ct(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    s32 pad;

    if (set_cnt != 10) {
        set_pos[set_cnt].x = this->actor.world.pos.x;
        set_pos[set_cnt].y = this->actor.world.pos.y;
        set_pos[set_cnt].z = this->actor.world.pos.z;
        sw_num[set_cnt] = PARAMS_GET_U(this->actor.params, 0, 8);
        set_cnt++;
    }
    if (set_cnt >= 2) {
        this->actor.params = 0xFF;
        Actor_delete(&this->actor);
        return;
    }
    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gPoeFieldSkel, &gPoeFieldFloatAnim, this->jointTable, this->morphTable, 10);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &PoFieldAcOcPipeData);
    ClObjPipe_ct(play, &this->flameCollider);
    ClObjPipe_set5(play, &this->flameCollider, &this->actor, &PoFieldAtPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &PoFieldBtlData, &PoFieldStatusData);
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point2_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                            255, 255, 255, 0);
    this->actor.shape.shadowDraw = Actor_shadow_circle;
    mode_dummy_init(this, play);
}

void En_Po_Field_actor_dt(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->actor.params != 0xFF) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
        ClObjPipe_dt(play, &this->flameCollider);
        ClObjPipe_dt(play, &this->collider);
    }
}

static void mode_dummy_init(EnPoField* this, PlayState* play) {
    this->actor.update = En_Po_Field_actor_move;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
    this->actor.shape.rot.x = 0;
    Light_point_color_set(&this->lightInfo, 0, 0, 0, 0);
    this->actionTimer = 200;
    Actor_set_scale(&this->actor, 0.0f);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_ALL;
    this->actor.colChkInfo.health = PoFieldStatusData.health;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actionFunc = mode_dummy;
}

static void mode_start_init(EnPoField* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPoeFieldAppearAnim);
    this->actor.draw = En_Po_Field_actor_draw;
    this->lightColor.r = 255;
    this->lightColor.g = 255;
    this->lightColor.b = 210;
    this->lightColor.a = 0;
    this->actor.shape.shadowAlpha = 0;
    this->actor.shape.yOffset = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
    this->actor.home.pos.y = this->actor.world.pos.y;
    if (this->actor.params == EN_PO_FIELD_BIG) {
        this->actor.speed = 12.0f;
        this->collider.dim.radius = 35;
        this->collider.dim.height = 100;
        this->collider.dim.yShift = 10;
        this->actor.shape.shadowScale = 45.0f;
        this->scaleModifier = 0.014f;
        this->actor.naviEnemyId = NAVI_ENEMY_BIG_POE;
    } else {
        this->actor.speed = 0.0f;
        this->collider.dim.radius = PoFieldAcOcPipeData.dim.radius;
        this->collider.dim.height = PoFieldAcOcPipeData.dim.height;
        this->collider.dim.yShift = PoFieldAcOcPipeData.dim.yShift;
        this->actor.shape.shadowScale = 37.0f;
        this->scaleModifier = 0.01f;
        this->actor.naviEnemyId = NAVI_ENEMY_POE_WASTELAND;
    }
    this->actionFunc = mode_start;
}

static void mode_rotate_init(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPoeFieldFloatAnim);
    this->collider.base.acFlags |= AC_ON;
    this->scaleModifier = this->actor.xzDistToPlayer;
    xyz_t_move(&this->actor.home.pos, &player->actor.world.pos);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (this->actionFunc != mode_damage) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionTimer = 600;
        this->unk_194 = 32;
    }
    this->actionFunc = mode_rotate;
}

static void mode_escape_init(EnPoField* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeFieldFleeAnim, -5.0f);
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = mode_escape;
    this->actor.speed = 12.0f;
    if (this->actionFunc != mode_damage) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
        this->actionTimer = 2000;
        this->unk_194 = 32;
    }
}

static void mode_damage_init(EnPoField* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPoeFieldDamagedAnim, -6.0f);
    if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT)) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, this->collider.base.ac) + 0x8000;
    }
    this->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    this->actor.speed = 5.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->actionFunc = mode_damage;
}

static void mode_down_init(EnPoField* this) {
    this->actionTimer = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.naviEnemyId = NAVI_ENEMY_NONE;
    if (this->flameTimer >= 20) {
        this->flameTimer = 19;
    }
    this->actionFunc = mode_down;
}

static void mode_disappear_init(EnPoField* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    this->actionTimer = 16;
    this->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    Actor_SE_set(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    this->actionFunc = mode_disappear;
}

static void mode_kantera_drop_init(EnPoField* this, PlayState* play) {
    this->actor.update = En_Po_Field_actor_after_move;
    this->actor.draw = En_Po_Field_actor_after_draw;
    this->actor.shape.shadowDraw = NULL;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.gravity = -1.0f;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.shape.rot.x = -0x8000;
    this->actionTimer = 60;
    this->actor.world.pos.y -= 15.0f;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actionFunc = mode_kantera_drop;
}

static void mode_soul_appear_init(EnPoField* this) {
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->actor.shape.rot.y = 0;
    this->lightColor.a = 0;
    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.home.pos.y = this->actor.world.pos.y;
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EV_METAL_BOX_BOUND);
    if (this->actor.params == EN_PO_FIELD_BIG) {
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
    }
    this->actionFunc = mode_soul_appear;
}

static void mode_soul_init(EnPoField* this) {
    this->actor.home.pos.y = this->actor.world.pos.y;
    Actor_world_to_eye(&this->actor, -10.0f);
    this->collider.dim.radius = 13;
    this->collider.dim.height = 30;
    this->collider.dim.yShift = 0;
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y - 20.0f;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    this->actor.textId = 0x5005;
    this->actionTimer = 400;
    this->unk_194 = 32;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_soul;
}

static void mode_soul_delete_init(EnPoField* this) {
    this->actionFunc = mode_soul_delete;
}

static void mode_soul_get_init(EnPoField* this) {
    this->actionFunc = mode_soul_get;
    this->actor.home.pos.y = this->actor.world.pos.y - 15.0f;
}

void set_po_field_position_y(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_194 == 0) {
        this->unk_194 = 32;
    }
    if (this->unk_194 != 0) {
        this->unk_194 -= 1;
    }
    if (this->actor.floorHeight == BGCHECK_Y_MIN) {
        mode_disappear_init(this);
        return;
    }
    add_calc2(
        &this->actor.home.pos.y,
        ((player->actor.world.pos.y > this->actor.floorHeight) ? player->actor.world.pos.y : this->actor.floorHeight) +
            13.0f,
        0.2f, 5.0f);
    this->actor.world.pos.y = sin_s(this->unk_194 * 0x800) * 13.0f + this->actor.home.pos.y;
}

void set_po_field_speedF(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 speed =
        ((player->stateFlags1 & PLAYER_STATE1_23) && player->rideActor != NULL) ? player->rideActor->speed : 12.0f;

    if (this->actor.xzDistToPlayer < 300.0f) {
        this->actor.speed = speed * 1.5f + 2.0f;
    } else if (this->actor.xzDistToPlayer < 400.0f) {
        this->actor.speed = speed * 1.25f + 2.0f;
    } else if (this->actor.xzDistToPlayer < 500.0f) {
        this->actor.speed = speed + 2.0f;
    } else {
        this->actor.speed = 12.0f;
    }
    this->actor.speed = CLAMP_MIN(this->actor.speed, 12.0f);
}

static void mode_dummy(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 spawnDist;
    s32 i;
    s32 bgId;

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actionTimer == 0) {
        for (i = 0; i < set_cnt; i++) {
            if (fabsf(set_pos[i].x - player->actor.world.pos.x) < 150.0f &&
                fabsf(set_pos[i].z - player->actor.world.pos.z) < 150.0f) {
                if (Actor_Environment_sw_Check(play, sw_num[i])) {
                    if (player->stateFlags1 & PLAYER_STATE1_23) { // Player riding Epona
                        return;
                    } else {
                        this->actor.params = EN_PO_FIELD_SMALL;
                        spawnDist = 300.0f;
                    }
                } else if (player->stateFlags1 & PLAYER_STATE1_23 || fqrand() < 0.4f) {
                    this->actor.params = EN_PO_FIELD_BIG;
                    this->spawnFlagIndex = i;
                    spawnDist = 480.0f;
                } else {
                    this->actor.params = EN_PO_FIELD_SMALL;
                    spawnDist = 300.0f;
                }
                this->actor.world.pos.x = sin_s(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.x;
                this->actor.world.pos.z = cos_s(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.z;
                this->actor.world.pos.y = player->actor.world.pos.y + 1000.0f;
                this->actor.world.pos.y = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                                     &this->actor, &this->actor.world.pos);
                if (this->actor.world.pos.y != BGCHECK_Y_MIN) {
                    this->actor.shape.rot.y = Actor_search_actor_angleY(&this->actor, &player->actor);
                    mode_start_init(this);
                } else {
                    return;
                }
            }
        }
    }
}

static void mode_start(EnPoField* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->lightColor.a = 255;
        Actor_set_scale(&this->actor, this->scaleModifier);
        if (this->actor.params == EN_PO_FIELD_BIG) {
            mode_escape_init(this);
        } else {
            mode_rotate_init(this, play);
        }
    } else if (this->skelAnime.curFrame > 10.0f) {
        this->lightColor.a = ((this->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    } else {
        this->actor.scale.x += this->scaleModifier * 0.1f;
        this->actor.scale.y = this->actor.scale.x;
        this->actor.scale.z = this->actor.scale.x;
    }
    this->actor.shape.shadowAlpha = this->lightColor.a;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.params == EN_PO_FIELD_BIG) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
        set_po_field_speedF(this, play);
    }
}

static void mode_rotate(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 temp_v1 = 16 - this->unk_194;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (ABS(temp_v1) < 16) {
        this->actor.world.rot.y += 512.0f * fabsf(sin_s(this->unk_194 * 0x800));
    }
    add_calc2(&this->scaleModifier, 180.0f, 0.5f, 10.0f);
    add_calc2(&this->actor.home.pos.x, player->actor.world.pos.x, 0.2f, 6.0f);
    add_calc2(&this->actor.home.pos.z, player->actor.world.pos.z, 0.2f, 6.0f);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x800, 0x200);
    if (this->actor.home.pos.x - player->actor.world.pos.x > 100.0f) {
        this->actor.home.pos.x = player->actor.world.pos.x + 100.0f;
    } else if (this->actor.home.pos.x - player->actor.world.pos.x < -100.0f) {
        this->actor.home.pos.x = player->actor.world.pos.x + -100.0f;
    }
    if (this->actor.home.pos.z - player->actor.world.pos.z > 100.0f) {
        this->actor.home.pos.z = player->actor.world.pos.z + 100.0f;
    } else if (this->actor.home.pos.z - player->actor.world.pos.z < -100.0f) {
        this->actor.home.pos.z = player->actor.world.pos.z + -100.0f;
    }
    this->actor.world.pos.x = this->actor.home.pos.x - (sin_s(this->actor.world.rot.y) * this->scaleModifier);
    this->actor.world.pos.z = this->actor.home.pos.z - (cos_s(this->actor.world.rot.y) * this->scaleModifier);
    if (this->actionTimer == 0) {
        mode_disappear_init(this);
    } else {
        po_field_fire_ct(this);
    }
    set_po_field_position_y(this, play);
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

static void mode_escape(EnPoField* this, PlayState* play) {
    f32 temp_f6;
    s16 phi_t0;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (Actor_search_position_distanceXZ(&this->actor, &map_center_pos) > 3000.0f) {
        phi_t0 = (s16)(this->actor.yawTowardsPlayer - Actor_search_position_angleY(&this->actor, &map_center_pos) - 0x8000) *
                 0.2f;
    } else {
        phi_t0 = 0;
    }
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer - phi_t0, 6, 0x400);
    set_po_field_speedF(this, play);
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    temp_f6 = sin_s(this->actionTimer * 0x800) * 3.0f;
    this->actor.world.pos.x -= temp_f6 * cos_s(this->actor.shape.rot.y);
    this->actor.world.pos.z += temp_f6 * sin_s(this->actor.shape.rot.y);
    if (this->actionTimer == 0 || this->actor.xzDistToPlayer > 1500.0f) {
        mode_disappear_init(this);
    } else {
        set_po_field_position_y(this, play);
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

static void mode_damage(EnPoField* this, PlayState* play) {
    chase_f(&this->actor.speed, 0.0f, 0.5f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this);
        } else if (this->actor.params == EN_PO_FIELD_BIG) {
            mode_escape_init(this);
        } else {
            mode_rotate_init(this, play);
        }
    }
}

static void mode_down(EnPoField* this, PlayState* play) {
    Vec3f sp6C;
    f32 sp68;
    s32 pad;
    s32 pad1;
    f32 temp_f0;

    this->actionTimer++;
    if (this->actionTimer < 8) {
        if (this->actionTimer < 5) {
            sp6C.y = sin_s(this->actionTimer * 0x1000 - 0x4000) * 23.0f + (this->actor.world.pos.y + 40.0f);
            sp68 = cos_s(this->actionTimer * 0x1000 - 0x4000) * 23.0f;
            sp6C.x = sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * sp68 + this->actor.world.pos.x;
            sp6C.z = cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * sp68 + this->actor.world.pos.z;
        } else {
            sp6C.y = this->actor.world.pos.y + 40.0f + 15.0f * (this->actionTimer - 5);
            sp6C.x = sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.x;
            sp6C.z = cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.z;
        }
        _Effect_SS_Db_ct(play, &sp6C, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        sp6C.x = (this->actor.world.pos.x + this->actor.world.pos.x) - sp6C.x;
        sp6C.z = (this->actor.world.pos.z + this->actor.world.pos.z) - sp6C.z;
        _Effect_SS_Db_ct(play, &sp6C, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        sp6C.x = this->actor.world.pos.x;
        sp6C.z = this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &sp6C, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        if (this->actionTimer == 1) {
            Actor_SE_set(&this->actor, NA_SE_EN_EXTINCT);
        }
    } else if (this->actionTimer == 28) {
        mode_kantera_drop_init(this, play);
    } else if (this->actionTimer >= 19) {
        temp_f0 = (28 - this->actionTimer) * 0.001f;
        this->actor.world.pos.y += 5.0f;
        this->actor.scale.z = temp_f0;
        this->actor.scale.y = temp_f0;
        this->actor.scale.x = temp_f0;
    }
    if (this->actionTimer == 18) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

static void mode_disappear(EnPoField* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    this->actor.shape.rot.y += 0x1000;
    this->lightColor.a = this->actionTimer * 15.9375f;
    this->actor.shape.shadowAlpha = this->lightColor.a;
    if (this->actionTimer == 0) {
        mode_dummy_init(this, play);
    }
}

static void mode_kantera_drop(EnPoField* this, PlayState* play) {
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 6.0f, 0, 1, 1, 15, OBJECT_PO_FIELD, 10,
                                 gPoeFieldLanternDL);
        mode_soul_appear_init(this);
    } else if (this->actionTimer == 0) {
        mode_dummy_init(this, play);
    }
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_2);
}

void po_field_soul_set(EnPoField* this, s32 arg1) {
    EnPoFieldInfo* info = &soul_status_data[this->actor.params];
    f32 multiplier;

    this->lightColor.a = CLAMP(this->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = this->lightColor.a * (1.0f / 255);
        this->actor.scale.x = this->actor.scale.z = 0.0056000003f * multiplier + 0.0014000001f;
        this->actor.scale.y = 0.007f - 0.007f * multiplier + 0.007f;
    } else {
        multiplier = 1.0f;
        this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = this->lightColor.a * (0.007f / 255);
        this->actor.world.pos.y = this->actor.home.pos.y + ((1.0f / 17.0f) * this->lightColor.a);
    }
    this->lightColor.r = info->lightColor.r * multiplier;
    this->lightColor.g = info->lightColor.g * multiplier;
    this->lightColor.b = info->lightColor.b * multiplier;
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, info->lightColor.r, info->lightColor.g, info->lightColor.b,
                              this->lightColor.a * (200.0f / 255));
}

static void mode_soul_appear(EnPoField* this, PlayState* play) {
    this->actor.home.pos.y += 2.0f;
    po_field_soul_set(this, 20);
    if (this->lightColor.a == 255) {
        mode_soul_init(this);
    }
}

static void mode_soul(EnPoField* this, PlayState* play) {
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (Actor_talk_check(&this->actor, play)) {
        mode_soul_get_init(this);
        return;
    }
    if (this->actionTimer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        mode_soul_delete_init(this);
        return;
    }
    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request(&this->actor, play);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
    this->actor.world.pos.y = sin_s(this->unk_194 * 0x800) * 5.0f + this->actor.home.pos.y;
    if (this->unk_194 != 0) {
        this->unk_194 -= 1;
    }
    if (this->unk_194 == 0) {
        this->unk_194 = 32;
    }
    this->collider.dim.pos.y = this->actor.world.pos.y - 20.0f;
    Actor_world_to_eye(&this->actor, -10.0f);
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->lightInfo.params.point.color[0],
                              this->lightInfo.params.point.color[1], this->lightInfo.params.point.color[2],
                              this->lightColor.a * (200.0f / 255));
}

static void mode_soul_delete(EnPoField* this, PlayState* play) {
    po_field_soul_set(this, -13);
    if (this->lightColor.a == 0) {
        mode_dummy_init(this, play);
    }
}

static void mode_soul_get(EnPoField* this, PlayState* play) {
    if (this->actor.textId != 0x5005) {
        po_field_soul_set(this, -13);
    } else {
        Actor_level_SE_set(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (pad_on_check(play)) {
            Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (play->msgCtx.choiceIndex == 0) {
                if (findEmptyBottle()) {
                    Actor_SE_set(&this->actor, NA_SE_EN_PO_BIG_GET);
                    if (this->actor.params == 0) {
                        item_get_setting(play, ITEM_BOTTLE_POE);
                        this->actor.textId = 0x5008;
                    } else {
                        this->actor.textId = 0x508F;
                        item_get_setting(play, ITEM_BOTTLE_BIG_POE);
                        Actor_Environment_sw_On(play, sw_num[this->spawnFlagIndex]);
                    }
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
                    this->actor.textId = 0x5006;
                }
            } else {
                this->actor.textId = 0x5007;
                Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
            }
            message_set2(play, this->actor.textId);
            return;
        }
    } else if (Actor_talk_end_check(&this->actor, play)) {
        mode_soul_delete_init(this);
    }
}

void En_Po_Field_damage_proc(EnPoField* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_PO_DAMAGE);
            }
            mode_damage_init(this);
        }
    }
}

void po_field_fire_ct(EnPoField* this) {
    if (this->flameTimer == 0) {
        this->flamePosition.x = this->lightInfo.params.point.x;
        this->flamePosition.y = this->lightInfo.params.point.y;
        this->flamePosition.z = this->lightInfo.params.point.z;
        this->flameTimer = 70;
        this->flameRotation = this->actor.shape.rot.y;
    }
}

void po_field_fire_move(EnPoField* this, PlayState* play) {
    if (this->flameTimer != 0) {
        if (this->flameTimer != 0) {
            this->flameTimer--;
        }
        if (this->flameCollider.base.atFlags & AT_HIT) {
            this->flameCollider.base.atFlags &= ~AT_HIT;
            this->flameTimer = 19;
        }
        if (this->flameTimer < 20) {
            chase_f(&this->flameScale, 0.0f, 0.00015f);
            return;
        }
        if (chase_f(&this->flameScale, 0.003f, 0.0006f) != 0) {
            this->flamePosition.x += 2.5f * sin_s(this->flameRotation);
            this->flamePosition.z += 2.5f * cos_s(this->flameRotation);
        }
        this->flameCollider.dim.pos.x = this->flamePosition.x;
        this->flameCollider.dim.pos.y = this->flamePosition.y;
        this->flameCollider.dim.pos.z = this->flamePosition.z;
        CollisionCheck_setAT(play, &play->colChkCtx, &this->flameCollider.base);
    }
}

void po_field_fire_draw(EnPoField* this, PlayState* play) {
    f32 sp4C;
    s32 pad;

    if (this->flameTimer != 0) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 1669);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                    (play->gameplayFrames * -20) % 512, 32, 128));
        sp4C = this->flameScale * 85000.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, sp4C);
        Matrix_translate(this->flamePosition.x, this->flamePosition.y, this->flamePosition.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000)), MTXMODE_APPLY);
        if (this->flameTimer >= 20) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            Matrix_scale(this->flameScale, this->flameScale, this->flameScale, MTXMODE_APPLY);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, sp4C, 0, 0, 0);
            Matrix_scale((this->flameScale * 0.7f) + 0.00090000004f, (0.003f - this->flameScale) + 0.003f, 0.003f,
                         MTXMODE_APPLY);
        }
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_po_field.c", 1709);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 1712);
    }
}

void set_po_field_color_data(EnPoField* this) {
    s16 temp_var;

    if (this->actionFunc == mode_escape) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 80);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        temp_var = this->lightColor.b + 5;
        this->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (this->actionFunc == mode_damage) {
        if (this->actor.colorFilterTimer & 2) {
            this->lightColor.r = 0;
            this->lightColor.g = 0;
            this->lightColor.b = 0;
        } else {
            this->lightColor.r = 80;
            this->lightColor.g = 255;
            this->lightColor.b = 225;
        }
    } else {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 255);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        if (this->lightColor.b > 210) {
            temp_var = this->lightColor.b - 5;
            this->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->lightColor.b + 5;
            this->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void set_po_field_kantera_color_data(EnPoField* this) {
    f32 rand;

    if (this->actionFunc == mode_start && this->skelAnime.curFrame < 12.0f) {
        this->soulColor.r = this->soulColor.g = this->soulColor.b = (s16)(this->skelAnime.curFrame * 16.66f) + 55;
        this->soulColor.a = this->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = fqrand();
        this->soulColor.r = (s16)(rand * 30.0f) + 225;
        this->soulColor.g = (s16)(rand * 100.0f) + 155;
        this->soulColor.b = (s16)(rand * 160.0f) + 95;
        this->soulColor.a = 200;
    }
}

void En_Po_Field_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoField* this = (EnPoField*)thisx;

    En_Po_Field_damage_proc(this, play);
    this->actionFunc(this, play);
    po_field_fire_move(this, play);
    if (this->actionFunc == mode_escape || this->actionFunc == mode_damage ||
        this->actionFunc == mode_start) {
        Actor_position_moveF(&this->actor);
    }
    if (this->actionFunc != mode_dummy) {
        Actor_world_to_eye(&this->actor, 42.0f);
        Actor_BGcheck2(play, &this->actor, 0.0f, 27.0f, 60.0f, UPDBGCHECKINFO_FLAG_2);
        set_po_field_color_data(this);
        set_po_field_kantera_color_data(this);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 en_po_field_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                                Gfx** gfxP) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->lightColor.a == 0 || limbIndex == 7 || (this->actionFunc == mode_down && this->actionTimer >= 2)) {
        *dList = NULL;
    } else if (this->actor.params == EN_PO_FIELD_BIG) {
        if (limbIndex == 1) {
            *dList = gBigPoeFaceDL;
        } else if (limbIndex == 8) {
            *dList = gBigPoeCloakDL;
        } else if (limbIndex == 9) {
            *dList = gBigPoeBodyDL;
        }
    }
    if (this->actionFunc == mode_disappear && limbIndex == 7) {
        Matrix_scale(this->actionTimer / 16.0f, this->actionTimer / 16.0f, this->actionTimer / 16.0f, MTXMODE_APPLY);
    }
    return false;
}

void en_po_field_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->actionFunc == mode_down && this->actionTimer >= 2 && limbIndex == 8) {
        MATRIX_FINALIZE_AND_LOAD((*gfxP)++, play->state.gfxCtx, "../z_en_po_field.c", 1916);
        gSPDisplayList((*gfxP)++, gPoeFieldBurnDL);
    }
    if (limbIndex == 7) {
        Vec3f vec;
        Matrix_Position(&local_light_pos, &vec);
        if (this->actionFunc == mode_down && this->actionTimer >= 19 && this->actor.scale.x != 0.0f) {
            f32 mtxScale = 0.01f / this->actor.scale.x;
            Matrix_scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_get(&kantera_mtx);
        if (this->actionFunc == mode_down && this->actionTimer == 27) {
            this->actor.world.pos.x = kantera_mtx.xw;
            this->actor.world.pos.y = kantera_mtx.yw;
            this->actor.world.pos.z = kantera_mtx.zw;
        }
        Light_point2_ct(&this->lightInfo, vec.x, vec.y, vec.z, this->soulColor.r, this->soulColor.g,
                                this->soulColor.b, this->soulColor.a * (200.0f / 255));
    }
}

void En_Po_Field_actor_draw(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    EnPoFieldInfo* info = &soul_status_data[this->actor.params];

    if (this->actionFunc != mode_dummy) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 1976);
        _texture_z_light_fog_prim(play->state.gfxCtx);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   anime_envcolor(play->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        if (this->lightColor.a == 255 || this->lightColor.a == 0) {
            gSPSegment(POLY_OPA_DISP++, 0x08,
                       anime_envcolor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                    this->lightColor.a));
            gSPSegment(POLY_OPA_DISP++, 0x0C, Actor_change_render_mode + 2);
            POLY_OPA_DISP =
                Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_po_field_display1,
                               en_po_field_display2, &this->actor, POLY_OPA_DISP);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       anime_envcolor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                    this->lightColor.a));
            gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);
            POLY_XLU_DISP =
                Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_po_field_display1,
                               en_po_field_display2, &this->actor, POLY_XLU_DISP);
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, this->soulColor.r, this->soulColor.g, this->soulColor.b, 255);
        Matrix_put(&kantera_mtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_po_field.c", 2033);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 2039);
    }
    po_field_fire_draw(this, play);
}

void En_Po_Field_actor_after_move(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc == mode_kantera_drop) {
        set_po_field_kantera_color_data(this);
    }
    po_field_fire_move(this, play);
}

void En_Po_Field_actor_after_draw(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    s32 pad;
    EnPoFieldInfo* info = &soul_status_data[this->actor.params];

    OPEN_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 2077);
    if (this->actionFunc == mode_kantera_drop) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   anime_envcolor(play->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        Light_point2_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, this->soulColor.r, this->soulColor.g, this->soulColor.b, 200);
        gDPSetEnvColor(POLY_OPA_DISP++, this->soulColor.r, this->soulColor.g, this->soulColor.b, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_po_field.c", 2104);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                    (play->gameplayFrames * info->unk_9) & 0x1FF, 0x20, 0x80));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(info->soulTexture));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, info->primColor.r, info->primColor.g, info->primColor.b,
                        this->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 255);
        Matrix_rotateY((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_po_field.c", 2143);
        gSPDisplayList(POLY_XLU_DISP++, gPoeFieldSoulDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_field.c", 2149);
    po_field_fire_draw(this, play);
}
