/*
 * File: z_obj_dekujr.c
 * Overlay: ovl_Obj_Dekujr
 * Description: Deku Tree Sprout
 */

#include "z_obj_dekujr.h"
#include "assets/objects/object_dekujr/object_dekujr.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void Obj_Dekujr_actor_ct(Actor* thisx, PlayState* play);
void Obj_Dekujr_actor_dt(Actor* thisx, PlayState* play);
void Obj_Dekujr_actor_move(Actor* thisx, PlayState* play);
void Obj_Dekujr_actor_draw(Actor* thisx, PlayState* play);

void dekujr_demo(ObjDekujr* this, PlayState* play);

ActorProfile Obj_Dekujr_Profile = {
    /**/ ACTOR_OBJ_DEKUJR,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DEKUJR,
    /**/ sizeof(ObjDekujr),
    /**/ Obj_Dekujr_actor_ct,
    /**/ Obj_Dekujr_actor_dt,
    /**/ Obj_Dekujr_actor_move,
    /**/ Obj_Dekujr_actor_draw,
};

static ColliderCylinderInitToActor OcInfoData = {
    {
        NULL,
        0x00,
        0x00,
        0x39,
        COLSHAPE_CYLINDER,
    },
    { 0x02, { 0x00000000, 0x00, 0x00 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 60, 80, 0, { 0, 0, 0 } },
};

void Obj_Dekujr_actor_ct(Actor* thisx, PlayState* play) {
    ObjDekujr* this = (ObjDekujr*)thisx;
    s32 pad;

    if (z_common_data.save.cutsceneIndex < 0xFFF0) {
        if (!LINK_IS_ADULT) {
            Actor_delete(thisx);
            return;
        }
        this->unk_19C = 2;
        this->unk_19B = 0;
    } else {
        this->unk_19C = 0;
        this->unk_19B = 1;
    }
    if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        Actor_delete(thisx);
    } else {
        Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);
        ClObjPipe_ct(play, &this->collider);
        OcInfoData.base.actor = thisx;
        ClObjPipe_set(play, &this->collider, &OcInfoData);
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
        thisx->textId = set_talk_message(play, 0xF);
        Actor_set_scale(thisx, 0.4f);
    }
}

void Obj_Dekujr_actor_dt(Actor* thisx, PlayState* play) {
}

static void demo_start_pos_set(CsCmdActorCue* cue, Vec3f* dest) {
    dest->x = cue->startPos.x;
    dest->y = cue->startPos.y;
    dest->z = cue->startPos.z;
}

static void demo_end_pos_set(CsCmdActorCue* cue, Vec3f* dest) {
    dest->x = cue->endPos.x;
    dest->y = cue->endPos.y;
    dest->z = cue->endPos.z;
}

void dekujr_demo(ObjDekujr* this, PlayState* play) {
    CsCmdActorCue* cue;
    Vec3f initPos;
    Vec3f finalPos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 cueDuration;
    f32 gravity;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->unk_19C = 2;
        this->unk_19B = 0;
    } else {
        if (play->csCtx.curFrame == 351) {
            Actor_SE_set(&this->actor, NA_SE_EV_COME_UP_DEKU_JR);
        }

        cue = play->csCtx.actorCues[1];

        if (cue != NULL) {
            demo_start_pos_set(cue, &initPos);
            demo_end_pos_set(cue, &finalPos);

            if (this->unk_19C == 0) {
                this->actor.world.pos = initPos;
                this->unk_19C = 1;
            }

            this->actor.shape.rot.x = cue->rot.x;
            this->actor.shape.rot.y = cue->rot.y;
            this->actor.shape.rot.z = cue->rot.z;

            this->actor.velocity = velocity;

            if (cue->endFrame >= play->csCtx.curFrame) {
                cueDuration = cue->endFrame - cue->startFrame;

                this->actor.velocity.x = (finalPos.x - initPos.x) / cueDuration;
                gravity = this->actor.gravity;
                this->actor.velocity.y = (finalPos.y - initPos.y) / cueDuration;
                this->actor.velocity.y += gravity;

                if (this->actor.velocity.y < this->actor.minVelocityY) {
                    this->actor.velocity.y = this->actor.minVelocityY;
                }

                this->actor.velocity.z = (finalPos.z - initPos.z) / cueDuration;
            }
        }
    }
}

void Obj_Dekujr_actor_move(Actor* thisx, PlayState* play) {
    ObjDekujr* this = (ObjDekujr*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    if ((z_common_data.save.cutsceneIndex >= 0xFFF0) && (this->unk_19B == 0)) {
        this->unk_19C = 0;
        this->unk_19B = 1;
    }
    if (this->unk_19B == 1) {
        dekujr_demo(this, play);
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        talk_to_player(play, &this->actor, 0xF, &this->unk_1A0);
        Actor_world_to_eye(&this->actor, 40.0f);
    }
}

void Obj_Dekujr_actor_draw(Actor* thisx, PlayState* play) {
    u32 frameCount;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_dekujr.c", 370);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_dekujr.c", 379);
    gSPDisplayList(POLY_OPA_DISP++, object_dekujr_DL_0030D0);

    frameCount = play->state.frames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frameCount % 128, 0, 32, 32, 1, frameCount % 128,
                                0, 32, 32));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_dekujr.c", 399);
    gSPDisplayList(POLY_XLU_DISP++, object_dekujr_DL_0032D8);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_dekujr.c", 409);
}
