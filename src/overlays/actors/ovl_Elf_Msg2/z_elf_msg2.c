/*
 * File: z_elf_msg2.c
 * Overlay: ovl_Elf_Msg2
 * Description: Targetable Navi check spot
 */

#include "z_elf_msg2.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Elf_Msg2_Actor_ct(Actor* thisx, PlayState* play);
void Elf_Msg2_Actor_dt(Actor* thisx, PlayState* play);
void Elf_Msg2_Actor_move(Actor* thisx, PlayState* play);
#if DEBUG_ASSETS
void ElfMsg2_Draw(Actor* thisx, PlayState* play);
#endif

static s32 get_message_number(ElfMsg2* this);
static void move_wait(ElfMsg2* this, PlayState* play);
void move_lock(ElfMsg2* this, PlayState* play);

ActorProfile Elf_Msg2_Profile = {
    /**/ ACTOR_ELF_MSG2,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ElfMsg2),
    /**/ Elf_Msg2_Actor_ct,
    /**/ Elf_Msg2_Actor_dt,
    /**/ Elf_Msg2_Actor_move,
#if DEBUG_ASSETS
    /**/ ElfMsg2_Draw,
#else
    /**/ NULL,
#endif
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

void Elf_Msg2_actor_set_process(ElfMsg2* this, ElfMsg2ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Checks a scene flag - if flag is set, the actor is killed and function returns 1. Otherwise returns 0.
 * Can also set a switch flag from params while killing.
 */
s32 delete_check_elf_msg2(ElfMsg2* this, PlayState* play) {
    if ((this->actor.world.rot.y > 0) && (this->actor.world.rot.y < 0x41) &&
        Actor_Environment_sw_Check(play, this->actor.world.rot.y - 1)) {
        LOG_STRING("共倒れ", "../z_elf_msg2.c", 171); // "Mutual destruction"
        if (PARAMS_GET_U(this->actor.params, 8, 6) != 0x3F) {
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));
        }
        Actor_delete(&this->actor);
        return 1;
    } else if ((this->actor.world.rot.y == -1) && Actor_Environment_room_clear_Check(play, this->actor.room)) {
        LOG_STRING("共倒れ２", "../z_elf_msg2.c", 182); // "Mutual destruction 2"
        if (PARAMS_GET_U(this->actor.params, 8, 6) != 0x3F) {
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));
        }
        Actor_delete(&this->actor);
        return 1;
    } else if (PARAMS_GET_U(this->actor.params, 8, 6) == 0x3F) {
        return 0;
    } else if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        LOG_STRING("共倒れ", "../z_elf_msg2.c", 192); // "Mutual destruction"
        Actor_delete(&this->actor);
        return 1;
    }
    return 0;
}

void Elf_Msg2_Actor_ct(Actor* thisx, PlayState* play) {
    ElfMsg2* this = (ElfMsg2*)thisx;

    PRINTF(VT_FGCOL(CYAN) " Elf_Msg2_Actor_ct %04x\n\n" VT_RST, this->actor.params);
    if (!delete_check_elf_msg2(this, play)) {
        if ((this->actor.world.rot.x > 0) && (this->actor.world.rot.x < 8)) {
            this->actor.attentionRangeType = this->actor.world.rot.x - 1;
        }
        ValueSet_process(thisx, value_init);
        if (this->actor.world.rot.y >= 0x41) {
            Elf_Msg2_actor_set_process(this, move_wait);
        } else {
            Elf_Msg2_actor_set_process(this, move_lock);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP;
            this->actor.textId = get_message_number(this);
        }
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    }
}

void Elf_Msg2_Actor_dt(Actor* thisx, PlayState* play) {
}

static s32 get_message_number(ElfMsg2* this) {
    return PARAMS_GET_U(this->actor.params, 0, 8) + 0x100;
}

/**
 * Runs while Navi text is up. Kills the actor upon closing the text box unless rot.z == 1, can also set a switch flag
 * from params.
 */
void talk_lock(ElfMsg2* this, PlayState* play) {
    s32 switchFlag;

    if (Actor_talk_end_check(&this->actor, play)) {
        if (this->actor.world.rot.z != 1) {
            Actor_delete(&this->actor);
            switchFlag = PARAMS_GET_U(this->actor.params, 8, 6);
            if (switchFlag != 0x3F) {
                Actor_Environment_sw_On(play, switchFlag);
            }
        } else {
            Elf_Msg2_actor_set_process(this, move_lock);
        }
    }
}

/**
 * Runs while Navi text is not up.
 */
void move_lock(ElfMsg2* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        Elf_Msg2_actor_set_process(this, talk_lock);
    }
}

/**
 * Idles until a switch flag is set, at which point the actor becomes interactable
 */
static void move_wait(ElfMsg2* this, PlayState* play) {
    // If (y >= 0x41) && (y <= 0x80), Idles until switch flag (actor.world.rot.y - 0x41) is set
    // If (y > 0x80), Idles forever (Bug?)
    if ((this->actor.world.rot.y >= 0x41) && (this->actor.world.rot.y <= 0x80) &&
        (Actor_Environment_sw_Check(play, (this->actor.world.rot.y - 0x41)))) {
        Elf_Msg2_actor_set_process(this, move_lock);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP;
        this->actor.textId = get_message_number(this);
    }
}

void Elf_Msg2_Actor_move(Actor* thisx, PlayState* play) {
    ElfMsg2* this = (ElfMsg2*)thisx;

    if (!delete_check_elf_msg2(this, play)) {
        this->actionFunc(this, play);
    }
}

#if DEBUG_ASSETS
#include "assets/overlays/ovl_Elf_Msg2/ovl_Elf_Msg2.c"

void ElfMsg2_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_elf_msg2.c", 355);

    if (R_NAVI_MSG_REGION_ALPHA == 0) {
        return;
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 100, 255, R_NAVI_MSG_REGION_ALPHA);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_elf_msg2.c", 362);
    gSPDisplayList(POLY_XLU_DISP++, D_809ADC38);
    gSPDisplayList(POLY_XLU_DISP++, sCubeDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_elf_msg2.c", 367);
}
#endif
