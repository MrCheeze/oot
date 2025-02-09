/*
 * File: z_en_gb.c
 * Overlay: ovl_En_Gb
 * Description: Poe Seller
 */

#include "z_en_gb.h"
#include "assets/objects/object_ps/object_ps.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Gb_actor_ct(Actor* thisx, PlayState* play);
void En_Gb_actor_dt(Actor* thisx, PlayState* play);
void En_Gb_actor_move(Actor* thisx, PlayState* play2);
void En_Gb_actor_draw(Actor* thisx, PlayState* play);

void Gb_TalkWait(EnGb* this, PlayState* play);
void Gb_Tataku(EnGb* this, PlayState* play);
void Gb_Buy_BigGhost(EnGb* this, PlayState* play);
void Gb_Buy_Ghost(EnGb* this, PlayState* play);
void Gb_Talk(EnGb* this, PlayState* play);
void Gb_Final_BigGhost(EnGb* this, PlayState* play);
void Gb_CarryWait(EnGb* this, PlayState* play);
void Gb_CarryFinish(EnGb* this, PlayState* play);

void Gb_Soul_disp(EnGb* this, PlayState* play);
void Gb_Soul_move(EnGb* this, PlayState* play);

ActorProfile En_Gb_Profile = {
    /**/ ACTOR_EN_GB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_PS,
    /**/ sizeof(EnGb),
    /**/ En_Gb_actor_ct,
    /**/ En_Gb_actor_dt,
    /**/ En_Gb_actor_move,
    /**/ En_Gb_actor_draw,
};

static EnGbCagedSoulInfo Gb_Soul_Data[] = {
    { { 255, 255, 170, 255 }, { 255, 200, 0, 255 }, gPoeSellerAngrySoulTex, -15 },
    { { 255, 255, 170, 255 }, { 0, 150, 0, 255 }, gPoeSellerHappySoulTex, -12 },
    { { 255, 170, 255, 255 }, { 100, 0, 150, 255 }, gPoeSellerSadSoulTex, -8 },
};

static ColliderCylinderInitType1 GbPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 75, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 Gb_BTPipeData[] = {
    {
        {
            COL_MATERIAL_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 4, 20, 0, { 0, 0, 0 } },
    },
    {
        {
            COL_MATERIAL_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 4, 20, 0, { 0, 0, 0 } },
    },
    {
        {
            COL_MATERIAL_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 10, 20, 0, { 0, 0, 0 } },
    },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2200, ICHAIN_STOP),
};

// relative positions of poe souls
static Vec3f soul_init_pos[] = {
    { -8.0f, 112.0f, -8.0f },
    { -3.0f, 112.0f, 29.0f },
    { 31.0f, 112.0f, 29.0f },
    { 31.0f, 112.0f, -8.0f },
};

// relative positions of bottles
static Vec3f bottle_offset[] = {
    { -48.0f, 0.0f, 34.0f },
    { -55.0f, 0.0f, 49.0f },
    { -48.0f, 0.0f, 60.0f },
};

void Gb_Message_Set(EnGb* this) {
    if (GET_INFTABLE(INFTABLE_B6)) {
        this->textId = 0x70F5;
    } else {
        this->textId = 0x70F4;
    }
}

void En_Gb_actor_ct(Actor* thisx, PlayState* play) {
    EnGb* this = (EnGb*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 i;
    f32 rand;
    Vec3f focusOffset;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gPoeSellerCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gPoeSellerSkel, &gPoeSellerIdleAnim, this->jointTable, this->morphTable,
                       12);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->dyna.actor, &GbPipeData);

    for (i = 0; i < ARRAY_COUNT(Gb_BTPipeData); i++) {
        ClObjPipe_ct(play, &this->bottlesColliders[i]);
        ClObjPipe_set3(play, &this->bottlesColliders[i], &this->dyna.actor, &Gb_BTPipeData[i]);
    }

    this->light = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point_ct(&this->lightInfo, this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y,
                              this->dyna.actor.home.pos.z, 255, 255, 255, 200);

    Shape_Info_init(&this->dyna.actor.shape, 0.0f, Actor_shadow_circle, 35.0f);
    Actor_set_scale(&this->dyna.actor, 0.01f);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->dyna.actor.speed = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.gravity = -1.0f;
    this->actionTimer = (s16)rnd_f(100.0f) + 100;

    for (i = 0; i < ARRAY_COUNT(soul_init_pos); i++) {
        s32 pad;

        this->cagedSouls[i].infoIdx = (s32)rnd_f(30.0f) % 3;
        this->cagedSouls[i].unk_14.x = this->cagedSouls[i].translation.x =
            soul_init_pos[i].x + this->dyna.actor.world.pos.x;
        this->cagedSouls[i].unk_14.y = this->cagedSouls[i].translation.y =
            soul_init_pos[i].y + this->dyna.actor.world.pos.y;
        this->cagedSouls[i].unk_14.z = this->cagedSouls[i].translation.z =
            soul_init_pos[i].z + this->dyna.actor.world.pos.z;
        this->cagedSouls[i].unk_1 = 1;
        this->cagedSouls[i].unk_3 = this->cagedSouls[i].unk_2 = 0;
        this->cagedSouls[i].unk_20 = this->cagedSouls[i].unk_24 = 0.0f;
        this->cagedSouls[i].unk_6 = rnd_f(40.0f);
        this->cagedSouls[i].rotate180 = this->cagedSouls[i].unk_6 & 1;
        this->cagedSouls[i].unk_28 = 0.2f;
    }

    rand = fqrand();
    this->lightColor.r = (s8)(rand * 30.0f) + 225;
    this->lightColor.g = (s8)(rand * 100.0f) + 155;
    this->lightColor.b = (s8)(rand * 160.0f) + 95;
    this->lightColor.a = 200;
    Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateXYZ(this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                     MTXMODE_APPLY);
    focusOffset.x = focusOffset.y = 0.0f;
    focusOffset.z = 44.0f;
    Matrix_Position(&focusOffset, &this->dyna.actor.focus.pos);
    this->dyna.actor.focus.pos.y += 62.5f;
    Gb_Message_Set(this);
    this->actionFunc = Gb_TalkWait;
}

void En_Gb_actor_dt(Actor* thisx, PlayState* play) {
    EnGb* this = (EnGb*)thisx;

    ClObjPipe_dt(play, &this->collider);
    Global_light_list_delete(play, &play->lightCtx, this->light);
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Gb_CrossPosition_Set(EnGb* this) {
    s32 i;
    Vec3f sp48;
    Vec3f sp3C;

    Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateXYZ(this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                     MTXMODE_APPLY);
    sp48.x = sp48.y = 0.0f;
    sp48.z = 25.0f;
    Matrix_Position(&sp48, &sp3C);
    this->collider.dim.pos.x = sp3C.x;
    this->collider.dim.pos.y = sp3C.y;
    this->collider.dim.pos.z = sp3C.z;

    for (i = 0; i < ARRAY_COUNT(bottle_offset); i++) {
        Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_rotateXYZ(this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                         MTXMODE_APPLY);
        Matrix_Position(&bottle_offset[i], &sp3C);
        this->bottlesColliders[i].dim.pos.x = sp3C.x;
        this->bottlesColliders[i].dim.pos.y = sp3C.y;
        this->bottlesColliders[i].dim.pos.z = sp3C.z;
    }
}

s32 Gb_SoulEscapeCheck(EnGb* this) {
    s32 i;
    for (i = 0; i < ARRAY_COUNT(this->cagedSouls); i++) {
        if (this->cagedSouls[i].unk_3) {
            return 1;
        }
    }
    return 0;
}

void Gb_SoulEscapeSet(EnGb* this) {
    Skeleton_Info2_init(&this->skelAnime, &gPoeSellerSwingStickAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gPoeSellerSwingStickAnim), ANIMMODE_ONCE, 0.0f);
    Actor_SE_set(&this->dyna.actor, NA_SE_EV_NALE_MAGIC);
    this->actionFunc = Gb_Tataku;
}

void Gb_TalkWait(EnGb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Gb_SoulEscapeCheck(this)) {
        if (this->actionTimer != 0) {
            this->actionTimer--;
        } else {
            Gb_SoulEscapeSet(this);
            return;
        }
    }
    if (Actor_talk_check(&this->dyna.actor, play)) {
        s32 pad;

        switch (Actor_get_item_check(play)) {
            case EXCH_ITEM_NONE:
                Gb_Message_Set(this);
                this->actionFunc = Gb_Talk;
                break;
            case EXCH_ITEM_BOTTLE_POE:
                player->actor.textId = 0x70F6;
                this->actionFunc = Gb_Buy_Ghost;
                break;
            case EXCH_ITEM_BOTTLE_BIG_POE:
                player->actor.textId = 0x70F7;
                this->actionFunc = Gb_Buy_BigGhost;
                break;
        }
    } else if (this->dyna.actor.xzDistToPlayer < 100.0f) {
        Actor_talk_request_get_item(&this->dyna.actor, play, 100.0f, EXCH_ITEM_BOTTLE_POE);
    }
}

void Gb_Talk(EnGb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        if (!GET_INFTABLE(INFTABLE_B6)) {
            SET_INFTABLE(INFTABLE_B6);
        }
        Gb_Message_Set(this);
        this->actionFunc = Gb_TalkWait;
    }
}

void Gb_Buy_Ghost(EnGb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        if (!GET_INFTABLE(INFTABLE_B6)) {
            SET_INFTABLE(INFTABLE_B6);
        }
        Gb_Message_Set(this);
        bottle_item_change(play, GET_PLAYER(play), ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        lupy_increase(10);
        this->actionFunc = Gb_TalkWait;
    }
}

void Gb_Buy_BigGhost(EnGb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        if (!GET_INFTABLE(INFTABLE_B6)) {
            SET_INFTABLE(INFTABLE_B6);
        }
        Gb_Message_Set(this);
        bottle_item_change(play, GET_PLAYER(play), ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        lupy_increase(50);
        HIGH_SCORE(HS_POE_POINTS) += 100;
        if (HIGH_SCORE(HS_POE_POINTS) != 1000) {
            if (HIGH_SCORE(HS_POE_POINTS) > 1100) {
                HIGH_SCORE(HS_POE_POINTS) = 1100;
            }
            this->actionFunc = Gb_TalkWait;
        } else {
            Player* player = GET_PLAYER(play);

            player->exchangeItemId = EXCH_ITEM_NONE;
            this->textId = 0x70F8;
            message_set2(play, this->textId);
            this->actionFunc = Gb_Final_BigGhost;
        }
    }
}

void Gb_Final_BigGhost(EnGb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        Actor_carry_request_set2(&this->dyna.actor, play, GI_BOTTLE_EMPTY, 100.0f, 10.0f);
        this->actionFunc = Gb_CarryWait;
    }
}

void Gb_CarryWait(EnGb* this, PlayState* play) {
    if (Actor_carry_check(&this->dyna.actor, play)) {
        this->dyna.actor.parent = NULL;
        this->actionFunc = Gb_CarryFinish;
    } else {
        Actor_carry_request_set2(&this->dyna.actor, play, GI_BOTTLE_EMPTY, 100.0f, 10.0f);
    }
}

void Gb_CarryFinish(EnGb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        Actor_talk_check(&this->dyna.actor, play);
        Gb_Message_Set(this);
        this->actionFunc = Gb_TalkWait;
    }
}

void Gb_Tataku(EnGb* this, PlayState* play) {
    if (this->skelAnime.curFrame == Si2_anime_end_frame(&gPoeSellerSwingStickAnim)) {
        Skeleton_Info2_init(&this->skelAnime, &gPoeSellerIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gPoeSellerIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->actionFunc = Gb_TalkWait;
    } else if (this->skelAnime.curFrame == 18.0f) {
        this->cagedSouls[1].unk_1 = 3;
        this->cagedSouls[1].unk_3 = 1;
        this->cagedSouls[2].unk_1 = 3;
        this->cagedSouls[2].unk_3 = 1;
        this->cagedSouls[3].unk_1 = 3;
        this->cagedSouls[3].unk_3 = 1;
        if (this->actionFunc) {} // these ifs cannot just contain a constant
        this->cagedSouls[0].unk_1 = 3;
        this->cagedSouls[0].unk_3 = 1;
        if (this->actionFunc) {}
        this->actionTimer = (s16)rnd_f(600.0f) + 600;
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_WOOD_HIT);
    }
}

void En_Gb_actor_move(Actor* thisx, PlayState* play2) {
    EnGb* this = (EnGb*)thisx;
    PlayState* play = play2;
    s32 i;
    f32 rand;

    this->frameTimer++;
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actionFunc(this, play);
    this->dyna.actor.textId = this->textId;
    Gb_CrossPosition_Set(this);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    for (i = 0; i < ARRAY_COUNT(this->bottlesColliders); i++) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->bottlesColliders[i].base);
    }

    rand = fqrand();
    this->lightColor.r = (s8)(rand * 30.0f) + 225;
    this->lightColor.g = (s8)(rand * 100.0f) + 155;
    this->lightColor.b = (s8)(rand * 160.0f) + 95;
    this->lightColor.a = 200;
    Gb_Soul_move(this, play);
}

void En_Gb_actor_draw(Actor* thisx, PlayState* play) {
    EnGb* this = (EnGb*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_gb.c", 763);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 255);

    Light_point_ct(&this->lightInfo, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                              this->lightColor.a);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->dyna.actor);
    Gb_Soul_disp(this, play);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_gb.c", 796);
}

void Gb_Soul_move(EnGb* this, PlayState* play) {
    f32 temp_f20;
    s32 i;

    for (i = 0; i < 4; i++) {
        switch (this->cagedSouls[i].unk_1) {
            case 0:
                add_calc2(&this->cagedSouls[i].unk_20, 1.0f, 0.02f, this->cagedSouls[i].unk_24);
                add_calc2(&this->cagedSouls[i].unk_24, 1.0f, 0.001f, 1.0f);
                if ((this->cagedSouls[i].unk_28 - .01f) <= this->cagedSouls[i].unk_20) {
                    this->cagedSouls[i].unk_20 = this->cagedSouls[i].unk_28;
                    this->cagedSouls[i].unk_24 = 0.0f;
                    this->cagedSouls[i].unk_1 = 1;
                    this->cagedSouls[i].unk_2 = 2;
                    this->cagedSouls[i].unk_6 = 0;
                }
                break;
            case 1:
                if (this->cagedSouls[i].unk_6 != 0) {
                    this->cagedSouls[i].unk_6--;
                } else {
                    this->cagedSouls[i].unk_3 = 0;
                    this->cagedSouls[i].unk_24 = 0.0f;
                    this->cagedSouls[i].unk_1 = this->cagedSouls[i].unk_2;
                }
                break;
            case 2:
                add_calc2(&this->cagedSouls[i].unk_20, 0.0f, 0.02f, this->cagedSouls[i].unk_24);
                add_calc2(&this->cagedSouls[i].unk_24, 1.0f, 0.001f, 1.0f);
                if (this->cagedSouls[i].unk_20 <= 0.01f) {
                    this->cagedSouls[i].unk_28 = this->cagedSouls[i].unk_28 + 0.2f;
                    if (this->cagedSouls[i].unk_28 > 1.0f) {
                        this->cagedSouls[i].unk_28 = 1.0f;
                    }
                    this->cagedSouls[i].unk_20 = 0.0f;
                    this->cagedSouls[i].unk_24 = 0.0f;
                    this->cagedSouls[i].unk_1 = 1;
                    this->cagedSouls[i].unk_2 = 0;
                    this->cagedSouls[i].unk_6 = 0;
                }
                break;
            case 3:
                add_calc2(&this->cagedSouls[i].unk_20, 0.0f, 0.5f, 1.0f);
                if (this->cagedSouls[i].unk_20 <= 0.01f) {
                    this->cagedSouls[i].unk_28 = 0.2f;
                    this->cagedSouls[i].unk_20 = 0.0f;
                    this->cagedSouls[i].unk_24 = 0.0f;
                    this->cagedSouls[i].unk_1 = 1;
                    this->cagedSouls[i].unk_2 = 0;
                    this->cagedSouls[i].unk_6 = (s16)rnd_f(60.0f) + 60;
                }
                break;
        }

        temp_f20 = this->cagedSouls[i].unk_20 * 60.0f;
        if ((i == 0) || (i == 3)) {
            this->cagedSouls[i].translation.x = this->cagedSouls[i].unk_14.x;
            this->cagedSouls[i].translation.y = this->cagedSouls[i].unk_14.y + temp_f20;
            this->cagedSouls[i].translation.z = this->cagedSouls[i].unk_14.z;
        } else if (i == 1) {
            s16 rot = this->dyna.actor.world.rot.y - 0x4000;

            this->cagedSouls[i].translation.x = this->cagedSouls[i].unk_14.x + sin_s(rot) * temp_f20;
            this->cagedSouls[i].translation.z = this->cagedSouls[i].unk_14.z + cos_s(rot) * temp_f20;
            this->cagedSouls[i].translation.y = this->cagedSouls[i].unk_14.y;
        } else {
            s16 rot = this->dyna.actor.world.rot.y + 0x4000;

            this->cagedSouls[i].translation.x = this->cagedSouls[i].unk_14.x + sin_s(rot) * temp_f20;
            this->cagedSouls[i].translation.z = this->cagedSouls[i].unk_14.z + cos_s(rot) * temp_f20;
            this->cagedSouls[i].translation.y = this->cagedSouls[i].unk_14.y;
        }
    }
}

void Gb_Soul_disp(EnGb* this, PlayState* play) {
    s32 pad;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_gb.c", 914);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < 4; i++) {
        s32 idx = this->cagedSouls[i].infoIdx;

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                    (u32)(Gb_Soul_Data[idx].timerMultiplier * this->frameTimer) % 512, 32, 128));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(Gb_Soul_Data[idx].texture));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, Gb_Soul_Data[idx].prim.r, Gb_Soul_Data[idx].prim.g,
                        Gb_Soul_Data[idx].prim.b, Gb_Soul_Data[idx].prim.a);
        gDPSetEnvColor(POLY_XLU_DISP++, Gb_Soul_Data[idx].env.r, Gb_Soul_Data[idx].env.g, Gb_Soul_Data[idx].env.b,
                       Gb_Soul_Data[idx].env.a);

        Matrix_push();
        Matrix_translate(this->cagedSouls[i].translation.x, this->cagedSouls[i].translation.y,
                         this->cagedSouls[i].translation.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);

        if (this->cagedSouls[i].rotate180) {
            Matrix_rotateXYZ(0, -0x8000, 0, MTXMODE_APPLY);
        }
        Matrix_scale(0.007f, 0.007f, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_gb.c", 955);
        gSPDisplayList(POLY_XLU_DISP++, gPoeSellerCagedSoulDL);

        Matrix_pull();
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_gb.c", 962);
}
