/*
 * File: z_en_po_desert.c
 * Overlay: ovl_En_Po_Desert
 * Description: Guide Poe (Haunted Wasteland)
 */

#include "z_en_po_desert.h"
#include "assets/objects/object_po_field/object_po_field.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_REACT_TO_LENS | ACTOR_FLAG_IGNORE_QUAKE)

void En_Po_Desert_actor_ct(Actor* thisx, PlayState* play);
void En_Po_Desert_actor_dt(Actor* thisx, PlayState* play);
void En_Po_Desert_actor_move(Actor* thisx, PlayState* play);
void En_Po_Desert_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnPoDesert* this, PlayState* play);
static void mode_wait(EnPoDesert* this, PlayState* play);
static void mode_fly(EnPoDesert* this, PlayState* play);
static void mode_disappear(EnPoDesert* this, PlayState* play);

ActorProfile En_Po_Desert_Profile = {
    /**/ ACTOR_EN_PO_DESERT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_PO_FIELD,
    /**/ sizeof(EnPoDesert),
    /**/ En_Po_Desert_actor_ct,
    /**/ En_Po_Desert_actor_dt,
    /**/ En_Po_Desert_actor_move,
    /**/ En_Po_Desert_actor_draw,
};

static ColliderCylinderInit PoDesertOcPipeData = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_NONE,
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

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_POE_WASTELAND, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 3200, ICHAIN_STOP),
};

void En_Po_Desert_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoDesert* this = (EnPoDesert*)thisx;

    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gPoeFieldSkel, &gPoeFieldFloatAnim, this->jointTable, this->morphTable, 10);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &PoDesertOcPipeData);
    this->lightColor.r = 255;
    this->lightColor.g = 255;
    this->lightColor.b = 210;
    this->lightColor.a = 255;
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                              255, 255, 255, 200);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 37.0f);
    this->currentPathPoint = 1;
    this->actor.params = PARAMS_GET_U(this->actor.params, 8, 8);
    this->targetY = this->actor.world.pos.y;
    mode_wait_init(this, play);
}

void En_Po_Desert_actor_dt(Actor* thisx, PlayState* play) {
    EnPoDesert* this = (EnPoDesert*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait_init(EnPoDesert* this, PlayState* play) {
    Path* path = &play->pathList[this->actor.params];
    Vec3s* pathPoint;

    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    pathPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->currentPathPoint];
    this->actor.home.pos.x = pathPoint->x;
    this->actor.home.pos.y = pathPoint->y;
    this->actor.home.pos.z = pathPoint->z;
    this->initDistToNextPoint = Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos);
    this->initDistToNextPoint = CLAMP_MIN(this->initDistToNextPoint, 1.0f);
    this->currentPathPoint++;
    this->yDiff = this->actor.home.pos.y - this->actor.world.pos.y;
    this->actor.speed = 0.0f;
    if (path->count == this->currentPathPoint) {
        this->currentPathPoint = 0;
    }
    this->actionFunc = mode_wait;
}

static void mode_fly_init(EnPoDesert* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeFieldFloatAnim, -5.0f);
    this->actionFunc = mode_fly;
}

static void mode_disappear_init(EnPoDesert* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    this->actionTimer = 16;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    this->actionFunc = mode_disappear;
}

void set_po_desert_position_y(EnPoDesert* this) {
    if (this->speedModifier == 0) {
        this->speedModifier = 32;
    }
    if (this->speedModifier != 0) {
        this->speedModifier--;
    }
    this->actor.world.pos.y = sin_s(this->speedModifier * 0x800) * 13.0f + this->targetY;
}

static void mode_wait(EnPoDesert* this, PlayState* play) {
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    if (this->actor.xzDistToPlayer < 200.0f && (this->currentPathPoint != 2 || play->actorCtx.lensActive)) {
        if (this->currentPathPoint == 2) {
            if (Game_play_demo_mode_check(play)) {
                this->actor.shape.rot.y += 0x800;
                return;
            }
            message_set(play, 0x600B, NULL);
        }
        mode_fly_init(this);
    } else {
        this->actor.shape.rot.y += 0x800;
    }
}

static void mode_fly(EnPoDesert* this, PlayState* play) {
    f32 temp_f20;

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    temp_f20 = sinf(this->actionTimer * (M_PI / 20.0f)) * 5.0f;
    this->actor.world.pos.x += temp_f20 * cos_s(this->actor.shape.rot.y);
    this->actor.world.pos.z += temp_f20 * sin_s(this->actor.shape.rot.y);
    if (this->actionTimer == 0) {
        this->actionTimer = 40;
    }
    temp_f20 = Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos);
    this->actor.world.rot.y = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
    adds(&this->actor.shape.rot.y, this->actor.world.rot.y + 0x8000, 5, 0x400);
    this->actor.speed = sinf(this->speedModifier * (M_PI / 32.0f)) * 2.5f + 5.5f;
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    this->targetY = this->actor.home.pos.y - ((temp_f20 * this->yDiff) / this->initDistToNextPoint);
    if (temp_f20 < 40.0f) {
        if (this->currentPathPoint != 0) {
            mode_wait_init(this, play);
        } else {
            mode_disappear_init(this);
        }
    }
}

static void mode_disappear(EnPoDesert* this, PlayState* play) {
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    this->actor.shape.rot.y += 0x2000;
    this->lightColor.a = this->actionTimer * 15.9375f;
    this->actor.shape.shadowAlpha = this->lightColor.a;
    if (this->actionTimer == 0) {
        Actor_delete(&this->actor);
    }
}

void En_Po_Desert_actor_move(Actor* thisx, PlayState* play) {
    EnPoDesert* this = (EnPoDesert*)thisx;
    s32 pad;

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    set_po_desert_position_y(this);
    Actor_BGcheck2(play, &this->actor, 0.0f, 27.0f, 60.0f, UPDBGCHECKINFO_FLAG_2);
    Actor_world_to_eye(&this->actor, 42.0f);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    if (play->actorCtx.lensActive) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_REACT_TO_LENS;
        this->actor.shape.shadowDraw = Actor_shadow_circle;
    } else {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_REACT_TO_LENS);
    }
}

s32 en_po_desert_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                                Gfx** gfxP) {
    EnPoDesert* this = (EnPoDesert*)thisx;
    f32 mtxScale;

    if (this->actionFunc == mode_disappear && limbIndex == 7) {
        mtxScale = this->actionTimer / 16.0f;
        Matrix_scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
    }
    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        *dList = NULL;
    }
    return false;
}

void en_po_desert_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    static Vec3f local_light_pos = { 0.0f, 1400.0f, 0.0f };

    EnPoDesert* this = (EnPoDesert*)thisx;
    f32 rand;
    Color_RGBA8 color;
    Vec3f lightPos;

    if (limbIndex == 7) {
        Matrix_Position(&local_light_pos, &lightPos);
        rand = fqrand();
        color.r = (s16)(rand * 30.0f) + 225;
        color.g = (s16)(rand * 100.0f) + 155;
        color.b = (s16)(rand * 160.0f) + 95;
        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
            gDPPipeSync((*gfxP)++);
            gDPSetEnvColor((*gfxP)++, color.r, color.g, color.b, 255);
            MATRIX_FINALIZE_AND_LOAD((*gfxP)++, play->state.gfxCtx, "../z_en_po_desert.c", 523);
            gSPDisplayList((*gfxP)++, gPoeFieldLanternDL);
            gSPDisplayList((*gfxP)++, gPoeFieldLanternTopDL);
            gDPPipeSync((*gfxP)++);
            gDPSetEnvColor((*gfxP)++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
        }
        Light_point_ct(&this->lightInfo, lightPos.x, lightPos.y, lightPos.z, color.r, color.g, color.b, 200);
    }
}

void En_Po_Desert_actor_draw(Actor* thisx, PlayState* play) {
    EnPoDesert* this = (EnPoDesert*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_po_desert.c", 559);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x0A, anime_envcolor(play->state.gfxCtx, 255, 85, 0, 255));
    gSPSegment(POLY_XLU_DISP++, 0x08,
               anime_envcolor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                            this->lightColor.a));
    if (this->actionFunc == mode_disappear) {
        gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode + 2);
    }
    POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   en_po_desert_display1, en_po_desert_display2, &this->actor, POLY_XLU_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_desert.c", 597);
}
