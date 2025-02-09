/*
 * File: z_en_bb.c
 * Overlay: ovl_En_Bb
 * Description: Bubble (Flying Skull Enemy)
 */

#include "z_en_bb.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_Bb/object_Bb.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT)

#define vBombHopPhase actionVar1
#define vTrailIdx actionVar1
#define vTrailMaxAlpha actionVar2
#define vMoveAngleY actionVar2
#define vFlameTimer actionVar2

typedef enum EnBbAction {
    /* 0 */ BB_DAMAGE,
    /* 1 */ BB_KILL,
    /* 2 */ BB_FLAME_TRAIL,
    /* 3 */ BB_DOWN,
    /* 4 */ BB_STUNNED,
    /* 5 */ BB_UNUSED,
    /* 6 */ BB_BLUE,
    /* 7 */ BB_RED,
    /* 8 */ BB_WHITE,
    /* 9 */ BB_GREEN
} EnBbAction;

typedef enum EnBbMoveMode {
    /* 0 */ BBMOVE_NORMAL,
    /* 1 */ BBMOVE_NOCLIP,
    /* 2 */ BBMOVE_HIDDEN
} EnBbMoveMode;

typedef enum EnBbBlueActionState {
    /* 0 */ BBBLUE_NORMAL,
    /* 1 */ BBBLUE_AGGRO
} EnBbBlueActionState;

typedef enum EnBbRedActionState {
    /* 0 */ BBRED_WAIT,
    /* 1 */ BBRED_ATTACK,
    /* 2 */ BBRED_HIDE
} EnBbRedActionState;

typedef enum EnBbGreenActionState {
    /* 0 */ BBGREEN_FLAME_ON,
    /* 1 */ BBGREEN_FLAME_OFF
} EnBbGreenActionState;

// Main functions

void En_Bb_Actor_ct(Actor* thisx, PlayState* play);
void En_Bb_Actor_dt(Actor* thisx, PlayState* play);
void En_Bb_move(Actor* thisx, PlayState* play2);
void En_Bb_display(Actor* thisx, PlayState* play);

// Helper functions

static void set_direct(EnBb* this);
static void set_nextPathIndex(EnBb* this, PlayState* play);

// Action functions

void En_Bb_Actor_mode_wait_init(EnBb* this);
void En_Bb_Actor_mode_wait(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_down_init(EnBb* this, PlayState* play);
void En_Bb_Actor_mode_down(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_damage(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_rnd_move_init(EnBb* this);
void En_Bb_Actor_mode_rnd_move(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_bound_init(EnBb* this);
void En_Bb_Actor_mode_bound(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_teil_init(PlayState* play, EnBb* this);
void En_Bb_Actor_mode_teil(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_br_move_init(PlayState* play, EnBb* this);
void En_Bb_Actor_mode_br_move(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_roll_move_init(EnBb* this, PlayState* play);
void En_Bb_Actor_mode_roll_move(EnBb* this, PlayState* play);

void En_Bb_Actor_mode_paralyze(EnBb* this, PlayState* play);

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0xC),
    /* Light arrow   */ DMG_ENTRY(4, 0xB),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(3, 0x8),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable btl_data2 = {
    /* Deku nut      */ DMG_ENTRY(0, 0xD),
    /* Deku stick    */ DMG_ENTRY(0, 0xD),
    /* Slingshot     */ DMG_ENTRY(0, 0xD),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xD),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xD),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0x9),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable btl_data3 = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0xE),
    /* Slingshot     */ DMG_ENTRY(1, 0xE),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(4, 0x5),
    /* Ice arrow     */ DMG_ENTRY(2, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(4, 0x7),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

ActorProfile En_Bb_Profile = {
    /**/ ACTOR_EN_BB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BB,
    /**/ sizeof(EnBb),
    /**/ En_Bb_Actor_ct,
    /**/ En_Bb_Actor_dt,
    /**/ En_Bb_move,
    /**/ En_Bb_display,
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, -120, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 10, ICHAIN_STOP),
};

void En_Bb_actor_set_process(EnBb* this, EnBbActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

Actor* BombSearch2(PlayState* play, EnBb* this, f32 range) {
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dist;

    while (explosive != NULL) {
        if (explosive->params != 0) {
            explosive = explosive->next;
            continue;
        }
        dist = Actor_search_actor_distance(&this->actor, explosive);
        if ((explosive->params == 0) && (dist <= range)) {
            return explosive;
        }
        explosive = explosive->next;
    }
    return NULL;
}

void teil_make(PlayState* play, EnBb* this, s16 startAtZero) {
    EnBb* now = this;
    EnBb* next;
    s32 i;

    for (i = 0; i < 5; i++) {
        next = (EnBb*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BB, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 0, 0, 0, 0);
        if (next != NULL) {
            now->actor.child = &next->actor;
            next->actor.parent = &now->actor;
            next->targetActor = &this->actor;
            next->vTrailIdx = i + 1;
            next->actor.scale.x = 1.0f;
            next->vTrailMaxAlpha = next->flamePrimAlpha = 255 - (i * 40);
            next->flameScaleY = next->actor.scale.y = 0.8f - (i * 0.075f);
            next->flameScaleX = next->actor.scale.z = 1.0f - (i * 0.094f);
            if (startAtZero) {
                next->flamePrimAlpha = 0;
                next->flameScaleY = next->flameScaleX = 0.0f;
            }
            next->flameScrollMod = i + 1;
            next->timer = 2 * i + 2;
            next->flameEnvColor.r = 255;
            now = next;
        }
    }
}

void teil_del(EnBb* this) {
    Actor* actor = &this->actor;
    Actor* nextActor;

    while (actor->child != NULL) {
        nextActor = actor->child;

        if (nextActor->id == ACTOR_EN_BB) {
            nextActor->parent = NULL;
            actor->child = NULL;
            nextActor->params = ENBB_KILL_TRAIL;
        }
        actor = nextActor;
    }
    this->actor.child = NULL;
}

void En_Bb_Actor_ct(Actor* thisx, PlayState* play) {
    EffectBlureInit1 blureInit;
    s32 pad;
    EnBb* this = (EnBb*)thisx;

    ValueSet_process(thisx, value_init);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_Bb_Skel_001A30, &object_Bb_Anim_000444, this->jointTable,
                   this->morphTable, 16);
    this->unk_254 = 0;
    thisx->colChkInfo.health = 4;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, thisx, &JntSphData, this->elements);

    this->actionState = PARAMS_GET_NOMASK(thisx->params, 8);

    if (PARAMS_GET_NOSHIFT(thisx->params, 7, 1)) {
        thisx->params |= 0xFF00;
    }
    if (thisx->params <= ENBB_BLUE) {
        Shape_Info_init(&thisx->shape, 200.0f, Actor_shadow_circle, 35.0f);
    }
    if (PARAMS_GET_NOSHIFT(thisx->params, 8, 8)) {
        this->timer = 0;
        this->flameScaleY = 80.0f;
        this->flameScaleX = 100.0f;
        this->collider.elements[0].base.atElemFlags = ATELEM_ON | ATELEM_SFX_HARD;
        this->collider.elements[0].base.atDmgInfo.dmgFlags = DMG_DEFAULT;
        this->collider.elements[0].base.atDmgInfo.damage = 8;
        this->bobSize = this->actionState * 20.0f;
        this->flamePrimAlpha = 255;
        this->moveMode = BBMOVE_NORMAL;
        Actor_set_scale(thisx, 0.01f);
        switch (thisx->params) {
            case ENBB_BLUE:
                thisx->naviEnemyId = NAVI_ENEMY_BLUE_BUBBLE;
                thisx->colChkInfo.damageTable = &btl_data;
                this->flamePrimBlue = this->flameEnvColor.b = 255;
                thisx->world.pos.y += 50.0f;
                En_Bb_Actor_mode_rnd_move_init(this);
                thisx->flags |= ACTOR_FLAG_CAN_ATTACH_TO_ARROW;
                break;
            case ENBB_RED:
                thisx->naviEnemyId = NAVI_ENEMY_RED_BUBBLE;
                thisx->colChkInfo.damageTable = &btl_data2;
                this->flameEnvColor.r = 255;
                this->collider.elements[0].base.atDmgInfo.effect = 1;
                En_Bb_Actor_mode_teil_init(play, this);
                break;
            case ENBB_WHITE:
                thisx->naviEnemyId = NAVI_ENEMY_WHITE_BUBBLE;
                thisx->colChkInfo.damageTable = &btl_data3;
                this->path = this->actionState;
                blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] =
                    blureInit.p1StartColor[3] = blureInit.p2StartColor[0] = blureInit.p2StartColor[1] =
                        blureInit.p2StartColor[2] = blureInit.p2StartColor[3] = blureInit.p1EndColor[0] =
                            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] =
                                blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;

                blureInit.p1EndColor[3] = 0;
                blureInit.p2EndColor[3] = 0;
                blureInit.elemDuration = 16;
                blureInit.unkFlag = 0;
                blureInit.calcMode = 2;

                EffectAdd(play, &this->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
                En_Bb_Actor_mode_br_move_init(play, this);
                set_nextPathIndex(this, play);
                set_direct(this);
                thisx->flags |= ACTOR_FLAG_CAN_ATTACH_TO_ARROW;
                break;
            case ENBB_GREEN_BIG:
                this->path = this->actionState >> 4;
                this->collider.elements[0].dim.modelSphere.radius = 0x16;
                Actor_set_scale(thisx, 0.03f);
                FALLTHROUGH;
            case ENBB_GREEN:
                thisx->naviEnemyId = NAVI_ENEMY_GREEN_BUBBLE;
                this->bobSize = (this->actionState & 0xF) * 20.0f;
                thisx->colChkInfo.damageTable = &btl_data;
                this->flameEnvColor.g = 255;
                thisx->colChkInfo.health = 1;

                En_Bb_Actor_mode_roll_move_init(this, play);
                break;
        }
        thisx->focus.pos = thisx->world.pos;
    } else {
        En_Bb_Actor_mode_wait_init(this);
    }
    this->collider.elements[0].dim.worldSphere.radius =
        this->collider.elements[0].dim.modelSphere.radius * this->collider.elements[0].dim.scale;
}

void En_Bb_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void En_Bb_Actor_mode_wait_init(EnBb* this) {
    this->action = BB_FLAME_TRAIL;
    this->moveMode = BBMOVE_NOCLIP;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_wait);
}

void En_Bb_Actor_mode_wait(EnBb* this, PlayState* play) {
    if (this->actor.params == ENBB_KILL_TRAIL) {
        if (this->actor.parent == NULL) {
            En_Bb_Actor_mode_down_init(this, play);
        }
    } else {
        if (this->timer == 0) {
            if (((EnBb*)this->targetActor)->flameScaleY != 0.0f) {
                add_calc(&this->flameScaleY, this->actor.scale.y, 1.0f, this->actor.scale.y * 0.1f, 0.0f);
                add_calc(&this->flameScaleX, this->actor.scale.z, 1.0f, this->actor.scale.z * 0.1f, 0.0f);
                if (this->flamePrimAlpha != this->vTrailMaxAlpha) {
                    this->flamePrimAlpha += 10;
                    if (this->vTrailMaxAlpha < this->flamePrimAlpha) {
                        this->flamePrimAlpha = this->vTrailMaxAlpha;
                    }
                }
            } else {
                if (!this->flamePrimAlpha) {
                    Actor_delete(&this->actor);
                    return;
                } else if (this->flamePrimAlpha <= 20) {
                    this->flamePrimAlpha = 0;
                } else {
                    this->flamePrimAlpha -= 20;
                }
            }
            this->actor.world.pos = this->actor.parent->prevPos;
        } else {
            this->timer--;
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.parent->world.rot.y;
        }
    }
    if (this->actor.parent != NULL) {
        this->actor.velocity.y = this->actor.parent->velocity.y;
    }
}

void En_Bb_Actor_mode_down_init(EnBb* this, PlayState* play) {
    if (this->actor.params <= ENBB_BLUE) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.speed = -7.0f;
        this->timer = 5;
        this->actor.shape.rot.x += 0x4E20;
        Effect_sound_ct(play, &this->actor.projectedPos, NA_SE_EN_BUBLE_DEAD, 1, 1, 0x28);
    }
    this->action = BB_KILL;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_down);
}

void En_Bb_Actor_mode_down(EnBb* this, PlayState* play) {
    s16 enpartType = 3;
    Vec3f sp40 = { 0.0f, 0.5f, 0.0f };
    Vec3f sp34 = { 0.0f, 0.0f, 0.0f };

    if (this->actor.params <= ENBB_BLUE) {
        add_calc(&this->flameScaleY, 0.0f, 1.0f, 30.0f, 0.0f);
        add_calc(&this->flameScaleX, 0.0f, 1.0f, 30.0f, 0.0f);
        if (this->timer != 0) {
            this->timer--;
            this->actor.shape.rot.x -= 0x4E20;
            return;
        }

        if (this->bodyBreak.val == BODYBREAK_STATUS_FINISHED) {
            Part_Break_init(&this->bodyBreak, 12, play);
        }

        if ((this->dmgEffect == 7) || (this->dmgEffect == 5)) {
            enpartType = 11;
        }

        if (!Part_break(&this->actor, &this->bodyBreak, play, enpartType)) {
            return;
        }
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xD0);
    } else {
        if (this->flamePrimAlpha) {
            if (this->flamePrimAlpha <= 20) {
                this->flamePrimAlpha = 0;
            } else {
                this->flamePrimAlpha -= 20;
            }
            return;
        }
    }
    Actor_delete(&this->actor);
}

void En_Bb_Actor_mode_damage_init(EnBb* this) {
    this->action = BB_DAMAGE;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
    if (this->actor.params > ENBB_GREEN) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            this->actor.speed = -7.0f;
        }
        this->actor.shape.yOffset = 1500.0f;
    }
    if (this->actor.params == ENBB_RED) {
        teil_del(this);
    }
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
    this->timer = 5;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_damage);
}

void En_Bb_Actor_mode_damage(EnBb* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    if (this->actor.speed == 0.0f) {
        this->actor.shape.yOffset = 200.0f;
        En_Bb_Actor_mode_bound_init(this);
    }
}

void En_Bb_Actor_mode_rnd_move_init(EnBb* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
    this->actor.speed = (fqrand() * 0.5f) + 0.5f;
    this->timer = (fqrand() * 20.0f) + 40.0f;
    this->unk_264 = (fqrand() * 30.0f) + 180.0f;
    this->targetActor = NULL;
    this->action = BB_BLUE;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_rnd_move);
}

void En_Bb_Actor_mode_rnd_move(EnBb* this, PlayState* play) {
    Actor* explosive;
    s16 moveYawToWall;
    s16 thisYawToWall;
    s16 afterHitAngle;

    add_calc(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
    add_calc(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
        add_calc(&this->actor.world.pos.y, this->actor.floorHeight + 50.0f + this->flyHeightMod, 1.0f, 0.5f,
                           0.0f);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (cosf_table(this->bobPhase) == 0.0f) {
        if (this->charge) {
            this->bobSpeedMod = fqrand() * 2.0f;
        } else {
            this->bobSpeedMod = fqrand() * 4.0f;
        }
    }
    this->actor.world.pos.y += cosf_table(this->bobPhase) * (1.0f + this->bobSpeedMod);
    this->bobPhase += 0.2f;
    add_calc(&this->actor.speed, this->maxSpeedXZ, 1.0f, 0.5f, 0.0f);

    if (search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) > 300.0f) {
        this->vMoveAngleY = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        add_calc_short_angle2(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x7D0, 0);
    } else {
        this->timer--;
        if (this->timer <= 0) {
            this->charge ^= true;
            this->flyHeightMod = (s16)(cosf_table(this->bobPhase) * 10.0f);
            this->actor.speed = 0.0f;
            if (this->charge && (this->targetActor == NULL)) {
                this->vMoveAngleY = this->actor.world.rot.y;
                if (this->actor.xzDistToPlayer < 200.0f) {
                    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000184);
                    this->vMoveAngleY = this->actor.yawTowardsPlayer;
                }
                this->maxSpeedXZ = (fqrand() * 1.5f) + 6.0f;
                this->timer = (fqrand() * 5.0f) + 20.0f;
                this->actionState = BBBLUE_NORMAL;
            } else {
                Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
                this->maxSpeedXZ = (fqrand() * 1.5f) + 1.0f;
                this->timer = (fqrand() * 20.0f) + 40.0f;
                this->vMoveAngleY = sinf_table(this->bobPhase) * 65535.0f;
            }
        }
        if ((this->actor.xzDistToPlayer < 150.0f) && (this->actionState != BBBLUE_NORMAL)) {
            if (!this->charge) {
                Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000184);
                this->maxSpeedXZ = (fqrand() * 1.5f) + 6.0f;
                this->timer = (fqrand() * 5.0f) + 20.0f;
                this->vMoveAngleY = this->actor.yawTowardsPlayer;
                this->actionState = this->charge = true; // Sets actionState to BBBLUE_AGGRO
            }
        } else if (this->actor.xzDistToPlayer < 200.0f) {
            this->vMoveAngleY = this->actor.yawTowardsPlayer;
        }
        if (this->targetActor == NULL) {
            explosive = BombSearch2(play, this, 300.0f);
        } else if (this->targetActor->params == 0) {
            explosive = this->targetActor;
        } else {
            explosive = NULL;
        }
        if (explosive != NULL) {
            this->vMoveAngleY = Actor_search_actor_angleY(&this->actor, explosive);
            if ((this->vBombHopPhase == 0) && (explosive != this->targetActor)) {
                this->vBombHopPhase = -0x8000;
                this->targetActor = explosive;
                this->actor.speed *= 0.5f;
            }
            add_calc_short_angle2(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x1388, 0);
            add_calc(&this->actor.world.pos.x, explosive->world.pos.x, 1.0f, 1.5f, 0.0f);
            add_calc(&this->actor.world.pos.y, explosive->world.pos.y + 40.0f, 1.0f, 1.5f, 0.0f);
            add_calc(&this->actor.world.pos.z, explosive->world.pos.z, 1.0f, 1.5f, 0.0f);
        } else {
            this->targetActor = NULL;
        }
        if (this->vBombHopPhase != 0) {
            this->actor.world.pos.y += -cos_s(this->vBombHopPhase) * 10.0f;
            this->vBombHopPhase += 0x1000;
            add_calc_short_angle2(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x7D0, 0);
        }
        thisYawToWall = this->actor.wallYaw - this->actor.world.rot.y;
        moveYawToWall = this->actor.wallYaw - this->vMoveAngleY;
        if ((this->targetActor == NULL) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
            (ABS(thisYawToWall) > 0x4000 || ABS(moveYawToWall) > 0x4000)) {
            this->vMoveAngleY = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
            add_calc_short_angle2(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0xBB8, 0);
        }
    }
    add_calc_short_angle2(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x3E8, 0);
    if ((this->collider.base.acFlags & AC_HIT) || (this->collider.base.atFlags & AT_HIT)) {
        this->vMoveAngleY = this->actor.yawTowardsPlayer + 0x8000;
        if (this->collider.base.acFlags & AC_HIT) {
            afterHitAngle = -0x8000;
        } else {
            afterHitAngle = 0x4000;
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_BITE);
            if (play->gameplayFrames & 1) {
                afterHitAngle = -0x4000;
            }
        }
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + afterHitAngle;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.atFlags &= ~AT_HIT;
    }

    if (this->maxSpeedXZ >= 6.0f) {
        if ((s32)this->skelAnime.curFrame == 0 || (s32)this->skelAnime.curFrame == 5) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_MOUTH);
        } else if ((s32)this->skelAnime.curFrame == 2 || (s32)this->skelAnime.curFrame == 7) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_WING);
        }
    } else {
        if ((s32)this->skelAnime.curFrame == 5) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_WING);
        }
    }
    if (((s32)this->skelAnime.curFrame == 0) && (fqrand() < 0.1f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Bb_Actor_mode_bound_init(EnBb* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
    this->action = BB_DOWN;
    this->timer = 200;
    this->actor.colorFilterTimer = 0;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.speed = 3.0f;
    this->flameScaleX = 0.0f;
    this->flameScaleY = 0.0f;
    this->actor.gravity = -2.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_DOWN);
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_bound);
}

void En_Bb_Actor_mode_bound(EnBb* this, PlayState* play) {
    s16 yawDiff = this->actor.world.rot.y - this->actor.wallYaw;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        if (ABS(yawDiff) > 0x4000) {
            this->actor.world.rot.y = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
        }
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }
    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        if (this->actor.params == ENBB_RED) {
            s32 floorType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

            if ((floorType == FLOOR_TYPE_2) || (floorType == FLOOR_TYPE_3) || (floorType == FLOOR_TYPE_9)) {
                this->moveMode = BBMOVE_HIDDEN;
                this->timer = 10;
                this->actionState++;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->action = BB_RED;
                En_Bb_actor_set_process(this, En_Bb_Actor_mode_teil);
                return;
            }
        }
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.7f;
        } else {
            this->actor.velocity.y = 10.0f;
        }
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
        add_calc_short_angle2(&this->actor.world.rot.y, -this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((s32)this->skelAnime.curFrame == 5) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_WING);
    }
    if (this->timer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_UP);
        switch (this->actor.params) {
            case ENBB_BLUE:
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                En_Bb_Actor_mode_rnd_move_init(this);
                break;
            case ENBB_RED:
                if (this->actor.velocity.y == 10.0f) {
                    En_Bb_Actor_mode_teil_init(play, this);
                    teil_make(play, this, true);
                }
                break;
            case ENBB_WHITE:
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                En_Bb_Actor_mode_br_move_init(play, this);
                this->actor.world.pos.y -= 60.0f;
                break;
        }
    } else {
        this->timer--;
    }
}

void En_Bb_Actor_mode_teil_init(PlayState* play, EnBb* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000184);
    if (this->action == BB_DOWN) {
        this->actor.speed = 5.0f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 16.0f;
        this->actionState = BBRED_ATTACK;
        this->timer = 0;
        this->moveMode = BBMOVE_NORMAL;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    } else {
        this->actor.colChkInfo.health = 4;
        this->timer = 0;
        this->actionState = BBRED_WAIT;
        this->moveMode = BBMOVE_HIDDEN;
        this->actor.world.pos.y -= 80.0f;
        this->actor.home.pos = this->actor.world.pos;
        this->actor.velocity.y = this->actor.gravity = this->actor.speed = 0.0f;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->action = BB_RED;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_teil);
}

void En_Bb_Actor_mode_teil(EnBb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 floorType;
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    switch (this->actionState) {
        case BBRED_WAIT:
            if ((Actor_search_actor_distance(&this->actor, &player->actor) <= 250.0f) && (ABS(yawDiff) <= 0x4000) &&
                (this->timer == 0)) {
                this->actor.speed = 5.0f;
                this->actor.gravity = -1.0f;
                this->actor.velocity.y = 18.0f;
                this->moveMode = BBMOVE_NOCLIP;
                this->timer = 7;
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                this->actionState++;
                teil_make(play, this, false);
            }
            break;
        case BBRED_ATTACK:
            if (this->timer == 0) {
                this->moveMode = BBMOVE_NORMAL;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
            this->bobPhase += fqrand();
            add_calc(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
            add_calc(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                yawDiff = this->actor.world.rot.y - this->actor.wallYaw;
                if (ABS(yawDiff) > 0x4000) {
                    this->actor.world.rot.y =
                        this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
                }
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
            }
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                floorType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
                if ((floorType == FLOOR_TYPE_2) || (floorType == FLOOR_TYPE_3) || (floorType == FLOOR_TYPE_9)) {
                    this->moveMode = BBMOVE_HIDDEN;
                    this->timer = 10;
                    this->actionState++;
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                } else {
                    this->actor.velocity.y *= -1.06f;
                    if (this->actor.velocity.y > 13.0f) {
                        this->actor.velocity.y = 13.0f;
                    }
                    this->actor.world.rot.y = sinf_table(this->bobPhase) * 65535.0f;
                }
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            }
            this->actor.shape.rot.y = this->actor.world.rot.y;
            if (BlastVsMyCheck(play, &this->collider.base) != NULL) {
                En_Bb_Actor_mode_bound_init(this);
            }
            break;
        case BBRED_HIDE:
            if (this->timer == 0) {
                this->actor.speed = 0.0f;
                this->actor.gravity = 0.0f;
                this->actor.velocity.y = 0.0f;
                this->actionState = BBRED_WAIT;
                this->timer = 120;
                this->actor.world.pos = this->actor.home.pos;
                this->actor.shape.rot = this->actor.world.rot = this->actor.home.rot;
                teil_del(this);
            }
            break;
    }
    if (this->actionState != BBRED_WAIT) {
        if (((s32)this->skelAnime.curFrame == 0) || ((s32)this->skelAnime.curFrame == 5)) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_MOUTH);
        }
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLEFALL_FIRE - SFX_FLAG);
    }
}

static void set_direct(EnBb* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y = search_position_angleY(&this->actor.world.pos, &this->waypointPos);
}

static void set_nextPathIndex(EnBb* this, PlayState* play) {
    Path* path = &play->pathList[this->path];
    Vec3s* point;

    if (this->waypoint == (s16)(path->count - 1)) {
        this->waypoint = 0;
    } else {
        this->waypoint++;
    }
    point = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
    this->waypointPos.x = point->x;
    this->waypointPos.y = point->y;
    this->waypointPos.z = point->z;
}

void En_Bb_Actor_mode_br_move_init(PlayState* play, EnBb* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
    this->actor.speed = 0.0f;
    this->actor.world.pos.y += 60.0f;
    this->flameScaleX = 100.0f;
    this->action = BB_WHITE;
    this->waypoint = 0;
    this->timer = (fqrand() * 30.0f) + 40.0f;
    this->maxSpeedXZ = 7.0f;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_br_move);
}

void En_Bb_Actor_mode_br_move(EnBb* this, PlayState* play) {
    if (this->actor.speed == 0.0f) {
        f32 distL1;
        f32 vx;
        f32 vz;
        s16 pitch = search_position_angleX(&this->actor.world.pos, &this->waypointPos);
        f32 vy = sin_s(pitch) * this->maxSpeedXZ;
        f32 vxz = cos_s(pitch) * this->maxSpeedXZ;

        vx = sin_s(this->actor.shape.rot.y) * vxz;
        vz = cos_s(this->actor.shape.rot.y) * vxz;
        distL1 = add_calc(&this->actor.world.pos.x, this->waypointPos.x, 1.0f, ABS(vx), 0.0f);
        distL1 += add_calc(&this->actor.world.pos.y, this->waypointPos.y, 1.0f, ABS(vy), 0.0f);
        distL1 += add_calc(&this->actor.world.pos.z, this->waypointPos.z, 1.0f, ABS(vz), 0.0f);
        this->bobPhase += (0.05f + (fqrand() * 0.01f));
        if (distL1 == 0.0f) {
            this->timer--;
            if (this->timer == 0) {
                set_nextPathIndex(this, play);
                set_direct(this);
                Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000184);
                this->timer = fqrand() * 30.0f + 40.0f;
            } else {
                if (this->moveMode != BBMOVE_NORMAL) {
                    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
                }
                this->actor.world.rot.y += 0x1F40;
            }
            this->moveMode = BBMOVE_NORMAL;
            this->maxSpeedXZ = 0.0f;
        } else {
            this->moveMode = BBMOVE_NOCLIP;
            this->maxSpeedXZ = 10.0f;
        }
        if (this->collider.base.atFlags & AT_HIT) {
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_BITE);
            this->collider.base.atFlags &= ~AT_HIT;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    } else if (add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
        set_direct(this);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (((s32)this->skelAnime.curFrame == 0) && (fqrand() <= 0.1f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }

    if ((this->maxSpeedXZ != 0.0f) && (((s32)this->skelAnime.curFrame == 0) || ((s32)this->skelAnime.curFrame == 5))) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_MOUTH);
    } else if (((s32)this->skelAnime.curFrame == 2) || ((s32)this->skelAnime.curFrame == 7)) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_WING);
    }
}

void En_Bb_Actor_mode_roll_move_init(EnBb* this, PlayState* play) {
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };

    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
    this->moveMode = BBMOVE_NOCLIP;
    this->actionState = BBGREEN_FLAME_ON;
    this->bobPhase = fqrand();
    this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.params == ENBB_GREEN_BIG) {
        set_nextPathIndex(this, play);
        set_direct(this);
    }
    Matrix_translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_rotateZ(this->bobPhase, MTXMODE_APPLY);
    bobOffset.y = this->bobSize;
    Matrix_Position(&bobOffset, &this->actor.world.pos);
    this->targetActor = NULL;
    this->action = BB_GREEN;
    this->actor.speed = 0.0f;
    this->vFlameTimer = (fqrand() * 30.0f) + 180.0f;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_roll_move);
}

void En_Bb_Actor_mode_roll_move_init2(EnBb* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_Bb_Anim_000444);
    this->moveMode = BBMOVE_NOCLIP;
    this->actionState = BBGREEN_FLAME_ON;
    this->targetActor = NULL;
    this->action = BB_GREEN;
    this->actor.speed = 0.0f;
    this->vFlameTimer = (fqrand() * 30.0f) + 180.0f;
    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_roll_move);
}

void En_Bb_Actor_mode_roll_move(EnBb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };
    Vec3f nextPos = player->actor.world.pos;

    nextPos.y += 30.0f;
    if (this->actor.params == ENBB_GREEN_BIG) {
        if (this->actor.speed == 0.0f) {
            s16 pitch = search_position_angleX(&this->actor.home.pos, &this->waypointPos);
            s16 yaw = search_position_angleY(&this->actor.home.pos, &this->waypointPos);
            f32 vy = sin_s(pitch) * this->maxSpeedXZ;
            f32 vxz = cos_s(pitch) * this->maxSpeedXZ;
            f32 vz;
            f32 vx;
            f32 distL1;

            add_calc_short_angle2(&this->actor.world.rot.y, yaw, 1, 0x3E8, 0);
            vx = sin_s(this->actor.world.rot.y) * vxz;
            distL1 = cos_s(this->actor.world.rot.y) * vxz;
            vz = add_calc(&this->actor.home.pos.x, this->waypointPos.x, 1.0f, ABS(vx), 0.0f);
            vz += add_calc(&this->actor.home.pos.y, this->waypointPos.y, 1.0f, ABS(vy), 0.0f);
            vz += add_calc(&this->actor.home.pos.z, this->waypointPos.z, 1.0f, ABS(distL1), 0.0f);
            this->bobPhase += (0.05f + (fqrand() * 0.01f));
            if (vz == 0.0f) {
                set_nextPathIndex(this, play);
            }
            this->moveMode = BBMOVE_NOCLIP;
            this->maxSpeedXZ = 10.0f;
            if (this->collider.base.atFlags & AT_HIT) {
                Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_BITE);
                this->collider.base.atFlags &= ~AT_HIT;
            }
            if (cosf_table(this->bobPhase) == 0.0f) {
                if (this->charge) {
                    this->bobSpeedMod = fqrand();
                } else {
                    this->bobSpeedMod = fqrand() * 3.0f;
                    Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_LAUGH);
                }
            }
            this->actor.shape.rot.y = this->actor.world.rot.y;
        } else if (add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
            set_direct(this);
        }
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 0);
        add_calc_short_angle2(&this->actor.shape.rot.x, search_position_angleX(&this->actor.world.pos, &nextPos), 1, 0xFA0, 0);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (cosf_table(this->bobPhase) <= 0.002f) {
        this->bobSpeedMod = fqrand() * 0.05f;
    }
    Matrix_translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_rotateZ(this->bobPhase, MTXMODE_APPLY);
    bobOffset.y = this->bobSize;
    Matrix_Position(&bobOffset, &nextPos);
    add_calc(&this->actor.world.pos.x, nextPos.x, 1.0f, this->bobPhase * 0.75f, 0.0f);
    add_calc(&this->actor.world.pos.y, nextPos.y, 1.0f, this->bobPhase * 0.75f, 0.0f);
    add_calc(&this->actor.world.pos.z, nextPos.z, 1.0f, this->bobPhase * 0.75f, 0.0f);
    this->bobPhase += 0.1f + this->bobSpeedMod;
    if (BlastVsMyCheck(play, &this->collider.base) || (--this->vFlameTimer == 0)) {
        this->actionState++;
        this->timer = (fqrand() * 30.0f) + 60.0f;
        if (this->vFlameTimer != 0) {
            this->collider.base.acFlags &= ~AC_HIT;
        }
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_DOWN);
    }
    if (this->actionState != BBGREEN_FLAME_ON) {
        this->timer--;
        if (this->timer == 0) {
            this->actionState = BBGREEN_FLAME_ON;
            this->vFlameTimer = (fqrand() * 30.0f) + 180.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_UP);
        }
        add_calc(&this->flameScaleY, 0.0f, 1.0f, 10.0f, 0.0f);
        add_calc(&this->flameScaleX, 0.0f, 1.0f, 10.0f, 0.0f);
    } else {
        add_calc(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
        add_calc(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    }
    if ((s32)this->skelAnime.curFrame == 5) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_WING);
    }
    if (((s32)this->skelAnime.curFrame == 0) && (fqrand() < 0.1f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }
}

void En_Bb_Actor_mode_paralyze_init(EnBb* this) {
    this->action = BB_STUNNED;
    if (this->actor.params != ENBB_WHITE) {
        if (this->actor.params != ENBB_RED) {
            if (this->actor.params > ENBB_GREEN) {
                this->actor.gravity = -2.0f;
                this->actor.shape.yOffset = 1500.0f;
            }
            this->actor.speed = 0.0f;
            this->flameScaleX = 0.0f;
            this->flameScaleY = 0.0f;
        } else {
            teil_del(this);
        }
    }
    switch (this->dmgEffect) {
        case 8:
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 200, COLORFILTER_BUFFLAG_OPA, 80);
            break;
        case 9:
            this->fireIceTimer = 0x30;
            FALLTHROUGH;
        case 15:
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 180, COLORFILTER_BUFFLAG_OPA, 80);
            break;
    }
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    En_Bb_actor_set_process(this, En_Bb_Actor_mode_paralyze);
}

void En_Bb_Actor_mode_paralyze(EnBb* this, PlayState* play) {
    s16 yawDiff = this->actor.world.rot.y - this->actor.wallYaw;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        if (ABS(yawDiff) > 0x4000) {
            this->actor.world.rot.y = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
        }
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.4f;
        } else {
            this->actor.velocity.y = 0.0f;
        }
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
    }
    if (this->actor.colorFilterTimer == 0) {
        this->actor.shape.yOffset = 200.0f;
        if (this->actor.colChkInfo.health != 0) {
            if ((this->actor.params == ENBB_GREEN) || (this->actor.params == ENBB_GREEN_BIG)) {
                En_Bb_Actor_mode_roll_move_init2(this);
            } else if (this->actor.params == ENBB_WHITE) {
                this->action = BB_WHITE;
                En_Bb_actor_set_process(this, En_Bb_Actor_mode_br_move);
            } else {
                En_Bb_Actor_mode_bound_init(this);
            }
        } else {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            En_Bb_Actor_mode_down_init(this, play);
        }
    }
}

void En_Bb_damage_proc(EnBb* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;
        if (this->action != BB_DOWN) {
            if (this->actor.params >= ENBB_RED) {
                this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
                if (this->actor.params == ENBB_RED) {
                    teil_del(this);
                }
                En_Bb_Actor_mode_bound_init(this);
                return;
            }
            this->actionVar2 = 1;
        }
    }
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->dmgEffect = this->actor.colChkInfo.damageEffect;
        Hit_bit_set(&this->actor, &this->collider.elements[0].base, false);
        switch (this->dmgEffect) {
            case 7:
                this->actor.freezeTimer = this->collider.elements[0].base.acHitElem->atDmgInfo.damage;
                FALLTHROUGH;
            case 5:
                this->fireIceTimer = 0x30;
                //! @bug
                //! Setting fireIceTimer here without calling Set_Fog causes a crash if the bubble is
                //! killed in a single hit by an attack with damage effect 5 or 7 while actor updating is halted. Using
                //! Din's Fire on a white bubble will do just that. The mechanism is complex and described below.
                goto block_15;
            case 6:
                this->actor.freezeTimer = this->collider.elements[0].base.acHitElem->atDmgInfo.damage;
                break;
            case 8:
            case 9:
            case 15:
                if (this->action != BB_STUNNED) {
                    hp_down(&this->actor);
                    En_Bb_Actor_mode_paralyze_init(this);
                }
                break;
            default:
            block_15:
                if ((this->dmgEffect == 14) || (this->dmgEffect == 12) || (this->dmgEffect == 11) ||
                    (this->dmgEffect == 10) || (this->dmgEffect == 7) || (this->dmgEffect == 5)) {
                    if ((this->action != BB_DOWN) || (this->timer < 190)) {
                        hp_down(&this->actor);
                    }
                    if ((this->action != BB_DOWN) && (this->actor.params != ENBB_WHITE)) {
                        En_Bb_Actor_mode_bound_init(this);
                    }
                } else {
                    if (((this->action == BB_DOWN) && (this->timer < 190)) ||
                        ((this->actor.params != ENBB_WHITE) && (this->flameScaleX < 20.0f))) {
                        hp_down(&this->actor);
                    } else {
                        this->collider.base.acFlags |= AC_HIT;
                    }
                }
                if (this->actor.colChkInfo.health == 0) {
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                    if (this->actor.params == ENBB_RED) {
                        teil_del(this);
                    }
                    En_Bb_Actor_mode_down_init(this, play);
                    //! @bug
                    //! Because Din's Fire kills the bubble in a single hit, Set_Fog is never called and
                    //! colorFilterParams is never set. And because Din's Fire halts updating during its cutscene,
                    //! En_Bb_Actor_mode_down doesn't kill the bubble on the next frame like it should. This combines with
                    //! the bug in En_Bb_display below to crash the game.
                } else if ((this->actor.params == ENBB_WHITE) &&
                           ((this->action == BB_WHITE) || (this->action == BB_STUNNED))) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
                    this->actor.speed = -8.0f;
                    this->maxSpeedXZ = 0.0f;
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
                } else if (((this->action == BB_DOWN) && (this->timer < 190)) ||
                           ((this->actor.params != ENBB_WHITE) && (this->flameScaleX < 20.0f))) {
                    En_Bb_Actor_mode_damage_init(this);
                }
                FALLTHROUGH;
            case 13:
                break;
        }
    }
}

void En_Bb_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBb* this = (EnBb*)thisx;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    Vec3f sp40 = { 0.0f, -0.6f, 0.0f };
    Color_RGBA8 sp3C = { 0, 0, 255, 255 };
    Color_RGBA8 sp38 = { 0, 0, 0, 0 };
    f32 sp34 = -15.0f;

    if (this->actor.params <= ENBB_BLUE) {
        En_Bb_damage_proc(this, play);
    }
    if (this->actor.colChkInfo.damageEffect != 0xD) {
        this->actionFunc(this, play);
        if ((this->actor.params <= ENBB_BLUE) && (this->actor.speed >= -6.0f) &&
            !(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
            Actor_position_moveF(&this->actor);
        }
        if (this->moveMode == BBMOVE_NORMAL) {
            if ((this->actor.world.pos.y - 20.0f) <= this->actor.floorHeight) {
                sp34 = 20.0f;
            }
            Actor_BGcheck2(play, &this->actor, sp34, 25.0f, 20.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        }
        this->actor.focus.pos = this->actor.world.pos;
        this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
        this->collider.elements[0].dim.worldSphere.center.y =
            this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y);
        this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

        if ((this->action > BB_KILL) && ((this->actor.speed != 0.0f) || (this->action == BB_GREEN))) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if ((this->action > BB_FLAME_TRAIL) &&
            ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) &&
            (this->moveMode != BBMOVE_HIDDEN)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void en_bb_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnBb* this = (EnBb*)thisx;

    Part_Break_Get(&this->bodyBreak, limbIndex, 4, 15, 15, dList, BODYBREAK_OBJECT_SLOT_DEFAULT);
}

static Vec3f ice_pos[] = {
    { 13.0f, 10.0f, 0.0f }, { 5.0f, 25.0f, 5.0f },   { -5.0f, 25.0f, 5.0f },  { -13.0f, 10.0f, 0.0f },
    { 5.0f, 25.0f, -5.0f }, { -5.0f, 25.0f, -5.0f }, { 0.0f, 10.0f, -13.0f }, { 5.0f, 0.0f, 5.0f },
    { 5.0f, 0.0f, -5.0f },  { 0.0f, 10.0f, 13.0f },  { -5.0f, 0.0f, 5.0f },   { -5.0f, 0.0f, -5.0f },
};

void En_Bb_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;
    Vec3f blureBase1 = { 0.0f, 5000.0f, 0.0f };
    Vec3f blureBase2 = { 0.0f, 2000.0f, 0.0f };
    Vec3f blureVtx1;
    Vec3f blureVtx2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bb.c", 2044);

    blureBase1.z = this->maxSpeedXZ * 80.0f;
    blureBase2.z = this->maxSpeedXZ * 80.0f;
    if (this->moveMode != BBMOVE_HIDDEN) {
        if (this->actor.params <= ENBB_BLUE) {
            _texture_z_light_fog_prim(play->state.gfxCtx);
            Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, en_bb_display2,
                              this);

            if (this->fireIceTimer != 0) {
                this->actor.colorFilterTimer++;
                //! @bug:
                //! The purpose of this is to counteract Actor_info_call_actor decrementing colorFilterTimer. However,
                //! the above bugs mean unk_2A8 can be nonzero without damage effects ever having been set.
                //! This routine will then increment colorFilterTimer, and on the next frame Actor_draw will try
                //! to draw the unset colorFilterParams. This causes a divide-by-zero error, crashing the game.
                if (1) {}
                this->fireIceTimer--;
                if ((this->fireIceTimer % 4) == 0) {
                    Vec3f sp70;
                    s32 index = this->fireIceTimer >> 2;

                    sp70.x = this->actor.world.pos.x + ice_pos[index].x;
                    sp70.y = this->actor.world.pos.y + ice_pos[index].y;
                    sp70.z = this->actor.world.pos.z + ice_pos[index].z;

                    if ((this->dmgEffect != 7) && (this->dmgEffect != 5)) {
                        Effect_En_Ice_ct0(play, &this->actor, &sp70, 0x96, 0x96, 0x96, 0xFA, 0xEB, 0xF5,
                                                       0xFF, 0.8f);
                    } else {
                        sp70.y -= 17.0f;
                        Effect_En_Fire_ct(play, &this->actor, &sp70, 0x28, 1, 0, -1);
                    }
                }
            }
            Matrix_translate(0.0f, this->flameScaleX * -40.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_translate(0.0f, -40.0f, 0.0f, MTXMODE_APPLY);
        }
        if (this->actor.params != ENBB_WHITE) {
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                        ((play->gameplayFrames + (this->flameScrollMod * 10)) *
                                         (-20 - (this->flameScrollMod * -2))) %
                                            0x200,
                                        0x20, 0x80));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, this->flamePrimBlue, this->flamePrimAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, this->flameEnvColor.r, this->flameEnvColor.g, this->flameEnvColor.b, 0);
            Matrix_rotateY(
                BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000)),
                MTXMODE_APPLY);
            Matrix_scale(this->flameScaleX * 0.01f, this->flameScaleY * 0.01f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_bb.c", 2106);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        } else {
            Matrix_Position(&blureBase1, &blureVtx1);
            Matrix_Position(&blureBase2, &blureVtx2);
            if ((this->maxSpeedXZ != 0.0f) && (this->action == BB_WHITE) && !(play->gameplayFrames & 1) &&
                (this->actor.colChkInfo.health != 0)) {
                EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIdx), &blureVtx1, &blureVtx2);
            } else if (this->action != BB_WHITE) {
                EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIdx));
            }
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bb.c", 2127);
}
