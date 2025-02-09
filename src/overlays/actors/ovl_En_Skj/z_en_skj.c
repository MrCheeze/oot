#include "z_en_skj.h"
#include "overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.h"
#include "assets/objects/object_skj/object_skj.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Skj_actor_ct(Actor* thisx, PlayState* play2);
void En_Skj_actor_dt(Actor* thisx, PlayState* play);
void En_Skj_actor_move(Actor* thisx, PlayState* play);
void En_Skj_actor_draw(Actor* thisx, PlayState* play);

void En_Skj_Ocarina_actor_move(Actor* thisx, PlayState* play);
void En_Skj_OcarinaT_actor_move(Actor* thisx, PlayState* play);

void Change_Skj_Landing(EnSkj* this);
void Change_Skj_Serch(EnSkj* this);

void Skj_CenterPositionSet(EnSkj* this);
void Change_Skj_Trio_Wait(EnSkj* this);
void Change_Skj_Dance(EnSkj* this);
void Change_Skj_Trio_Ocarina_Escape(EnSkj* this);
void Change_Skj_Trio_Ocarina_Dance(EnSkj* this);
void Change_Skj_Back_Jump(EnSkj* this);
void Change_Skj_Attack_Reverse(EnSkj* this);
void Change_Skj_Burn(EnSkj* this);
void Change_Skj_Side_Walk(EnSkj* this);
void Change_Skj_Ocarina_Duo(EnSkj* this);
void Change_Skj_Duo_MsgWait(EnSkj* this);
void Change_Skj_Duo_BuyMaskTalk(EnSkj* this);
void Change_Skj_Duo_FailureTalkWait(EnSkj* this);
void Change_Skj_Duo_Talk(EnSkj* this);
void Change_Skj_Duo_CarryStart(EnSkj* this);
void Change_Skj_Duo_CarryFinish(EnSkj* this);
void Change_Skj_Duo_BuyMaskJump(EnSkj* this);
void Change_Skj_Duo_BuyMaskLanding(EnSkj* this);
void Change_Skj_Duo_BuyMaskWalk(EnSkj* this);
void Change_Skj_Duo_BuyMaskEscape(EnSkj* this);
void Change_Skj_Duo_FailureTalk(EnSkj* this);
void Change_Skj_Down(EnSkj* this);
void Change_Skj_Hit(EnSkj* this);
void Change_Skj_OcarinaT_FailureTalkEnd(EnSkj* this, PlayState* play);
void Change_Skj_Duo_BuyMask(EnSkj* this, PlayState* play);
f32 Skj_TalkDistance(EnSkj* this);
s32 Skj_Cross(EnSkj* this, PlayState* play);
void Change_Skj_Duo_BuyMaskFinish(EnSkj* this, PlayState* play);
void Skj_OcarinaT_PlayerAngleSet(EnSkj* this, Player* player);

void Skj_OcarinaT_TalkWait(EnSkj* this, PlayState* play);
void Skj_OcarinaT_Talk(EnSkj* this, PlayState* play);
void Skj_OcarinaT_TalkWait2(EnSkj* this, PlayState* play);
void Skj_OcarinaT_Play_1(EnSkj* this, PlayState* play);
void Skj_OcarinaT_Failure(EnSkj* this, PlayState* play);
void Skj_OcarinaT_Success(EnSkj* this, PlayState* play);
void Skj_OcarinaT_SuccessTalkWait(EnSkj* this, PlayState* play);
void Skj_OcarinaT_SuccessCarryWait(EnSkj* this, PlayState* play);
void Skj_OcarinaT_SuccessCarryFinish(EnSkj* this, PlayState* play);
void Skj_OcarinaT_FailureTalkWait(EnSkj* this, PlayState* play);
void Skj_OcarinaT_FailureTalkEnd(EnSkj* this, PlayState* play);
void Skj_OcarinaT_Finish(EnSkj* this, PlayState* play);

void Skj_Back_Jump(EnSkj* this, PlayState* play);
void Skj_Attack(EnSkj* this, PlayState* play);
void Skj_Dance(EnSkj* this, PlayState* play);
void Skj_Down(EnSkj* this, PlayState* play);
void Skj_Hit(EnSkj* this, PlayState* play);
void Skj_Landing(EnSkj* this, PlayState* play);
void Skj_Serch(EnSkj* this, PlayState* play);
void Skj_Side_Walk(EnSkj* this, PlayState* play);
void Skj_Attack_Reverse(EnSkj* this, PlayState* play);
void Skj_Burn(EnSkj* this, PlayState* play);
void Skj_Talk_Wait(EnSkj* this, PlayState* play);
void Skj_Ocarina_Duo(EnSkj* this, PlayState* play);
void Skj_Duo_Talk(EnSkj* this, PlayState* play);
void Skj_Duo_MsgWait(EnSkj* this, PlayState* play);
void Skj_Duo_CarryStart(EnSkj* this, PlayState* play);
void Skj_Duo_CarryFinish(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskTalk(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskJump(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskLanding(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskWalk(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMask(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskFinish(EnSkj* this, PlayState* play);
void Skj_Duo_BuyMaskEscape(EnSkj* this, PlayState* play);
void Skj_Duo_FailureTalkWait(EnSkj* this, PlayState* play);
void Skj_Duo_FailureTalk(EnSkj* this, PlayState* play);
void Skj_Trio_Wait(EnSkj* this, PlayState* play);
void Skj_Trio_Ocarina_Wait(EnSkj* this, PlayState* play);
void Skj_Trio_Ocarina_Dance(EnSkj* this, PlayState* play);
void Skj_Trio_Ocarina_Escape(EnSkj* this, PlayState* play);

void Skj_Set_BlueBlood(PlayState* play, Vec3f* pos);

void Change_Skj_Talk_Wait(EnSkj* this);

#define songFailTimer multiuseTimer
#define battleExitTimer multiuseTimer

typedef enum SkullKidAnim {
    /* 0 */ SKJ_ANIM_BACKFLIP,
    /* 1 */ SKJ_ANIM_SHOOT_NEEDLE,
    /* 2 */ SKJ_ANIM_PLAY_FLUTE,
    /* 3 */ SKJ_ANIM_DIE,
    /* 4 */ SKJ_ANIM_HIT,
    /* 5 */ SKJ_ANIM_LAND,
    /* 6 */ SKJ_ANIM_LOOK_LEFT_RIGHT,
    /* 7 */ SKJ_ANIM_FIGHTING_STANCE,
    /* 8 */ SKJ_ANIM_WALK_TO_PLAYER,
    /* 9 */ SKJ_ANIM_WAIT
} SkullKidAnim;

typedef enum SkullKidStumpSide {
    /* 0 */ SKULL_KID_LEFT,
    /* 1 */ SKULL_KID_RIGHT
} SkullKidStumpSide;

typedef enum SkullKidOcarinaGameState {
    /* 0 */ SKULL_KID_OCARINA_WAIT,
    /* 1 */ SKULL_KID_OCARINA_PLAY_NOTES,
    /* 2 */ SKULL_KID_OCARINA_LEAVE_GAME
} SkullKidOcarinaGameState;

typedef enum SkullKidAction {
    /* 00 */ SKJ_ACTION_FADE,
    /* 01 */ SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE,
    /* 02 */ SKJ_ACTION_SARIA_SONG_IDLE,
    /* 03 */ SKJ_ACTION_WAIT_FOR_DEATH_ANIM,
    /* 04 */ SKJ_ACTION_PICK_NEXT_FIHGT_ACTION,
    /* 05 */ SKJ_ACTION_WAIT_FOR_LAND_ANIM,
    /* 06 */ SKJ_ACTION_RESET_FIGHT,
    /* 07 */ SKJ_ACTION_FIGHT,
    /* 08 */ SKJ_ACTION_NEEDLE_RECOVER,
    /* 09 */ SKJ_ACTION_SPAWN_DEATH_EFFECT,
    /* 10 */ SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE,
    /* 11 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG,
    /* 12 */ SKJ_ACTION_SARIA_SONG_AFTER_SONG,
    /* 13 */ SKJ_ACTION_SARIA_TALK,
    /* 14 */ SKJ_ACTION_UNK14,
    /* 15 */ SKJ_ACTION_SARIA_SONG_CHANGE_MODE,
    /* 16 */ SKJ_ACTION_SARIA_SONG_START_TRADE,
    /* 17 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING,
    /* 18 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM,
    /* 19 */ SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER,
    /* 20 */ SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK,
    /* 21 */ SKJ_ACTION_SARIA_SONG_TAKE_MASK,
    /* 22 */ SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT,
    /* 23 */ SKJ_ACTION_SARIA_SONG_WRONG_SONG,
    /* 24 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT,
    /* 25 */ SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER,
    /* 26 */ SKJ_ACTION_OCARINA_GAME_IDLE,
    /* 27 */ SKJ_ACTION_OCARINA_GAME_PLAY,
    /* 28 */ SKJ_ACTION_OCARINA_GAME_LEAVE
} SkullKidAction;

typedef struct EnSkjUnkStruct {
    u8 unk_0;
    EnSkj* skullkid;
} EnSkjUnkStruct;

static EnSkjUnkStruct ocarina = { 0, NULL };
static EnSkjUnkStruct ocarinaT[] = { { 0, NULL }, { 0, NULL } };

ActorProfile En_Skj_Profile = {
    /**/ ACTOR_EN_SKJ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SKJ,
    /**/ sizeof(EnSkj),
    /**/ En_Skj_actor_ct,
    /**/ En_Skj_actor_dt,
    /**/ En_Skj_actor_move,
    /**/ En_Skj_actor_draw,
};

static ColliderCylinderInitType1 EnSkjOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x0, 0x08 },
        { 0xFFCFFFFF, 0x0, 0x0 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 8, 48, 0, { 0, 0, 0 } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 Trio_PresentItem[] = {
    GI_RUPEE_GREEN,
    GI_RUPEE_BLUE,
    GI_HEART_PIECE,
    GI_RUPEE_RED,
};

static AnimationMinimalInfo animData[] = {
    { &gSkullKidBackflipAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidShootNeedleAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidPlayFluteAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidDieAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidHitAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLandAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLookLeftAndRightAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidFightingStanceAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWalkToPlayerAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWaitAnim, ANIMMODE_LOOP, 0.0f },
};

static EnSkjActionFunc SkjProcData[] = {
    Skj_Back_Jump,
    Skj_Attack,
    Skj_Dance,
    Skj_Down,
    Skj_Hit,
    Skj_Landing,
    Skj_Serch,
    Skj_Side_Walk,
    Skj_Attack_Reverse,
    Skj_Burn,
    Skj_Talk_Wait,
    Skj_Ocarina_Duo,
    Skj_Duo_Talk,
    Skj_Duo_MsgWait,
    Skj_Duo_CarryStart,
    Skj_Duo_CarryFinish,
    Skj_Duo_BuyMaskTalk,
    Skj_Duo_BuyMaskJump,
    Skj_Duo_BuyMaskLanding,
    Skj_Duo_BuyMaskWalk,
    Skj_Duo_BuyMask,
    Skj_Duo_BuyMaskFinish,
    Skj_Duo_BuyMaskEscape,
    Skj_Duo_FailureTalkWait,
    Skj_Duo_FailureTalk,
    Skj_Trio_Wait,
    Skj_Trio_Ocarina_Wait,
    Skj_Trio_Ocarina_Dance,
    Skj_Trio_Ocarina_Escape,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

static s32 skj_talk_flag; // gets set if ACTOR_FLAG_TALK is set

void CHG_Skj_Animation(EnSkj* this, u8 index) {
    f32 endFrame = Si2_anime_end_frame(animData[index].animation);

    this->animIndex = index;
    Skeleton_Info2_init(&this->skelAnime, animData[index].animation, 1.0f, 0.0f, endFrame,
                     animData[index].mode, animData[index].morphFrames);
}

void CHG_Skj_Mode(EnSkj* this, u8 action) {
    this->action = action;
    this->actionFunc = SkjProcData[action];

    switch (action) {
        case SKJ_ACTION_FADE:
        case SKJ_ACTION_WAIT_FOR_DEATH_ANIM:
        case SKJ_ACTION_PICK_NEXT_FIHGT_ACTION:
        case SKJ_ACTION_SPAWN_DEATH_EFFECT:
        case SKJ_ACTION_SARIA_SONG_START_TRADE:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM:
        case SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER:
        case SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK:
        case SKJ_ACTION_SARIA_SONG_TAKE_MASK:
        case SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT:
        case SKJ_ACTION_SARIA_SONG_WRONG_SONG:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT:
        case SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER:
        case SKJ_ACTION_OCARINA_GAME_IDLE:
        case SKJ_ACTION_OCARINA_GAME_PLAY:
        case SKJ_ACTION_OCARINA_GAME_LEAVE:
            this->unk_2D3 = 0;
            break;
        default:
            this->unk_2D3 = 1;
            break;
    }
}

void Skj_CenterPositionSet(EnSkj* this) {
    Vec3f mult;

    mult.x = 0.0f;
    mult.y = 0.0f;
    mult.z = 120.0f;

    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
    Matrix_Position(&mult, &this->center);

    this->center.x += this->actor.world.pos.x;
    this->center.z += this->actor.world.pos.z;
}

void Skj_profile_message_set(EnSkj* this) {
    switch (this->actor.params) {
        case 0:
            if (GET_ITEMGETINF(ITEMGETINF_39)) {
                this->actor.naviEnemyId = NAVI_ENEMY_SKULL_KID_MASK;
            } else if (GET_ITEMGETINF(ITEMGETINF_16)) {
                this->actor.naviEnemyId = NAVI_ENEMY_SKULL_KID_FRIENDLY;
            } else {
                this->actor.naviEnemyId = NAVI_ENEMY_SKULL_KID; // No Sarias song no skull mask
            }
            break;

        case 1:
        case 2:
            this->actor.naviEnemyId = NAVI_ENEMY_SKULL_KID;
            break;

        default:
            this->actor.naviEnemyId = NAVI_ENEMY_SKULL_KID_ADULT;
            break;
    }
}

void En_Skj_actor_ct(Actor* thisx, PlayState* play2) {
    s16 type = PARAMS_GET_U(thisx->params, 10, 6);
    EnSkj* this = (EnSkj*)thisx;
    PlayState* play = play2;
    s32 pad;

    ValueSet_process(thisx, value_init);
    switch (type) {
        case 5: // Invisible on the small stump (sarias song))
            ocarina.unk_0 = 1;
            ocarina.skullkid = (EnSkj*)thisx;
            this->actor.destroy = NULL;
            this->actor.draw = NULL;
            this->actor.update = En_Skj_Ocarina_actor_move;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            this->actor.flags |= 0;
            Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_PROP);
            break;

        case 6: // Invisible on the short stump (ocarina game)
            ocarina.unk_0 = 1;
            ocarina.skullkid = (EnSkj*)thisx;
            this->actor.destroy = NULL;
            this->actor.draw = NULL;
            this->actor.update = En_Skj_OcarinaT_actor_move;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            this->actor.flags |= 0;
            Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_PROP);
            this->actor.focus.pos.x = 1230.0f;
            this->actor.focus.pos.y = -90.0f;
            this->actor.focus.pos.z = 450.0f;
            this->actionFunc = Skj_OcarinaT_TalkWait;
            break;

        default:
            this->actor.params = type;
            if (((this->actor.params != 0) && (this->actor.params != 1)) && (this->actor.params != 2)) {
                if (INV_CONTENT(ITEM_TRADE_ADULT) < ITEM_POACHERS_SAW) {
                    Actor_delete(&this->actor);
                    return;
                }
            }

            Skj_profile_message_set(this);
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSkullKidSkel, &gSkullKidPlayFluteAnim, this->jointTable,
                               this->morphTable, 19);
            if ((type >= 0) && (type < 3)) {
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
                Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
            }

            if ((type < 0) || (type >= 7)) {
                this->actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;
            }

            if ((type > 0) && (type < 3)) {
                this->actor.attentionRangeType = ATTENTION_RANGE_7;
                this->posCopy = this->actor.world.pos;
                ocarinaT[type - 1].unk_0 = 1;
                ocarinaT[type - 1].skullkid = this;
                this->minigameState = 0;
                this->alpha = 0;
                Change_Skj_Trio_Wait(this);
            } else {
                this->alpha = 255;
                Change_Skj_Dance(this);
            }

            this->actor.colChkInfo.damageTable = &btl_data;
            this->actor.colChkInfo.health = 10;
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set3(play, &this->collider, &this->actor, &EnSkjOcInfoData);
            Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 40.0f);
            Actor_set_scale(thisx, 0.01f);
            this->actor.textId = this->textId = 0;
            this->multiuseTimer = 0;
            this->backflipFlag = 0;
            this->needlesToShoot = 3;
            this->hitsUntilDodge = 3;
            this->actor.speed = 0.0f;
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = -1.0f;
            Skj_CenterPositionSet(this);

#if DEBUG_FEATURES
            {
                Player* player = GET_PLAYER(play);

                PRINTF("Player_X : %f\n", player->actor.world.pos.x);
                PRINTF("Player_Z : %f\n", player->actor.world.pos.z);
                PRINTF("World_X  : %f\n", this->actor.world.pos.x);
                PRINTF("World_Z  : %f\n", this->actor.world.pos.z);
                PRINTF("Center_X : %f\n", this->center.x);
                PRINTF("Center_Z : %f\n\n", this->center.z);
            }
#endif

            break;
    }
}

void En_Skj_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

int CHK_Skj_Ocarina_Player_Distance(Player* player, EnSkj* this) {
    f32 xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    f32 zDiff = player->actor.world.pos.z - this->actor.world.pos.z;
    f32 yDiff = player->actor.world.pos.y - this->actor.world.pos.y;

    return (SQ(xDiff) + SQ(zDiff) <= 676.0f) && (yDiff >= 0.0f);
}

f32 Skj_TalkDistance(EnSkj* this) {
    EnSkj* temp_v0;
    f32 zDiff;
    f32 xDiff;

    temp_v0 = ocarina.skullkid;
    xDiff = temp_v0->actor.world.pos.x - this->actor.world.pos.x;
    zDiff = temp_v0->actor.world.pos.z - this->actor.world.pos.z;
    return sqrtf(SQ(xDiff) + SQ(zDiff)) + 26.0f;
}

f32 Skj_TalkHight(EnSkj* this) {
    return fabsf(ocarina.skullkid->actor.world.pos.y - this->actor.world.pos.y) + 10.0f;
}

s32 Skj_SetNeedle(EnSkj* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    Vec3f pos2;
    EnSkjneedle* needle;

    pos.x = 1.5f;
    pos.y = 0.0f;
    pos.z = 40.0f;

    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
    Matrix_Position(&pos, &pos2);

    pos2.x += this->actor.world.pos.x;
    pos2.z += this->actor.world.pos.z;
    pos2.y = this->actor.world.pos.y + 27.0f;

    needle = (EnSkjneedle*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_SKJNEEDLE, pos2.x, pos2.y, pos2.z,
                                       this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 0);
    if (needle != NULL) {
        needle->killTimer = 100;
        needle->actor.speed = 24.0f;
        return 1;
    }
    return 0;
}

void Skj_Set_BlueBlood(PlayState* play, Vec3f* pos) {
    EffectSparkInit effect;
    s32 sp20;

    effect.position.x = pos->x;
    effect.position.y = pos->y;
    effect.position.z = pos->z;
    effect.uDiv = 5;
    effect.vDiv = 5;

    effect.colorStart[0].r = 0;
    effect.colorStart[0].g = 0;
    effect.colorStart[0].b = 128;
    effect.colorStart[0].a = 255;

    effect.colorStart[1].r = 0;
    effect.colorStart[1].g = 0;
    effect.colorStart[1].b = 128;
    effect.colorStart[1].a = 255;

    effect.colorStart[2].r = 0;
    effect.colorStart[2].g = 0;
    effect.colorStart[2].b = 128;
    effect.colorStart[2].a = 255;

    effect.colorStart[3].r = 0;
    effect.colorStart[3].g = 0;
    effect.colorStart[3].b = 128;
    effect.colorStart[3].a = 255;

    effect.colorEnd[0].r = 0;
    effect.colorEnd[0].g = 0;
    effect.colorEnd[0].b = 32;
    effect.colorEnd[0].a = 0;

    effect.colorEnd[1].r = 0;
    effect.colorEnd[1].g = 0;
    effect.colorEnd[1].b = 32;
    effect.colorEnd[1].a = 0;

    effect.colorEnd[2].r = 0;
    effect.colorEnd[2].g = 0;
    effect.colorEnd[2].b = 64;
    effect.colorEnd[2].a = 0;

    effect.colorEnd[3].r = 0;
    effect.colorEnd[3].g = 0;
    effect.colorEnd[3].b = 64;
    effect.colorEnd[3].a = 0;

    effect.speed = 8.0f;
    effect.gravity = -1.0f;

    effect.timer = 0;
    effect.duration = 8;

    EffectAdd(play, &sp20, EFFECT_SPARK, 0, 1, &effect);
}

s32 Skj_Cross(EnSkj* this, PlayState* play) {
    s16 yawDiff;
    Vec3f effectPos;

    if (!((this->unk_2D3 == 0) || (skj_talk_flag != 0) || !(this->collider.base.acFlags & AC_HIT))) {
        this->collider.base.acFlags &= ~AC_HIT;
        switch (this->actor.colChkInfo.damageEffect) {
            case 0xF:
                effectPos.x = this->collider.elem.acDmgInfo.hitPos.x;
                effectPos.y = this->collider.elem.acDmgInfo.hitPos.y;
                effectPos.z = this->collider.elem.acDmgInfo.hitPos.z;

                Skj_Set_BlueBlood(play, &effectPos);
                Effect_SS_HitMark_ct(play, 1, &effectPos);

                yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
                if ((this->action == 2) || (this->action == 6)) {
                    if ((yawDiff > 0x6000) || (yawDiff < -0x6000)) {
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                        Change_Skj_Down(this);
                        return 1;
                    }
                }

                hp_down(&this->actor);
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);

                if (this->actor.colChkInfo.health != 0) {
                    if (this->hitsUntilDodge != 0) {
                        this->hitsUntilDodge--;
                    }
                    if (this->dodgeResetTimer == 0) {
                        this->dodgeResetTimer = 60;
                    }
                    Change_Skj_Hit(this);
                    return 1;
                }
                Change_Skj_Down(this);
                return 1;

            case 0:
                this->backflipFlag = 1;
                Change_Skj_Back_Jump(this);
                return 1;

            default:
                break;
        }
    }
    return 0;
}

s32 Skj_Search_Player(EnSkj* this, PlayState* play) {
    s16 yawDiff;

    if (this->actor.xzDistToPlayer < this->unk_2EC) {
        if (player_big_SE_action_check(play) != 0) {
            return 1;
        }
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;

    if ((yawDiff < this->unk_2C8) && (-this->unk_2C8 < yawDiff)) {
        return 1;
    }

    return 0;
}

void Change_Skj_Back_Jump(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speed = -8.0f;

    CHG_Skj_Animation(this, SKJ_ANIM_BACKFLIP);
    CHG_Skj_Mode(this, SKJ_ACTION_FADE);
}

void Skj_Back_Jump(EnSkj* this, PlayState* play) {
    u32 alpha = this->alpha;

    if (this->unk_2D6 == 2) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        this->unk_2D6 = 0;
    }

    alpha -= 20;

    if (this->backflipFlag != 0) {
        if (alpha > 255) {
            alpha = 0;
        }

        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }

    if (this->actor.velocity.y <= 0.0f) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
            Change_Skj_Landing(this);
        }
    }
}

void Change_Skj_Attack(EnSkj* this) {
    this->needlesToShoot = 3;
    this->needleShootTimer = 0;
    CHG_Skj_Animation(this, SKJ_ANIM_SHOOT_NEEDLE);
    CHG_Skj_Mode(this, SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE);
}

void Skj_Attack(EnSkj* this, PlayState* play) {
    u8 val;
    s16 lastFrame = Si2_anime_end_frame(&gSkullKidShootNeedleAnim);

    if ((this->skelAnime.curFrame == lastFrame) && (this->needleShootTimer == 0)) {
        val = this->needlesToShoot;
        if (this->needlesToShoot != 0) {
            Skj_SetNeedle(this, play);
            this->needleShootTimer = 4;
            val--;
            this->needlesToShoot = val;

        } else {
            Change_Skj_Attack_Reverse(this);
        }
    }
}

void Change_Skj_Dance(EnSkj* this) {
    this->unk_2C8 = 0xAAA;
    this->unk_2EC = 200.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_PLAY_FLUTE);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_IDLE);
}

void Skj_Dance(EnSkj* this, PlayState* play) {
    if (this->actor.params == 0) {
        if (!GET_ITEMGETINF(ITEMGETINF_16) && (this->actor.xzDistToPlayer < 200.0f)) {
            this->backflipFlag = 1;
            Change_Skj_Back_Jump(this);
        } else if (ocarina.unk_0 != 0) {
            Player* player = GET_PLAYER(play);
            if (CHK_Skj_Ocarina_Player_Distance(player, ocarina.skullkid)) {
                Change_Skj_Talk_Wait(this);
                player->stateFlags2 |= PLAYER_STATE2_23;
                player->unk_6A8 = &ocarina.skullkid->actor;
            }
        }
    } else {
        if (Skj_Search_Player(this, play) != 0) {
            Change_Skj_Serch(this);
        }
    }
}

void Change_Skj_Down(EnSkj* this) {
    CHG_Skj_Animation(this, SKJ_ANIM_DIE);
    CHG_Skj_Mode(this, SKJ_ACTION_WAIT_FOR_DEATH_ANIM);
}

void Skj_Down(EnSkj* this, PlayState* play) {
    s16 lastFrame = Si2_anime_end_frame(&gSkullKidDieAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        Change_Skj_Burn(this);
    }
}

void Change_Skj_Hit(EnSkj* this) {
    CHG_Skj_Animation(this, SKJ_ANIM_HIT);
    CHG_Skj_Mode(this, SKJ_ACTION_PICK_NEXT_FIHGT_ACTION);
}

void Skj_Hit(EnSkj* this, PlayState* play) {
    s16 lastFrame = Si2_anime_end_frame(&gSkullKidHitAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        if (this->hitsUntilDodge == 0) {
            this->hitsUntilDodge = 3;
            Change_Skj_Back_Jump(this);
        } else {
            Change_Skj_Side_Walk(this);
        }
    }
}

void Change_Skj_Landing(EnSkj* this) {
    Skj_CenterPositionSet(this);
    this->actor.speed = 0.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_LAND);
    CHG_Skj_Mode(this, SKJ_ACTION_WAIT_FOR_LAND_ANIM);
}

void Skj_Landing(EnSkj* this, PlayState* play) {
    s16 lastFrame = Si2_anime_end_frame(&gSkullKidLandAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        Change_Skj_Side_Walk(this);
    }
}

void Change_Skj_Serch(EnSkj* this) {
    this->unk_2C8 = 0x2000;
    this->battleExitTimer = 400;
    this->unk_2EC = 600.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_LOOK_LEFT_RIGHT);
    CHG_Skj_Mode(this, SKJ_ACTION_RESET_FIGHT);
}

void Skj_Serch(EnSkj* this, PlayState* play) {
    if (this->battleExitTimer == 0) {
        Change_Skj_Dance(this);
    } else if (Skj_Search_Player(this, play) != 0) {
        this->battleExitTimer = 600;
        Change_Skj_Side_Walk(this);
    }
}

void Change_Skj_Side_Walk(EnSkj* this) {
    this->needleShootTimer = 60;
    this->unk_2C8 = 0x2000;
    this->unk_2F0 = 0.0f;
    this->unk_2EC = 600.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_FIGHTING_STANCE);
    CHG_Skj_Mode(this, SKJ_ACTION_FIGHT);
}

void Skj_Side_Walk(EnSkj* this, PlayState* play) {
    Vec3f pos1;
    Vec3f pos2;
    s32 pad[3];
    f32 prevPosX;
    f32 prevPosZ;
    f32 phi_f14;
    s16 yawDistToPlayer;

    if (this->needleShootTimer == 0) {
        Change_Skj_Attack(this);
    } else if (this->battleExitTimer != 0) {
        yawDistToPlayer =
            add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->unk_2F0, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        add_calc2(&this->unk_2F0, 2000.0f, 1.0f, 200.0f);

        pos1.x = 0.0f;
        pos1.y = 0.0f;
        pos1.z = -120.0f;

        Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
        Matrix_Position(&pos1, &pos2);
        prevPosX = this->actor.world.pos.x;
        prevPosZ = this->actor.world.pos.z;
        if (1) {}
        this->actor.world.pos.x = this->center.x + pos2.x;
        this->actor.world.pos.z = this->center.z + pos2.z;

        phi_f14 = sqrtf(SQ(this->actor.world.pos.x - prevPosX) + SQ(this->actor.world.pos.z - prevPosZ));
        phi_f14 = CLAMP_MAX(phi_f14, 10.0f);
        phi_f14 /= 10.0f;

        this->skelAnime.playSpeed = (yawDistToPlayer < 0) ? -(1.0f + phi_f14) : (1.0f + phi_f14);

    } else if (Skj_Search_Player(this, play) != 0) {
        this->backflipFlag = 1;
        Change_Skj_Back_Jump(this);
    } else {
        Change_Skj_Dance(this);
    }
}

void Change_Skj_Attack_Reverse(EnSkj* this) {
    Skeleton_Info_reverse(&this->skelAnime);
    CHG_Skj_Mode(this, SKJ_ACTION_NEEDLE_RECOVER);
}

void Skj_Attack_Reverse(EnSkj* this, PlayState* play) {
    if (this->skelAnime.curFrame == 0.0f) {
        Change_Skj_Side_Walk(this);
    }
}

void Change_Skj_Burn(EnSkj* this) {
    this->backflipFlag = 1;
    CHG_Skj_Mode(this, SKJ_ACTION_SPAWN_DEATH_EFFECT);
}

void Skj_Burn(EnSkj* this, PlayState* play) {
    Vec3f effectPos;
    Vec3f effectVel;
    Vec3f effectAccel;
    u32 phi_v0;

    phi_v0 = this->alpha - 4;

    if (phi_v0 > 255) {
        phi_v0 = 0;
    }
    this->alpha = phi_v0;
    this->actor.shape.shadowAlpha = phi_v0;

    effectPos.x = rnd_fx(30.0f) + this->actor.world.pos.x;
    effectPos.y = rnd_fx(30.0f) + this->actor.world.pos.y;
    effectPos.z = rnd_fx(30.0f) + this->actor.world.pos.z;

    effectAccel.z = 0.0f;
    effectAccel.y = 0.0f;
    effectAccel.x = 0.0f;

    effectVel.z = 0.0f;
    effectVel.y = 0.0f;
    effectVel.x = 0.0f;

    _Effect_SS_Db_ct(play, &effectPos, &effectVel, &effectAccel, 100, 10, 255, 255, 255, 255, 0, 0, 255, 1, 9, 1);
}

void Change_Skj_Talk_Wait(EnSkj* this) {
    this->textId = 0x10BC;

    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
}

void Skj_Talk_Wait(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // When link pulls out the Ocarina center him on the stump
    // Link was probably supposed to be pointed towards skull kid as well
    if (player->stateFlags2 & PLAYER_STATE2_24) {
        player->stateFlags2 |= PLAYER_STATE2_25;
        player->unk_6A8 = &ocarina.skullkid->actor;
        player->actor.world.pos.x = ocarina.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = ocarina.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = ocarina.skullkid->actor.world.pos.z;
        Skj_OcarinaT_PlayerAngleSet(ocarina.skullkid, player);
        ocarina_set_111(play, OCARINA_ACTION_CHECK_SARIA);
        Change_Skj_Ocarina_Duo(this);
    } else if (skj_talk_flag != 0) {
        player->actor.world.pos.x = ocarina.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = ocarina.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = ocarina.skullkid->actor.world.pos.z;
        if ((mask_check(play) == PLAYER_MASK_SKULL) && !GET_ITEMGETINF(ITEMGETINF_39)) {
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            Change_Skj_Duo_BuyMaskTalk(this);
        } else {
            Change_Skj_Duo_MsgWait(this);
        }
    } else if (!CHK_Skj_Ocarina_Player_Distance(player, ocarina.skullkid)) {
        Change_Skj_Dance(this);
    } else {
        player->stateFlags2 |= PLAYER_STATE2_23;
        if (GET_ITEMGETINF(ITEMGETINF_16)) {
            if (GET_ITEMGETINF(ITEMGETINF_39)) {
                this->textId = get_mask_message(play, MASK_REACTION_SET_SKULL_KID);
                if (this->textId == 0) {
                    this->textId = 0x1020;
                }
            } else if (mask_check(play) == PLAYER_MASK_NONE) {
                this->textId = 0x10BC;
            } else if (mask_check(play) == PLAYER_MASK_SKULL) {
                this->textId = 0x101B;
            } else {
                this->textId = get_mask_message(play, MASK_REACTION_SET_SKULL_KID);
            }
            Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
        }
    }
}

void Change_Skj_Ocarina_Duo(EnSkj* this) {
    this->unk_2D6 = 0;
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG);
}

void Skj_Ocarina_Duo(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // Played a song thats not Saria's song
    if (!GET_ITEMGETINF(ITEMGETINF_16) &&
        ((play->msgCtx.msgMode == MSGMODE_OCARINA_FAIL) || (play->msgCtx.msgMode == MSGMODE_OCARINA_FAIL_NO_TEXT))) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        message_close(play);
        player->unk_6A8 = &this->actor;
        Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
        Change_Skj_Duo_FailureTalkWait(this);
    } else {
        if ((play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) && (this->unk_2D6 == 0)) {
            this->unk_2D6 = 1;
            CHG_Skj_Animation(this, SKJ_ANIM_PLAY_FLUTE);
        } else if ((this->unk_2D6 != 0) && (play->msgCtx.msgMode == MSGMODE_SONG_DEMONSTRATION_DONE)) {
            this->unk_2D6 = 0;
            CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
        }
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_00;
            this->unk_2D6 = 0;
            CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
            CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (!GET_ITEMGETINF(ITEMGETINF_16)) {
                // Saria's song has been played for the first titme
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                player->unk_6A8 = &this->actor;
                Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
                this->textId = 0x10BB;
                Change_Skj_Duo_Talk(this);
            } else {
                play->msgCtx.ocarinaMode = OCARINA_MODE_05;
            }
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
            player->stateFlags2 &= ~PLAYER_STATE2_24;
            Actor_delete(&this->actor);
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= PLAYER_STATE2_23;
        } else {
            if (play->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
                z_common_data.sunsSongState = SUNSSONG_INACTIVE;
                if (GET_ITEMGETINF(ITEMGETINF_16)) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
                    this->textId = 0x10BD;
                    Change_Skj_Duo_Talk(this);
                } else {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
                    Change_Skj_Duo_FailureTalkWait(this);
                }
            }
        }
    }
}

void Change_Skj_Duo_Talk(EnSkj* this) {
    this->unk_2D6 = 0;
    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_AFTER_SONG);
}

void Skj_Duo_Talk(EnSkj* this, PlayState* play) {
    if (skj_talk_flag != 0) {
        Change_Skj_Duo_MsgWait(this);
    } else {
        Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
    }
}

void Change_Skj_Duo_MsgWait(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_TALK);
}

void Skj_Duo_MsgWait(EnSkj* this, PlayState* play) {
    s32 pad;

    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        if (GET_ITEMGETINF(ITEMGETINF_16)) {
            Change_Skj_Talk_Wait(this);
        } else {
            Change_Skj_Duo_CarryStart(this);
            Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, Skj_TalkDistance(this),
                               Skj_TalkHight(this));
        }
    }
}

void Change_Skj_Duo_CarryStart(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_UNK14);
}

void Skj_Duo_CarryStart(EnSkj* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        Change_Skj_Duo_CarryFinish(this);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, Skj_TalkDistance(this), Skj_TalkHight(this));
    }
}

void Change_Skj_Duo_CarryFinish(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_CHANGE_MODE);
}

void Skj_Duo_CarryFinish(EnSkj* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        SET_ITEMGETINF(ITEMGETINF_16);
        Skj_profile_message_set(this);
        Change_Skj_Talk_Wait(this);
    }
}

void Change_Skj_Duo_BuyMaskTalk(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_START_TRADE);
}

void Skj_Duo_BuyMaskTalk(EnSkj* this, PlayState* play) {
    u8 sp1F = message_check(&play->msgCtx);

    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
    if ((sp1F == TEXT_STATE_DONE) && pad_on_check(play)) {
        Change_Skj_Duo_BuyMaskJump(this);
    }
}

void Change_Skj_Duo_BuyMaskJump(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speed = 2.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_BACKFLIP);
    Skeleton_Info_reverse(&this->skelAnime);
    this->skelAnime.curFrame = this->skelAnime.startFrame;
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING);
}

void Skj_Duo_BuyMaskJump(EnSkj* this, PlayState* play) {
    if (this->actor.velocity.y <= 0.0f) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
            this->actor.speed = 0.0f;
            Change_Skj_Duo_BuyMaskLanding(this);
        }
    }
}

void Change_Skj_Duo_BuyMaskLanding(EnSkj* this) {
    CHG_Skj_Animation(this, SKJ_ANIM_LAND);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM);
}

void Skj_Duo_BuyMaskLanding(EnSkj* this, PlayState* play) {
    s16 lastFrame = Si2_anime_end_frame(&gSkullKidLandAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        Change_Skj_Duo_BuyMaskWalk(this);
    }
}

void Change_Skj_Duo_BuyMaskWalk(EnSkj* this) {
    this->unk_2F0 = 0.0f;
    this->actor.speed = 2.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_WALK_TO_PLAYER);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER);
}

void Skj_Duo_BuyMaskWalk(EnSkj* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, this->unk_2F0, 0);
    add_calc2(&this->unk_2F0, 2000.0f, 1.0f, 100.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.xzDistToPlayer < 120.0f) {
        this->actor.speed = 0.0f;
        Change_Skj_Duo_BuyMask(this, play);
    }
}

void Change_Skj_Duo_BuyMask(EnSkj* this, PlayState* play) {
    message_set(play, 0x101C, &this->actor);
    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK);
}

void Skj_Duo_BuyMask(EnSkj* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                Change_Skj_Duo_BuyMaskFinish(this, play);
                break;
            case 1: // No
                message_set2(play, 0x101D);
                Change_Skj_Duo_BuyMaskEscape(this);
                break;
        }
    }
}

void Change_Skj_Duo_BuyMaskFinish(EnSkj* this, PlayState* play) {
    message_set2(play, 0x101E);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_TAKE_MASK);
}

void Skj_Duo_BuyMaskFinish(EnSkj* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        lupy_increase(10);
        SET_ITEMGETINF(ITEMGETINF_39);
        Skj_profile_message_set(this);
        mask_cancel(play);
        item_get_setting(play, ITEM_SOLD_OUT);
        message_set2(play, 0x101F);
        Change_Skj_Duo_BuyMaskEscape(this);
    }
}

void Change_Skj_Duo_BuyMaskEscape(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT);
}

void Skj_Duo_BuyMaskEscape(EnSkj* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        this->backflipFlag = 1;
        Change_Skj_Back_Jump(this);
    }
}

void Change_Skj_Duo_FailureTalkWait(EnSkj* this) {
    this->textId = 0x1041;
    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WRONG_SONG);
}

void Skj_Duo_FailureTalkWait(EnSkj* this, PlayState* play) {
    if (skj_talk_flag != 0) {
        Change_Skj_Duo_FailureTalk(this);
    } else {
        Actor_talk_request2(&this->actor, play, Skj_TalkDistance(this));
    }
}

void Change_Skj_Duo_FailureTalk(EnSkj* this) {
    CHG_Skj_Mode(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT);
}

void Skj_Duo_FailureTalk(EnSkj* this, PlayState* play) {
    u8 state = message_check(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if (state == TEXT_STATE_DONE && pad_on_check(play)) {
        Change_Skj_Talk_Wait(this);
        player->stateFlags2 |= PLAYER_STATE2_23;
        player->unk_6A8 = (Actor*)ocarina.skullkid;
    }
}

void Change_Skj_Trio_Wait(EnSkj* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER);
}

void Skj_Trio_Wait(EnSkj* this, PlayState* play) {
    if (this->playerInRange) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        CHG_Skj_Mode(this, SKJ_ACTION_OCARINA_GAME_IDLE);
    }
}

s32 Check_Skj_Trio_Ocarina_Escape(EnSkj* this) {
    s32 paramDecr = this->actor.params - 1;

    if (ocarinaT[paramDecr].unk_0 == 2) {
        Change_Skj_Trio_Ocarina_Escape(this);
        return true;
    }
    return false;
}

void Change_Skj_Trio_Ocarina_Wait(EnSkj* this) {
    CHG_Skj_Animation(this, SKJ_ANIM_WAIT);
    CHG_Skj_Mode(this, SKJ_ACTION_OCARINA_GAME_IDLE);
}

void Skj_Trio_Alpha_ON(EnSkj* this) {
    if (this->alpha != 255) {
        this->alpha += 20;

        if (this->alpha > 255) {
            this->alpha = 255;
        }
    }
}

void Skj_Trio_Ocarina_Wait(EnSkj* this, PlayState* play) {
    Skj_Trio_Alpha_ON(this);

    if (!Check_Skj_Trio_Ocarina_Escape(this) && (this->minigameState != 0)) {
        Change_Skj_Trio_Ocarina_Dance(this);
    }
}

void Change_Skj_Trio_Ocarina_Dance(EnSkj* this) {
    CHG_Skj_Animation(this, SKJ_ANIM_PLAY_FLUTE);
    CHG_Skj_Mode(this, SKJ_ACTION_OCARINA_GAME_PLAY);
}

void Skj_Trio_Ocarina_Dance(EnSkj* this, PlayState* play) {
    Skj_Trio_Alpha_ON(this);

    if (!Check_Skj_Trio_Ocarina_Escape(this) && (this->minigameState == 0)) {
        Change_Skj_Trio_Ocarina_Wait(this);
    }
}

void Change_Skj_Trio_Ocarina_Escape(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speed = -8.0f;
    CHG_Skj_Animation(this, SKJ_ANIM_BACKFLIP);
    CHG_Skj_Mode(this, SKJ_ACTION_OCARINA_GAME_LEAVE);
}

void Skj_Trio_Ocarina_Escape(EnSkj* this, PlayState* play) {
    s32 paramsDecr = this->actor.params - 1;

    ocarinaT[paramsDecr].unk_0 = 0;
    ocarinaT[paramsDecr].skullkid = NULL;
    this->backflipFlag = 1;
    Change_Skj_Back_Jump(this);
}

void En_Skj_actor_move(Actor* thisx, PlayState* play) {
    Vec3f dropPos;
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    skj_talk_flag = Actor_talk_check(&this->actor, play);

    this->timer++;

    if (this->multiuseTimer != 0) {
        this->multiuseTimer--;
    }

    if (this->needleShootTimer != 0) {
        this->needleShootTimer--;
    }

    if (this->dodgeResetTimer != 0) {
        this->dodgeResetTimer--;
    }

    if (this->dodgeResetTimer == 0) {
        this->hitsUntilDodge = 3;
    }

    if ((this->backflipFlag != 0) && (this->alpha == 0)) {
        if (this->action == 9) {
            dropPos.x = this->actor.world.pos.x;
            dropPos.y = this->actor.world.pos.y;
            dropPos.z = this->actor.world.pos.z;

            Item_set0(play, &dropPos, ITEM00_RUPEE_ORANGE);
        }
        Actor_delete(&this->actor);
        return;
    }

    Actor_world_to_eye(&this->actor, 30.0f);
    Actor_set_scale(&this->actor, 0.01f);
    this->actionFunc(this, play);
    this->actor.textId = this->textId;
    Skj_Cross(this, play);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if ((this->unk_2D3 != 0) && (skj_talk_flag == 0)) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);

        if (this->actor.colorFilterTimer == 0) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
}

void En_Skj_Ocarina_actor_move(Actor* thisx, PlayState* play) {
    EnSkj* this = (EnSkj*)thisx;

    skj_talk_flag = Actor_talk_check(&this->actor, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}

void Skj_OcarinaT_PlayerAngleSet(EnSkj* this, Player* player) {
    add_calc_short_angle2(&player->actor.shape.rot.y, this->actor.world.rot.y, 5, 2000, 0);
    player->actor.world.rot.y = player->actor.shape.rot.y;
    player->yaw = player->actor.shape.rot.y;
}

void Skj_OcarinaT_TalkWait(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CHK_Skj_Ocarina_Player_Distance(player, this)) {
        ocarinaT[SKULL_KID_LEFT].skullkid->playerInRange = true;
        ocarinaT[SKULL_KID_RIGHT].skullkid->playerInRange = true;

        if (player->stateFlags2 & PLAYER_STATE2_24) {
            player->stateFlags2 |= PLAYER_STATE2_25;
            Na_StartForceNatureSound(NATURE_ID_KOKIRI_REGION);
            Skj_OcarinaT_PlayerAngleSet(this, player);
            player->unk_6A8 = &this->actor;
            message_set(play, 0x10BE, &this->actor);
            this->actionFunc = Skj_OcarinaT_Talk;
        } else {
            this->actionFunc = Skj_OcarinaT_TalkWait2;
        }
    }
}

void Skj_OcarinaT_TalkWait2(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_24) {
        player->stateFlags2 |= PLAYER_STATE2_25;
        Na_StartForceNatureSound(NATURE_ID_KOKIRI_REGION);
        Skj_OcarinaT_PlayerAngleSet(this, player);
        player->unk_6A8 = &this->actor;
        message_set(play, 0x10BE, &this->actor);
        this->actionFunc = Skj_OcarinaT_Talk;
    } else if (CHK_Skj_Ocarina_Player_Distance(player, this)) {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void Skj_OcarinaT_Talk(EnSkj* this, PlayState* play) {
    u8 dialogState = message_check(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    Skj_OcarinaT_PlayerAngleSet(this, player);

    if (dialogState == TEXT_STATE_CLOSING) {
        ocarina_set(play, OCARINA_ACTION_MEMORY_GAME);
        if (ocarinaT[SKULL_KID_LEFT].skullkid != NULL) {
            ocarinaT[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_PLAY_NOTES;
        }
        this->songFailTimer = 160;
        this->actionFunc = Skj_OcarinaT_Play_1;
    }
}

void Skj_OcarinaT_Play_1(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skj_OcarinaT_PlayerAngleSet(this, player);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) { // failed the game
        message_close(play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &this->actor;
        Actor_talk_request2(&this->actor, play, 26.0f);
        this->textId = 0x102D;
        this->actionFunc = Skj_OcarinaT_Failure;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_0F) { // completed the game
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        message_close(play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &this->actor;
        Actor_talk_request2(&this->actor, play, 26.0f);
        this->textId = 0x10BF;
        this->actionFunc = Skj_OcarinaT_Success;
    } else { // playing the game
        switch (play->msgCtx.msgMode) {
            case MSGMODE_MEMORY_GAME_LEFT_SKULLKID_WAIT:
                if (ocarinaT[SKULL_KID_LEFT].skullkid != NULL) {
                    ocarinaT[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Nai_FxFlagCheck(NA_SE_SY_METRONOME)) {
                    if (ocarinaT[SKULL_KID_RIGHT].skullkid != NULL) {
                        ocarinaT[SKULL_KID_RIGHT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    ocarina_round_next(play);
                }
                break;
            case MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_WAIT:
                if (ocarinaT[SKULL_KID_RIGHT].skullkid != NULL) {
                    ocarinaT[SKULL_KID_RIGHT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Nai_FxFlagCheck(NA_SE_SY_METRONOME)) {
                    ocarina_round_next(play);
                    this->songFailTimer = 160;
                }
                break;
            case MSGMODE_MEMORY_GAME_PLAYER_PLAYING:
                if (this->songFailTimer != 0) {
                    this->songFailTimer--;
                } else { // took too long, game failed
                    Na_StartSystemSe_F(NA_SE_SY_OCARINA_ERROR);
                    message_close(play);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    Actor_talk_request2(&this->actor, play, 26.0f);
                    this->textId = 0x102D;
                    this->actionFunc = Skj_OcarinaT_Failure;
                }
                break;
            case MSGMODE_MEMORY_GAME_START_NEXT_ROUND:
                if (!Nai_FxFlagCheck(NA_SE_SY_METRONOME)) {
                    if (ocarinaT[SKULL_KID_LEFT].skullkid != NULL) {
                        ocarinaT[SKULL_KID_LEFT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    this->songFailTimer = 160;
                    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_FLUTE);
                    Na_SetOcarinaSeq(OCARINA_SONG_MEMORY_GAME + 1, 1);
                    play->msgCtx.msgMode = MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING;
                    play->msgCtx.stateTimer = 2;
                }
                break;
        }
    }
}

void Skj_OcarinaT_Failure(EnSkj* this, PlayState* play) {
    if (skj_talk_flag) {
        this->actionFunc = Skj_OcarinaT_FailureTalkWait;
    } else {
        Actor_talk_request2(&this->actor, play, 26.0f);
    }
}

void Skj_OcarinaT_FailureTalkWait(EnSkj* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        Change_Skj_OcarinaT_FailureTalkEnd(this, play);
    }
}

void Change_Skj_OcarinaT_FailureTalkEnd(EnSkj* this, PlayState* play) {
    message_set2(play, 0x102E);
    this->actionFunc = Skj_OcarinaT_FailureTalkEnd;
}

void Skj_OcarinaT_FailureTalkEnd(EnSkj* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: { // yes
                Player* player = GET_PLAYER(play);

                player->stateFlags3 |= PLAYER_STATE3_5; // makes player take ocarina out right away after closing box
                this->actionFunc = Skj_OcarinaT_TalkWait;
                break;
            }
            case 1: // no
                this->actionFunc = Skj_OcarinaT_Finish;
                break;
        }
    }
}

void Skj_OcarinaT_Success(EnSkj* this, PlayState* play) {
    if (skj_talk_flag) {
        this->actionFunc = Skj_OcarinaT_SuccessTalkWait;
    } else {
        Actor_talk_request2(&this->actor, play, 26.0f);
    }
}

void Skj_OcarinaT_SuccessTalkWait(EnSkj* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        Actor_carry_request_set2(&this->actor, play,
                           Trio_PresentItem[z_common_data.save.info.playerData.ocarinaGameRoundNum], 26.0f, 26.0f);
        this->actionFunc = Skj_OcarinaT_SuccessCarryWait;
    }
}

void Skj_OcarinaT_SuccessCarryWait(EnSkj* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = Skj_OcarinaT_SuccessCarryFinish;
    } else {
        Actor_carry_request_set2(&this->actor, play,
                           Trio_PresentItem[z_common_data.save.info.playerData.ocarinaGameRoundNum], 26.0f, 26.0f);
    }
}

void Skj_OcarinaT_SuccessCarryFinish(EnSkj* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        s32 ocarinaGameRoundNum = z_common_data.save.info.playerData.ocarinaGameRoundNum;

        if (z_common_data.save.info.playerData.ocarinaGameRoundNum < 3) {
            z_common_data.save.info.playerData.ocarinaGameRoundNum++;
        }

        if (ocarinaGameRoundNum == 2) {
            SET_ITEMGETINF(ITEMGETINF_17);
            this->actionFunc = Skj_OcarinaT_Finish;
        } else {
            Change_Skj_OcarinaT_FailureTalkEnd(this, play);
        }
    }
}

void Skj_OcarinaT_Finish(EnSkj* this, PlayState* play) {
    if (ocarinaT[SKULL_KID_LEFT].skullkid != NULL) {
        ocarinaT[SKULL_KID_LEFT].unk_0 = 2;
    }

    if (ocarinaT[SKULL_KID_RIGHT].skullkid != NULL) {
        ocarinaT[SKULL_KID_RIGHT].unk_0 = 2;
    }

    if ((ocarinaT[SKULL_KID_LEFT].unk_0 == 2) &&
        (ocarinaT[SKULL_KID_RIGHT].unk_0 == 2)) {
        Na_StopForceNatureSound();
        Actor_delete(&this->actor);
    }
}

void En_Skj_OcarinaT_actor_move(Actor* thisx, PlayState* play) {
    EnSkj* this = (EnSkj*)thisx;

    skj_talk_flag = Actor_talk_check(&this->actor, play);
    this->timer++;

    this->actor.focus.pos.x = 1230.0f;
    this->actor.focus.pos.y = -90.0f;
    this->actor.focus.pos.z = 450.0f;

#if DEBUG_FEATURES
    if (BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
#endif

    this->actionFunc(this, play);

    this->actor.textId = this->textId;
    this->actor.xzDistToPlayer = 50.0;
}

s32 Skj_Display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return 0;
}

void Skj_Display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_skj.c", 2417);

    if ((limbIndex == 11) && GET_ITEMGETINF(ITEMGETINF_39)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Matrix_push();
        Matrix_rotateXYZ(-0x4000, 0, 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_skj.c", 2430);
        gSPDisplayList(POLY_OPA_DISP++, gSkullKidSkullMaskDL);
        Matrix_pull();
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_skj.c", 2437);
}

Gfx* skj_xlu(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 3
    dList = dListHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* skj_opa(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 2
    dList = dListHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void En_Skj_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_skj.c", 2475);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->alpha < 255) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, skj_xlu(play->state.gfxCtx, this->alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, skj_opa(play->state.gfxCtx, this->alpha));
    }

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          Skj_Display1, Skj_Display2, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_skj.c", 2495);
}
