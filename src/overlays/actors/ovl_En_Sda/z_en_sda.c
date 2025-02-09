/**
 * File: z_en_sda.c
 * Overlay: ovl_En_Sda
 * Description: Dynamic shadow for Link
 */

#include "z_en_sda.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Sda_actor_ct(Actor* thisx, PlayState* play);
void En_Sda_actor_dt(Actor* thisx, PlayState* play);
void En_Sda_actor_move(Actor* thisx, PlayState* play);
void En_Sda_actor_draw(Actor* thisx, PlayState* play);

void p_shadow_cont(EnSda* this, u8* shadowTexture, Player* player, PlayState* play);
void p_shadow_disp(u8* shadowTexture, Player* player, PlayState* play);
void p_shadow_tex_set(Player* player, u8* shadowTexture, f32 arg2);

ActorProfile En_Sda_Profile = {
    /**/ ACTOR_EN_SDA,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnSda),
    /**/ En_Sda_actor_ct,
    /**/ En_Sda_actor_dt,
    /**/ En_Sda_actor_move,
    /**/ En_Sda_actor_draw,
};

static Vec3f zero = { 0.0f, 0.0f, 0.0f };

static s16 p_en_dataf[] = {
    1, 2, 3, 3, 2, 1,
};

static s16 p_en_data0[] = {
    2, 3, 4, 4, 4, 3, 2, 0,
};

static s16 p_en_data1[] = {
    2, 3, 4, 4, 4, 4, 3, 2,
};

static s16 p_en_data2[] = {
    2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2,
};

static s16 p_en_data3[] = {
    2, 4, 5, 6, 7, 8, 8, 8, 8, 7, 6, 5, 4, 2,
};

static s16 p_hokan_pt[] = {
    1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 10, 2, 12, 13, 0,
};

static u8 p_size_data[] = {
    2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
};

static s8 p_joint_no[PLAYER_BODYPART_MAX] = {
    2, 9, 10, 11, 12, 13, 14, 0, 15, -1, 3, 4, 5, 6, 7, 8, -1, 1,
};

static Vec3f shield_pos_data[] = {
    { -1.0f, 2.0f, -0.2f }, { 0.0f, 2.0f, -0.5f },   { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },
    { -1.0f, 1.0f, -0.2f }, { 0.0f, 1.0f, -0.2f },   { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },
    { -2.0f, 0.0f, -0.5f }, { -1.0f, 0.0f, -0.2f },  { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },
    { 2.0f, 0.0f, -0.5f },  { -2.0f, -1.0f, -0.5f }, { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },
    { 1.0f, -1.0f, -0.2f }, { 2.0f, -1.0f, -0.5f },  { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },
    { 1.0f, -2.0f, -0.2f }, { 0.0f, -3.0f, -0.5f },
};

// Unused, identical to shield_pos_data
static Vec3f shield_pos_data_child[] = {
    { -1.0f, 2.0f, -0.2f }, { 0.0f, 2.0f, -0.5f },   { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },
    { -1.0f, 1.0f, -0.2f }, { 0.0f, 1.0f, -0.2f },   { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },
    { -2.0f, 0.0f, -0.5f }, { -1.0f, 0.0f, -0.2f },  { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },
    { 2.0f, 0.0f, -0.5f },  { -2.0f, -1.0f, -0.5f }, { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },
    { 1.0f, -1.0f, -0.2f }, { 2.0f, -1.0f, -0.5f },  { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },
    { 1.0f, -2.0f, -0.2f }, { 0.0f, -3.0f, -0.5f },
};

static u32 dammy[] = { 0, 0 };

#include "assets/overlays/ovl_En_Sda/z_en_sda.c"

static Vec3f shape_pos[16];

void En_Sda_actor_ct(Actor* thisx, PlayState* play) {
}

void En_Sda_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Sda_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSda* this = (EnSda*)thisx;
    Player* player;

    PRINTF("SDA MOVE\n");

    if (this->actor.params == 1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }

    this->actor.world.pos = player->actor.world.pos;

    PRINTF("SDA MOVE END\n");
}

void En_Sda_actor_draw(Actor* thisx, PlayState* play) {
    EnSda* this = (EnSda*)thisx;
    Player* player;
    u8* shadowTexture = GRAPH_ALLOC(play->state.gfxCtx, 0x1000);

    PRINTF("SDA DRAW \n");

    if (this->actor.params == 1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }

    player->actor.shape.shadowAlpha = 0;
    p_shadow_cont(this, shadowTexture, player, play);

    if (KREG(0) < 5) {
        p_shadow_disp(shadowTexture, player, play);
    }

    PRINTF("SDA DRAW END\n");
}

void p_shadow_tex_set(Player* player, u8* shadowTexture, f32 arg2) {
    s16 temp_t0;
    s16 temp_t1;
    s16 temp_v1;
    s16 temp_v0;
    s16 phi_a0;
    s16 phi_a3;
    s16 i;
    s16 j;
    Vec3f lerp;
    Vec3f sp88;
    Vec3f sp7C;

    for (i = 0; i < 16; i++) {
        if ((arg2 == 0.0f) || ((j = p_hokan_pt[i]) >= 0)) {
            if (arg2 > 0.0f) {
                lerp.x = shape_pos[i].x + (shape_pos[j].x - shape_pos[i].x) * arg2;
                lerp.y = shape_pos[i].y + (shape_pos[j].y - shape_pos[i].y) * arg2;
                lerp.z = shape_pos[i].z + (shape_pos[j].z - shape_pos[i].z) * arg2;

                sp88.x = lerp.x - player->actor.world.pos.x;
                sp88.y = lerp.y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = lerp.z - player->actor.world.pos.z;
            } else {
                sp88.x = shape_pos[i].x - player->actor.world.pos.x;
                sp88.y = shape_pos[i].y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = shape_pos[i].z - player->actor.world.pos.z;
            }
            Matrix_Position(&sp88, &sp7C);
            sp7C.x *= (1.0f + (BREG(49) / 100.0f));
            sp7C.y *= (1.0f + (BREG(49) / 100.0f));
            temp_t0 = sp7C.x + 32.0f;
            temp_t1 = (s16)sp7C.y << 6;

            if (p_size_data[i] == 2) {
                for (j = 0, phi_a3 = -0x180; j < 12; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_data2[j]; phi_a0 < p_en_data2[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (p_size_data[i] == 1) {
                for (j = 0, phi_a3 = -0x100; j < 8; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_data1[j]; phi_a0 < p_en_data1[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (p_size_data[i] == 0) {
                for (j = 0, phi_a3 = -0xC0; j < 7; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_data0[j]; phi_a0 < p_en_data0[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (p_size_data[i] == 4) {
                for (j = 0, phi_a3 = -0x1C0; j < 14; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_data3[j]; phi_a0 < p_en_data3[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else {
                for (j = 0, phi_a3 = -0x80; j < 6; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_dataf[j]; phi_a0 < p_en_dataf[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            }
        }
    }
}

void p_shadow_cont(EnSda* this, u8* shadowTexture, Player* player, PlayState* play) {
    s16 temp_t0;
    s16 temp_t1;
    s16 temp_v0;
    s16 temp_v1;
    s16 phi_a0;
    s16 phi_a3;
    s16 i;
    s16 j;
    Vec3f sp194;
    Vec3f sp188;
    s32* shadowTextureTemp32;
    u8* shadowTextureTemp;
    Vec3s sp178;
    Vec3f sp16C;
    Vec3f sp64[22];

    PRINTF("SDA CONT \n");
    if (BREG(57) != 0) {
        for (shadowTextureTemp = shadowTexture, i = 0; i < 0x1000; i++, shadowTextureTemp++) {
            if ((i >= 0 && i < 0x40) || (i >= 0xFC0 && i < 0x1000) || ((i & 0x3F) == 0) || ((i & 0x3F) == 0x3F)) {
                *shadowTextureTemp = 255;
            } else {
                *shadowTextureTemp = 0;
            }
        }
    } else {
        for (shadowTextureTemp32 = (s32*)shadowTexture, i = 0; i < 0x400; i++, shadowTextureTemp32++) {
            *shadowTextureTemp32 = 0;
        }
    }
    Matrix_rotateX((BREG(50) + 70) / 100.0f, MTXMODE_NEW);
    for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
        if (p_joint_no[i] >= 0) {
            shape_pos[p_joint_no[i]] = player->bodyPartsPos[i];
        }
    }
    PRINTF("SDA CONT 2\n");
    shape_pos[0].y += 3.0f;
    shape_pos[15].x = shape_pos[0].x + ((shape_pos[15].x - shape_pos[0].x) * 1.2f);
    shape_pos[15].y = shape_pos[0].y + ((shape_pos[15].y - shape_pos[0].y) * -1.2f);
    shape_pos[15].z = shape_pos[0].z + ((shape_pos[15].z - shape_pos[0].z) * 1.2f);
    for (i = 0; i < 6; i++) {
        p_shadow_tex_set(player, shadowTexture, i / 5.0f);
    }
    PRINTF("SDA CONT 3\n");
    if (this->actor.params != 1) {
        Matrix_to_rotate_new(&player->shieldMf, &sp178, false);
        sp178.y += (KREG(87) << 0xF) + 0x8000;
        sp178.x *= (KREG(88) - 1);
        Matrix_mult(&player->shieldMf, MTXMODE_NEW);
        Matrix_Position(&zero, &sp16C);
        Matrix_rotateY(BINANG_TO_RAD_ALT(sp178.y), MTXMODE_NEW);
        Matrix_rotateX(BINANG_TO_RAD_ALT(sp178.x), MTXMODE_APPLY);
        for (i = 0; i < 22; i++) {
            Matrix_Position(&shield_pos_data[i], &sp188);
            sp64[i].x = (((KREG(82) / 100.0f) + 4.0f) * sp188.x) + sp16C.x;
            sp64[i].y = (((KREG(82) / 100.0f) + 4.0f) * sp188.y) + sp16C.y;
            sp64[i].z = (((KREG(82) / 100.0f) + 4.0f) * sp188.z) + sp16C.z;
        }
        Matrix_rotateX((BREG(50) + 70) / 100.0f, MTXMODE_NEW);
        for (i = 0; i < 22; i++) {
            sp194.x = sp64[i].x - player->actor.world.pos.x;
            sp194.y = sp64[i].y - player->actor.world.pos.y + KREG(80) + 16.0f;
            sp194.z = sp64[i].z - player->actor.world.pos.z;
            Matrix_Position(&sp194, &sp188);
            sp188.x *= (1.0f + (KREG(90) / 100.0f));
            sp188.y *= (1.0f + (KREG(90) / 100.0f));
            temp_t0 = sp188.x + 32.0f;
            temp_t1 = (s16)sp188.y << 6;

            for (j = 0; j < 6; j++) {
                for (j = 0, phi_a3 = -0xC0; j < 7; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -p_en_data0[j]; phi_a0 < p_en_data0[j] - 1; phi_a0++) {
                        temp_v0 = temp_t0 + phi_a0;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x40)) {
                            temp_v1 = temp_t1 + phi_a3;
                            if ((temp_v1 >= 0) && (temp_v1 < 0x1000)) {
                                shadowTexture[temp_v0 + temp_v1] = 255;
                            }
                        }
                    }
                }
            }
        }
    }
    if (BREG(61) == 1) {
        for (shadowTextureTemp = shadowTexture, i = 0; i < 0x1000; i++, shadowTextureTemp++) {
            if (*shadowTextureTemp != 0) {
                *shadowTextureTemp = -((i >> 6) * (BREG(60) + 4)) + (255 - BREG(61));
            }
        }
    }
    PRINTF("SDA CONT 4\n");
}

void p_shadow_disp(u8* shadowTexture, Player* player, PlayState* play) {
    s32 pad;
    f32 tempx;
    f32 tempz;
    s16 phi_s1;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_sda.c", 826);

    PRINTF("SDA D 1\n");
    _texture_decal_shadow(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 0, (BREG(52) + 50));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    Matrix_translate(player->actor.world.pos.x, player->actor.floorHeight, player->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateY(BREG(51) / 100.0f, MTXMODE_APPLY);
    Matrix_scale(1.0f, 1.0f, (BREG(63) / 10.0f) + 1.0f, MTXMODE_APPLY);
    tempx = (BREG(62) / 10.0f) + 2.0f;
    tempz = ((player->actor.world.pos.y - player->actor.floorHeight + BREG(54)) * (BREG(55) - 5) / 10.0f) + BREG(58) -
            20.0f;
    Matrix_translate(tempx, 0.0f, tempz, MTXMODE_APPLY);
    Matrix_scale(((BREG(56) - 250) / 1000.0f) + 0.6f, 1.0f, ((BREG(59) - 250) / 1000.0f) + 0.6f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_sda.c", 860);
    gSPDisplayList(POLY_XLU_DISP++, p_shadow_MODE);
    gDPLoadTextureBlock(POLY_XLU_DISP++, shadowTexture, G_IM_FMT_I, G_IM_SIZ_8b, 0x40, 0x40, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_XLU_DISP++, p_shadow_model);

    for (phi_s1 = 0; phi_s1 < KREG(78); phi_s1++) {
        Matrix_scale((KREG(79) / 100.0f) + 1.0f, 1.0f, (KREG(79) / 100.0f) + 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_sda.c", 877);
        gSPDisplayList(POLY_XLU_DISP++, p_shadow_model);
    }
    PRINTF("SDA D 2\n");
    CLOSE_DISPS(gfxCtx, "../z_en_sda.c", 882);
}
