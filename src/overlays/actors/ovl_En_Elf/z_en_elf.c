/*
 * File: z_en_elf.c
 * Overlay: ovl_En_Elf
 * Description: Fairy
 */

#include "z_en_elf.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

#define FAIRY_FLAG_TIMED (1 << 8)
#define FAIRY_FLAG_BIG (1 << 9)

void En_Elf_actor_ct(Actor* thisx, PlayState* play);
void En_Elf_actor_dt(Actor* thisx, PlayState* play);
void En_Elf_actor_move(Actor* thisx, PlayState* play);
void En_Elf_actor_draw(Actor* thisx, PlayState* play);
void En_Elf_actor_move_link(Actor* thisx, PlayState* play);
void En_Elf_actor_move_talk(Actor* thisx, PlayState* play);
void En_Elf_actor_move_talk_naby0(Actor* thisx, PlayState* play);
void En_Elf_actor_move_talk_saria0(Actor* thisx, PlayState* play);
void En_Elf_actor_move_talk_saria1(Actor* thisx, PlayState* play);
void En_Elf_actor_move_talk_saria2(Actor* thisx, PlayState* play);

// Navi
static void move_normal(EnElf* this, PlayState* play);

// Healing Fairies
void move_floating(EnElf* this, PlayState* play);
void move_runaway(EnElf* this, PlayState* play);

// Healing Fairies Revive From Death
void move_regenerate(EnElf* this, PlayState* play);
void move_regenerate1(EnElf* this, PlayState* play);

// Kokiri Fairies
void move_option(EnElf* this, PlayState* play);

// Fairy Spawner
static void move_stop(EnElf* this, PlayState* play);

// Move(?) functions
void floating_mode_check(EnElf* this, PlayState* play);
void check_revise_circle2(EnElf* this, PlayState* play);
void check_revise_circle(EnElf* this, PlayState* play);
void check_revise_omega(EnElf* this, PlayState* play);
void runaway_mode_check(EnElf* this, PlayState* play);
void interest_mode_check(EnElf* this, PlayState* play);

// misc
void elf_egg_color_check(EnElf* this, PlayState* play);
void check_mode_z_en_elf(EnElf* this, PlayState* play);
void En_Elf_set_kirakira(EnElf* this, PlayState* play, s32 sparkleLife);
void DemoEffMoveProc_global(Vec3f* dest, PlayState* play, s32 cueChannel);

ActorProfile En_Elf_Profile = {
    /**/ ACTOR_EN_ELF,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnElf),
    /**/ En_Elf_actor_ct,
    /**/ En_Elf_actor_dt,
    /**/ En_Elf_actor_move,
    /**/ En_Elf_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 8, ICHAIN_STOP),
};

static Color_RGBAf status_color[] = {
    { 255.0f, 255.0f, 255.0f, 255.0f },
    { 255.0f, 220.0f, 220.0f, 255.0f },
};

static Color_RGBAf status_color_side[] = {
    { 255.0f, 255.0f, 255.0f, 255.0f },
    { 255.0f, 50.0f, 100.0f, 255.0f },
};

typedef struct FairyColorFlags {
    u8 r, g, b;
} FairyColorFlags;

static FairyColorFlags color_table[] = {
    { 0, 0, 0 }, { 1, 0, 0 }, { 1, 2, 0 }, { 1, 0, 2 }, { 0, 1, 0 }, { 2, 1, 0 }, { 0, 1, 2 },
    { 0, 0, 1 }, { 2, 0, 1 }, { 0, 2, 1 }, { 1, 1, 0 }, { 1, 0, 1 }, { 0, 1, 1 },
};

void En_Elf_actor_set_process(EnElf* this, EnElfActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Elf_actor_set_status(EnElf* this, s32 arg1) {
    this->unk_2A8 = arg1;

    switch (this->unk_2A8) {
        case 0:
            this->unk_2AE = 0x400;
            this->unk_2B0 = 0x200;
            this->func_2C8 = check_revise_circle;
            this->unk_2C0 = 100;
            this->unk_2B4 = 5.0f;
            this->unk_2B8 = 20.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 12:
            this->unk_2AE = 0x400;
            this->unk_2B0 = 0x200;
            this->func_2C8 = check_revise_circle;
            this->unk_2C0 = 100;
            this->unk_2B4 = 1.0f;
            this->unk_2B8 = 5.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 10:
            this->unk_2AE = 0x400;
            this->unk_2B0 = 0;
            this->func_2C8 = check_revise_circle;
            this->unk_2B8 = 0.0f;
            this->unk_2B4 = 5.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 9:
            this->unk_2AE = 0x1000;
            this->unk_2B0 = 0x200;
            this->func_2C8 = check_revise_circle;
            this->unk_2B4 = 3.0f;
            this->unk_2B8 = 10.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 7:
            this->func_2C8 = check_revise_circle;
            this->unk_2AE = 0x1E;
            this->unk_2C0 = 1;
            this->unk_2B4 = 0.0f;
            this->unk_2B8 = 0.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 8:
            this->unk_2AE = 0x1000;
            this->unk_2B0 = 0x200;
            this->func_2C8 = check_revise_circle;
            this->unk_2B4 = 0.0f;
            this->unk_2B8 = 0.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 1:
            this->unk_2AE = 0x1000;
            this->unk_2B0 = 0x800;
            this->func_2C8 = check_revise_circle;
            this->unk_2B4 = 5.0f;
            this->unk_2B8 = 7.5f;
            this->skelAnime.playSpeed = 2.0f;
            break;
        case 2:
            this->unk_2AE = 0x400;
            this->unk_2B0 = 0x1000;
            this->func_2C8 = check_revise_circle2;
            this->unk_2B4 = 10.0f;
            this->unk_2B8 = 20.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 3:
            this->unk_2B0 = 0x600;
            this->func_2C8 = check_revise_omega;
            this->unk_2B8 = 1.0f;
            this->unk_2B4 = 1.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
        case 4:
            this->unk_2B0 = 0x800;
            this->func_2C8 = check_revise_omega;
            this->unk_2B4 = 20.0f;
            this->unk_2B8 = 10.0;
            this->skelAnime.playSpeed = 2.0f;
            break;
        case 5:
            this->unk_2B0 = 0x200;
            this->func_2C8 = check_revise_omega;
            this->unk_2B4 = 10.0f;
            this->unk_2B8 = 10.0f;
            this->skelAnime.playSpeed = 0.5f;
            break;
        case 6:
            this->unk_2AE = 0x1000;
            this->unk_2B0 = 0x800;
            this->func_2C8 = check_revise_circle;
            this->unk_2B4 = 60.0f;
            this->unk_2B8 = 20.0f;
            this->skelAnime.playSpeed = 2.0f;
            break;
        case 11:
            this->unk_2AE = 0x400;
            this->unk_2B0 = 0x2000;
            this->func_2C8 = check_revise_circle;
            this->unk_2C0 = 42;
            this->unk_2B4 = 5.0f;
            this->unk_2B8 = 1.0f;
            this->skelAnime.playSpeed = 1.0f;
            break;
    }
}

static s32 goto_home_check(Vec3f* this, Vec3f* arg1, f32 arg2) {
    return SQ(arg2) < (SQ(arg1->x - this->x) + SQ(arg1->z - this->z));
}

void interest_mode_check(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!goto_home_check(&this->actor.world.pos, &player->actor.world.pos, 30.0f)) {
        this->unk_2B8 = 0.5f;
    } else {
        this->unk_2B8 = 2.0f;
    }

    if (this->unk_2C0 > 0) {
        this->unk_2C0--;
    } else {
        this->unk_2A8 = 1;
        this->unk_2AC = 0x80;
        this->unk_2B8 = rnd_f(1.0f) + 0.5f;
        this->unk_2B0 = rnd_fx(32767.0f);
        this->func_2C8 = floating_mode_check;
    }
}

void runaway_mode_check(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (goto_home_check(&this->actor.world.pos, &player->actor.world.pos, 50.0f)) {
        if (this->unk_2C0 > 0) {
            this->unk_2C0--;
        } else {
            this->unk_2A8 = 1;
            this->unk_2AC = 0x80;
            this->unk_2B8 = rnd_f(1.0f) + 0.5f;
            this->unk_2B0 = rnd_fx(32767.0f);
            this->func_2C8 = floating_mode_check;
        }
    }
}

void floating_mode_check(EnElf* this, PlayState* play) {
    f32 xzDistToPlayer;

    if (this->unk_2C0 > 0) {
        this->unk_2C0--;
    } else {
        xzDistToPlayer = this->actor.xzDistToPlayer;
        if (xzDistToPlayer < 50.0f) {
            if (fqrand() < 0.2f) {
                this->unk_2A8 = 2;
                this->unk_2AC = 0x400;
                this->unk_2B8 = 2.0f;
                this->func_2C8 = runaway_mode_check;
                this->actor.speed = 1.5f;
                this->unk_2C0 = (s16)rnd_f(8.0f) + 4;
            } else {
                this->unk_2C0 = 10;
            }
        } else {
            if (xzDistToPlayer > 150.0f) {
                xzDistToPlayer = 150.0f;
            }

            xzDistToPlayer = ((xzDistToPlayer - 50.0f) * 0.95f) + 0.05f;

            if (fqrand() < xzDistToPlayer) {
                this->unk_2A8 = 3;
                this->unk_2AC = 0x200;
                this->unk_2B8 = (xzDistToPlayer * 2.0f) + 1.0f;
                this->func_2C8 = interest_mode_check;
                this->unk_2C0 = (s16)rnd_f(16.0f) + 0x10;
            } else {
                this->unk_2C0 = 10;
            }
        }
    }

    if (fqrand() < 0.1f) {
        this->unk_2A8 = 1;
        this->unk_2AC = 0x80;
        this->unk_2B8 = rnd_f(0.5f) + 0.5f;
        this->unk_2B0 = rnd_fx(32767.0f);
    }
}

void elf_mode_change_floating(EnElf* this, PlayState* play) {
    if (goto_home_check(&this->unk_28C, &this->actor.world.pos, 100.0f)) {
        this->unk_2A8 = 0;
        this->unk_2AC = 0x200;
        this->func_2C8 = floating_mode_check;
        this->unk_2B8 = 1.5f;
    } else {
        this->func_2C8(this, play);
    }
}

f32 rnd_color(s32 colorFlag) {
    switch (colorFlag) {
        case 1:
            return rnd_f(55.0f) + 200.0f;
        case 2:
            return rnd_f(255.0f);
        default:
            return 0.0f;
    }
}

void En_Elf_actor_ct(Actor* thisx, PlayState* play) {
    EnElf* this = (EnElf*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 colorConfig;
    s32 i;

    ValueSet_process(thisx, value_init);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gFairySkel, &gFairyAnim, this->jointTable, this->morphTable, 15);
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 15.0f);
    thisx->shape.shadowAlpha = 0xFF;

    Light_point2_ct(&this->lightInfoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 255, 255,
                            255, 0);
    this->lightNodeGlow = Global_light_list_new(play, &play->lightCtx, &this->lightInfoGlow);

    Light_point_ct(&this->lightInfoNoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 255,
                              255, 255, 0);
    this->lightNodeNoGlow = Global_light_list_new(play, &play->lightCtx, &this->lightInfoNoGlow);

    this->fairyFlags = 0;
    this->disappearTimer = 600;
    this->unk_2A4 = 0.0f;
    colorConfig = 0;

    switch (thisx->params) {
        case FAIRY_NAVI:
            thisx->room = -1;
            En_Elf_actor_set_process(this, move_normal);
            En_Elf_actor_set_status(this, 0);
            this->fairyFlags |= 4;
            thisx->update = En_Elf_actor_move_link;
            this->elfMsg = NULL;
            this->unk_2C7 = 0x14;

            if ((z_common_data.save.info.playerData.naviTimer >= 25800) ||
                (z_common_data.save.info.playerData.naviTimer < 3000)) {
                z_common_data.save.info.playerData.naviTimer = 0;
            }
            break;
        case FAIRY_REVIVE_BOTTLE:
            colorConfig = -1;
            En_Elf_actor_set_process(this, move_runaway);
            this->unk_2B8 = search_position_distanceXZ(&thisx->world.pos, &player->actor.world.pos);
            this->unk_2AC = player->actor.shape.rot.y;
            this->unk_2B0 = -0x1000;
            this->unk_28C.y = thisx->world.pos.y - player->actor.world.pos.y;
            this->unk_2AA = 0;
            this->unk_2B4 = 0.0f;
            break;
        case FAIRY_REVIVE_DEATH:
            colorConfig = -1;
            En_Elf_actor_set_process(this, move_regenerate);
            this->unk_2B8 = 0.0f;
            this->unk_2AC = player->actor.shape.rot.y;
            this->unk_2B0 = 0;
            this->unk_28C.y = thisx->world.pos.y - player->actor.world.pos.y;
            this->unk_2AA = 0;
            this->unk_2B4 = 7.0f;
            break;
        case FAIRY_HEAL_BIG:
            this->fairyFlags |= FAIRY_FLAG_BIG;
            thisx->shape.shadowDraw = Actor_shadow_white_circle;
            FALLTHROUGH;
        case FAIRY_HEAL_TIMED:
            this->fairyFlags |= FAIRY_FLAG_TIMED;
            FALLTHROUGH;
        case FAIRY_HEAL:
            colorConfig = -1;
            En_Elf_actor_set_process(this, move_floating);
            this->unk_2B4 = rnd_f(10.0f) + 10.0f;
            this->unk_2AA = 0;
            this->unk_2AE = (s16)(rnd_f(1048.0f)) + 0x200;
            this->unk_28C = thisx->world.pos;
            this->unk_2BC = rnd_fx(32767.0f);
            this->func_2C8 = floating_mode_check;
            elf_mode_change_floating(this, play);
            this->unk_2C0 = 0;
            this->disappearTimer = 240;
            break;
        case FAIRY_KOKIRI:
            colorConfig = rnd_f(11.99f) + 1.0f;
            En_Elf_actor_set_process(this, move_option);
            En_Elf_actor_set_status(this, 0);
            break;
        case FAIRY_SPAWNER:
            En_Elf_actor_set_process(this, move_stop);
            En_Elf_actor_set_status(this, 8);

            for (i = 0; i < 8; i++) {
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, thisx->world.pos.x, thisx->world.pos.y - 30.0f,
                            thisx->world.pos.z, 0, 0, 0, FAIRY_HEAL);
            }
            break;
        default:
            ASSERT(0, "0", "../z_en_elf.c", 1103);
            break;
    }

    this->unk_2A0 = 3.0f;
    this->innerColor = status_color[0];

    if (colorConfig > 0) {
        this->outerColor.r = rnd_color(color_table[colorConfig].r);
        this->outerColor.g = rnd_color(color_table[colorConfig].g);
        this->outerColor.b = rnd_color(color_table[colorConfig].b);
        this->outerColor.a = 0.0f;
    } else {
        this->innerColor = status_color[-colorConfig];
        this->outerColor = status_color_side[-colorConfig];
    }
}

void down_tough(EnElf* this, s32 arg1) {
}

void up_tough(EnElf* this, s16 increment) {
    if (this->disappearTimer < 600) {
        this->disappearTimer += increment;
    }
}

void En_Elf_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnElf* this = (EnElf*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNodeGlow);
    Global_light_list_delete(play, &play->lightCtx, this->lightNodeNoGlow);
}

void check_revise_circle(EnElf* this, PlayState* play) {
    this->unk_28C.x = sin_s(this->unk_2AC) * this->unk_2B8;
    this->unk_28C.y = sin_s(this->unk_2AA) * this->unk_2B4;
    this->unk_28C.z = cos_s(this->unk_2AC) * this->unk_2B8;
    this->unk_2AC += this->unk_2B0;
    this->unk_2AA += this->unk_2AE;
}

void check_revise_circle2(EnElf* this, PlayState* play) {
    f32 xzScale;

    xzScale = (cos_s(this->unk_2AA) * this->unk_2B4) + this->unk_2B8;

    this->unk_28C.x = sin_s(this->unk_2AC) * xzScale;
    this->unk_28C.y = 0.0f;
    this->unk_28C.z = cos_s(this->unk_2AC) * xzScale;

    this->unk_2AC += this->unk_2B0;
    this->unk_2AA += this->unk_2AE;
}

void check_revise_omega(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_2AA = (this->unk_2AC * 2) & 0xFFFF;
    this->unk_28C.x = sin_s(this->unk_2AC) * this->unk_2B8;
    this->unk_28C.y = sin_s(this->unk_2AA) * this->unk_2B4;
    this->unk_28C.z = -sin_s(player->actor.shape.rot.y) * this->unk_28C.x;
    this->unk_28C.x = cos_s(player->actor.shape.rot.y) * this->unk_28C.x;
    this->unk_2AC += this->unk_2B0;
}

void chase_position_y(EnElf* this, Vec3f* targetPos, f32 arg2) {
    f32 yVelTarget;
    f32 yVelDirection;

    yVelTarget = ((targetPos->y + this->unk_28C.y) - this->actor.world.pos.y) * arg2;
    yVelDirection = (yVelTarget >= 0.0f) ? 1.0f : -1.0f;
    yVelTarget = fabsf(yVelTarget);
    yVelTarget = CLAMP(yVelTarget, 0.0f, 20.0f) * yVelDirection;
    chase_f(&this->actor.velocity.y, yVelTarget, 32.0f);
}

void chase_position(EnElf* this, Vec3f* targetPos, f32 arg2) {
    f32 xVelTarget;
    f32 zVelTarget;
    f32 xVelDirection;
    f32 zVelDirection;

    xVelTarget = ((targetPos->x + this->unk_28C.x) - this->actor.world.pos.x) * arg2;
    zVelTarget = ((targetPos->z + this->unk_28C.z) - this->actor.world.pos.z) * arg2;

    xVelDirection = (xVelTarget >= 0.0f) ? 1.0f : -1.0f;
    zVelDirection = (zVelTarget >= 0.0f) ? 1.0f : -1.0f;

    xVelTarget = fabsf(xVelTarget);
    zVelTarget = fabsf(zVelTarget);

    xVelTarget = CLAMP(xVelTarget, 0.0f, 20.0f) * xVelDirection;
    zVelTarget = CLAMP(zVelTarget, 0.0f, 20.0f) * zVelDirection;

    chase_position_y(this, targetPos, arg2);
    chase_f(&this->actor.velocity.x, xVelTarget, 1.5f);
    chase_f(&this->actor.velocity.z, zVelTarget, 1.5f);
    Actor_position_move(&this->actor);
}

void chase_position5(EnElf* this, Vec3f* targetPos) {
    chase_position_y(this, targetPos, 0.2f);
    this->actor.velocity.x = (targetPos->x + this->unk_28C.x) - this->actor.world.pos.x;
    this->actor.velocity.z = (targetPos->z + this->unk_28C.z) - this->actor.world.pos.z;
    Actor_position_move(&this->actor);
    this->actor.world.pos.x = targetPos->x + this->unk_28C.x;
    this->actor.world.pos.z = targetPos->z + this->unk_28C.z;
}

void chase_position2(EnElf* this, Vec3f* targetPos) {
    chase_position_y(this, targetPos, 0.2f);
    this->actor.velocity.x = this->actor.velocity.z = 0.0f;
    Actor_position_move(&this->actor);
    this->actor.world.pos.x = targetPos->x + this->unk_28C.x;
    this->actor.world.pos.z = targetPos->z + this->unk_28C.z;
}

void chase_up_down_position(EnElf* this, Vec3f* targetPos) {
    f32 yVelTarget;
    f32 yVelDirection;

    yVelTarget = (((sin_s(this->unk_2AA) * this->unk_2B4) + targetPos->y) - this->actor.world.pos.y) * 0.2f;
    yVelDirection = (yVelTarget >= 0.0f) ? 1.0f : -1.0f;
    this->unk_2AA += this->unk_2AE;
    yVelTarget = fabsf(yVelTarget);
    yVelTarget = CLAMP(yVelTarget, 0.0f, 20.0f) * yVelDirection;
    chase_f(&this->actor.velocity.y, yVelTarget, 1.5f);
}

void chase_position3(EnElf* this, PlayState* play) {
    s32 pad[2];
    Player* player = GET_PLAYER(play);
    s16 targetYaw;
    Vec3f* unk_28C = &this->unk_28C;

    add_calc(&this->actor.speed, this->unk_2B8, 0.2f, 0.5f, 0.01f);

    switch (this->unk_2A8) {
        case 0:
            targetYaw = atans_table(-(this->actor.world.pos.z - unk_28C->z), -(this->actor.world.pos.x - unk_28C->x));
            break;

        case 3:
            targetYaw = atans_table(-(this->actor.world.pos.z - player->actor.world.pos.z),
                                    -(this->actor.world.pos.x - player->actor.world.pos.x));
            break;

        case 2:
            targetYaw = atans_table(this->actor.world.pos.z - player->actor.world.pos.z,
                                    this->actor.world.pos.x - player->actor.world.pos.x);
            break;

        default:
            targetYaw = this->unk_2B0;
            break;
    }

    add_calc_short_angle2(&this->unk_2BC, targetYaw, 10, this->unk_2AC, 0x20);
    this->actor.world.rot.y = this->unk_2BC;
    Actor_position_moveF(&this->actor);
}

void chase_position4(EnElf* this, Vec3f* arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 xVelTarget;
    f32 zVelTarget;
    f32 xzVelocity;
    f32 clampedXZ;

    xVelTarget = ((arg1->x + this->unk_28C.x) - this->actor.world.pos.x) * arg4;
    zVelTarget = ((arg1->z + this->unk_28C.z) - this->actor.world.pos.z) * arg4;
    arg4 += 0.3f;
    arg3 += 30.0f;

    chase_position_y(this, arg1, arg4);

    xzVelocity = sqrtf(SQ(xVelTarget) + SQ(zVelTarget));

    this->actor.speed = clampedXZ = CLAMP(xzVelocity, arg2, arg3);

    if ((xzVelocity != clampedXZ) && (xzVelocity != 0.0f)) {
        xzVelocity = clampedXZ / xzVelocity;
        xVelTarget *= xzVelocity;
        zVelTarget *= xzVelocity;
    }

    chase_f(&this->actor.velocity.x, xVelTarget, 5.0f);
    chase_f(&this->actor.velocity.z, zVelTarget, 5.0f);
    Actor_position_move(&this->actor);
}

void move_floating(EnElf* this, PlayState* play) {
    Player* refActor = GET_PLAYER(play);
    s32 pad;
    Player* player = GET_PLAYER(play);
    f32 heightDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (fqrand() < 0.05f) {
        this->unk_2B4 = rnd_f(10.0f) + 10.0f;
        this->unk_2AE = (s16)(rnd_f(1024.0f)) + 0x200;
    }

    elf_mode_change_floating(this, play);
    this->unk_28C.y = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y;
    chase_up_down_position(this, &this->unk_28C);
    chase_position3(this, play);

    if ((this->unk_2A8 == 2) || (this->unk_2A8 == 3)) {
        En_Elf_set_kirakira(this, play, 16);
    }

    if (Actor_carry_check(&this->actor, play)) {
        Actor_delete(&this->actor);
        return;
    }

    if (!player_demo_check(play)) {
        heightDiff = this->actor.world.pos.y - refActor->actor.world.pos.y;

        if ((heightDiff > 0.0f) && (heightDiff < 60.0f)) {
            if (!goto_home_check(&this->actor.world.pos, &refActor->actor.world.pos, 10.0f)) {
                life_meter_play(play, 128);
                if (this->fairyFlags & FAIRY_FLAG_BIG) {
                    magic_mode_check(play);
                }
                this->unk_2B8 = 50.0f;
                this->unk_2AC = refActor->actor.shape.rot.y;
                this->unk_2B0 = -0x1000;
                this->unk_28C.y = 30.0f;
                this->unk_2B4 = 0.0f;
                this->unk_2AA = 0;
                En_Elf_actor_set_process(this, move_runaway);
                return;
            }
        }

        if (this->fairyFlags & FAIRY_FLAG_TIMED) {
            if (this->disappearTimer > 0) {
                this->disappearTimer--;
            } else {
                this->disappearTimer--;

                if (this->disappearTimer > -10) {
                    Actor_set_scale(&this->actor, ((this->disappearTimer + 10) * 0.008f) * 0.1f);
                } else {
                    Actor_delete(&this->actor);
                    return;
                }
            }
        }

        if (!(this->fairyFlags & FAIRY_FLAG_BIG)) {
            // GI_MAX in this case allows the player to catch the actor in a bottle
            Actor_carry_request_set2(&this->actor, play, GI_MAX, 80.0f, 60.0f);
        }
    }
}

void move_option(EnElf* this, PlayState* play) {
    Vec3f parentPos;
    Actor* parent;

    Skeleton_Info2_anime_play(&this->skelAnime);
    check_revise_circle(this, play);
    parent = this->actor.parent;

    if ((parent != NULL) && (parent->update != NULL)) {
        parentPos = this->actor.parent->world.pos;
        parentPos.y += ((1500.0f * this->actor.scale.y) + 40.0f);
        chase_position(this, &parentPos, 0.2f);
    } else {
        Actor_delete(&this->actor);
    }

    this->unk_2BC = atans_table(this->actor.velocity.z, this->actor.velocity.x);
}

static void move_stop(EnElf* this, PlayState* play) {
}

void move_runaway(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc(&this->unk_2B8, 30.0f, 0.1f, 4.0f, 1.0f);

    this->unk_28C.x = cos_s(this->unk_2AC) * this->unk_2B8;
    this->unk_28C.y += this->unk_2B4;

    switch (this->unk_2AA) {
        case 0:
            if (this->unk_2B4 < 2.0f) {
                this->unk_2B4 += 0.1f;
            } else {
                this->unk_2AA++;
            }
            break;
        case 1:
            if (this->unk_2B4 > -1.0f) {
                this->unk_2B4 -= 0.2f;
            }
    }

    this->unk_28C.z = sin_s(this->unk_2AC) * -this->unk_2B8;
    this->unk_2AC += this->unk_2B0;
    chase_position(this, &player->actor.world.pos, 0.2f);

    if (this->unk_2B4 < 0.0f) {
        if ((this->unk_28C.y < 20.0f) && (this->unk_28C.y > 0.0f)) {
            Actor_set_scale(&this->actor, (this->unk_28C.y * 0.008f) * 0.05f);
        }
    }

    if (this->unk_28C.y < -10.0f) {
        Actor_delete(&this->actor);
        return;
    }

    this->unk_2BC = atans_table(this->actor.velocity.z, this->actor.velocity.x);
    En_Elf_set_kirakira(this, play, 32);
    Actor_SE_set(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void move_regenerate1(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_28C.y > 200.0f) {
        Actor_delete(&this->actor);
        return;
    }

    if (this->unk_2AE >= 0x7E) {
        this->unk_2B8 += 0.1f;
        this->unk_2B4 += 0.5f;
        this->unk_28C.y += this->unk_2B4;
    } else {
        this->unk_2AE++;

        if (this->unk_2B8 < 30.0f) {
            this->unk_2B8 += 0.5f;
        }

        if (this->unk_28C.y > 0.0f) {
            this->unk_28C.y -= 0.7f;
        }
    }

    this->unk_28C.x = cos_s(this->unk_2AC) * this->unk_2B8;
    this->unk_28C.z = sin_s(this->unk_2AC) * -this->unk_2B8;
    this->unk_2AC += this->unk_2B0;
    chase_position5(this, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    this->unk_2BC = atans_table(this->actor.velocity.z, this->actor.velocity.x);
    En_Elf_set_kirakira(this, play, 32);
    Actor_SE_set(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void move_regenerate(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);

    this->unk_28C.z = 0.0f;
    this->unk_28C.x = 0.0f;
    this->unk_28C.y += this->unk_2B4;
    this->unk_2B4 -= 0.35f;

    if (this->unk_2B4 <= 0.0f) {
        En_Elf_actor_set_process(this, move_regenerate1);
        this->unk_2B0 = 0x800;
        this->unk_2AE = 0;
        this->unk_2B4 = 0.0f;
        this->unk_2B8 = 1.0f;
    }

    chase_position5(this, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    Actor_set_scale(&this->actor, (1.0f - (SQ(this->unk_2B4) * SQ(1.0f / 9.0f))) * 0.008f);
    this->unk_2BC = atans_table(this->actor.velocity.z, this->actor.velocity.x);
    En_Elf_set_kirakira(this, play, 32);
    Actor_SE_set(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void move_common_before(EnElf* this, PlayState* play) {
    if (this->fairyFlags & 4) {
        elf_egg_color_check(this, play);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->func_2C8 == NULL) {
        ASSERT(0, "0", "../z_en_elf.c", 1725);
    }

    this->func_2C8(this, play);
}

void move_common_after(EnElf* this, PlayState* play) {
    s16 glowLightRadius;

    glowLightRadius = 100;

    if (this->unk_2A8 == 8) {
        glowLightRadius = 0;
    }

    if (this->fairyFlags & 0x20) {
        Player* player = GET_PLAYER(play);

        Light_point_ct(&this->lightInfoNoGlow, player->actor.world.pos.x,
                                  (s16)(player->actor.world.pos.y) + 60.0f, player->actor.world.pos.z, 255, 255, 255,
                                  200);
    } else {
        Light_point_ct(&this->lightInfoNoGlow, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 255, -1);
    }

    Light_point2_ct(&this->lightInfoGlow, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 255, 255, 255, glowLightRadius);

    this->unk_2BC = atans_table(this->actor.velocity.z, this->actor.velocity.x);

    Actor_set_scale(&this->actor, this->actor.scale.x);
}

static void move_normal(EnElf* this, PlayState* play) {
    Vec3f nextPos;
    Vec3f prevPos;
    Player* player = GET_PLAYER(play);
    Actor* naviHoverActor;
    f32 xScale;
    f32 distFromPlayerHat;

    check_mode_z_en_elf(this, play);
    move_common_before(this, play);

    xScale = 0.0f;

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[8] != NULL)) {
        DemoEffMoveProc_global(&nextPos, play, 8);

        if (play->csCtx.actorCues[8]->id == 5) {
            if (1) {}
            En_Elf_set_kirakira(this, play, 16);
        }

        prevPos = this->actor.world.pos;

        if (this->unk_2A8 == 0xA) {
            chase_position2(this, &nextPos);
        } else {
            chase_position(this, &nextPos, 0.2f);
        }

        if ((play->sceneId == SCENE_LINKS_HOUSE) && (z_common_data.sceneLayer == 4)) {
            // play dash sound effect as Navi enters Links house in the intro
            if (1) {}
            if (play->csCtx.curFrame == 55) {
                Actor_SE_set(&this->actor, NA_SE_EV_FAIRY_DASH);
            }

            // play dash sound effect in intervals as Navi is waking up Link in the intro
            if (this->unk_2A8 == 6) {
                if (this->fairyFlags & 0x40) {
                    if (prevPos.y < this->actor.world.pos.y) {
                        this->fairyFlags &= ~0x40;
                    }
                } else {
                    if (this->actor.world.pos.y < prevPos.y) {
                        this->fairyFlags |= 0x40;
                        Actor_SE_set(&this->actor, NA_SE_EV_FAIRY_DASH);
                    }
                }
            }
        }
    } else {
        distFromPlayerHat = search_position_distance(&player->bodyPartsPos[PLAYER_BODYPART_HAT], &this->actor.world.pos);

        switch (this->unk_2A8) {
            case 7:
                chase_position(this, &player->bodyPartsPos[PLAYER_BODYPART_HAT], 1.0f - this->unk_2AE * (1.0f / 30.0f));
                xScale = search_position_distance(&player->bodyPartsPos[PLAYER_BODYPART_HAT], &this->actor.world.pos);

                if (distFromPlayerHat < 7.0f) {
                    this->unk_2C0 = 0;
                    xScale = 0.0f;
                } else if (distFromPlayerHat < 25.0f) {
                    xScale = (xScale - 5.0f) * 0.05f;
                    xScale = 1.0f - xScale;
                    xScale = (1.0f - SQ(xScale)) * 0.008f;
                } else {
                    xScale = 0.008f;
                }
                En_Elf_set_kirakira(this, play, 16);
                break;
            case 8:
                chase_position(this, &player->bodyPartsPos[PLAYER_BODYPART_HAT], 0.2f);
                this->actor.world.pos = player->bodyPartsPos[PLAYER_BODYPART_HAT];
                up_tough(this, 1);
                break;
            case 11:
                nextPos = player->bodyPartsPos[PLAYER_BODYPART_HAT];
                nextPos.y += 1500.0f * this->actor.scale.y;
                chase_position5(this, &nextPos);
                En_Elf_set_kirakira(this, play, 16);

                if (this->unk_2B8 <= 19.0f) {
                    this->unk_2B8 += 1.0f;
                }

                if (this->unk_2B8 >= 21.0f) {
                    this->unk_2B8 -= 1.0f;
                }

                if (this->unk_2C0 < 0x20) {
                    this->unk_2B0 = (this->unk_2C0 * 0xF0) + 0x200;
                    down_tough(this, 1);
                }
                break;
            case 12:
                nextPos = GET_ACTIVE_CAM(play)->eye;
                nextPos.y += (-2000.0f * this->actor.scale.y);
                chase_position4(this, &nextPos, 0.0f, 20.0f, 0.2f);
                break;
            default:
                up_tough(this, 1);
                nextPos = play->actorCtx.attention.naviHoverPos;
                nextPos.y += (1500.0f * this->actor.scale.y);
                naviHoverActor = play->actorCtx.attention.naviHoverActor;

                if (naviHoverActor != NULL) {
                    chase_position4(this, &nextPos, 0.0f, 20.0f, 0.2f);

                    if (this->actor.speed >= 5.0f) {
                        En_Elf_set_kirakira(this, play, 16);
                    }
                } else {
                    if ((this->timer % 32) == 0) {
                        this->unk_2A0 = rnd_f(7.0f) + 3.0f;
                    }

                    if (this->fairyFlags & 2) {
                        if (distFromPlayerHat < 30.0f) {
                            this->fairyFlags ^= 2;
                        }

                        chase_position4(this, &nextPos, 0.0f, 20.0f, 0.2f);
                        En_Elf_set_kirakira(this, play, 16);
                    } else {
                        if (distFromPlayerHat > 100.0f) {
                            this->fairyFlags |= 2;

                            if (this->unk_2C7 == 0) {
                                Actor_SE_set(&this->actor, NA_SE_EV_FAIRY_DASH);
                            }

                            this->unk_2C0 = 0x64;
                        }
                        chase_position4(this, &nextPos, 0.0f, this->unk_2A0, 0.2f);
                    }
                }
                break;
        }
    }

    if (this->unk_2A8 == 7) {
        this->actor.scale.x = xScale;
    } else if (this->unk_2A8 == 8) {
        this->actor.scale.x = 0.0f;
    } else {
        add_calc(&this->actor.scale.x, 0.008f, 0.3f, 0.00080000004f, 0.000080000005f);
    }

    move_common_after(this, play);
}

void color_morf(Color_RGBAf* dest, Color_RGBAf* newColor, Color_RGBAf* curColor, f32 rate) {
    Color_RGBAf rgbaDiff;

    rgbaDiff.r = (newColor->r - curColor->r);
    rgbaDiff.g = (newColor->g - curColor->g);
    rgbaDiff.b = (newColor->b - curColor->b);
    rgbaDiff.a = (newColor->a - curColor->a);

    dest->r += (rgbaDiff.r * rate);
    dest->g += (rgbaDiff.g * rate);
    dest->b += (rgbaDiff.b * rate);
    dest->a += (rgbaDiff.a * rate);
}

void elf_egg_color_check(EnElf* this, PlayState* play) {
    Actor* naviHoverActor = play->actorCtx.attention.naviHoverActor;
    Player* player = GET_PLAYER(play);
    f32 transitionRate;
    u16 sfxId;

    if (play->actorCtx.attention.naviMoveProgressFactor != 0.0f) {
        this->unk_2C6 = 0;
        this->unk_29C = 1.0f;

        if (this->unk_2C7 == 0) {
            Actor_SE_set(&this->actor, NA_SE_EV_FAIRY_DASH);
        }

    } else {
        if (this->unk_2C6 == 0) {
            if ((naviHoverActor == NULL) ||
                (search_position_distance(&this->actor.world.pos, &play->actorCtx.attention.naviHoverPos) < 50.0f)) {
                this->unk_2C6 = 1;
            }
        } else if (this->unk_29C != 0.0f) {
            if (chase_f(&this->unk_29C, 0.0f, 0.25f) != 0) {
                this->innerColor = play->actorCtx.attention.naviInnerColor;
                this->outerColor = play->actorCtx.attention.naviOuterColor;
            } else {
                transitionRate = 0.25f / this->unk_29C;
                color_morf(&this->innerColor, &play->actorCtx.attention.naviInnerColor, &this->innerColor,
                                  transitionRate);
                color_morf(&this->outerColor, &play->actorCtx.attention.naviOuterColor, &this->outerColor,
                                  transitionRate);
            }
        }
    }

    if (this->fairyFlags & 1) {
        if ((naviHoverActor == NULL) || (player->focusActor == NULL)) {
            this->fairyFlags ^= 1;
        }
    } else {
        if ((naviHoverActor != NULL) && (player->focusActor != NULL)) {
            if (naviHoverActor->category == ACTORCAT_NPC) {
                sfxId = NA_SE_VO_NAVY_HELLO;
            } else {
                sfxId = (naviHoverActor->category == ACTORCAT_ENEMY) ? NA_SE_VO_NAVY_ENEMY : NA_SE_VO_NAVY_HEAR;
            }

            if (this->unk_2C7 == 0) {
                Actor_SE_set(&this->actor, sfxId);
            }

            this->fairyFlags |= 1;
        }
    }
}

void check_mode_z_en_elf(EnElf* this, PlayState* play) {
    s32 temp;
    Actor* naviHoverActor;
    Player* player = GET_PLAYER(play);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[8] != NULL) {
            switch (play->csCtx.actorCues[8]->id) {
                case 4:
                    temp = 9;
                    break;
                case 3:
                    temp = 6;
                    break;
                case 1:
                    temp = 10;
                    break;
                default:
                    temp = 0;
                    break;
            }
        } else {
            temp = 0;
            this->unk_2C0 = 100;
        }

    } else {
        naviHoverActor = play->actorCtx.attention.naviHoverActor;

        // `R_SCENE_CAM_TYPE` is not a bit field, but this conditional checks for a specific bit.
        // This `& 0x10` check will pass for either `SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT`, `SCENE_CAM_TYPE_FIXED`, or
        // `SCENE_CAM_TYPE_SHOOTING_GALLERY`.
        // However, of these three, only `SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT` is used with `VIEWPOINT_PIVOT`,
        // so here the bit check is equivalent to checking for `SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT`.
        if ((player->stateFlags1 & PLAYER_STATE1_10) ||
            ((R_SCENE_CAM_TYPE & 0x10) && Game_play_pr_vr_switch_pr_check(play, VIEWPOINT_PIVOT))) {
            temp = 12;
            this->unk_2C0 = 100;
        } else if (naviHoverActor == NULL || naviHoverActor->category == ACTORCAT_NPC) {
            if (naviHoverActor != NULL) {
                this->unk_2C0 = 100;
                player->stateFlags2 |= PLAYER_STATE2_NAVI_ACTIVE;
                temp = 0;
            } else {
                switch (this->unk_2A8) {
                    case 0:
                        if (this->unk_2C0 != 0) {
                            this->unk_2C0--;
                            temp = 0;
                        } else {
                            if (this->unk_2C7 == 0) {
                                Actor_SE_set(&this->actor, NA_SE_EV_NAVY_VANISH);
                            }
                            temp = 7;
                        }
                        break;
                    case 7:
                        if (this->unk_2C0 != 0) {
                            if (this->unk_2AE > 0) {
                                this->unk_2AE--;
                                temp = 7;
                            } else {
                                player->stateFlags2 |= PLAYER_STATE2_NAVI_ACTIVE;
                                temp = 0;
                            }
                        } else {
                            temp = 8;
                            up_tough(this, 10);
                        }
                        break;
                    case 8:
                        temp = 8;
                        break;
                    case 11:
                        temp = this->unk_2A8;
                        if (this->unk_2C0 > 0) {
                            this->unk_2C0--;
                        } else {
                            temp = 0;
                        }
                        break;
                    default:
                        temp = 0;
                        break;
                }
            }
        } else {
            temp = 1;
        }

        switch (temp) {
            case 0:
                if (!(player->stateFlags2 & PLAYER_STATE2_NAVI_ACTIVE)) {
                    temp = 7;
                    if (this->unk_2C7 == 0) {
                        Actor_SE_set(&this->actor, NA_SE_EV_NAVY_VANISH);
                    }
                }
                break;
            case 8:
                if (player->stateFlags2 & PLAYER_STATE2_NAVI_ACTIVE) {
                    down_tough(this, 0x32);
                    this->unk_2C0 = 42;
                    temp = 11;
                    if (this->unk_2C7 == 0) {
                        Actor_SE_set(&this->actor, NA_SE_EV_FAIRY_DASH);
                    }
                }
                break;
            case 7:
                player->stateFlags2 &= ~PLAYER_STATE2_NAVI_ACTIVE;
                break;
            default:
                player->stateFlags2 |= PLAYER_STATE2_NAVI_ACTIVE;
                break;
        }
    }

    if (temp != this->unk_2A8) {
        En_Elf_actor_set_status(this, temp);

        if (temp == 11) {
            this->unk_2B8 = search_position_distanceXZ(&player->bodyPartsPos[PLAYER_BODYPART_HAT], &this->actor.world.pos);
            this->unk_2AC = search_position_angleY(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_HAT]);
        }
    }
}

void En_Elf_set_kirakira(EnElf* this, PlayState* play, s32 sparkleLife) {
    static Vec3f kirakira_vec = { 0.0f, -0.05f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, -0.025f, 0.0f };
    s32 pad;
    Vec3f sparklePos;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    sparklePos.x = rnd_fx(6.0f) + this->actor.world.pos.x;
    sparklePos.y = (fqrand() * 6.0f) + this->actor.world.pos.y;
    sparklePos.z = rnd_fx(6.0f) + this->actor.world.pos.z;

    primColor.r = this->innerColor.r;
    primColor.g = this->innerColor.g;
    primColor.b = this->innerColor.b;

    envColor.r = this->outerColor.r;
    envColor.g = this->outerColor.g;
    envColor.b = this->outerColor.b;

    Effect_SS_KiraKira_sc_ct_ct(play, &sparklePos, &kirakira_vec, &kirakira_acc, &primColor, &envColor, 1000,
                                    sparkleLife);
}

void Elf_Shadow_Process(EnElf* this, PlayState* play) {
    s32 pad;
    s32 bgId;

    this->actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac2(play, &play->colCtx, &this->actor.floorPoly, &bgId,
                                                         &this->actor, &this->actor.world.pos);
    this->actor.shape.shadowAlpha = 50;
}

// move to talk to player
void move_talk_before(EnElf* this, PlayState* play) {
    Vec3f headCopy;
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->fairyFlags & 0x10) {
        pos = play->actorCtx.attention.naviHoverPos;

        if ((player->focusActor == NULL) || (&player->actor == player->focusActor) ||
            (&this->actor == player->focusActor)) {
            pos.x = player->bodyPartsPos[PLAYER_BODYPART_HEAD].x + (sin_s(player->actor.shape.rot.y) * 20.0f);
            pos.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 5.0f;
            pos.z = player->bodyPartsPos[PLAYER_BODYPART_HEAD].z + (cos_s(player->actor.shape.rot.y) * 20.0f);
        }

        this->actor.focus.pos = pos;
        this->fairyFlags &= ~0x10;
    }

    move_common_before(this, play);
    headCopy = this->actor.focus.pos;

    chase_position4(this, &headCopy, 0, 20.0f, 0.2f);

    if (this->actor.speed >= 5.0f) {
        En_Elf_set_kirakira(this, play, 16);
    }

    add_calc(&this->actor.scale.x, 0.008f, 0.3f, 0.00080000004f, 0.000080000005f);
    move_common_after(this, play);
}

// move after talking to player
void move_talk_after(EnElf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc_short_angle2(&this->actor.shape.rot.y, this->unk_2BC, 5, 0x1000, 0x400);
    this->timer++;
    chase_f(&this->unk_2A4, 1.0f, 0.05f);
    set_add_light_global(play, SQ(this->unk_2A4), player->actor.projectedPos.z + 780.0f, 0.2f, 0.5f);
}

// ask to talk to saria again
void En_Elf_actor_move_talk_saria2(Actor* thisx, PlayState* play) {
    EnElf* this = (EnElf*)thisx;

    move_talk_before(this, play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                message_set2(play, get_sa_message(play));
                this->actor.update = En_Elf_actor_move_talk_saria1;
                break;
            case 1: // no
                message_close(play);
                this->actor.update = En_Elf_actor_move_link;
                En_Elf_actor_set_status(this, 0);
                this->fairyFlags &= ~0x20;
                break;
        }
    }

    move_talk_after(this, play);
}

void En_Elf_actor_move_talk_saria1(Actor* thisx, PlayState* play) {
    EnElf* this = (EnElf*)thisx;

    move_talk_before(this, play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, 0xE3);
        this->actor.update = En_Elf_actor_move_talk_saria2;
    }

    move_talk_after(this, play);
}

void En_Elf_actor_move_talk_saria0(Actor* thisx, PlayState* play) {
    EnElf* this = (EnElf*)thisx;

    move_talk_before(this, play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, get_sa_message(play));
        this->actor.update = En_Elf_actor_move_talk_saria1;
    }

    move_talk_after(this, play);
}

// ask to talk to navi
void En_Elf_actor_move_talk_naby0(Actor* thisx, PlayState* play) {
    s32 naviTextId;
    EnElf* this = (EnElf*)thisx;

    move_talk_before(this, play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                naviTextId = get_elf_message(play);

                if (naviTextId != 0) {
                    message_set2(play, naviTextId);
                } else {
                    message_set2(play, 0x15F);
                }

                this->actor.update = En_Elf_actor_move_talk;
                break;
            case 1: // no
                message_close(play);
                this->actor.update = En_Elf_actor_move_link;
                En_Elf_actor_set_status(this, 0);
                this->fairyFlags &= ~0x20;
                break;
        }
    }

    move_talk_after(this, play);
}

// ask to talk to saria
void En_Elf_actor_move_talk(Actor* thisx, PlayState* play) {
    EnElf* this = (EnElf*)thisx;

    move_talk_before(this, play);

    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (pad_on_check(play)) {
            play->msgCtx.unk_E3F2 = 0xFF;

            switch (play->msgCtx.choiceIndex) {
                case 0: // yes
                    this->actor.update = En_Elf_actor_move_talk_saria0;
                    message_set2(play, 0xE2);
                    break;
                case 1: // no
                    this->actor.update = En_Elf_actor_move_talk_naby0;
                    message_set2(play, 0xE1);
                    break;
            }
        }
    } else if (Actor_talk_end_check(thisx, play)) {
        this->actor.update = En_Elf_actor_move_link;
        En_Elf_actor_set_status(this, 0);
        this->fairyFlags &= ~0x20;
    }

    move_talk_after(this, play);
}

void En_Elf_actor_move_link(Actor* thisx, PlayState* play) {
    u8 unk2C7;
    s32 pad;
    Player* player = GET_PLAYER(play);
    EnElf* this = (EnElf*)thisx;

    if (player->naviTextId == 0) {
        if (player->focusActor == NULL) {
#if DEBUG_FEATURES
            if (((z_common_data.save.info.playerData.naviTimer >= 600) &&
                 (z_common_data.save.info.playerData.naviTimer <= 3000)) ||
                (nREG(89) != 0))
#else
            if ((z_common_data.save.info.playerData.naviTimer >= 600) &&
                (z_common_data.save.info.playerData.naviTimer <= 3000))
#endif
            {
                player->naviTextId = get_elf_message(play);

                if (player->naviTextId == 0x15F) {
                    player->naviTextId = 0;
                }
            }
        }
    } else if (player->naviTextId < 0) {
        // trigger dialog instantly for negative message IDs
        thisx->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }

    if (Actor_talk_check(thisx, play)) {
        Na_StartFxmixSe(&_dummy_zero_f, NA_SE_VO_SK_LAUGH, 0x20);
        thisx->focus.pos = thisx->world.pos;

        if (thisx->textId == get_elf_message(play)) {
            this->fairyFlags |= 0x80;
            z_common_data.save.info.playerData.naviTimer = 3001;
        }

        this->fairyFlags |= 0x10;
        this->fairyFlags |= 0x20;
        thisx->update = En_Elf_actor_move_talk;
        En_Elf_actor_set_status(this, 3);

        if (this->elfMsg != NULL) {
            this->elfMsg->actor.flags |= ACTOR_FLAG_TALK;
        }

        thisx->flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        this->actionFunc(this, play);
        thisx->shape.rot.y = this->unk_2BC;

#if DEBUG_FEATURES
        // `z_common_data.save.info.sceneFlags[127].chest` (like in the debug string) instead of `HIGH_SCORE(HS_HBA)`
        // matches too, but, with how the `SaveContext` struct is currently defined, it is an out-of-bounds read in the
        // `sceneFlags` array. It is theorized the original `room_inf` (currently `sceneFlags`) was an array of length
        // 128, not broken up like currently into structs. Structs are currently used because they're easier to work
        // with and still match. There is another occurrence of this elsewhere.
        nREG(80) = HIGH_SCORE(HS_HBA);
        if ((nREG(81) != 0) && (HIGH_SCORE(HS_HBA) != 0)) {
            LOG_NUM("z_common_data.memory.information.room_inf[127][ 0 ]", HIGH_SCORE(HS_HBA), "../z_en_elf.c", 2595);
        }
#endif

        if (!Game_play_demo_mode_check(play)) {
            if (z_common_data.save.info.playerData.naviTimer < 25800) {
                z_common_data.save.info.playerData.naviTimer++;
            } else if (!(this->fairyFlags & 0x80)) {
                z_common_data.save.info.playerData.naviTimer = 0;
            }
        }
    }

    this->elfMsg = NULL;
    this->timer++;

    if (this->unk_2A4 > 0.0f) {
        chase_f(&this->unk_2A4, 0.0f, 0.05f);
        set_add_light_global(play, SQ(this->unk_2A4) * this->unk_2A4, player->actor.projectedPos.z + 780.0f, 0.2f,
                                 0.5f);
    }

    // temp probably fake match
    unk2C7 = this->unk_2C7;
    if (unk2C7 > 0) {
        this->unk_2C7--;
    }

    if ((this->unk_2C7 == 0) && (play->csCtx.state != CS_STATE_IDLE)) {
        this->unk_2C7 = 1;
    }

    Elf_Shadow_Process(this, play);
}

void En_Elf_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnElf* this = (EnElf*)thisx;

    this->actionFunc(this, play);
    this->actor.shape.rot.y = this->unk_2BC;
    this->timer++;

    if (this->fairyFlags & FAIRY_FLAG_BIG) {
        Elf_Shadow_Process(this, play);
    }
}

s32 before_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static Vec3f light_local_offset = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    f32 scale;
    Vec3f mtxMult;
    EnElf* this = (EnElf*)thisx;

    if (limbIndex == 8) {
        scale = ((sin_s(this->timer * 4096) * 0.1f) + 1.0f) * 0.012f;

        if (this->fairyFlags & FAIRY_FLAG_BIG) {
            scale *= 2.0f;
        }

        scale *= (this->actor.scale.x * 124.99999f);
        Matrix_Position(&light_local_offset, &mtxMult);
        Matrix_translate(mtxMult.x, mtxMult.y, mtxMult.z, MTXMODE_NEW);
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    }

    // do not draw wings for big fairies
    if (this->fairyFlags & FAIRY_FLAG_BIG) {
        if (limbIndex == 4 || limbIndex == 7 || limbIndex == 11 || limbIndex == 14) {
            *dList = NULL;
        }
    }

    return false;
}

void En_Elf_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    f32 alphaScale;
    s32 envAlpha;
    EnElf* this = (EnElf*)thisx;
    s32 pad1;
    Gfx* dListHead;
    Player* player = GET_PLAYER(play);

    if ((this->unk_2A8 != 8) && !(this->fairyFlags & 8)) {
        if (!(player->stateFlags1 & PLAYER_STATE1_20) || (kREG(90) < this->actor.projectedPos.z)) {
            dListHead = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Gfx) * 4);

            OPEN_DISPS(play->state.gfxCtx, "../z_en_elf.c", 2730);

            texture_z(play->state.gfxCtx);

            envAlpha = (this->timer * 50) & 0x1FF;
            envAlpha = (envAlpha > 255) ? 511 - envAlpha : envAlpha;

            alphaScale = this->disappearTimer < 0 ? (this->disappearTimer * (7.0f / 6000.0f)) + 1.0f : 1.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08, dListHead);
            gDPPipeSync(dListHead++);
            gDPSetPrimColor(dListHead++, 0, 0x01, (u8)this->innerColor.r, (u8)this->innerColor.g,
                            (u8)this->innerColor.b, (u8)(this->innerColor.a * alphaScale));

            if (this->fairyFlags & 4) {
                gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_CLD_SURF2);
            } else {
                gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
            }

            gSPEndDisplayList(dListHead);
            gDPSetEnvColor(POLY_XLU_DISP++, (u8)this->outerColor.r, (u8)this->outerColor.g, (u8)this->outerColor.b,
                           (u8)(envAlpha * alphaScale));
            POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           before_draw, NULL, this, POLY_XLU_DISP);

            CLOSE_DISPS(play->state.gfxCtx, "../z_en_elf.c", 2793);
        }
    }
}

void DemoEffMoveProc_global(Vec3f* dest, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];
    f32 lerp;

    startPos.x = cue->startPos.x;
    startPos.y = cue->startPos.y;
    startPos.z = cue->startPos.z;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    lerp = get_parcent(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    dest->x = ((endPos.x - startPos.x) * lerp) + startPos.x;
    dest->y = ((endPos.y - startPos.y) * lerp) + startPos.y;
    dest->z = ((endPos.z - startPos.z) * lerp) + startPos.z;
}
