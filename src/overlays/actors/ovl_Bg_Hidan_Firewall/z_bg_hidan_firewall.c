/*
 * File: z_bg_hidan_firewall.c
 * Overlay: ovl_Bg_Hidan_Firewall
 * Description: Proximity Triggered Flame Wall
 */

#include "z_bg_hidan_firewall.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "segmented_address.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Firewall_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Firewall_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Firewall_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Firewall_actor_draw(Actor* thisx, PlayState* play);

s32 check_player_pos(BgHidanFirewall* this, PlayState* play);
static void mode_wait(BgHidanFirewall* this, PlayState* play);
void mode_fire_start(BgHidanFirewall* this, PlayState* play);
static void mode_fire(BgHidanFirewall* this, PlayState* play);
void hidan_firewall_attack_hit(BgHidanFirewall* this, PlayState* play);
void set_hian_firewall_atoc_pipe(BgHidanFirewall* this, PlayState* play);

ActorProfile Bg_Hidan_Firewall_Profile = {
    /**/ ACTOR_BG_HIDAN_FIREWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanFirewall),
    /**/ Bg_Hidan_Firewall_actor_ct,
    /**/ Bg_Hidan_Firewall_actor_dt,
    /**/ Bg_Hidan_Firewall_actor_move,
    /**/ NULL,
};

static ColliderCylinderInit HidanFirewallAtOcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 30, 83, 0, { 0 } },
};

static CollisionCheckInfoInit HidanFirewallStatusData = { 1, 80, 100, MASS_IMMOVABLE };

void Bg_Hidan_Firewall_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanFirewall* this = (BgHidanFirewall*)thisx;

    this->actor.scale.x = this->actor.scale.z = 0.12f;
    this->actor.scale.y = 0.01f;

    this->unk_150 = 0;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &HidanFirewallAtOcPipeData);

    this->collider.dim.pos.y = this->actor.world.pos.y;

    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HidanFirewallStatusData);

    this->actionFunc = mode_wait;
}

void Bg_Hidan_Firewall_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanFirewall* this = (BgHidanFirewall*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 check_player_pos(BgHidanFirewall* this, PlayState* play) {
    Player* player;
    Vec3f playerRelativePos;

    player = GET_PLAYER(play);
    Actor_search_position_project_distanceXZ(&this->actor, &playerRelativePos, &player->actor.world.pos);

    if (fabsf(playerRelativePos.x) < 100.0f && fabsf(playerRelativePos.z) < 120.0f) {
        return 1;
    }
    return 0;
}

static void mode_wait(BgHidanFirewall* this, PlayState* play) {
    if (check_player_pos(this, play) != 0) {
        this->actor.draw = Bg_Hidan_Firewall_actor_draw;
        this->actor.params = 5;
        this->actionFunc = mode_fire_start;
    }
}

void mode_fire_start(BgHidanFirewall* this, PlayState* play) {

    if (this->actor.params != 0) {
        this->actor.params--;
    }
    if (this->actor.params == 0) {
        this->actionFunc = mode_fire;
    }
}

static void mode_fire(BgHidanFirewall* this, PlayState* play) {
    if (check_player_pos(this, play) != 0) {
        chase_f(&this->actor.scale.y, 0.1f, 0.01f / 0.4f);
    } else {
        if (chase_f(&this->actor.scale.y, 0.01f, 0.01f) != 0) {
            this->actor.draw = NULL;
            this->actionFunc = mode_wait;
        } else {
            this->actor.params = 0;
        }
    }
}

void hidan_firewall_attack_hit(BgHidanFirewall* this, PlayState* play) {
    s16 phi_a3;

    if (Actor_player_direction_check(&this->actor, 0x4000)) {
        phi_a3 = this->actor.shape.rot.y;
    } else {
        phi_a3 = this->actor.shape.rot.y + 0x8000;
    }

    Actor_player_power_damage_set(play, &this->actor, 5.0f, phi_a3, 1.0f);
}

void set_hian_firewall_atoc_pipe(BgHidanFirewall* this, PlayState* play) {
    Player* player;
    Vec3f playerRelativePos;
    f32 temp_ret;
    f32 sp28;
    f32 phi_f0;

    player = GET_PLAYER(play);

    Actor_search_position_project_distanceXZ(&this->actor, &playerRelativePos, &player->actor.world.pos);
    if (playerRelativePos.x < -70.0f) {
        playerRelativePos.x = -70.0f;
    } else {
        if (70.0f < playerRelativePos.x) {
            phi_f0 = 70.0f;
        } else {
            phi_f0 = playerRelativePos.x;
        }
        playerRelativePos.x = phi_f0;
    }
    if (this->actor.params == 0) {
        if (0.0f < playerRelativePos.z) {
            playerRelativePos.z = -25.0f;
            this->actor.params = -1;
        } else {
            playerRelativePos.z = 25.0f;
            this->actor.params = 1;
        }
    } else {
        playerRelativePos.z = this->actor.params * 25.0f;
    }
    sp28 = sin_s(this->actor.shape.rot.y);
    temp_ret = cos_s(this->actor.shape.rot.y);
    this->collider.dim.pos.x = this->actor.world.pos.x + playerRelativePos.x * temp_ret + playerRelativePos.z * sp28;
    this->collider.dim.pos.z = this->actor.world.pos.z - playerRelativePos.x * sp28 + playerRelativePos.z * temp_ret;
}

void Bg_Hidan_Firewall_actor_move(Actor* thisx, PlayState* play) {
    BgHidanFirewall* this = (BgHidanFirewall*)thisx;
    s32 pad;

    this->unk_150 = (this->unk_150 + 1) % 8;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        hidan_firewall_attack_hit(this, play);
    }

    this->actionFunc(this, play);
    if (this->actionFunc == mode_fire) {
        set_hian_firewall_atoc_pipe(this, play);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        Actor_level_SE_set(&this->actor, NA_SE_EV_FIRE_PLATE - SFX_FLAG);
    }
}

static void* fire_txt[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void Bg_Hidan_Firewall_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanFirewall* this = (BgHidanFirewall*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_firewall.c", 448);

    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(fire_txt[this->unk_150]));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_hidan_firewall.c", 458);
    gSPDisplayList(POLY_XLU_DISP++, gFireTempleFireballUpperHalfDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_firewall.c", 463);
}
