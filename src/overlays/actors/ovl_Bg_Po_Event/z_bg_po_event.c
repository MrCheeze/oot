/*
 * File: z_bg_po_event.c
 * Overlay: ovl_Bg_Po_Event
 * Description: Poe sisters' paintings and puzzle blocks
 */

#include "z_bg_po_event.h"
#include "assets/objects/object_po_sisters/object_po_sisters.h"

#define FLAGS 0

void Bg_Po_Event_actor_ct(Actor* thisx, PlayState* play);
void Bg_Po_Event_actor_dt(Actor* thisx, PlayState* play);
void Bg_Po_Event_actor_move(Actor* thisx, PlayState* play);
void Bg_Po_Event_actor_draw(Actor* thisx, PlayState* play);

static void mode_block_stop(BgPoEvent* this, PlayState* play);
void mode_block_yure(BgPoEvent* this, PlayState* play);
void mode_block_drop(BgPoEvent* this, PlayState* play);
static void mode_block_wait(BgPoEvent* this, PlayState* play);
static void mode_block_move(BgPoEvent* this, PlayState* play);
void mode_block_rot(BgPoEvent* this, PlayState* play);
void mode_block_end(BgPoEvent* this, PlayState* play);
void mode_emily_picture_wait(BgPoEvent* this, PlayState* play); // Amy is the green Poe
void mode_emily_picture_wait2(BgPoEvent* this, PlayState* play);
void mode_no_picture(BgPoEvent* this, PlayState* play);
void mode_picture_appear(BgPoEvent* this, PlayState* play);
void mode_picture_wait(BgPoEvent* this, PlayState* play);
void mode_picture_hit(BgPoEvent* this, PlayState* play);

ActorProfile Bg_Po_Event_Profile = {
    /**/ ACTOR_BG_PO_EVENT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_PO_SISTERS,
    /**/ sizeof(BgPoEvent),
    /**/ Bg_Po_Event_actor_ct,
    /**/ Bg_Po_Event_actor_dt,
    /**/ Bg_Po_Event_actor_move,
    /**/ Bg_Po_Event_actor_draw,
};

static ColliderTrisElementInit PoEventPictureAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f }, { 25.0f, -33.0f, 0.0f } } },
    },
};

static ColliderTrisInit PoEventPictureAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    PoEventPictureAcTrisElemData,
};

static u8 work_num2 = 0;

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

static u8 work_num;

void po_event_picture_ct(BgPoEvent* this, PlayState* play) {
    static s16 home_position_x[] = { -1302, -866, 1421, 985 };
    static s16 home_position_y[] = { 1107, 1091 };
    static s16 home_position_z[] = { -3384, -3252 };
    ColliderTrisElementInit* item;
    Vec3f* vtxVec;
    s32 i1;
    s32 i2;
    Vec3f sp9C[3];
    f32 coss;
    f32 sins;
    f32 scaleY;
    s32 phi_t2;
    Actor* newPainting;

    sins = sin_s(this->dyna.actor.shape.rot.y);
    coss = cos_s(this->dyna.actor.shape.rot.y);
    if (this->type == 4) {
        sins *= 2.4f;
        scaleY = 1.818f;
        coss *= 2.4f;
    } else {
        scaleY = 1.0f;
    }
    for (i1 = 0; i1 < PoEventPictureAcTrisData.count; i1++) {
        item = &PoEventPictureAcTrisData.elements[i1];
        if (1) {} // This section looks like a macro of some sort.
        for (i2 = 0; i2 < 3; i2++) {
            vtxVec = &item->dim.vtx[i2];
            sp9C[i2].x = (vtxVec->x * coss) + (this->dyna.actor.home.pos.x + (sins * vtxVec->z));
            sp9C[i2].y = (vtxVec->y * scaleY) + this->dyna.actor.home.pos.y;
            sp9C[i2].z = this->dyna.actor.home.pos.z + (coss * vtxVec->z) - (vtxVec->x * sins);
        }
        CollisionCheck_Uty_setTrisPos(&this->collider, i1, &sp9C[0], &sp9C[1], &sp9C[2]);
    }
    if ((this->type != 4) && (this->index != 2)) {
        phi_t2 = (this->type == 2) ? this->index : this->index + 2;
        newPainting = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_PO_EVENT,
                                         home_position_x[phi_t2], home_position_y[this->index], home_position_z[this->index], 0,
                                         this->dyna.actor.shape.rot.y + 0x8000, 0,
                                         ((this->index + 1) << 0xC) + (this->type << 8) + this->dyna.actor.params);
        if (newPainting == NULL) {
            Actor_delete(&this->dyna.actor);
            return;
        }
        if (this->index == 0) {
            if (this->dyna.actor.child->child == NULL) {
                Actor_delete(&this->dyna.actor);
                return;
            }
            this->dyna.actor.parent = this->dyna.actor.child->child;
            this->dyna.actor.child->child->child = &this->dyna.actor;
        }
    }
    this->timer = 0;
    if (this->type == 4) {
        work_num = 0;
        this->actionFunc = mode_emily_picture_wait;
    } else {
        work_num = (s32)(fqrand() * 3.0f) % 3;
        this->actionFunc = mode_no_picture;
    }
}

void po_event_block_ct(BgPoEvent* this, PlayState* play) {
    static s16 home_position_x[] = { 2149, 1969, 1909 };
    static s16 home_position_z[] = { -1410, -1350, -1530 };
    Actor* newBlock;
    CollisionHeader* colHeader = NULL;
    s32 bgId;

    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    DynaPolyUty_bgdi_SG2KSG(&gPoSistersAmyBlockCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if ((this->type == 0) && (this->index != 3)) {
        newBlock = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_PO_EVENT,
                                      home_position_x[this->index], this->dyna.actor.world.pos.y, home_position_z[this->index], 0,
                                      this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z - 0x4000,
                                      ((this->index + 1) << 0xC) + (this->type << 8) + this->dyna.actor.params);
        if (newBlock == NULL) {
            Actor_delete(&this->dyna.actor);
            return;
        }
        if (this->index == 0) {
            if (this->dyna.actor.child->child == NULL) {
                Actor_delete(&this->dyna.actor);
                return;
            }
            if (this->dyna.actor.child->child->child == NULL) {
                Actor_delete(&this->dyna.actor);
                Actor_delete(this->dyna.actor.child);
                return;
            }
            this->dyna.actor.parent = this->dyna.actor.child->child->child;
            this->dyna.actor.child->child->child->child = &this->dyna.actor;
        }
    }
    this->dyna.actor.world.pos.y = 833.0f;
    this->dyna.actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId,
                                                              &this->dyna.actor, &this->dyna.actor.world.pos);
    this->actionFunc = mode_block_stop;
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Po_Event_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    ValueSet_process(thisx, value_init);
    this->type = PARAMS_GET_U(thisx->params, 8, 4);
    this->index = PARAMS_GET_U(thisx->params, 12, 4);
    thisx->params &= 0x3F;

    if (this->type >= 2) {
        ClObjTris_ct(play, &this->collider);
        ClObjTris_set5_nzm(play, &this->collider, thisx, &PoEventPictureAcTrisData, this->colliderItems);
        if (Actor_Environment_sw_Check(play, thisx->params)) {
            Actor_delete(thisx);
        } else {
            po_event_picture_ct(this, play);
        }
    } else {
        MoveBG_ct(&this->dyna, 0);
        if (Actor_Environment_sw_Check(play, thisx->params)) {
            Actor_delete(thisx);
        } else {
            po_event_block_ct(this, play);
        }
    }
}

void Bg_Po_Event_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    if (this->type >= 2) {
        ClObjTris_dt_nzf(play, &this->collider);
    } else {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        if ((this->type == 1) && (z_common_data.timerSeconds > 0)) {
            z_common_data.timerState = TIMER_STATE_STOP;
        }
    }
}

static void mode_block_stop(BgPoEvent* this, PlayState* play) {
    this->dyna.actor.world.pos.y = 833.0f;
    if (work_num == 0x3F) {
        if (this->type == 1) {
            makeOnepointDemo(play, 3150, 65, NULL, CAM_ID_MAIN);
        }
        this->timer = 45;
        this->actionFunc = mode_block_yure;
    } else if (this->dyna.actor.xzDistToPlayer > 50.0f) {
        if (this->type != 1) {
            work_num |= (1 << this->index);
        } else {
            work_num |= 0x10;
        }
    } else if (this->type != 1) {
        work_num &= ~(1 << this->index);
    } else {
        work_num &= ~0x10;
    }
}

void mode_block_yure(BgPoEvent* this, PlayState* play) {
    DECR(this->timer);
    if (this->timer < 15) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + 2.0f * ((this->timer % 3) - 1);
        if (!(this->timer % 4)) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
        }
    }
    if (this->timer == 0) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        work_num = 0;
        this->timer = 60;
        this->actionFunc = mode_block_drop;
    }
}

void check_po_event_block_set(BgPoEvent* this) {
    s32 phi_v1;
    s32 phi_a1;
    s32 phi_t0;
    s32 phi_a3;

    if ((this->index == 3) || (this->index == 1)) {
        phi_v1 = this->dyna.actor.world.pos.z;
        phi_a1 = this->dyna.actor.child->world.pos.z;
        if (this->index == 3) {
            phi_a3 = this->dyna.actor.world.pos.x;
            phi_t0 = this->dyna.actor.child->world.pos.x;
        } else { // this->index == 1
            phi_a3 = this->dyna.actor.child->world.pos.x;
            phi_t0 = this->dyna.actor.world.pos.x;
        }
    } else {
        phi_v1 = this->dyna.actor.world.pos.x;
        phi_a1 = this->dyna.actor.child->world.pos.x;
        if (this->index == 0) {
            phi_a3 = this->dyna.actor.world.pos.z;
            phi_t0 = this->dyna.actor.child->world.pos.z;
        } else { // this->index == 2
            phi_a3 = this->dyna.actor.child->world.pos.z;
            phi_t0 = this->dyna.actor.world.pos.z;
        }
    }
    if ((phi_v1 == phi_a1) && ((phi_t0 - phi_a3) == 60)) {
        work_num |= (1 << this->index);
    } else {
        work_num &= ~(1 << this->index);
    }
}

void mode_block_drop(BgPoEvent* this, PlayState* play) {
    static s32 first_drop_flg = 0;

    this->dyna.actor.velocity.y++;
    if (chase_f(&this->dyna.actor.world.pos.y, 433.0f, this->dyna.actor.velocity.y)) {
        this->dyna.actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
        this->dyna.actor.velocity.y = 0.0f;
        work_num2++;
        if (this->type != 1) {
            check_po_event_block_set(this);
        } else {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
            ActorQuakeRumbleSet(&this->dyna.actor, play, 5, 5);
            event_timer_set(this->timer);
            if (first_drop_flg == 0) {
                first_drop_flg = 1;
            } else {
                player_demo_mode_set(play, &GET_PLAYER(play)->actor, PLAYER_CSACTION_7);
            }
        }
        this->direction = 0;
        this->actionFunc = mode_block_wait;
    }
}

static void mode_block_wait(BgPoEvent* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* amy;

    if (work_num == 0xF) {
        this->actionFunc = mode_block_end;
        if ((this->type == 0) && (this->index == 0)) {
            amy = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->dyna.actor.world.pos.x + 30.0f,
                              this->dyna.actor.world.pos.y - 30.0f, this->dyna.actor.world.pos.z + 30.0f, 0,
                              this->dyna.actor.shape.rot.y, 0, this->dyna.actor.params + 0x300);
            if (amy != NULL) {
                makeOnepointDemo(play, 3170, 30, amy, CAM_ID_MAIN);
            }
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
            z_common_data.timerState = TIMER_STATE_STOP;
        }
    } else {
        if ((z_common_data.timerSeconds == 0) && (work_num2 == 5)) {
            player->stateFlags2 &= ~PLAYER_STATE2_4;
            work_num = 0x10;
            work_num2 = 0;
        }
        if ((work_num == 0x40) || ((work_num == 0x10) && !player_demo_check(play))) {
            this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z;
            this->actionFunc = mode_block_rot;
            if (work_num == 0x10) {
                work_num = 0x40;
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_RISING);
                player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_8);
            }
        } else if (this->dyna.unk_150 != 0.0f) {
            if (this->direction == 0) {
                if (MoveBG_checkMoveArea(play, &this->dyna, 0x1E, 0x32, -0x14) != 0) {
                    work_num2--;
                    this->direction = (this->dyna.unk_150 >= 0.0f) ? 1.0f : -1.0f;
                    this->actionFunc = mode_block_move;
                } else {
                    player->stateFlags2 &= ~PLAYER_STATE2_4;
                    this->dyna.unk_150 = 0.0f;
                }
            } else {
                player->stateFlags2 &= ~PLAYER_STATE2_4;
                this->dyna.unk_150 = 0.0f;
                DECR(this->direction);
            }
        } else {
            this->direction = 0;
        }
    }
}

static void mode_block_move(BgPoEvent* this, PlayState* play) {
    static f32 move_length = 0.0f;
    f32 displacement;
    s32 blockStop;
    Player* player = GET_PLAYER(play);

    this->dyna.actor.speed += 0.1f;
    this->dyna.actor.speed = CLAMP_MAX(this->dyna.actor.speed, 2.0f);
    blockStop = chase_f(&move_length, 20.0f, this->dyna.actor.speed);
    displacement = this->direction * move_length;
    this->dyna.actor.world.pos.x = (sin_s(this->dyna.unk_158) * displacement) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (cos_s(this->dyna.unk_158) * displacement) + this->dyna.actor.home.pos.z;
    if (blockStop) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        if ((this->dyna.unk_150 > 0.0f) && (MoveBG_checkMoveArea(play, &this->dyna, 0x1E, 0x32, -0x14) == 0)) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        this->dyna.unk_150 = 0.0f;
        this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
        move_length = 0.0f;
        this->dyna.actor.speed = 0.0f;
        this->direction = 5;
        work_num2++;
        this->actionFunc = mode_block_wait;
        if (this->type == 1) {
            return;
        }
        check_po_event_block_set(this);
        check_po_event_block_set((BgPoEvent*)this->dyna.actor.parent);
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void mode_block_rot(BgPoEvent* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
    if (chase_f(&this->dyna.actor.world.pos.y, 493.0f, 1.0f) &&
        chase_angle(&this->dyna.actor.shape.rot.z, this->dyna.actor.world.rot.z - 0x4000, 0x400)) {

        this->index = (this->index + 1) % 4;
        this->actionFunc = mode_block_drop;
        work_num = 0;
        if (this->type == 1) {
            this->timer += 10;
            this->timer = CLAMP_MAX(this->timer, 120);
        }
    }
}

void mode_block_end(BgPoEvent* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
    if (chase_f(&this->dyna.actor.world.pos.y, 369.0f, 2.0f)) {
        work_num = 0x20;
        Actor_delete(&this->dyna.actor);
    }
}

void mode_emily_picture_wait(BgPoEvent* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        work_num |= 0x20;
        this->timer = 5;
        Set_Fog(&this->dyna.actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 5);
        Actor_SE_set(&this->dyna.actor, NA_SE_EN_PO_LAUGH2);
        this->actionFunc = mode_emily_picture_wait2;
    }
}

void mode_emily_picture_wait2(BgPoEvent* this, PlayState* play) {
    Vec3f pos;

    if (work_num == 0xF) {
        pos.x = this->dyna.actor.world.pos.x - 5.0f;
        pos.y = rnd_fx(120.0f) + this->dyna.actor.world.pos.y;
        pos.z = rnd_fx(120.0f) + this->dyna.actor.world.pos.z;
        _Effect_SS_Db_ct(play, &pos, &zero_vec, &zero_vec, 170, 0, 200, 255, 100, 170, 0, 255, 0, 1, 9, true);
    } else if (work_num == 0x20) {
        Actor_delete(&this->dyna.actor);
    } else {
        DECR(this->timer);
    }
}

s32 set_po_event_appear_picture(BgPoEvent* this) {
    if ((this->dyna.actor.parent != NULL) && (this->dyna.actor.child != NULL)) {
        if (fqrand() < 0.5f) {
            work_num = ((BgPoEvent*)this->dyna.actor.parent)->index;
        } else {
            work_num = ((BgPoEvent*)this->dyna.actor.child)->index;
        }
    } else if (this->dyna.actor.parent != NULL) {
        work_num = ((BgPoEvent*)this->dyna.actor.parent)->index;
    } else if (this->dyna.actor.child != NULL) {
        work_num = ((BgPoEvent*)this->dyna.actor.child)->index;
    } else {
        return false;
    }
    return true;
}

void mode_no_picture(BgPoEvent* this, PlayState* play) {
    if (work_num == this->index) {
        this->timer = 255;
        this->actionFunc = mode_picture_appear;
    }
}

void mode_picture_appear(BgPoEvent* this, PlayState* play) {
    this->timer -= 20;
    if (this->timer <= 0) {
        this->timer = 1000;
        this->actionFunc = mode_picture_wait;
    }
}

void mode_picture_disappear(BgPoEvent* this, PlayState* play) {
    this->timer += 20;
    if (this->timer >= 255) {
        set_po_event_appear_picture(this);
        this->actionFunc = mode_no_picture;
    }
}

void mode_picture_wait(BgPoEvent* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    Player* player = GET_PLAYER(play);

    DECR(this->timer);

    if (((this->timer == 0) || ((thisx->xzDistToPlayer < 150.0f) && (thisx->yDistToPlayer < 50.0f)) ||
         (player_bow_ready_check(player) && (thisx->xzDistToPlayer < 320.0f) &&
          ((this->index != 2) ? (thisx->yDistToPlayer < 100.0f) : (thisx->yDistToPlayer < 0.0f)) &&
          Actor_player_look_direction_check(thisx, 0x2000, play))) &&
        ((thisx->parent != NULL) || (thisx->child != NULL))) {
        /*The third condition in the || is checking if
            1) Link is holding a ranged weapon
            2) Link is too close in the xz plane
            3) Link is too close in the y direction. The painting
               under the balcony allows him to be closer.
            4) Link is within 45 degrees of facing the painting. */
        this->timer = 0;
        Actor_SE_set(thisx, NA_SE_EN_PO_LAUGH);
        this->actionFunc = mode_picture_disappear;
    } else if (this->collider.base.acFlags & AC_HIT) {
        if (!set_po_event_appear_picture(this)) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, thisx->world.pos.x, thisx->world.pos.y - 40.0f,
                        thisx->world.pos.z, 0, thisx->shape.rot.y, 0, thisx->params + ((this->type - 1) << 8));
            makeOnepointDemo(play, 3160, 80, thisx, CAM_ID_MAIN);
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);

        } else {
            Actor_SE_set(thisx, NA_SE_EN_PO_LAUGH2);
            makeOnepointDemo(play, 3160, 35, thisx, CAM_ID_MAIN);
        }
        if (thisx->parent != NULL) {
            thisx->parent->child = NULL;
            thisx->parent = NULL;
        }
        if (thisx->child != NULL) {
            thisx->child->parent = NULL;
            thisx->child = NULL;
        }
        this->timer = 20;
        this->actionFunc = mode_picture_hit;
    }
}

void mode_picture_hit(BgPoEvent* this, PlayState* play) {
    Vec3f sp54;

    this->timer--;
    sp54.x = (sin_s(this->dyna.actor.shape.rot.y) * 5.0f) + this->dyna.actor.world.pos.x;
    sp54.y = rnd_fx(66.0f) + this->dyna.actor.world.pos.y;
    sp54.z = rnd_fx(50.0f) + this->dyna.actor.world.pos.z;
    if (this->timer >= 0) {
        if (this->type == 2) {
            _Effect_SS_Db_ct(play, &sp54, &zero_vec, &zero_vec, 100, 0, 255, 255, 150, 170, 255, 0, 0, 1, 9, true);
        } else {
            _Effect_SS_Db_ct(play, &sp54, &zero_vec, &zero_vec, 100, 0, 200, 255, 255, 170, 50, 100, 255, 1, 9,
                                 true);
        }
    }
    if (this->timer == 0) {
        this->dyna.actor.draw = NULL;
    }
    if (this->timer < -60) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Po_Event_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    this->actionFunc(this, play);
    if ((this->actionFunc == mode_emily_picture_wait) || (this->actionFunc == mode_picture_wait)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Bg_Po_Event_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* display_model[] = {
        gPoSistersAmyBlockDL,     gPoSistersAmyBethBlockDL, gPoSistersJoellePaintingDL,
        gPoSistersBethPaintingDL, gPoSistersAmyPaintingDL,
    };
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;
    u8 alpha;
    Vec3f sp58;
    Vec3f sp4C;
    f32 sp48;
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_po_event.c", 1481);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    if ((this->type == 3) || (this->type == 2)) {
        if (this->actionFunc == mode_no_picture) {
            alpha = 255;
        } else if (this->actionFunc == mode_picture_wait) {
            alpha = 0;
        } else {
            alpha = this->timer;
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, alpha);
    }
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_po_event.c", 1501);
    gSPDisplayList(POLY_OPA_DISP++, display_model[this->type]);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_po_event.c", 1508);

    if ((this->type == 0) || (this->type == 1)) {
        sp48 = (833.0f - this->dyna.actor.world.pos.y) * 0.0025f;
        if (!(sp48 > 1.0f)) {
            sp58.x = this->dyna.actor.world.pos.x;
            sp58.y = this->dyna.actor.world.pos.y - 30.0f;
            sp58.z = this->dyna.actor.world.pos.z;
            sp4C.y = 1.0f;
            sp4C.x = sp4C.z = (sp48 * 0.3f) + 0.4f;
            Shadow_draw(&sp58, &sp4C, (u8)(155.0f + sp48 * 100.0f), play);
        }
    }
}
