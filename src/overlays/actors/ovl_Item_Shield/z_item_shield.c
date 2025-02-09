/*
 * File: z_item_shield.c
 * Overlay: ovl_Item_Shield
 * Description: Deku Shield
 */

#include "terminal.h"
#include "z_item_shield.h"
#include "assets/objects/object_link_child/object_link_child.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Item_Shield_Actor_ct(Actor* thisx, PlayState* play);
void Item_Shield_Actor_dt(Actor* thisx, PlayState* play);
void Item_Shield_Actor_move(Actor* thisx, PlayState* play);
void Item_Shield_Actor_draw(Actor* thisx, PlayState* play);

void move_fire_set(ItemShield* this, PlayState* play);
static void move_wait(ItemShield* this, PlayState* play);

static ColliderCylinderInit ItemShield_OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 15, 0, { 0, 0, 0 } },
};

ActorProfile Item_Shield_Profile = {
    /**/ ACTOR_ITEM_SHIELD,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_LINK_CHILD,
    /**/ sizeof(ItemShield),
    /**/ Item_Shield_Actor_ct,
    /**/ Item_Shield_Actor_dt,
    /**/ Item_Shield_Actor_move,
    /**/ Item_Shield_Actor_draw,
};

static Color_RGBA8 prim = { 255, 255, 0, 255 };
static Color_RGBA8 env = { 255, 0, 0, 255 };

void Item_Shield_actor_set_process(ItemShield* this, ItemShieldActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Item_Shield_Actor_ct(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;
    s32 i;

    this->timer = 0;
    this->unk_19C = 0;

    switch (this->actor.params) {
        case 0:
            Shape_Info_init(&this->actor.shape, 1400.0f, NULL, 0.0f);
            this->actor.shape.rot.x = 0x4000;
            Item_Shield_actor_set_process(this, move_wait);
            break;

        case 1:
            Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
            Item_Shield_actor_set_process(this, move_fire_set);
            this->unk_19C |= 2;
            for (i = 0; i < 8; i++) {
                this->unk_19E[i] = 1 + 2 * i;
                this->unk_1A8[i].x = rnd_fx(10.0f);
                this->unk_1A8[i].y = rnd_fx(10.0f);
                this->unk_1A8[i].z = rnd_fx(10.0f);
            }
            break;
    }

    Actor_set_scale(&this->actor, 0.01f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ItemShield_OcInfoData);
    PRINTF(VT_FGCOL(GREEN) "Item_Shild %d \n" VT_RST, this->actor.params);
}

void Item_Shield_Actor_dt(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void move_fall(ItemShield* this, PlayState* play) {
    Actor_position_moveF(&this->actor);
    if (Actor_carry_check(&this->actor, play)) {
        Actor_delete(&this->actor);
        return;
    }
    Actor_carry_request_set2(&this->actor, play, GI_SHIELD_DEKU, 30.0f, 50.0f);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->timer--;
        if (this->timer < 60) {
            if (this->timer & 1) {
                this->unk_19C |= 2;
            } else {
                this->unk_19C &= ~2;
            }
        }
        if (this->timer == 0) {
            Actor_delete(&this->actor);
        }
    }
}

static void move_wait(ItemShield* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        Actor_delete(&this->actor);
        return;
    }
    Actor_carry_request_set2(&this->actor, play, GI_SHIELD_DEKU, 30.0f, 50.0f);
    if (this->collider.base.acFlags & AC_HIT) {
        Item_Shield_actor_set_process(this, move_fall);
        this->actor.velocity.y = 4.0f;
        this->actor.minVelocityY = -4.0f;
        this->actor.gravity = -0.8f;
        this->actor.speed = 0.0f;
        this->timer = 160;
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void move_fire_fall(ItemShield* this, PlayState* play) {
    static Vec3f pos = { 0.0f, 0.0f, 0.0f };
    static f32 scl[] = { 0.3f, 0.6f,  0.9f, 1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
                                1.0f, 0.85f, 0.7f, 0.55f, 0.4f, 0.25f, 0.1f, 0.0f };
    static f32 color[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f,
                                0.6f, 0.4f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 temp;

    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    this->actor.shape.yOffset = ABS(sin_s(this->actor.shape.rot.x)) * 1500.0f;

    for (i = 0; i < 8; i++) {
        temp = 15 - this->unk_19E[i];
        pos.x = this->unk_1A8[i].x;
        pos.y = this->unk_1A8[i].y + (this->actor.shape.yOffset * 0.01f) + (scl[temp] * -10.0f * 0.2f);
        pos.z = this->unk_1A8[i].z;
        fire_tail_effect_set3(play, &this->actor, &pos, scl[temp] * 0.2f, -1, color[temp]);
        if (this->unk_19E[i] != 0) {
            this->unk_19E[i]--;
        } else if (this->timer > 16) {
            this->unk_19E[i] = 15;
            this->unk_1A8[i].x = rnd_fx(15.0f);
            this->unk_1A8[i].y = rnd_fx(10.0f);
            this->unk_1A8[i].z = rnd_fx(15.0f);
        }
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->unk_198 -= this->actor.shape.rot.x >> 1;
        this->unk_198 -= this->unk_198 >> 2;
        this->actor.shape.rot.x += this->unk_198;
        if ((this->timer >= 8) && (this->timer < 24)) {
            Actor_set_scale(&this->actor, (this->timer - 8) * 0.000625f);
        }
        if (this->timer != 0) {
            this->timer--;
        } else {
            Actor_delete(&this->actor);
        }
    }
}

void move_fire_set(ItemShield* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    MtxF* shield = &player->shieldMf;

    this->actor.world.pos.x = shield->xw;
    this->actor.world.pos.y = shield->yw;
    this->actor.world.pos.z = shield->zw;
    this->unk_19C &= ~2;

    this->actor.shape.rot.y = atans_table(-shield->zz, -shield->xz);
    this->actor.shape.rot.x = atans_table(-shield->yz, sqrtf(shield->zz * shield->zz + shield->xz * shield->xz));

    if (ABS(this->actor.shape.rot.x) > 0x4000) {
        this->unk_19C |= 1;
    }

    Item_Shield_actor_set_process(this, move_fire_fall);

    this->actor.velocity.y = 4.0;
    this->actor.minVelocityY = -4.0;
    this->actor.gravity = -0.8;
    this->unk_198 = 0;
    this->timer = 70;
    this->actor.speed = 0;
}

void Item_Shield_Actor_move(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    this->actionFunc(this, play);
}

void Item_Shield_Actor_draw(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    if (!(this->unk_19C & 2)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_item_shield.c", 457);
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_item_shield.c", 460);
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gLinkChildDekuShieldDL));
        CLOSE_DISPS(play->state.gfxCtx, "../z_item_shield.c", 465);
    }
}
