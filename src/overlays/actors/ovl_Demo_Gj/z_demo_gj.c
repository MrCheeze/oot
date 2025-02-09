/*
 * File: z_demo_gj.c
 * Overlay: Demo_Gj
 * Description: Ganon battle rubble.
 */

#include "z_demo_gj.h"

#include "libc64/math64.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "regs.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"

#include "assets/objects/object_gj/object_gj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Gj_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Gj_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Gj_main(Actor* thisx, PlayState* play);
void Demo_Gj_draw(Actor* thisx, PlayState* play);

static ColliderCylinderInitType1 Demo_Gj_after1_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 30, 100, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 Demo_Gj_after2_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 25, 110, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 Demo_Gj_after7_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 25, 200, 0, { 0, 0, 0 } },
};

typedef void (*DemoGjUpdateFunc)(DemoGj*, PlayState*);
typedef void (*DemoGjDrawFunc)(DemoGj*, PlayState*);

// bits 11-15
s32 Demo_Gj_GetUpper_arg_data(DemoGj* this) {
    s32 type = PARAMS_GET_U(this->dyna.actor.params, 11, 5);

    return type;
}

// bits 8-10
s32 Demo_Gj_GetMidder_arg_data(DemoGj* this) {
    s32 amount = PARAMS_GET_U(this->dyna.actor.params, 8, 3);

    return amount;
}

// bits 0-7
s32 Demo_Gj_GetLower_arg_data(DemoGj* this) {
    s32 type = PARAMS_GET_U(this->dyna.actor.params, 0, 8);

    return type;
}

void Demo_Gj_ct_forCorect(DemoGj* this, PlayState* play, ColliderCylinder* cylinder,
                         ColliderCylinderInitType1* cylinderInit) {
    ClObjPipe_ct(play, cylinder);
    ClObjPipe_set3(play, cylinder, &this->dyna.actor, cylinderInit);
}

s32 Demo_Gj_Check_Break_forStand(DemoGj* this, PlayState* play, ColliderCylinder* cylinder) {
    if (BlastVsMyCheck(play, &cylinder->base) != NULL) {
        return true;
    }
    return false;
}

void Demo_Gj_dt_forCorect(DemoGj* this, PlayState* play) {
    switch (Demo_Gj_GetLower_arg_data(this)) {
        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1:
            ClObjPipe_dt(play, &this->cylinders[0]);
            ClObjPipe_dt(play, &this->cylinders[1]);
            ClObjPipe_dt(play, &this->cylinders[2]);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2:
            ClObjPipe_dt(play, &this->cylinders[0]);
            ClObjPipe_dt(play, &this->cylinders[1]);
            ClObjPipe_dt(play, &this->cylinders[2]);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL:
            ClObjPipe_dt(play, &this->cylinders[0]);
            break;
    }
}

void Demo_Gj_Actor_dt(Actor* thisx, PlayState* play) {
    DemoGj* this = (DemoGj*)thisx;

    Demo_Gj_dt_forCorect(this, play);
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Demo_Gj_Set_BreakSound(DemoGj* this, PlayState* play) {
    Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 50, NA_SE_EV_GRAVE_EXPLOSION);
}

void Birth1_Dust_In_Demo_Gj(PlayState* play, Vec3f* pos, f32 arg2) {
    static Vec3f vec = { 0.0f, 6.0f, 0.0f };
    static Vec3f acc = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 prim = { 0, 0, 0, 0 };
    static Color_RGBA8 env = { 0, 0, 0, 0 };
    f32 temp = arg2 * 0.2f;

    if (1) {}
    Effect_SS_Dust_sc_cl_co_li_ct(play, pos, &vec, &acc, &prim, &env, temp * fqrand() + arg2, 15, 90);
}

void Birth_Item_In_Demo_Gj(DemoGj* this, PlayState* play) {
    Vec3f* pos = &this->dyna.actor.world.pos;
    s16 collectible = Demo_Gj_GetUpper_arg_data(this);
    s32 amount = Demo_Gj_GetMidder_arg_data(this);
    s32 i;

    for (i = 0; i < amount; i++) {
        Item_set0(play, pos, collectible);
    }
}

void Birth_Effect_Piece_In_Demo_Gj(DemoGj* this, PlayState* play, Vec3f* initialPos, Vec3f* direction) {
    Vec3f explosionPos;
    Vec3f vec;
    s32 phi_s0;
    f32 aux;
    s16 theta = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        explosionPos.x = sin_s(theta) * 16.0f;
        explosionPos.y = (fqrand() * 5.0f) + 2.0f;
        explosionPos.z = cos_s(theta) * 16.0f;

        vec.x = (explosionPos.x * 0.6f) + (12.0f * direction->x);
        vec.y = (fqrand() * 36.0f) + 6.0f;
        vec.z = (explosionPos.z * 0.6f) + (12.0f * direction->z);

        explosionPos.x += initialPos->x;
        explosionPos.y += initialPos->y;
        explosionPos.z += initialPos->z;

        aux = fqrand();
        if (aux < 0.1f) {
            phi_s0 = 0x61;
        } else if (aux < 0.7f) {
            phi_s0 = 0x41;
        } else {
            phi_s0 = 0x21;
        }

        Effect_Kakera_ct2(play, &explosionPos, &vec, initialPos, -200, phi_s0, 10, 10, 0,
                             fqrand() * 20.0f + 20.0f, 20, 300, (s32)(fqrand() * 30.0f) + 30, -1,
                             OBJECT_GEFF, &gGanonsCastleRubbleAroundArenaDL[28]);

        theta += 0x2AAA;
    }

    Demo_Gj_Set_BreakSound(this, play);
}

s32 Demo_Gj_Check_DemoScene(void) {
    if (!IS_CUTSCENE_LAYER) {
        return false;
    }
    return true;
}

s32 Demo_Gj_Search_Boss_Ganon(DemoGj* this, PlayState* play) {
    Actor* actor;

    if (this->ganon == NULL) {
        actor = play->actorCtx.actorLists[ACTORCAT_BOSS].head;

        while (actor != NULL) {
            if (actor->id == ACTOR_BOSS_GANON2) {
                this->ganon = (BossGanon2*)actor;

                // "Demo_Gj_Search_Boss_Ganon %d: Discover Ganon !!!!"
                PRINTF("Demo_Gj_Search_Boss_Ganon %d:ガノン発見!!!!\n", this->dyna.actor.params);
                return true;
            }
            actor = actor->next;
        }

        // "Demo_Gj_Search_Boss_Ganon %d: I couldn't find Ganon"
        PRINTF("Demo_Gj_Search_Boss_Ganon %d:ガノン発見出来ず\n", this->dyna.actor.params);
        return false;
    }
    //! @bug Missing return value when `this->ganon` is already set. No caller uses the return value
    //! so it doesn't matter.
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Demo_Gj_Actor_init_BGdata_common(DemoGj* this, PlayState* play, CollisionHeader* header) {
    s32 pad[3];
    CollisionHeader* newHeader;

    if (header != NULL) {
        ValueSet_process(&this->dyna.actor, value_init);
        MoveBG_ct(&this->dyna, 0);
        newHeader = NULL;
        DynaPolyUty_bgdi_SG2KSG(header, &newHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, newHeader);
    }
}

// TODO: find a better name
s32 Demo_Gj_Actor_init_battle_common(DemoGj* this, PlayState* play, s32 updateMode, s32 drawConfig, CollisionHeader* header) {
    if (!Demo_Gj_Check_DemoScene()) {
        this->updateMode = updateMode;
        this->drawConfig = drawConfig;
        Demo_Gj_Actor_init_BGdata_common(this, play, header);
        return true;
    }
    Actor_delete(&this->dyna.actor);
    return false;
}

void Demo_Gj_draw_normal_common(DemoGj* this, PlayState* play, Gfx* displayList) {
    if (!DEBUG_FEATURES || kREG(0) == 0) {
        GraphicsContext* gfxCtx = play->state.gfxCtx;

        OPEN_DISPS(gfxCtx, "../z_demo_gj.c", 1163);

        _texture_z_light_fog_prim(gfxCtx);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_FINALIZE(gfxCtx, "../z_demo_gj.c", 1165),
                  G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, displayList);
        gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

        CLOSE_DISPS(gfxCtx, "../z_demo_gj.c", 1169);
    }
}

void Demo_Gj_draw_move_common(DemoGj* this, PlayState* play, Gfx* displayList) {
    s32 pad;
    GraphicsContext* gfxCtx;
    s16 x = this->rotationVec.x;
    s16 y = this->rotationVec.y;
    s16 z = this->rotationVec.z;
    s32 pad2;
    Mtx* matrix;

    gfxCtx = play->state.gfxCtx;
    matrix = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));

    OPEN_DISPS(gfxCtx, "../z_demo_gj.c", 1187);

    Matrix_push();
    Matrix_rotateXYZ(x, y, z, MTXMODE_APPLY);
    MATRIX_TO_MTX(matrix, "../z_demo_gj.c", 1193);
    Matrix_pull();

    _texture_z_light_fog_prim(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, matrix, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, displayList);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gj.c", 1201);
}

void Demo_Gj_common_Reflect(DemoGj* this, PlayState* play) {
    f32 yPosition = this->dyna.actor.world.pos.y;
    f32* yVelocity = &this->dyna.actor.velocity.y;
    f32* speedXZ = &this->dyna.actor.speed;
    Vec3s* unk_172 = &this->unk_172;
    f32 verticalTranslation;
    Vec3f vec;
    f32 verticalFactor;
    f32 xzPlaneFactor;

    switch (Demo_Gj_GetLower_arg_data(this)) {
        case DEMOGJ_TYPE_RUBBLE_PILE_1:
            verticalTranslation = kREG(23);
            vec.x = kREG(24) * 0.01f + 1.0f;
            vec.y = kREG(25) * 0.01f + 1.0f;
            vec.z = kREG(26) * 0.01f + 1.0f;
            verticalFactor = kREG(27) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(28) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_2:
            verticalTranslation = kREG(36);
            vec.x = kREG(37) * 0.01f + 1.0f;
            vec.y = kREG(38) * 0.01f + 1.0f;
            vec.z = kREG(39) * 0.01f + 1.0f;
            verticalFactor = kREG(40) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(41) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_3:
            verticalTranslation = kREG(49);
            vec.x = kREG(50) * 0.01f + 1.0f;
            vec.y = kREG(51) * 0.01f + 1.0f;
            vec.z = kREG(52) * 0.01f + 1.0f;
            verticalFactor = kREG(53) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(54) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_4:
            verticalTranslation = kREG(62);
            vec.x = kREG(63) * 0.01f + 1.0f;
            vec.y = kREG(64) * 0.01f + 1.0f;
            vec.z = kREG(65) * 0.01f + 1.0f;
            verticalFactor = kREG(66) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(67) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_5:
            verticalTranslation = kREG(75);
            vec.x = kREG(76) * 0.01f + 1.0f;
            vec.y = kREG(77) * 0.01f + 1.0f;
            vec.z = kREG(78) * 0.01f + 1.0f;
            verticalFactor = kREG(79) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(80) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_6:
            verticalTranslation = kREG(88);
            vec.x = kREG(89) * 0.01f + 1.0f;
            vec.y = kREG(90) * 0.01f + 1.0f;
            vec.z = kREG(91) * 0.01f + 1.0f;
            verticalFactor = kREG(92) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(93) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_7:
            verticalTranslation = kREG(10) + -190.0f;
            vec.x = kREG(9) * 0.01f + 1.0f;
            vec.y = kREG(8) * 0.01f + 1.0f;
            vec.z = kREG(7) * 0.01f + 1.0f;
            verticalFactor = kREG(6) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(5) * 0.01f + 1.0f;
            break;

        default:
            // "Demo_Gj_common_Reflect : This arg_data is not supported = %d"
            PRINTF(VT_FGCOL(RED) "Demo_Gj_common_Reflect : そんなarg_dataには対応していない = %d\n" VT_RST,
                   this->dyna.actor.params);
            return;
    }

    yPosition += verticalTranslation;
    if (yPosition <= 1086.0f && (*yVelocity < 0.0f)) {
        if (!this->isRotated) {
            *yVelocity *= verticalFactor;
            *speedXZ *= xzPlaneFactor;

            unk_172->x *= vec.x;
            unk_172->y *= vec.y;
            unk_172->z *= vec.z;

            if (*yVelocity <= -this->dyna.actor.gravity) {
                *yVelocity = 0.0f;
                *speedXZ = 0.0f;

                unk_172->x = 0;
                unk_172->y = 0;
                unk_172->z = 0;
            }

            this->isRotated = true;
        }
    }
}

/*
 * Returns true if `ganon->unk_314` is equals to `arg1`.
 * `ganon->unk_314` can have the following values:
 *  0: Before the battle has started.
 *  1: When is set: Ganondorf starts rising from the rubble.
 *     What is happening: Ganondorf is moving vertically and has vertical vec.
 *     Proposed name: BOSSGANON2_MODE_GANONDORF_RISING
 *  2: When is set: Ganondorf has stopped rising in air.
 *     What is happening: The camera is in front of him, focusing the clouds and going down to focus him.
 *     Proposed name: BOSSGANON2_MODE_GANONDORF_FLOATING
 *  3: When is set: The camera has stopped moving and is focusing him.
 *     What is happening: Ganondorf raises his hand, shows the triforce and transforms into Ganon. The battle starts.
 *     This value is set during the whole real fight against Ganon. Without and with Master Sword.
 *     Proposed name: BOSSGANON2_MODE_GANON_FIGHTING
 *  4: When is set: Link has hit Ganon's tail for last time with Master Sword.
 *     What is happening: Ganon falls to the floor, Zelda uses her magic and tells Link to kill him.
 *     Proposed name: BOSSGANON2_MODE_GANON_DEFEATED
 *
 * Those values should probably be defined as macros or enums in `ovl_Boss_Ganon2/z_boss_ganon2.h`.
 * Proposed name for the function: `s32 DemoGj_CheckGanonMode(DemoGj* this, u8 mode)`
 */
s32 Demo_Gj_Check_demo_info(DemoGj* this, u8 arg1) {
    BossGanon2* ganon = this->ganon;

    if ((ganon != NULL) && (ganon->unk_314 == arg1)) {
        return true;
    }
    return false;
}

s32 Demo_Gj_Check_StandToMove_common(DemoGj* this, PlayState* play) {
    return Demo_Gj_Check_demo_info(this, 1);
}

// Ganondorf has stopped rising into the air and is just floating. Just before he transforms.
s32 Demo_Gj_Check_MoveToStop_common(DemoGj* this, PlayState* play) {
    return Demo_Gj_Check_demo_info(this, 2);
}

void Demo_Gj_Setup_Move_common(DemoGj* this, PlayState* play) {
    Actor* actor = &this->dyna.actor;
    s32 pad;
    Vec3f* pos = &actor->world.pos;
    Vec3s* unk_172;
    f32 xDistance;
    f32 zDistance;

    if (this->ganon != NULL) {
        xDistance = actor->world.pos.x - this->ganon->actor.world.pos.x;
        zDistance = actor->world.pos.z - this->ganon->actor.world.pos.z;
        unk_172 = &this->unk_172;

        switch (Demo_Gj_GetLower_arg_data(this)) {
            case DEMOGJ_TYPE_RUBBLE_PILE_1:
                actor->speed = kREG(16) + 10.0f;
                actor->velocity.y = kREG(17) + 40.0f;
                unk_172->x = kREG(18);
                unk_172->y = kREG(19) + 0x3E8;
                unk_172->z = kREG(20) + 0xBB8;
                actor->minVelocityY = kREG(21) * 0.01f + -29.0f;
                actor->gravity = kREG(22) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_2:
                actor->speed = kREG(29) + 10.0f;
                actor->velocity.y = kREG(30) + 40.0f;
                unk_172->x = kREG(31);
                unk_172->y = kREG(32) + 0x3E8;
                unk_172->z = kREG(33) + 0xBB8;
                actor->minVelocityY = kREG(34) * 0.01f + -29.0f;
                actor->gravity = kREG(35) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_3:
                actor->speed = kREG(42) + 10.0f;
                actor->velocity.y = kREG(43) + 40.0f;
                unk_172->x = kREG(44);
                unk_172->y = kREG(45) + 0x3E8;
                unk_172->z = kREG(46) + 0xBB8;
                actor->minVelocityY = kREG(47) * 0.01f + -29.0f;
                actor->gravity = kREG(48) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_4:
                actor->speed = kREG(55) + 10.0f;
                actor->velocity.y = kREG(56) + 40.0f;
                unk_172->x = kREG(57);
                unk_172->y = kREG(58) + 0x3E8;
                unk_172->z = kREG(59) + 0xBB8;
                actor->minVelocityY = kREG(60) * 0.01f + -29.0f;
                actor->gravity = kREG(61) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_5:
                actor->speed = kREG(68) + 10.0f;
                actor->velocity.y = kREG(69) + 40.0f;
                unk_172->x = kREG(70);
                unk_172->y = kREG(71) + 0x3E8;
                unk_172->z = kREG(72) + 0xBB8;
                actor->minVelocityY = kREG(73) * 0.01f + -29.0f;
                actor->gravity = kREG(74) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_6:
                actor->speed = kREG(81) + 10.0f;
                actor->velocity.y = kREG(82) + 40.0f;
                unk_172->x = kREG(83);
                unk_172->y = kREG(84) + 0x3E8;
                unk_172->z = kREG(85) + 0xBB8;
                actor->minVelocityY = kREG(86) * 0.01f + -29.0f;
                actor->gravity = kREG(87) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_7:
                actor->speed = kREG(94) + 10.0f;
                actor->velocity.y = kREG(95) + 70.0f;
                unk_172->x = kREG(15);
                unk_172->y = kREG(14) + 0x3E8;
                unk_172->z = kREG(13) + 0xBB8;
                actor->minVelocityY = kREG(12) * 0.01f + -29.0f;
                actor->gravity = kREG(11) * 0.01f + -5.0f;
                break;

            default:
                // "Demo_Gj_Setup_Move_common : This arg_data is not supported = %d"
                PRINTF(VT_FGCOL(RED) "Demo_Gj_Setup_Move_common : そんなarg_dataには対応していない = %d\n" VT_RST,
                       actor->params);
                break;
        }

        if (xDistance == 0.0f && zDistance == 0.0f) {
            Player* player = GET_PLAYER(play);

            xDistance = player->actor.world.pos.x - pos->x;
            zDistance = player->actor.world.pos.z - pos->z;

            if (xDistance != 0.0f || zDistance != 0.0f) {
                actor->world.rot.y = RAD_TO_BINANG(fatan2(xDistance, zDistance));
            }
        } else {
            actor->world.rot.y = RAD_TO_BINANG(fatan2(xDistance, zDistance));
        }
    }
}

void Demo_Gj_Check_StopDust_common(DemoGj* this) {
    if (Demo_Gj_Check_demo_info(this, 3)) {
        this->isTransformedIntoGanon = true;
    }
}

#include "z_demo_gj_battle2_1.inc.c"

#include "z_demo_gj_battle2_2.inc.c"

#include "z_demo_gj_battle2_3.inc.c"

#include "z_demo_gj_battle2_4.inc.c"

#include "z_demo_gj_battle2_5.inc.c"

#include "z_demo_gj_battle2_6.inc.c"

#include "z_demo_gj_battle2_7.inc.c"

#include "z_demo_gj_battle1.inc.c"

#include "z_demo_gj_after1.inc.c"

#include "z_demo_gj_after2.inc.c"

#include "z_demo_gj_after7.inc.c"

void Demo_Gj_main(Actor* thisx, PlayState* play) {
    static DemoGjUpdateFunc proc[] = {
        Demo_Gj_main_Stand_battle1,
        Demo_Gj_main_Stand_battle2_1,
        Demo_Gj_main_Stand_battle2_2,
        Demo_Gj_main_Stand_battle2_3,
        Demo_Gj_main_Stand_battle2_4,
        Demo_Gj_main_Stand_battle2_5,
        Demo_Gj_main_Stand_battle2_6,
        Demo_Gj_main_Stand_battle2_7,
        Demo_Gj_main_Move_battle2_1,
        Demo_Gj_main_Move_battle2_2,
        Demo_Gj_main_Move_battle2_3,
        Demo_Gj_main_Move_battle2_4,
        Demo_Gj_main_Move_battle2_5,
        Demo_Gj_main_Move_battle2_6,
        Demo_Gj_main_Move_battle2_7,
        Demo_Gj_main_Wait_after1,
        Demo_Gj_main_Wait_after2,
        Demo_Gj_main_Wait_after7,
        Demo_Gj_main_Battle_after1,
        Demo_Gj_main_Battle_after2,
        Demo_Gj_main_Battle_after7,
    };

    DemoGj* this = (DemoGj*)thisx;

    if (this->updateMode < 0 || this->updateMode >= ARRAY_COUNT(proc) ||
        proc[this->updateMode] == NULL) {
        // "The main mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->updateMode](this, play);
}

void Demo_Gj_Actor_ct(Actor* thisx, PlayState* play) {
    DemoGj* this = (DemoGj*)thisx;

    switch (Demo_Gj_GetLower_arg_data(this)) {
        case DEMOGJ_TYPE_AROUNDARENA:
            Demo_Gj_Actor_init_battle1(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_1:
            Demo_Gj_Actor_init_battle2_1(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_2:
            Demo_Gj_Actor_init_battle2_2(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_3:
            Demo_Gj_Actor_init_battle2_3(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_4:
            Demo_Gj_Actor_init_battle2_4(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_5:
            Demo_Gj_Actor_init_battle2_5(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_6:
            Demo_Gj_Actor_init_battle2_6(this, play);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_7:
            Demo_Gj_Actor_init_battle2_7(this, play);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1:
            Demo_Gj_Actor_init_after1(this, play);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2:
            Demo_Gj_Actor_init_after2(this, play);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL:
            Demo_Gj_Actor_init_after7(this, play);
            break;

        default:
            // "Demo_Gj_Actor_ct There is no such argument!!!!!!!!!!!!!!!!!!!!!!"
            PRINTF(VT_FGCOL(RED) "Demo_Gj_Actor_ct そんな引数は無い!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            Actor_delete(&this->dyna.actor);
    }
}

void Demo_Gj_draw_none(DemoGj* this, PlayState* play) {
}

void Demo_Gj_draw(Actor* thisx, PlayState* play) {
    static DemoGjDrawFunc proc[] = {
        Demo_Gj_draw_none,
        Demo_Gj_draw_normal_battle1,
        Demo_Gj_draw_normal_battle2_1,
        Demo_Gj_draw_normal_battle2_2,
        Demo_Gj_draw_normal_battle2_3,
        Demo_Gj_draw_normal_battle2_4,
        Demo_Gj_draw_normal_battle2_5,
        Demo_Gj_draw_normal_battle2_6,
        Demo_Gj_draw_normal_battle2_7,
        Demo_Gj_draw_move_battle2_1,
        Demo_Gj_draw_move_battle2_2,
        Demo_Gj_draw_move_battle2_3,
        Demo_Gj_draw_move_battle2_4,
        Demo_Gj_draw_move_battle2_5,
        Demo_Gj_draw_move_battle2_6,
        Demo_Gj_draw_move_battle2_7,
        Demo_Gj_draw_normal_after1,
        Demo_Gj_draw_normal_after2,
        Demo_Gj_draw_normal_after7,
    };

    DemoGj* this = (DemoGj*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= ARRAY_COUNT(proc) || proc[this->drawConfig] == NULL) {
        // "The drawing mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->drawConfig](this, play);
}

ActorProfile Demo_Gj_Profile = {
    /**/ ACTOR_DEMO_GJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GJ,
    /**/ sizeof(DemoGj),
    /**/ Demo_Gj_Actor_ct,
    /**/ Demo_Gj_Actor_dt,
    /**/ Demo_Gj_main,
    /**/ Demo_Gj_draw,
};
