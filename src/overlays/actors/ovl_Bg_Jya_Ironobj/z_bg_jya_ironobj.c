/*
 * File: z_bg_jya_ironobj.c
 * Overlay: ovl_Bg_Jya_Ironobj
 * Description: Destructable Iron Knuckle objects
 */

#include "z_bg_jya_ironobj.h"
#include "assets/objects/object_jya_iron/object_jya_iron.h"
#include "overlays/actors/ovl_En_Ik/z_en_ik.h"

#define FLAGS 0

typedef void (*BgJyaIronobjIkFunc)(BgJyaIronobj*, PlayState*, EnIk*);

void Bg_Jya_Ironobj_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Ironobj_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Ironobj_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Ironobj_actor_draw(Actor* thisx, PlayState* play);
static void mv_stop_init(BgJyaIronobj* this);
static void mv_stop(BgJyaIronobj* this, PlayState* play);

void set_haheniron_hashira(BgJyaIronobj* this, PlayState* play, EnIk* enIk);
void set_haheniron_isu(BgJyaIronobj* this, PlayState* play, EnIk* enIk);

static int IronknackP = 0;

ActorProfile Bg_Jya_Ironobj_Profile = {
    /**/ ACTOR_BG_JYA_IRONOBJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_JYA_IRON,
    /**/ sizeof(BgJyaIronobj),
    /**/ Bg_Jya_Ironobj_actor_ct,
    /**/ Bg_Jya_Ironobj_actor_dt,
    /**/ Bg_Jya_Ironobj_actor_move,
    /**/ Bg_Jya_Ironobj_actor_draw,
};

static Gfx* JIRON_ShapeTbl[] = { gPillarDL, gThroneDL };

static ColliderCylinderInit ClPipeDt_AC = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
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
    { 30, 150, 0, { 0, 0, 0 } },
};

static s16 Attack_ang_y_add[] = { 0x8700, 0x4000, 0xC000, 0x0000 };

void set_collision_ironobj(BgJyaIronobj* this, PlayState* play) {
    ColliderCylinder* colCylinder = &this->colCylinder;

    ClObjPipe_ct(play, colCylinder);
    ClObjPipe_set5(play, colCylinder, &this->dyna.actor, &ClPipeDt_AC);
    if (PARAMS_GET_U(this->dyna.actor.params, 0, 1) == 1) {
        this->colCylinder.dim.radius = 40;
        this->colCylinder.dim.height = 100;
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, colCylinder);
}

/*
 * Spawns particles for the destroyed pillar
 */
void set_haheniron_hashira(BgJyaIronobj* this, PlayState* play, EnIk* enIk) {
    // Pillar
    static s16 s[] = { 5, 8, 11, 14, 17, 20, 23, 26 };
    static s16 counter[] = { 18, 26, 34, 42, 50, 60, 70, 80 };
    static s16 airK1[] = { 48, 42, 36, 32, 28, 24, 20, 16 };

    s32 i;
    s32 j;
    s16 unkArg5;
    f32 temp_f22;
    Vec3f pos;
    Vec3f vel;
    f32 coss;
    s16 rotY;
    f32 sins;
    s32 pad[2];

#if DEBUG_FEATURES
    if (enIk->unk_2FF <= 0 || enIk->unk_2FF >= 4) {
        PRINTF("Error 攻撃方法が分からない(%s %d)\n", "../z_bg_jya_ironobj.c", 233);
        return;
    }
#endif

    PRINTF("¢ attack_type(%d)\n", enIk->unk_2FF);
    rotY = Actor_search_actor_angleY(&this->dyna.actor, &enIk->actor) + Attack_ang_y_add[enIk->unk_2FF - 1];

    for (i = 0; i < 8; i++) {
        Actor* actor =
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_JYA_HAHENIRON, this->dyna.actor.world.pos.x,
                        fqrand() * 80.0f + this->dyna.actor.world.pos.y + 20.0f, this->dyna.actor.world.pos.z, 0,
                        (s16)(fqrand() * 0x4000) + rotY - 0x2000, 0, 0);
        if (actor != NULL) {
            actor->speed = fqrand() * 8.0f + 9.0f;
            actor->velocity.y = fqrand() * 10.0f + 6.0f;
        }
    }
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_JYA_HAHENIRON, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y + 150.0f, this->dyna.actor.world.pos.z, 0, 0, 0, 1);
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_JYA_HAHENIRON, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, 0, 0, 2);
    sins = sin_s(rotY);
    coss = cos_s(rotY);
    for (j = 0; j < 32; j++) {
        f32 rand = fqrand();

        if (rand < 0.1f) {
            unkArg5 = 0x60;
        } else if (rand < 0.8f) {
            unkArg5 = 0x40;
        } else {
            unkArg5 = 0x20;
        }
        pos.x = this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + ((4.375f * j) + 10.0f);
        pos.z = this->dyna.actor.world.pos.z;
        temp_f22 = fabsf(j - 15.5f) * (1.0f / 31) + 0.5f;
        vel.x = 2.0f * ((fqrand() * 6.0f) - 3.0f) + (fqrand() * sins * 8.0f * temp_f22);
        vel.y = (fqrand() * 8.0f) - 3.0f;
        vel.z = 2.0f * ((fqrand() * 6.0f) - 3.0f) + (fqrand() * coss * 8.0f * temp_f22);
        Effect_Kakera_ct2(play, &pos, &vel, &pos, -350, unkArg5, airK1[j & 7], 4, 0, s[j & 7], 0, 5,
                             counter[j & 7], -1, OBJECT_JYA_IRON, gObjectJyaIronDL_000880);
        if (fqrand() < 0.26f) {
            dust_fly_set2(play, &pos, 200.0f, 1, s[j & 7] * 4 + 60, s[j & 7] * 4 + 80, 1);
        }
    }
}

/*
 * Spawns particles for the destroyed throne
 */
void set_haheniron_isu(BgJyaIronobj* this, PlayState* play, EnIk* enIk) {
    // Throne
    static s16 s[] = { 5, 8, 11, 14, 17, 20, 23, 26 };
    static s16 counter[] = { 18, 26, 34, 42, 50, 60, 70, 80 };
    static s16 airK1[] = { 48, 42, 36, 32, 28, 24, 20, 16 };

    s32 i;
    s32 j;
    s16 unkArg5;
    f32 temp_f22;
    Vec3f pos;
    Vec3f vel;
    f32 coss;
    s16 rotY;
    f32 sins;
    s32 pad[2];

#if DEBUG_FEATURES
    if (enIk->unk_2FF <= 0 || enIk->unk_2FF >= 4) {
        PRINTF("Error 攻撃方法が分からない(%s %d)\n", "../z_bg_jya_ironobj.c", 362);
        return;
    }
#endif

    PRINTF("¢ attack_type(%d)\n", enIk->unk_2FF);
    rotY = Actor_search_actor_angleY(&this->dyna.actor, &enIk->actor) + Attack_ang_y_add[enIk->unk_2FF - 1];
    for (i = 0; i < 8; i++) {
        Actor* actor =
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_JYA_HAHENIRON, this->dyna.actor.world.pos.x,
                        (fqrand() * 80.0f) + this->dyna.actor.world.pos.y + 10.0f, this->dyna.actor.world.pos.z,
                        0, ((s16)(s32)(fqrand() * 0x4000) + rotY) - 0x2000, 0, 0);
        if (actor != NULL) {
            actor->speed = fqrand() * 8.0f + 9.0f;
            actor->velocity.y = fqrand() * 10.0f + 6.0f;
        }
    }
    sins = sin_s(rotY);
    coss = cos_s(rotY);

    for (j = 0; j < 32; j++) {
        f32 rand = fqrand();

        if (rand < 0.1f) {
            unkArg5 = 0x60;
        } else if (rand < 0.8f) {
            unkArg5 = 0x40;
        } else {
            unkArg5 = 0x20;
        }
        pos.x = this->dyna.actor.world.pos.x + (fqrand() * 40 - 20);
        pos.y = this->dyna.actor.world.pos.y + (3.75f * j);
        pos.z = this->dyna.actor.world.pos.z + (fqrand() * 40 - 20);
        temp_f22 = fabsf(j - 15.5f) * (1.0f / 31) + 0.5f;
        vel.x = 2.0f * (fqrand() * 6.0f - 3.0f) + (fqrand() * sins * 8.0f * temp_f22);
        vel.y = fqrand() * 8.0f - 3.0f;
        vel.z = 2.0f * (fqrand() * 6.0f - 3.0f) + (fqrand() * coss * 8.0f * temp_f22);
        Effect_Kakera_ct2(play, &pos, &vel, &pos, -350, unkArg5, airK1[j & 7], 4, 0, s[j & 7], 0, 5,
                             counter[j & 7], -1, OBJECT_JYA_IRON, gObjectJyaIronDL_000880);
        if (fqrand() < 0.26f) {
            dust_fly_set2(play, &pos, 200.0f, 1, s[j & 7] * 4 + 60, s[j & 7] * 4 + 80, 1);
        }
    }
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static CollisionHeader* t_bg[] = { &gPillarCol, &gThroneCol };

void Bg_Jya_Ironobj_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaIronobj* this = (BgJyaIronobj*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    set_collision_ironobj(this, play);
    DynaPolyUty_bgdi_SG2KSG(t_bg[PARAMS_GET_U(thisx->params, 0, 1)], &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    mv_stop_init(this);
}

void Bg_Jya_Ironobj_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaIronobj* this = (BgJyaIronobj*)thisx;

    ClObjPipe_dt(play, &this->colCylinder);
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_stop_init(BgJyaIronobj* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgJyaIronobj* this, PlayState* play) {
    static BgJyaIronobjIkFunc hahen_proc[] = { set_haheniron_hashira, set_haheniron_isu };
    Actor* actor;
    Vec3f dropPos;
    s32 i;

    if (this->colCylinder.base.acFlags & AC_HIT) {
        actor = this->colCylinder.base.ac;
        this->colCylinder.base.acFlags &= ~AC_HIT;
        if (actor != NULL && actor->id == ACTOR_EN_IK) {
            hahen_proc[PARAMS_GET_U(this->dyna.actor.params, 0, 1)](this, play, (EnIk*)actor);
            Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 80, NA_SE_EN_IRONNACK_BREAK_PILLAR);
            dropPos.x = this->dyna.actor.world.pos.x;
            dropPos.y = this->dyna.actor.world.pos.y + 20.0f;
            dropPos.z = this->dyna.actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_set0(play, &dropPos, ITEM00_RECOVERY_HEART);
                dropPos.y += 18.0f;
            }
            Actor_delete(&this->dyna.actor);
            return;
        }
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder.base);
    }
}

void Bg_Jya_Ironobj_actor_move(Actor* thisx, PlayState* play) {
    BgJyaIronobj* this = (BgJyaIronobj*)thisx;

    this->actionFunc(this, play);
}

void Bg_Jya_Ironobj_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, JIRON_ShapeTbl[PARAMS_GET_U(thisx->params, 0, 1)]);
}
