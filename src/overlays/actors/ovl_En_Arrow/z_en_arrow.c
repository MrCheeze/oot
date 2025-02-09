/*
 * File: z_en_arrow.c
 * Overlay: ovl_En_Arrow
 * Description: Arrow, Deku Seed, and Deku Nut Projectile
 */

#include "z_en_arrow.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Arrow_actor_ct(Actor* thisx, PlayState* play);
void En_Arrow_actor_dt(Actor* thisx, PlayState* play);
void En_Arrow_actor_move(Actor* thisx, PlayState* play);
void En_Arrow_actor_draw(Actor* thisx, PlayState* play);

static void move_catch(EnArrow* this, PlayState* play);
static void move_shot(EnArrow* this, PlayState* play);
void move_hit(EnArrow* this, PlayState* play);
void move_flick(EnArrow* this, PlayState* play);

ActorProfile En_Arrow_Profile = {
    /**/ ACTOR_EN_ARROW,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnArrow),
    /**/ En_Arrow_actor_ct,
    /**/ En_Arrow_actor_dt,
    /**/ En_Arrow_actor_move,
    /**/ En_Arrow_actor_draw,
};

static ColliderQuadInit ATArrowInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000020, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(minVelocityY, -150, ICHAIN_STOP),
};

void En_Arrow_actor_set_process(EnArrow* this, EnArrowActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Arrow_actor_ct(Actor* thisx, PlayState* play) {
    EnArrow* this = (EnArrow*)thisx;

    ValueSet_process(&this->actor, value_init);

    if (this->actor.params == ARROW_CS_NUT) {
        this->isCsNut = true;
        this->actor.params = ARROW_NUT;
    }

    if (this->actor.params <= ARROW_SEED) {

        if (this->actor.params <= ARROW_0E) {
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gArrowSkel, &gArrow2Anim, NULL, NULL, 0);
        }

        if (this->actor.params <= ARROW_NORMAL) {
            static EffectBlureInit2 blure_info2 = {
                0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
                0, 1, 0, { 255, 255, 170, 255 }, { 0, 150, 0, 0 },
            };
            if (this->actor.params == ARROW_NORMAL_HORSE) {
                blure_info2.elemDuration = 4;
            } else {
                blure_info2.elemDuration = 16;
            }

            EffectAdd(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blure_info2);

        } else if (this->actor.params == ARROW_FIRE) {
            static EffectBlureInit2 blure_info2 = {
                0, 4, 0, { 0, 255, 200, 255 }, { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
                0, 1, 0, { 255, 200, 0, 255 }, { 255, 0, 0, 0 },
            };

            EffectAdd(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blure_info2);

        } else if (this->actor.params == ARROW_ICE) {
            static EffectBlureInit2 blure_info2 = {
                0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
                0, 1, 0, { 170, 255, 255, 255 }, { 0, 100, 255, 0 },
            };

            EffectAdd(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blure_info2);

        } else if (this->actor.params == ARROW_LIGHT) {
            static EffectBlureInit2 blure_info2 = {
                0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
                0, 1, 0, { 255, 255, 170, 255 }, { 255, 255, 0, 0 },
            };

            EffectAdd(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blure_info2);
        }

        ClObjSwrd_ct(play, &this->collider);
        ClObjSwrd_set5(play, &this->collider, &this->actor, &ATArrowInfoData);

        if (this->actor.params <= ARROW_NORMAL) {
            this->collider.elem.atElemFlags &= ~ATELEM_SFX_MASK;
            this->collider.elem.atElemFlags |= ATELEM_SFX_NORMAL;
        }

        if (this->actor.params < 0) {
            this->collider.base.atFlags = (AT_ON | AT_TYPE_ENEMY);
        } else if (this->actor.params <= ARROW_SEED) {
            static u32 at_bit_table[] = {
                DMG_ARROW_FIRE,  DMG_ARROW_NORMAL, DMG_ARROW_NORMAL, DMG_ARROW_FIRE, DMG_ARROW_ICE,
                DMG_ARROW_LIGHT, DMG_ARROW_UNK3,   DMG_ARROW_UNK1,   DMG_ARROW_UNK2, DMG_SLINGSHOT,
            };
            this->collider.elem.atDmgInfo.dmgFlags = at_bit_table[this->actor.params];
            LOG_HEX("this->at_info.cl_elem.at_btl_info.at_type", this->collider.elem.atDmgInfo.dmgFlags,
                    "../z_en_arrow.c", 707);
        }
    }

    En_Arrow_actor_set_process(this, move_catch);
}

void En_Arrow_actor_dt(Actor* thisx, PlayState* play) {
    EnArrow* this = (EnArrow*)thisx;

    if (this->actor.params <= ARROW_LIGHT) {
        EffectFreeIndex(play, this->effectIndex);
    }

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjSwrd_dt(play, &this->collider);

    if ((this->hitActor != NULL) && (this->hitActor->update != NULL)) {
        this->hitActor->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
    }
}

static void move_catch(EnArrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.parent == NULL) {
        if ((this->actor.params != ARROW_NUT) && (player->unk_A73 == 0)) {
            Actor_delete(&this->actor);
            return;
        }

        switch (this->actor.params) {
            case ARROW_SEED:
                player_SE_set(player, NA_SE_IT_SLING_SHOT);
                break;

            case ARROW_NORMAL_LIT:
            case ARROW_NORMAL_HORSE:
            case ARROW_NORMAL:
                player_SE_set(player, NA_SE_IT_ARROW_SHOT);
                break;

            case ARROW_FIRE:
            case ARROW_ICE:
            case ARROW_LIGHT:
                player_SE_set(player, NA_SE_IT_MAGIC_ARROW_SHOT);
                break;
        }

        En_Arrow_actor_set_process(this, move_shot);
        xyz_t_move(&this->unk_210, &this->actor.world.pos);

        if (this->actor.params >= ARROW_SEED) {
            Actor_vector_to_position_speed(&this->actor, 80.0f);
            this->timer = 15;
            this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
        } else {
            Actor_vector_to_position_speed(&this->actor, 150.0f);
            this->timer = 12;
        }
    }
}

void rebound_set(PlayState* play, EnArrow* this) {
    En_Arrow_actor_set_process(this, move_flick);
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gArrow1Anim);
    this->actor.world.rot.y += (s32)(24576.0f * (fqrand() - 0.5f)) + 0x8000;
    this->actor.velocity.y += (this->actor.speed * (0.4f + (0.4f * fqrand())));
    this->actor.speed *= (0.04f + 0.3f * fqrand());
    this->timer = 50;
    this->actor.gravity = -1.5f;
}

void set_carry_actor_pos(EnArrow* this, PlayState* play) {
    CollisionPoly* hitPoly;
    Vec3f posDiffLastFrame;
    Vec3f actorNextPos;
    Vec3f hitPos;
    f32 temp_f12;
    f32 scale;
    s32 bgId;

    xyz_t_sub(&this->actor.world.pos, &this->unk_210, &posDiffLastFrame);

    temp_f12 = ((this->actor.world.pos.x - this->hitActor->world.pos.x) * posDiffLastFrame.x) +
               ((this->actor.world.pos.y - this->hitActor->world.pos.y) * posDiffLastFrame.y) +
               ((this->actor.world.pos.z - this->hitActor->world.pos.z) * posDiffLastFrame.z);

    if (!(temp_f12 < 0.0f)) {
        scale = Math3DVecLengthSquare(&posDiffLastFrame);

        if (!(scale < 1.0f)) {
            scale = temp_f12 / scale;
            xyz_t_mult_v(&posDiffLastFrame, scale);
            xyz_t_add(&this->hitActor->world.pos, &posDiffLastFrame, &actorNextPos);

            if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->hitActor->world.pos, &actorNextPos, &hitPos, &hitPoly,
                                        true, true, true, true, &bgId)) {
                this->hitActor->world.pos.x = hitPos.x + ((actorNextPos.x <= hitPos.x) ? 1.0f : -1.0f);
                this->hitActor->world.pos.y = hitPos.y + ((actorNextPos.y <= hitPos.y) ? 1.0f : -1.0f);
                this->hitActor->world.pos.z = hitPos.z + ((actorNextPos.z <= hitPos.z) ? 1.0f : -1.0f);
            } else {
                xyz_t_move(&this->hitActor->world.pos, &actorNextPos);
            }
        }
    }
}

static void move_shot(EnArrow* this, PlayState* play) {
    CollisionPoly* hitPoly;
    s32 bgId;
    Vec3f hitPoint;
    Vec3f posCopy;
    s32 atTouched;
    u16 sfxId;
    Actor* hitActor;

    if (DECR(this->timer) == 0) {
        Actor_delete(&this->actor);
        return;
    }

    if (this->timer < 7.2000003f) {
        this->actor.gravity = -0.4f;
    }

    atTouched = (this->actor.params != ARROW_NORMAL_LIT) && (this->actor.params <= ARROW_SEED) &&
                (this->collider.base.atFlags & AT_HIT);

    if (atTouched || this->touchedPoly) {
        if (this->actor.params >= ARROW_SEED) {
            if (atTouched) {
                this->actor.world.pos.x = (this->actor.world.pos.x + this->actor.prevPos.x) * 0.5f;
                this->actor.world.pos.y = (this->actor.world.pos.y + this->actor.prevPos.y) * 0.5f;
                this->actor.world.pos.z = (this->actor.world.pos.z + this->actor.prevPos.z) * 0.5f;
            }

            if (this->actor.params == ARROW_NUT) {
                R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_M_FIRE1, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 0);
                sfxId = NA_SE_IT_DEKU;
            } else {
                sfxId = NA_SE_IT_SLING_REFLECT;
            }

            Effect_SS_Stone1_ct(play, &this->actor.world.pos, 0);
            Effect_SE_Info_new(play, &this->actor.world.pos, 20, sfxId);
            Actor_delete(&this->actor);
        } else {
            Effect_SS_HitMark_scl_ct(play, 0, 150, &this->actor.world.pos);

            if (atTouched && (this->collider.elem.atHitElem->elemMaterial != ELEM_MATERIAL_UNK4)) {
                hitActor = this->collider.base.at;

                if ((hitActor->update != NULL) && !(this->collider.base.atFlags & AT_BOUNCED) &&
                    (hitActor->flags & ACTOR_FLAG_CAN_ATTACH_TO_ARROW)) {
                    this->hitActor = hitActor;
                    set_carry_actor_pos(this, play);
                    xyz_t_sub(&hitActor->world.pos, &this->actor.world.pos, &this->unk_250);
                    hitActor->flags |= ACTOR_FLAG_ATTACHED_TO_ARROW;
                    this->collider.base.atFlags &= ~AT_HIT;
                    this->actor.speed /= 2.0f;
                    this->actor.velocity.y /= 2.0f;
                } else {
                    this->hitFlags |= 1;
                    this->hitFlags |= 2;

                    if (this->collider.elem.atHitElem->acElemFlags & ACELEM_HIT) {
                        this->actor.world.pos.x = this->collider.elem.atHitElem->acDmgInfo.hitPos.x;
                        this->actor.world.pos.y = this->collider.elem.atHitElem->acDmgInfo.hitPos.y;
                        this->actor.world.pos.z = this->collider.elem.atHitElem->acDmgInfo.hitPos.z;
                    }

                    rebound_set(play, this);
                    Actor_SE_set(&this->actor, NA_SE_IT_ARROW_STICK_CRE);
                }
            } else if (this->touchedPoly) {
                En_Arrow_actor_set_process(this, move_hit);
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gArrow2Anim);

                if (this->actor.params >= ARROW_NORMAL_LIT) {
                    this->timer = 60;
                } else {
                    this->timer = 20;
                }

                Actor_SE_set(&this->actor, NA_SE_IT_ARROW_STICK_OBJ);
                this->hitFlags |= 1;
            }
        }
    } else {
        xyz_t_move(&this->unk_210, &this->actor.world.pos);
        Actor_position_moveF(&this->actor);

        if ((this->touchedPoly =
                 T_BGCheck_ArrowLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &hitPoint,
                                            &this->actor.wallPoly, true, true, true, true, &bgId))) {
            jyabujyabu_kiru_check(play, &this->actor, this->actor.wallPoly, bgId, &hitPoint);
            xyz_t_move(&posCopy, &this->actor.world.pos);
            xyz_t_move(&this->actor.world.pos, &hitPoint);
        }

        if (this->actor.params <= ARROW_0E) {
            this->actor.shape.rot.x = atans_table(this->actor.speed, -this->actor.velocity.y);
        }
    }

    if (this->hitActor != NULL) {
        if (this->hitActor->update != NULL) {
            Vec3f sp60;
            Vec3f sp54;

            xyz_t_add(&this->unk_210, &this->unk_250, &sp60);
            xyz_t_add(&this->actor.world.pos, &this->unk_250, &sp54);

            if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp60, &sp54, &hitPoint, &hitPoly, true, true, true, true,
                                        &bgId)) {
                this->hitActor->world.pos.x = hitPoint.x + ((sp54.x <= hitPoint.x) ? 1.0f : -1.0f);
                this->hitActor->world.pos.y = hitPoint.y + ((sp54.y <= hitPoint.y) ? 1.0f : -1.0f);
                this->hitActor->world.pos.z = hitPoint.z + ((sp54.z <= hitPoint.z) ? 1.0f : -1.0f);
                xyz_t_sub(&this->hitActor->world.pos, &this->actor.world.pos, &this->unk_250);
                this->hitActor->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
                this->hitActor = NULL;
            } else {
                xyz_t_add(&this->actor.world.pos, &this->unk_250, &this->hitActor->world.pos);
            }

            if (this->touchedPoly && (this->hitActor != NULL)) {
                this->hitActor->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
                this->hitActor = NULL;
            }
        } else {
            this->hitActor = NULL;
        }
    }
}

void move_hit(EnArrow* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (DECR(this->timer) == 0) {
        Actor_delete(&this->actor);
    }
}

void move_flick(EnArrow* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_position_moveF(&this->actor);

    if (DECR(this->timer) == 0) {
        Actor_delete(&this->actor);
    }
}

void En_Arrow_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnArrow* this = (EnArrow*)thisx;
    Player* player = GET_PLAYER(play);

    if (this->isCsNut || ((this->actor.params >= ARROW_NORMAL_LIT) && (player->unk_A73 != 0)) ||
        !player_action_check(play, player)) {
        this->actionFunc(this, play);
    }

    if ((this->actor.params >= ARROW_FIRE) && (this->actor.params <= ARROW_0E)) {
        s16 elementalActorIds[] = { ACTOR_ARROW_FIRE, ACTOR_ARROW_ICE,  ACTOR_ARROW_LIGHT,
                                    ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE };

        if (this->actor.child == NULL) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, elementalActorIds[this->actor.params - 3],
                               this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
        }
    } else if (this->actor.params == ARROW_NORMAL_LIT) {
        static Vec3f fire_vec = { 0.0f, 0.5f, 0.0f };
        static Vec3f fire_acc = { 0.0f, 0.5f, 0.0f };
        static Color_RGBA8 fire_prim = { 255, 255, 100, 255 };
        static Color_RGBA8 fire_env = { 255, 50, 0, 0 };
        // spawn dust for the flame
        Effect_SS_Dust_sc_cl_co_ct(play, &this->unk_21C, &fire_vec, &fire_acc, &fire_prim, &fire_env, 100, 0, 8);
    }
}

void check_pos_calc(EnArrow* this, PlayState* play) {
    static Vec3f local_sword_top = { 0.0f, 400.0f, 1500.0f };
    static Vec3f local_sword_root = { 0.0f, -400.0f, 1500.0f };
    static Vec3f local_head = { 0.0f, 0.0f, -300.0f };
    Vec3f sp44;
    Vec3f sp38;
    s32 addBlureVertex;

    Matrix_Position(&local_head, &this->unk_21C);

    if (move_shot == this->actionFunc) {
        Matrix_Position(&local_sword_top, &sp44);
        Matrix_Position(&local_sword_root, &sp38);

        if (this->actor.params <= ARROW_SEED) {
            addBlureVertex = this->actor.params <= ARROW_LIGHT;

            if (this->hitActor == NULL) {
                addBlureVertex &= sword_attack_collision_set(play, &this->collider, &this->weaponInfo, &sp44, &sp38);
            } else {
                if (addBlureVertex) {
                    if ((sp44.x == this->weaponInfo.tip.x) && (sp44.y == this->weaponInfo.tip.y) &&
                        (sp44.z == this->weaponInfo.tip.z) && (sp38.x == this->weaponInfo.base.x) &&
                        (sp38.y == this->weaponInfo.base.y) && (sp38.z == this->weaponInfo.base.z)) {
                        addBlureVertex = false;
                    }
                }
            }

            if (addBlureVertex) {
                EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->effectIndex), &sp44, &sp38);
            }
        }
    }
}

void En_Arrow_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnArrow* this = (EnArrow*)thisx;
    u8 alpha;
    f32 scale;

    if (this->actor.params <= ARROW_0E) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Si2_Lod_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, this,
                          (this->actor.projectedPos.z < MREG(95)) ? 0 : 1);
    } else if (this->actor.speed != 0.0f) {
        alpha = (cos_s(this->timer * 5000) * 127.5f) + 127.5f;

        OPEN_DISPS(play->state.gfxCtx, "../z_en_arrow.c", 1346);

        texture_z_light_prim_xlu_disp(play->state.gfxCtx);

        if (this->actor.params == ARROW_SEED) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, alpha);
            scale = 50.0f;
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 12, 0, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 0, alpha);
            scale = 150.0f;
        }

        Matrix_push();
        Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
        // redundant check because this is contained in an if block for non-zero speed
        Matrix_rotateZ((this->actor.speed == 0.0f) ? 0.0f : BINANG_TO_RAD((play->gameplayFrames & 0xFF) * 4000),
                       MTXMODE_APPLY);
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_arrow.c", 1374);
        gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
        Matrix_pull();
        Matrix_rotateY(BINANG_TO_RAD(this->actor.world.rot.y), MTXMODE_APPLY);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_arrow.c", 1381);
    }

    check_pos_calc(this, play);
}
