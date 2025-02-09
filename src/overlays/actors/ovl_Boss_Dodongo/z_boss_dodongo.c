#include "z_boss_dodongo.h"
#include "assets/objects/object_kingdodongo/object_kingdodongo.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/scenes/dungeons/ddan_boss/ddan_boss_room_1.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Boss_Dodongo_actor_ct(Actor* thisx, PlayState* play);
void Boss_Dodongo_actor_dt(Actor* thisx, PlayState* play);
void Boss_Dodongo_actor_move(Actor* thisx, PlayState* play2);
void Boss_Dodongo_actor_draw(Actor* thisx, PlayState* play);

static void mode_start_demo_init(BossDodongo* this, PlayState* play);
static void mode_start_demo(BossDodongo* this, PlayState* play);
static void mode_walk(BossDodongo* this, PlayState* play);
void mode_fire_standby(BossDodongo* this, PlayState* play);
static void mode_fire(BossDodongo* this, PlayState* play);
void mode_roll_attack(BossDodongo* this, PlayState* play);
void set_bdfire(BossDodongo* this, PlayState* play, s16 params);
void mode_success(BossDodongo* this, PlayState* play);
static void mode_explode(BossDodongo* this, PlayState* play);
static void mode_down(BossDodongo* this, PlayState* play);
void mode_getup(BossDodongo* this, PlayState* play);
static void mode_walk_init(BossDodongo* this);
void mode_fail(BossDodongo* this, PlayState* play);
void mode_fail_init(BossDodongo* this);
static void mode_damage(BossDodongo* this, PlayState* play);
void Boss_Dodongo_Damage_check(BossDodongo* this, PlayState* play);
void in_cornar_check(BossDodongo* this, PlayState* play);
void see_wide_link_check(BossDodongo* this, PlayState* play);
f32 see_link_check(BossDodongo* this, PlayState* play);
f32 see_back_link_check(BossDodongo* this, PlayState* play);
void Kd_Eff_disp(PlayState* play);
void Kd_Eff_move(PlayState* play);

ActorProfile Boss_Dodongo_Profile = {
    /**/ ACTOR_EN_DODONGO,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_KINGDODONGO,
    /**/ sizeof(BossDodongo),
    /**/ Boss_Dodongo_actor_ct,
    /**/ Boss_Dodongo_actor_dt,
    /**/ Boss_Dodongo_actor_move,
    /**/ Boss_Dodongo_actor_draw,
};

#include "z_boss_dodongo.inc.c"

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_KING_DODONGO, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3000.0f, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 8200.0f, ICHAIN_STOP),
};

void texcel_mesh_816_KD(s16* arg0, u8* arg1, s16 arg2) {
    if (arg1[arg2] != 0) {
        arg0[arg2 / 2] = 0;
    }
}

void texcel_mesh_1616_KD(s16* arg0, u8* arg1, s16 arg2) {
    if (arg1[arg2] != 0) {
        arg0[arg2] = 0;
    }
}

void texcel_mesh_832_KD(s16* arg0, u8* arg1, s16 arg2) {
    if (arg1[arg2] != 0) {
        arg0[arg2] = 0;
    }
}

void texcel_mesh_1632_KD(s16* arg0, u8* arg1, s16 arg2) {
    s16 index;

    if (arg1[arg2] != 0) {
        index = ((arg2 & 0xF) + ((arg2 & 0xF0) * 2));
        arg0[index + 16] = 0;
        arg0[index] = 0;
    }
}

void texcel_mesh_3216_KD(s16* arg0, u8* arg1, s16 arg2) {
    s16 index;

    if (arg1[arg2] != 0) {
        index = ((arg2 & 0xF) * 2) + ((arg2 & 0xF0) * 2);
        arg0[index + 1] = 0;
        arg0[index] = 0;
    }
}

void tex_mesh_KD(u8* arg1, s16 arg2) {
    texcel_mesh_816_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015890), arg1, arg2);
    texcel_mesh_832_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_017210), arg1, arg2);
    texcel_mesh_1616_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015D90), arg1, arg2);
    texcel_mesh_1616_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016390), arg1, arg2);
    texcel_mesh_1616_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016590), arg1, arg2);
    texcel_mesh_1616_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016790), arg1, arg2);
    texcel_mesh_1632_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015990), arg1, arg2);
    texcel_mesh_1632_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015F90), arg1, arg2);
    texcel_mesh_3216_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016990), arg1, arg2);
    texcel_mesh_3216_KD(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016E10), arg1, arg2);
}

void texture_raster(u16* arg0, u16* floorTex, s32 arg2, f32 arg3) {
    s32 pad[2];
    s16 i;
    s16 i2;
    u16 sp54[2048];
    s16 temp;

    arg0 = SEGMENTED_TO_VIRTUAL(arg0);
    floorTex = SEGMENTED_TO_VIRTUAL(floorTex);

    for (i = 0; i < 2048; i += 32) {
        temp = sinf((((i / 32) + (s16)((arg2 * 50.0f) / 100.0f)) & 0x1F) * (M_PI / 16)) * arg3;
        for (i2 = 0; i2 < 32; i2++) {
            sp54[i + ((temp + i2) & 0x1F)] = floorTex[i + i2];
        }
    }
    for (i = 0; i < 32; i++) {
        temp = sinf(((i + (s16)((arg2 * 80.0f) / 100.0f)) & 0x1F) * (M_PI / 16)) * arg3;
        temp *= 32;
        for (i2 = 0; i2 < 2048; i2 += 32) {
            s16 temp2 = (temp + i2) & 0x7FF;

            arg0[i + temp2] = sp54[i + i2];
        }
    }
}

void Effect_kd_hinoko_ct_IN(PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, s16 countLimit) {
    s16 i;
    BossDodongoEffect* eff = (BossDodongoEffect*)play->specialEffects;

    for (i = 0; i < countLimit; i++, eff++) {
        if (eff->unk_24 == 0) {
            eff->unk_24 = 1;
            eff->unk_00 = *arg1;
            eff->unk_0C = *arg2;
            eff->unk_18 = *arg3;
            eff->unk_2C = arg4 / 1000.0f;
            eff->alpha = 255;
            eff->unk_25 = (s16)rnd_f(10.0f);
            break;
        }
    }
}

static s32 MouseVsBombCheck(BossDodongo* this, PlayState* play) {
    f32 dx;
    f32 dy;
    f32 dz;
    Actor* currentExplosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    Actor* thisx = &this->actor;

    while (currentExplosive != NULL) {
        if (currentExplosive == thisx) {
            currentExplosive = currentExplosive->next;
            continue;
        }

        dx = currentExplosive->world.pos.x - this->mouthPos.x;
        dy = currentExplosive->world.pos.y - this->mouthPos.y;
        dz = currentExplosive->world.pos.z - this->mouthPos.z;

        if ((fabsf(dx) < 40.0f) && (fabsf(dy) < 40.0f) && (fabsf(dz) < 40.0f)) {
            Actor_delete(currentExplosive);
            return true;
        }

        currentExplosive = currentExplosive->next;
    }

    return false;
}

void Boss_Dodongo_actor_ct(Actor* thisx, PlayState* play) {
    BossDodongo* this = (BossDodongo*)thisx;
    s16 i;

    play->specialEffects = this->effects;
    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 9200.0f, Actor_shadow_circle, 250.0f);
    Actor_set_scale(&this->actor, 0.01f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_kingdodongo_Skel_01B310, &object_kingdodongo_Anim_00F0D8, NULL, NULL,
                   0);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_kingdodongo_Anim_00F0D8);
    this->unk_1F8 = 1.0f;
    mode_start_demo_init(this, play);
    this->health = 12;
    this->colorFilterMin = 995.0f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->colorFilterMax = 1000.0f;
    this->unk_224 = 2.0f;
    this->unk_228 = 9200.0f;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &KdAcOcInfoJntSphData, this->items);

    if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) { // KD is dead
        u16* temp_s1_3 = SEGMENTED_TO_VIRTUAL(gDodongosCavernBossLavaFloorTex);
        u16* temp_s2 = SEGMENTED_TO_VIRTUAL(change_yogan_txt);
        u32 temp_v0;

        Actor_delete(&this->actor);
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, -890.0f, -1523.76f, -3304.0f, 0, 0, 0,
                           WARP_DUNGEON_CHILD);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_BREAKWALL, -890.0f, -1523.76f, -3304.0f, 0, 0, 0, 0x6000);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, -690.0f, -1523.76f, -3304.0f, 0, 0, 0, 0);

        for (i = 0; i < 2048; i++) {
            temp_v0 = i;
            temp_s1_3[temp_v0] = temp_s2[temp_v0];
        }
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Boss_Dodongo_actor_dt(Actor* thisx, PlayState* play) {
    BossDodongo* this = (BossDodongo*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mode_start_demo_init(BossDodongo* this, PlayState* play) {
    s16 frames = Si2_anime_end_frame(&object_kingdodongo_Anim_00F0D8);

    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_00F0D8, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_start_demo;
    this->csState = 0;
    this->unk_1BC = 1;
}

static void mode_start_demo(BossDodongo* this, PlayState* play) {
    f32 phi_f0;
    Camera* mainCam;
    Player* player = GET_PLAYER(play);
    Vec3f subCamEye;
    Vec3f subCamAt;
    Vec3f subCamUp;

    mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

    if (this->unk_196 != 0) {
        this->unk_196--;
    }

    if (this->unk_198 != 0) {
        this->unk_198--;
    }

    if (this->unk_19A != 0) {
        this->unk_19A--;
    }

    switch (this->csState) {
        case 0:
            if (player->actor.world.pos.y < -1223.76f) {
                this->csState = 1;
                this->actor.world.pos.x = -1390.0f;
                this->actor.world.pos.z = -3374.0f;
                this->unk_1A0 = 1;
            }
            break;
        case 1:
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            Gama_play_clear_camera_all(play);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->csState = 2;
            this->unk_196 = 0x3C;
            this->unk_198 = 160;
            player->actor.world.pos.y = -1023.76f;
            this->subCamEye.y = player->actor.world.pos.y - 480.0f + 50.0f;
            FALLTHROUGH;
        case 2:
            if (this->unk_198 >= 131) {
                player->actor.world.pos.x = -890.0f;
                player->actor.world.pos.z = -2804.0f;

                player->actor.speed = 0.0f;
                player->actor.shape.rot.y = player->actor.world.rot.y = 0x3FFF;

                this->subCamEye.x = -890.0f;
                this->subCamEye.z = player->actor.world.pos.z - 100.0f;

                this->subCamAt.x = player->actor.world.pos.x;
                this->subCamAt.y = player->actor.world.pos.y + 20.0f;
                this->subCamAt.z = player->actor.world.pos.z;
            }

            if (this->unk_198 == 110) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_9);
            }

            if (this->unk_198 == 5) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_12);
            }

            if (this->unk_198 < 6) {
                player->actor.shape.rot.y = -0x4001;
            } else {
                player->actor.shape.rot.y = 0x3FFF;
            }

            if (this->unk_198 < 60) {
                this->unk_1BC = 1;
            } else {
                this->unk_1BC = 2;
            }

            mode_walk(this, play);

            if (this->unk_196 == 1) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
            }

            if (this->unk_196 == 0) {
                add_calc(&this->subCamEye.x, this->vec.x + 30.0f, 0.2f, this->unk_204 * 20.0f, 0.0f);
                add_calc(&this->subCamEye.y, this->vec.y, 0.2f, this->unk_204 * 20.0f, 0.0f);
                add_calc(&this->subCamEye.z, this->vec.z + 10.0f, 0.2f, this->unk_204 * 20.0f, 0.0f);
                add_calc(&this->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
            } else {
                this->subCamAt.x = player->actor.world.pos.x;
                this->subCamAt.y = player->actor.world.pos.y + 20.0f;
                this->subCamAt.z = player->actor.world.pos.z;
            }

            if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE)) {
                if (this->unk_198 == 100) {
                    this->actor.world.pos.x = -1114.0f;
                    this->actor.world.pos.z = -2804.0f;
                    this->actor.world.rot.y = 0x3FFF;
                    this->unk_1A2 = 0;
                    this->unk_1A0 = 2;
                    this->csState = 4;
                    this->unk_196 = 30;
                    this->unk_198 = 150;
                    this->unk_204 = 0.0f;
                    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                                     Si2_anime_end_frame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, 0.0f);
                    Skeleton_Info2_anime_play(&this->skelAnime);
                }
            } else if (this->unk_198 == 0) {
                this->csState = 3;
                this->unk_19E = 0x14;
                this->unk_204 = 0.0f;
            }
            break;
        case 3:
            mode_walk(this, play);
            add_calc(&this->unk_20C, sinf(this->unk_19E * 0.05f) * 0.1f, 1.0f, 0.01f, 0.0f);
            add_calc(&this->subCamEye.x, this->vec.x + 90.0f, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamEye.y, this->vec.y + 50.0f, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamEye.z, this->vec.z, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamAt.y, this->vec.y - 10.0f, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
            if (fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < 200.0f) {
                this->csState = 4;
                this->unk_196 = 0x1E;
                this->unk_198 = 0x96;
                this->unk_204 = 0.0f;
                Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, -5.0f);
            }
            break;
        case 4:
            add_calc(&this->unk_20C, 0.0f, 1.0f, 0.01f, 0.0f);

            if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE)) {
                phi_f0 = -50.0f;
            } else {
                phi_f0 = 0.0f;
            }

            add_calc(&this->subCamEye.x, player->actor.world.pos.x + phi_f0 + 70.0f, 0.2f,
                               this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamEye.y, player->actor.world.pos.y + 10.0f, 0.2f, this->unk_204 * 20.0f,
                               0.0f);
            add_calc(&this->subCamEye.z, player->actor.world.pos.z - 60.0f, 0.2f, this->unk_204 * 20.0f,
                               0.0f);

            add_calc(&this->subCamAt.x, this->vec.x, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamAt.y, this->vec.y, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->subCamAt.z, this->vec.z, 0.2f, this->unk_204 * 20.0f, 0.0f);
            add_calc(&this->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);

            if (this->unk_196 == 0) {
                Skeleton_Info2_anime_play(&this->skelAnime);
                add_calc(&this->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
            }

            if (this->unk_198 == 0x64) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_OTAKEBI);
            }

            if (this->unk_198 == 0x5A) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE)) {
                    Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gKingDodongoTitleCardTex), 160, 180, 128, 40);
                }
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_FIRE_BOSS);
            }

            if (this->unk_198 == 0) {
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                mode_walk_init(this);
                this->unk_1DA = 50;
                this->unk_1BC = 0;
                player->actor.shape.rot.y = -0x4002;
                SET_EVENTCHKINF(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE);
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (this->unk_1B6 != 0) {
            this->unk_1B6--;
        }

        subCamEye.x = this->subCamEye.x;
        phi_f0 = sinf((this->unk_1B6 * 3.1415f * 90.0f) / 180.0f);
        subCamEye.y = (this->unk_1B6 * phi_f0 * 0.7f) + this->subCamEye.y;
        subCamEye.z = this->subCamEye.z;

        subCamAt.x = this->subCamAt.x;
        phi_f0 = sinf((this->unk_1B6 * 3.1415f * 90.0f) / 180.0f);
        subCamAt.y = (this->unk_1B6 * phi_f0 * 0.7f) + this->subCamAt.y;
        subCamAt.z = this->subCamAt.z;

        subCamUp.x = this->unk_20C;
        subCamUp.y = 1.0f;
        subCamUp.z = this->unk_20C;

        Gama_play_camera_lookat(play, this->subCamId, &subCamAt, &subCamEye, &subCamUp);
    }
}

static void mode_damage_init(BossDodongo* this) {
    if (this->actionFunc != mode_damage) {
        Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_001074, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_kingdodongo_Anim_001074), ANIMMODE_ONCE, -5.0f);
        this->actionFunc = mode_damage;
    }

    this->unk_1DA = 100;
}

void mode_success_init(BossDodongo* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_00E848, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_kingdodongo_Anim_00E848), ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_success;
    this->unk_1B0 = 10;
    this->unk_1C0 = 2;
    this->unk_1DA = 35;
    this->unk_1FC = 50.0f;
    this->unk_200 = 300.0f;
}

static void mode_walk_init(BossDodongo* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_01D934, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_kingdodongo_Anim_01D934), ANIMMODE_ONCE, -10.0f);
    this->unk_1AA = 0;
    this->actionFunc = mode_walk;
    this->unk_1DA = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_1E4 = 0.0f;
}

void mode_roll_attack_init(BossDodongo* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_00DF38, 1.0f, 0.0f, 59.0f, ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_roll_attack;
    this->numWallCollisions = 0;
    this->unk_1DA = 27;
}

static void mode_fire_init(BossDodongo* this) {
    this->actor.speed = this->unk_1E4 = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_0061D4, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_kingdodongo_Anim_0061D4), ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_fire;
    this->unk_1DA = 50;
    this->unk_1AE = 0;
}

void mode_fire_standby_init(BossDodongo* this) {
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_fire_standby;
    this->unk_1DA = 100;
    this->unk_1AC = 0;
    this->unk_1E2 = 1;
}

static void mode_damage(BossDodongo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc(&this->unk_1F8, 1.0f, 0.5f, 0.02f, 0.001f);
    add_calc(&this->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);

    if (Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&object_kingdodongo_Anim_001074))) {
        mode_roll_attack_init(this);
    }
}

void mode_success(BossDodongo* this, PlayState* play) {
    static Color_RGBA8 prim = { 255, 255, 0, 255 };
    static Color_RGBA8 env = { 255, 10, 0, 255 };
    s16 pad;
    Vec3f vec;
    Vec3f acc;
    Vec3f dustPos;
    s16 i;

    add_calc(&this->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1DA == 0) {
        for (i = 0; i < 30; i++) {
            vec.x = rnd_fx(20.0f);
            vec.y = rnd_fx(20.0f);
            vec.z = rnd_fx(20.0f);

            acc.x = vec.x * -0.1f;
            acc.y = vec.y * -0.1f;
            acc.z = vec.z * -0.1f;

            dustPos.x = this->actor.world.pos.x + (vec.x * 3.0f);
            dustPos.y = this->actor.world.pos.y + 90.0f + (vec.y * 3.0f);
            dustPos.z = this->actor.world.pos.z + (vec.z * 3.0f);

            Effect_SS_Dust_sc_cl_co_ct(play, &dustPos, &vec, &acc, &prim, &env, 500, 10, 10);
        }

        Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_004E0C, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_kingdodongo_Anim_004E0C), ANIMMODE_ONCE, -5.0f);
        this->actionFunc = mode_explode;
        Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DAMAGE);
        ActorQuakeRumbleSet(&this->actor, play, 4, 10);
        this->health -= 2;

        // make sure not to die from the bomb explosion
        if (this->health <= 0) {
            this->health = 1;
        }
    }
}

static void mode_explode(BossDodongo* this, PlayState* play) {
    this->unk_1BE = 10;
    add_calc(&this->unk_1F8, 1.3f, 1.0f, 0.1f, 0.001f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&object_kingdodongo_Anim_004E0C))) {
        Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_0042A8, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_kingdodongo_Anim_0042A8), ANIMMODE_LOOP, -5.0f);
        this->actionFunc = mode_down;
        this->unk_1DA = 100;
    }
}

static void mode_down(BossDodongo* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DOWN - SFX_FLAG);
    this->unk_1BE = 10;
    add_calc(&this->unk_1F8, 1.0f, 0.5f, 0.02f, 0.001f);
    add_calc(&this->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1DA == 0) {
        Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_009D10, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_kingdodongo_Anim_009D10), ANIMMODE_ONCE, -5.0f);
        this->actionFunc = mode_getup;
    }
}

void mode_getup(BossDodongo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&object_kingdodongo_Anim_009D10))) {
        mode_roll_attack_init(this);
    }
}

static void mode_fire(BossDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f unusedZeroVec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f unusedZeroVec2 = { 0.0f, 0.0f, 0.0f };

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_CRY);
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 17.0f)) {
        this->unk_1C8 = 28;
    }

    if ((this->skelAnime.curFrame > 17.0f) && (this->skelAnime.curFrame < 35.0f)) {
        set_bdfire(this, play, this->unk_1AE);
        this->unk_1AE++;
        add_calc(&this->unk_244, 0.0f, 1.0f, 8.0f, 0.0f);
    }

    if (this->unk_1DA == 0) {
        mode_roll_attack_init(this);
    }
}

void mode_fire_standby(BossDodongo* this, PlayState* play) {
    this->unk_1E2 = 1;

    if (this->unk_1AC > 20) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_BREATH - SFX_FLAG);
    }

    add_calc(&this->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1DA == 0) {
        mode_fire_init(this);
    } else {
        this->unk_1AC++;

        if ((this->unk_1AC > 20) && (this->unk_1AC < 82) && MouseVsBombCheck(this, play)) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DRINK);
            mode_success_init(this);
        }
    }
}

static Vec3f kd_target[] = {
    { -1390.0f, 0.0f, -3804.0f },
    { -1390.0f, 0.0f, -2804.0f },
    { -390.0f, 0.0f, -2804.0f },
    { -390.0f, 0.0f, -3804.0f },
};

static void mode_walk(BossDodongo* this, PlayState* play) {
    Vec3f* sp4C;
    f32 sp48;
    f32 sp44;

    if (this->unk_1AA == 0) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 14.0f)) {
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_kingdodongo_Anim_01CAE0);
            this->unk_1AA = 1;
        }
    } else if (this->unk_1BC != 2) {
        if (((s32)this->skelAnime.curFrame == 1) || ((s32)this->skelAnime.curFrame == 31)) {
            if ((s32)this->skelAnime.curFrame == 1) {
                _dust_ground_set(play, &this->actor, &this->unk_410, 25.0f, 10, 8.0f, 500, 10, false);
            } else {
                _dust_ground_set(play, &this->actor, &this->unk_404, 25.0f, 10, 8.0f, 500, 10, false);
            }

            if (this->unk_1BC != 0) {
                Na_StartSystemSe_F(NA_SE_EN_DODO_K_WALK);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_WALK);
            }

            if (this->subCamId == SUB_CAM_ID_DONE) {
                ActorQuakeRumbleSet(&this->actor, play, 4, 10);
            } else {
                this->unk_1B6 = 10;
                z_vibctl2_vib_force_set(0.0f, 180, 20, 100);
            }
        }
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    sp4C = &kd_target[this->unk_1A0];
    this->unk_1EC = 0.7f;
    add_calc(&this->unk_1E4, this->unk_1EC * 4.0f, 1.0f, this->unk_1EC * 0.25f, 0.0f);
    add_calc(&this->actor.world.pos.x, sp4C->x, 0.3f, this->unk_1E4, 0.0f);
    add_calc(&this->actor.world.pos.z, sp4C->z, 0.3f, this->unk_1E4, 0.0f);
    sp48 = sp4C->x - this->actor.world.pos.x;
    sp44 = sp4C->z - this->actor.world.pos.z;
    add_calc(&this->unk_1E8, 2000.0f, 1.0f, this->unk_1EC * 80.0f, 0.0f);
    add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(sp48, sp44)), 5,
                       (this->unk_1EC * this->unk_1E8), 5);
    add_calc_short_angle2(&this->unk_1C4, 0, 2, 2000, 0);

    if ((fabsf(sp48) <= 5.0f) && (fabsf(sp44) <= 5.0f)) {
        this->unk_1E8 = 0.0f;
        this->unk_1E4 = 0.0f;
        if (this->unk_1A2 == 0) {
            this->unk_1A0++;
            if (this->unk_1A0 >= 4) {
                this->unk_1A0 = 0;
            }
        } else {
            this->unk_1A0--;
            if (this->unk_1A0 < 0) {
                this->unk_1A0 = 3;
            }
        }
    }

    if ((this->unk_1DA == 0) && (this->unk_1BC == 0)) {
        if ((this->actor.xzDistToPlayer < 500.0f) && (this->unk_1A4 != 0) && !this->playerPosInRange) {
            mode_fire_standby_init(this);
            set_bdfire(this, play, -1);
        }

        if (!this->playerPosInRange && !this->playerYawInRange) {
            mode_roll_attack_init(this);
        }
    }
}

void mode_roll_attack(BossDodongo* this, PlayState* play) {
    Vec3f* sp5C;
    Vec3f sp50;
    f32 sp4C;
    f32 sp48;

    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1DA == 10) {
        this->actor.velocity.y = 15.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_CRY);
    }

    if (this->unk_1DA == 1) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_COLI2);
    }

    sp5C = &kd_target[this->unk_1A0];
    this->unk_1EC = 3.0f;

    if (this->unk_1DA == 0) {
        add_calc(&this->unk_1E4, this->unk_1EC * 5.0f, 1.0f, this->unk_1EC * 0.25f, 0.0f);
        add_calc(&this->actor.world.pos.x, sp5C->x, 1.0f, this->unk_1E4, 0.0f);
        add_calc(&this->actor.world.pos.z, sp5C->z, 1.0f, this->unk_1E4, 0.0f);
        this->unk_1C4 += 2000;

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->unk_228 = 7700.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);

            if ((this->unk_19E & 7) == 0) {
                setDamageCamera(&play->mainCamera, 2, 1, 8);
            }

            if (!(this->unk_19E & 1)) {
                _dust_ground_set(play, &this->actor, &this->actor.world.pos, 40.0f, 3, 8.0f, 500, 10, false);
            }
        }
    }

    sp4C = sp5C->x - this->actor.world.pos.x;
    sp48 = sp5C->z - this->actor.world.pos.z;
    add_calc(&this->unk_1E8, 2000.0f, 1.0f, this->unk_1EC * 100.0f, 0.0f);
    add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(sp4C, sp48)), 5,
                       this->unk_1EC * this->unk_1E8, 0);

    if (fabsf(sp4C) <= 15.0f && fabsf(sp48) <= 15.0f) {
        this->numWallCollisions++;

        if (this->numWallCollisions >= 2) {
            if (this->unk_1A6 != 0) {
                this->unk_1A2 = 1 - this->unk_1A2;
            }

            this->unk_1E8 = 0.0f;
            this->unk_1E4 = 0.0f;
            mode_walk_init(this);
            this->unk_228 = 9200.0f;
            this->actor.velocity.y = 20.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_COLI);
            setDamageCamera(&play->mainCamera, 2, 6, 8);
            sp50.x = this->actor.world.pos.x;
            sp50.y = this->actor.world.pos.y + 60.0f;
            sp50.z = this->actor.world.pos.z;
            dust_fly_set2(play, &sp50, 250.0f, 40, 800, 10, 0);
            ActorQuakeRumbleSet(&this->actor, play, 6, 15);
        } else {
            this->actor.velocity.y = 15.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_COLI2);
        }

        if (this->unk_1A2 == 0) {
            this->unk_1A0++;
            if (this->unk_1A0 >= 4) {
                this->unk_1A0 = 0;
            }
        } else {
            this->unk_1A0--;
            if (this->unk_1A0 < 0) {
                this->unk_1A0 = 3;
            }
        }
    }
}

void Boss_Dodongo_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossDodongo* this = (BossDodongo*)thisx;
    f32 temp_f0;
    s16 i;
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);
    s32 pad;

    this->unk_1E2 = 0;
    this->unk_19E++;

    if (this->unk_1DA != 0) {
        this->unk_1DA--;
    }

    if (this->unk_1DC != 0) {
        this->unk_1DC--;
    }

    if (this->unk_1DE != 0) {
        this->unk_1DE--;
    }

    if (this->unk_1C0 != 0) {
        this->unk_1C0--;
    }

    if (this->unk_1C8 != 0) {
        this->unk_1C8--;
    }

    temp_f0 = see_link_check(this, play);

    if (temp_f0 > 0.0f) {
        this->unk_1A4 = temp_f0;
    } else {
        this->unk_1A4 = 0;
    }

    temp_f0 = see_back_link_check(this, play);

    if (temp_f0 > 0.0f) {
        this->unk_1A6 = temp_f0;
    } else {
        this->unk_1A6 = 0;
    }

    see_wide_link_check(this, play);
    in_cornar_check(this, play);

    this->actionFunc(this, play);

    thisx->shape.rot.y = thisx->world.rot.y;

    add_calc(&thisx->shape.yOffset, this->unk_228, 1.0f, 100.0f, 0.0f);
    Actor_position_moveF(thisx);
    Boss_Dodongo_Damage_check(this, play);
    Actor_BGcheck2(play, thisx, 10.0f, 10.0f, 20.0f, UPDBGCHECKINFO_FLAG_2);
    add_calc(&this->unk_208, 0, 1, 0.001f, 0.0);
    add_calc(&this->unk_20C, 0, 1, 0.001f, 0.0);

    if ((this->unk_19E % 128) == 0) {
        for (i = 0; i < 50; i++) {
            this->unk_324[i] = (fqrand() * 0.25f) + 0.5f;
        }
    }

    for (i = 0; i < 50; i++) {
        this->unk_25C[i] += this->unk_324[i];
    }

    if (this->unk_1C8 != 0) {
        if (this->unk_1C8 >= 11) {
            add_calc(&this->unk_240, (this->unk_1C8 & 1) ? (40.0f) : (60.0f), 1.0f, 50.0f, 0.0f);
        } else {
            add_calc(&this->unk_240, 0.0f, 1, 10.0f, 0.0);
        }

        if ((play->envCtx.adjLight1Color[2] == 0) && (play->envCtx.adjAmbientColor[2] == 0)) {
            play->envCtx.adjLight1Color[0] = (u8)this->unk_240;
            play->envCtx.adjLight1Color[1] = (u8)(this->unk_240 * 0.1f);
            play->envCtx.adjAmbientColor[0] = (u8)this->unk_240;
            play->envCtx.adjAmbientColor[1] = (u8)(this->unk_240 * 0.1f);
        }
    }

    if (this->unk_1BE != 0) {
        if (this->unk_1BE >= 1000) {
            add_calc(&this->colorFilterR, 30.0f, 1, 20.0f, 0.0);
            add_calc(&this->colorFilterG, 10.0f, 1, 20.0f, 0.0);
        } else {
            this->unk_1BE--;
            add_calc(&this->colorFilterR, 255.0f, 1, 20.0f, 0.0);
            add_calc(&this->colorFilterG, 0.0f, 1, 20.0f, 0.0);
        }

        add_calc(&this->colorFilterB, 0.0f, 1, 20.0f, 0.0);
        add_calc(&this->colorFilterMin, 900.0f, 1, 10.0f, 0.0);
        add_calc(&this->colorFilterMax, 1099.0f, 1, 10.0f, 0.0);
    } else {
        add_calc(&this->colorFilterR, play->lightCtx.fogColor[0], 1, 5.0f, 0.0);
        add_calc(&this->colorFilterG, play->lightCtx.fogColor[1], 1, 5.0f, 0.0);
        add_calc(&this->colorFilterB, play->lightCtx.fogColor[2], 1, 5.0f, 0.0);
        add_calc(&this->colorFilterMin, play->lightCtx.fogNear, 1.0, 5.0f, 0.0);
        add_calc(&this->colorFilterMax, 1000.0f, 1, 5.0f, 0.0);
    }

    if (player->actor.world.pos.y < -1000.0f) {
        s16 phi_s0_3;
        s16 sp90;
        s16 magma2DrawMode;
        s16 magmaScale = 0;

        if (this->unk_224 > 1.9f) {
            phi_s0_3 = 1;
            magma2DrawMode = 0;
            sp90 = 0;
        } else if (this->unk_224 > 1.7f) {
            phi_s0_3 = 3;
            sp90 = 1;
            magma2DrawMode = 0;
        } else if (this->unk_224 > 1.4f) {
            phi_s0_3 = 7;
            sp90 = 3;
            magma2DrawMode = fqrand() * 1.9f;
        } else if (this->unk_224 > 1.1f) {
            phi_s0_3 = 7;
            sp90 = 4095;
            magma2DrawMode = fqrand() * 1.9f;
        } else {
            phi_s0_3 = 1;
            sp90 = -1;
            magma2DrawMode = 1;
            magmaScale = ((s16)(fqrand() * 50)) - 50;
        }

        if (player2->csAction >= PLAYER_CSACTION_10) {
            phi_s0_3 = -1;
        }

        if ((this->unk_19E & phi_s0_3) == 0) {
            static Color_RGBA8 prim[] = { { 255, 255, 0, 255 }, { 0, 0, 0, 150 } };
            static Color_RGBA8 env[] = { { 255, 0, 0, 255 }, { 0, 0, 0, 0 } };
            Vec3f sp84;
            f32 temp_f12;
            f32 temp_f10;

            temp_f12 = fqrand() * 330.0f;
            temp_f10 = fqrand() * 6.28f;
            sp84.x = (sinf(temp_f10) * temp_f12) + (-890.0f);
            sp84.y = -1523.76f;
            sp84.z = (cosf(temp_f10) * temp_f12) + (-3304.0f);
            Effect_SS_Magma2_ct(play, &sp84, &prim[magma2DrawMode], &env[magma2DrawMode],
                                  10 - (magma2DrawMode * 5), magma2DrawMode, magmaScale + 100);
        }

        if ((this->unk_19E & sp90) == 0) {
            Vec3f sp6C = { 0.0f, 0.0f, 0.0f };
            Vec3f sp60 = { 0.0f, 0.0f, 0.0f };
            Vec3f sp54;
            f32 sp50 = fqrand() * 330.0f;
            f32 sp4C = fqrand() * 6.28f;

            sp54.x = sinf(sp4C) * sp50 + (-890.0f);
            sp54.y = -1523.76f;
            sp54.z = cosf(sp4C) * sp50 + (-3304.0f);
            Effect_SS_G_Magma_ct(play, &sp54);
            for (i = 0; i < 4; i++) {
                sp60.y = 0.4f;
                sp60.x = rnd_fx(0.5f);
                sp60.z = rnd_fx(0.5f);
                sp50 = fqrand() * 330.0f;
                sp4C = fqrand() * 6.28f;
                sp54.x = sinf(sp4C) * sp50 + (-890.0f);
                sp54.y = -1513.76f;
                sp54.z = cosf(sp4C) * sp50 + (-3304.0f);
                Effect_kd_hinoko_ct_IN(play, &sp54, &sp6C, &sp60, ((s16)rnd_f(2.0f)) + 6, BOSS_DODONGO_EFFECT_COUNT);
            }
        }

        texture_raster((u16*)gDodongosCavernBossLavaFloorTex, (u16*)ddan_a_dameuy3_txt_6_S, this->unk_19E, this->unk_224);
    }

    if (this->unk_1C6 != 0) {
        u16* ptr1 = SEGMENTED_TO_VIRTUAL(ddan_a_dameuy3_txt_6_S);
        u16* ptr2 = SEGMENTED_TO_VIRTUAL(change_yogan_txt);
        s16 i2;
        s16 new_var;

        for (i2 = 0; i2 < 20; i2++) {
            new_var = this->unk_1C2 & 0x7FF;

            ptr1[new_var] = ptr2[new_var];
            this->unk_1C2 += 37;
        }
        add_calc(&this->unk_224, 0.0f, 1.0f, 0.01f, 0.0f);
    }

    if (this->unk_1BC == 0) {
        if (this->actionFunc != mode_fail) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

        if (this->actionFunc == mode_roll_attack) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
    }

    this->collider.elements[0].dim.scale = (this->actionFunc == mode_fire_standby) ? 0.0f : 1.0f;

    for (i = 6; i < 19; i++) {
        if (i == 12) {
            continue;
        }
        this->collider.elements[i].dim.scale = (this->actionFunc == mode_roll_attack) ? 0.0f : 1.0f;
    }

    if (this->unk_244 != 0) {
        MREG(64) = 1;
        MREG(65) = 255;
        MREG(66) = 80;
        MREG(67) = 0;
        MREG(68) = (u8)this->unk_244;
    } else {
        MREG(64) = 0;
    }

    add_calc(&this->unk_244, 0.0f, 1.0f, 2.0f, 0.0f);
    Kd_Eff_move(play);
}

s32 Boss_Dodongo_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    f32 mtxScaleY;
    f32 mtxScaleZ;
    BossDodongo* this = (BossDodongo*)thisx;

    switch (limbIndex) {
        case 6:
        case 7:
        case 8:
            if (this->unk_25C[limbIndex] != 0.0f) {}
            break;

        default:
            break;
    }

    Matrix_softcv3_mult(pos, rot);

    if (*dList != NULL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_dodongo.c", 3787);

        mtxScaleZ = 1.0f;
        mtxScaleY = 1.0f;

        if ((limbIndex == 33) || (limbIndex == 48)) {
            mtxScaleY = mtxScaleZ = this->unk_1F8;
        }

        Matrix_push();
        Matrix_scale(1.0f, mtxScaleY, mtxScaleZ, MTXMODE_APPLY);

        if ((limbIndex != 6) && (limbIndex != 7)) {
            Matrix_rotateX(this->unk_25C[limbIndex] * 0.115f, MTXMODE_APPLY);
            Matrix_rotateY(this->unk_25C[limbIndex] * 0.13f, MTXMODE_APPLY);
            Matrix_rotateZ(this->unk_25C[limbIndex] * 0.1f, MTXMODE_APPLY);
            Matrix_scale(1.0f - this->unk_208, this->unk_208 + 1.0f, 1.0f - this->unk_208, MTXMODE_APPLY);
            Matrix_rotateZ(-(this->unk_25C[limbIndex] * 0.1f), MTXMODE_APPLY);
            Matrix_rotateY(-(this->unk_25C[limbIndex] * 0.13f), MTXMODE_APPLY);
            Matrix_rotateX(-(this->unk_25C[limbIndex] * 0.115f), MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_dodongo.c", 3822);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();

        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_dodongo.c", 3826);
    }
    return true;
}

void Boss_Dodongo_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f eye_p = { 5000.0f, -2500.0f, 0.0f };
    static Vec3f fire_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f mouse_p = { 11500.0f, -3000.0f, 0.0f };
    static Vec3f mouse_hit_p = { 5000.0f, -2000.0f, 0.0f };
    static Vec3f r_foot_p = { 8000.0f, 0.0f, 0.0f };
    static Vec3f l_foot_p = { 8000.0f, 0.0f, 0.0f };
    BossDodongo* this = (BossDodongo*)thisx;

    if (limbIndex == 6) {
        Matrix_Position(&fire_p, &this->vec);
        Matrix_Position(&eye_p, &this->actor.focus.pos);
        Matrix_Position(&mouse_p, &this->firePos);
        Matrix_Position(&mouse_hit_p, &this->mouthPos);
    } else if (limbIndex == 39) {
        Matrix_Position(&r_foot_p, &this->unk_410);
    } else if (limbIndex == 46) {
        Matrix_Position(&l_foot_p, &this->unk_404);
    }
    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
}

void Boss_Dodongo_actor_draw(Actor* thisx, PlayState* play) {
    BossDodongo* this = (BossDodongo*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_dodongo.c", 3922);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    if ((this->unk_1C0 >= 2) && (this->unk_1C0 & 1)) {
        POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
    } else {
        POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, (u32)this->colorFilterR, (u32)this->colorFilterG,
                                   (u32)this->colorFilterB, 0, this->colorFilterMin, this->colorFilterMax);
    }

    Matrix_rotateZ(this->unk_23C, MTXMODE_APPLY);
    Matrix_rotateX((this->unk_1C4 / 32768.0f) * 3.14159f, MTXMODE_APPLY);

    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, Boss_Dodongo_draw_sub,
                      Boss_Dodongo_draw_sub2, this);

    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_dodongo.c", 3981);

    Kd_Eff_disp(play);
}

f32 see_link_check(BossDodongo* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    f32 sp2C;
    s32 pad;
    f32 temp_f2;
    f32 rotation;
    Player* player = GET_PLAYER(play);

    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

    rotation = cos_s(-this->actor.world.rot.y);
    sp2C = (sin_s(-this->actor.world.rot.y) * zDiff) + (rotation * xDiff);
    rotation = sin_s(-this->actor.world.rot.y);
    temp_f2 = (cos_s(-this->actor.world.rot.y) * zDiff) + (-rotation * xDiff);

    if ((fabsf(sp2C) < 150.0f) && (temp_f2 >= 100.0f) && (temp_f2 <= 2000.0f)) {
        return temp_f2;
    }
    return -1.0f;
}

f32 see_back_link_check(BossDodongo* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    f32 sp2C;
    s32 pad;
    f32 temp_f2;
    f32 rotation;
    Player* player = GET_PLAYER(play);

    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

    rotation = cos_s(-0x8000 - this->actor.world.rot.y);
    sp2C = (sin_s(-0x8000 - this->actor.world.rot.y) * zDiff) + (rotation * xDiff);
    rotation = sin_s(-0x8000 - this->actor.world.rot.y);
    temp_f2 = (cos_s(-0x8000 - this->actor.world.rot.y) * zDiff) + (-rotation * xDiff);

    if ((fabsf(sp2C) < 150.0f) && (100.0f <= temp_f2) && (temp_f2 <= 2000.0f)) {
        return temp_f2;
    }

    return -1.0f;
}

void see_wide_link_check(BossDodongo* this, PlayState* play) {
    s16 yawDiff = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

    if ((yawDiff < 0x38E3) && (-0x38E3 < yawDiff)) {
        this->playerYawInRange = true;
    } else {
        this->playerYawInRange = false;
    }
}

void in_cornar_check(BossDodongo* this, PlayState* play) {
    Vec3f* temp_v1;
    s16 i;

    this->playerPosInRange = false;

    for (i = 0; i < 4; i++) {
        temp_v1 = &kd_target[i];

        if ((fabsf(this->actor.world.pos.x - temp_v1->x) < 200.0f) &&
            (fabsf(this->actor.world.pos.z - temp_v1->z) < 200.0f)) {
            this->playerPosInRange = true;
            break;
        }
    }
}

void set_bdfire(BossDodongo* this, PlayState* play, s16 params) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_BDFIRE, this->vec.x, this->vec.y - 20.0f,
                       this->vec.z, 0, this->actor.shape.rot.y, 0, params);
}

void Boss_Dodongo_Damage_check(BossDodongo* this, PlayState* play) {
    s32 pad;
    ColliderElement* acHitElem;
    u8 swordDamage;
    s32 damage;
    s16 i;

    if ((this->health <= 0) && (this->actionFunc != mode_fail)) {
        mode_fail_init(this);
        Actor_info_finish(play, &this->actor);
        return;
    }

    if (this->unk_1C0 == 0) {
        if (this->actionFunc == mode_fire_standby) {
            for (i = 0; i < 19; i++) {
                if (this->collider.elements[i].base.acElemFlags & ACELEM_HIT) {
                    acHitElem = this->collider.elements[i].base.acHitElem;

                    if ((acHitElem->atDmgInfo.dmgFlags & DMG_BOOMERANG) ||
                        (acHitElem->atDmgInfo.dmgFlags & DMG_SLINGSHOT)) {
                        this->collider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
                        this->unk_1C0 = 2;
                        mode_walk_init(this);
                        this->unk_1DA = 0x32;
                        return;
                    }
                }
            }
        }

        if (this->collider.elements[0].base.acElemFlags & ACELEM_HIT) {
            this->collider.elements[0].base.acElemFlags &= ~ACELEM_HIT;
            acHitElem = this->collider.elements[0].base.acHitElem;
            if ((this->actionFunc == mode_down) || (this->actionFunc == mode_explode)) {
                swordDamage = damage = GetSwordAP(acHitElem->atDmgInfo.dmgFlags);

                if (damage != 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DAMAGE);
                    mode_damage_init(this);
                    this->unk_1C0 = 5;
                    this->health -= swordDamage;
                }
            }
        }
    }
}

void mode_fail_init(BossDodongo* this) {
    this->actor.speed = this->unk_1E4 = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_002D0C, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_kingdodongo_Anim_002D0C), ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_fail;
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_DEAD);
    this->unk_1DA = 0;
    this->csState = 0;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->unk_1BC = 1;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
}

void mode_fail(BossDodongo* this, PlayState* play) {
    Vec3f* cornerPos;
    Vec3f sp198;
    Vec3f sp184;
    f32 tempSin;
    f32 tempCos;
    f32 sp178;
    s16 i;
    Vec3f effectPos;
    Camera* mainCam;
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);

    switch (this->csState) {
        case 0:
            this->csState = 5;
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            this->subCamEye.x = mainCam->eye.x;
            this->subCamEye.y = mainCam->eye.y;
            this->subCamEye.z = mainCam->eye.z;
            this->subCamAt.x = mainCam->at.x;
            this->subCamAt.y = mainCam->at.y;
            this->subCamAt.z = mainCam->at.z;
            break;
        case 5:
            tempSin = sin_s(this->actor.shape.rot.y - 0x1388) * 150.0f;
            tempCos = cos_s(this->actor.shape.rot.y - 0x1388) * 150.0f;
            add_calc(&player->actor.world.pos.x, this->actor.world.pos.x + tempSin, 0.5f, 5.0f, 0.0f);
            add_calc(&player->actor.world.pos.z, this->actor.world.pos.z + tempCos, 0.5f, 5.0f, 0.0f);
            add_calc(&this->unk_208, 0.07f, 1.0f, 0.005f, 0.0f);
            tempSin = sin_s(this->actor.world.rot.y) * 230.0f;
            tempCos = cos_s(this->actor.world.rot.y) * 230.0f;
            add_calc(&this->subCamEye.x, this->actor.world.pos.x + tempSin, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamEye.y, this->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamEye.z, this->actor.world.pos.z + tempCos, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 30.0f, 0.1f);
            add_calc(&this->subCamAt.y, this->actor.focus.pos.y - 70.0f, 0.2f, 30.0f, 0.1f);
            add_calc(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 30.0f, 0.1f);
            if (Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&object_kingdodongo_Anim_002D0C))) {
                Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_003CF8, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&object_kingdodongo_Anim_003CF8), ANIMMODE_ONCE, -1.0f);
                this->csState = 6;
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_BREAKWALL, -890.0f, -1523.76f, -3304.0f, 0, 0, 0, 0x6000);
            }
            break;
        case 6:
            add_calc(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 30.0f, 0.1f);
            add_calc(&this->subCamAt.y, (this->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.1f);
            add_calc(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 30.0f, 0.1f);

            if (Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&object_kingdodongo_Anim_003CF8))) {
                Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_00DF38, 1.0f, 30.0f, 59.0f, ANIMMODE_ONCE,
                                 -1.0f);
                this->csState = 7;
                this->unk_228 = 7700.0f;
                this->unk_204 = 0.0f;
                this->unk_1E4 = 0.0f;
                this->numWallCollisions = 0;
                this->unk_19E = 0;
            }
            break;
        case 7:
            this->unk_1C4 += 0x7D0;
            add_calc(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 30.0f, 0.0f);
            add_calc(&this->subCamAt.y, (this->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.0f);
            add_calc(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 30.0f, 0.0f);
            add_calc(&this->subCamEye.x, -890.0f, 0.1f, this->unk_204 * 5.0f, 0.1f);
            add_calc(&this->subCamEye.z, -3304.0f, 0.1f, this->unk_204 * 5.0f, 0.1f);
            add_calc(&this->unk_204, 1.0f, 1.0f, 0.1f, 0.0f);
            if (this->unk_1DA == 1) {
                this->csState = 8;
                this->actor.speed = this->unk_1E4 / 1.5f;
                if (this->unk_1A2 == 0) {
                    this->unk_238 = 250.0f;
                } else {
                    this->unk_238 = -250.0f;
                }
                this->unk_1DA = 1000;
                this->unk_234 = 2000.0f;
            } else {
                cornerPos = &kd_target[this->unk_1A0];
                this->unk_1EC = 3.0f;
                add_calc(&this->unk_1E4, this->unk_1EC * 5.0f, 1.0f, this->unk_1EC * 0.25f, 0.0f);
                tempSin = cornerPos->x - this->actor.world.pos.x;
                tempCos = cornerPos->z - this->actor.world.pos.z;
                sp178 = sqrtf(SQ(tempSin) + SQ(tempCos)) - 200.0f;
                if ((sqrtf(SQ(tempSin) + SQ(tempCos)) < 200.0f) || (this->unk_1DA != 0)) {
                    sp178 = 0.0f;
                }
                sp178 = CLAMP_MAX(sp178, 70.0f);
                this->unk_23C = (sin_s(this->unk_19E * 1000) * -50.0f) / 100.0f;

                sp198.x = sin_s(this->unk_19E * 1000) * sp178;
                sp198.y = sp198.z = 0.0f;

                Matrix_rotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_NEW);
                Matrix_Position(&sp198, &sp184);

                add_calc(&this->actor.world.pos.x, cornerPos->x + sp184.x, 1.0f, this->unk_1E4, 0.0f);
                add_calc(&this->actor.world.pos.z, cornerPos->z + sp184.z, 1.0f, this->unk_1E4, 0.0f);
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);
                if ((this->unk_19E & 7) == 0) {
                    setDamageCamera(&play->mainCamera, 2, 1, 8);
                }
                if (!(this->unk_19E & 1)) {
                    _dust_ground_set(play, &this->actor, &this->actor.world.pos, 40.0f, 3, 8.0f, 500, 10,
                                             false);
                }
                tempSin = cornerPos->x - this->actor.world.pos.x;
                tempCos = cornerPos->z - this->actor.world.pos.z;
                add_calc(&this->unk_1E8, 1500.0f, 1.0f, this->unk_1EC * 100.0f, 0.0f);
                add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(tempSin, tempCos)), 5,
                                   (this->unk_1EC * this->unk_1E8), 0);

                if ((fabsf(tempSin) <= 15.0f) && (fabsf(tempCos) <= 15.0f)) {
                    Vec3f dustPos;

                    this->actor.velocity.y = 15.0f;
                    Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_COLI2);
                    if (this->unk_1A2 == 0) {
                        this->unk_1A0++;
                        if (this->unk_1A0 >= 4) {
                            this->unk_1A0 = 0;
                        }
                    } else {
                        this->unk_1A0--;
                        if (this->unk_1A0 < 0) {
                            this->unk_1A0 = 3;
                        }
                    }
                    this->unk_1DA = 0xA;
                    dustPos.x = this->actor.world.pos.x;
                    dustPos.y = this->actor.world.pos.y + 60.0f;
                    dustPos.z = this->actor.world.pos.z;
                    dust_fly_set2(play, &dustPos, 250.0f, 0x28, 0x320, 0xA, 0);
                }
            }
            break;
        case 8:
        case 9:
            if (this->unk_1DA == 884) {
                Skeleton_Info2_init(&this->skelAnime, &object_kingdodongo_Anim_0042A8, 1.0f, 0.0f,
                                 (f32)Si2_anime_end_frame(&object_kingdodongo_Anim_0042A8), ANIMMODE_LOOP, -20.0f);
                tempSin = this->subCamEye.x - this->actor.world.pos.x;
                tempCos = this->subCamEye.z - this->actor.world.pos.z;
                this->unk_22C = sqrtf(SQ(tempSin) + SQ(tempCos));
                this->unk_230 = fatan2(tempSin, tempCos);
                this->unk_1DC = 350;
                this->csState = 9;
            }
            if (this->unk_1DA < 854) {
                for (i = 0; i < 2; i++) {
                    tex_mesh_KD(mesh_data_KD, this->unk_1CC);
                    if (this->unk_1CC < 256) {
                        this->unk_1CC++;
                    }
                }
            }
            if (this->unk_1DA < 984) {
                add_calc_short_angle2(&this->unk_1C4, -0x4000, 0xA, 0x12C, 0);
            }
            if (this->unk_1DA == 904) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_END);
            }
            if (this->unk_1DA < 854) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_LAST - SFX_FLAG);
            }
            if (this->unk_1DA == 960) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_LAVA);
            }
            if (this->unk_1DA < 960) {
                add_calc(&this->actor.shape.shadowScale, 0.0f, 1.0f, 10.0f, 0.0f);
                if (this->unk_1DA >= 710) {

                    if (this->unk_1DA == 710) {
                        Vec3f sp124[] = {
                            { -440.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -3754.0f },
                            { -1340.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -2854.0f },
                        };
                        Vec3f spF4[] = {
                            { -890.0f, 0.0f, -2854.0f },
                            { -440.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -3754.0f },
                            { -1340.0f, 0.0f, -3304.0f },
                        };
                        Vec3f* phi_v0_2;

                        this->unk_1C6 = 1;
                        if (this->unk_1A2 == 0) {
                            phi_v0_2 = &sp124[this->unk_1A0];
                        } else {
                            phi_v0_2 = &spF4[this->unk_1A0];
                        }
                        player->actor.world.pos.x = phi_v0_2->x;
                        player->actor.world.pos.z = phi_v0_2->z;
                        this->unk_204 = 0.0f;
                    }
                    if (this->unk_1DA >= 885) {
                        add_calc(&this->unk_228, 200.0, 0.2f, 100.0f, 0.0f);
                    } else {
                        add_calc(&this->unk_228, -6600.0f, 0.2f, 30.0f, 0.0f);
                    }
                    {
                        static Vec3f vec = { 0.0f, 0.0f, 0.0f };
                        static Vec3f acc = { 0.0f, 1.0f, 0.0f };
                        static Color_RGBA8 prim = { 255, 255, 100, 255 };
                        static Color_RGBA8 env = { 255, 100, 0, 255 };
                        s16 colorIndex;
                        Color_RGBA8 magmaPrimColor2[] = { { 255, 255, 0, 255 }, { 0, 0, 0, 100 } };
                        Color_RGBA8 magmaEnvColor2[] = { { 255, 0, 0, 255 }, { 0, 0, 0, 0 } };

                        effectPos.x = rnd_fx(120.0f) + this->actor.focus.pos.x;
                        effectPos.y = rnd_f(50.0f) + this->actor.world.pos.y;
                        effectPos.z = rnd_fx(120.0f) + this->actor.focus.pos.z;
                        Effect_SS_Dust_sc_cl_co_ct(play, &effectPos, &vec, &acc, &prim, &env, 0x1F4, 0xA,
                                      0xA);
                        effectPos.x = rnd_fx(120.0f) + this->actor.focus.pos.x;
                        effectPos.y = -1498.76f;
                        effectPos.z = rnd_fx(120.0f) + this->actor.focus.pos.z;
                        colorIndex = (fqrand() * 1.9f);
                        Effect_SS_Magma2_ct(play, &effectPos, &magmaPrimColor2[colorIndex],
                                              &magmaEnvColor2[colorIndex], 10 - (colorIndex * 5), colorIndex,
                                              (s16)(fqrand() * 100.0f) + 100);
                    }
                }
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);
                if (!(this->unk_19E & 1)) {
                    _dust_ground_set(play, &this->actor, &this->actor.world.pos, 40.0f, 3, 8.0f, 500, 10,
                                             false);
                }
            }
            add_calc(&this->actor.speed, 0.0f, 0.2f, 0.1f, 0.0f);
            this->actor.world.rot.y += (s16)this->unk_238;
            this->unk_1C4 += (s16)this->unk_234;
            if (this->unk_1DA >= 0x367) {
                if (this->unk_1A2 == 0) {
                    if (this->unk_238 < 450.0f) {
                        this->unk_238 += 10.0f;
                    }
                } else if (-450.0f < this->unk_238) {
                    this->unk_238 -= 10.0f;
                }
            } else {
                add_calc(&this->unk_238, 0.0f, 0.05f, 40.0f, 0.0f);
            }
            add_calc(&this->unk_234, 0.0f, 0.2f, 17.0f, 0.0f);
            add_calc(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 30.0f, 0.0f);
            add_calc(&this->subCamAt.y, (this->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.0f);
            add_calc(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 30.0f, 0.0f);
            if (this->csState == 9) {
                if (this->unk_1DA < 0x2C6) {
                    Vec3f spAC[] = { { -390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -3804.0f },
                                     { -1390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -2804.0f } };

                    Vec3f sp7C[] = { { -890.0f, 0.0f, -2804.0f },
                                     { -390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -3804.0f },
                                     { -1390.0f, 0.0f, -3304.0f } };
                    Vec3f* sp78;
                    s32 pad74;

                    if (this->unk_1A2 == 0) {
                        sp78 = &spAC[this->unk_1A0];
                    } else {
                        sp78 = &sp7C[this->unk_1A0];
                    }

                    add_calc(&this->subCamEye.x, sp78->x, 0.2f, this->unk_204 * 20.0f, 0.0f);
                    add_calc(&this->subCamEye.y, player->actor.world.pos.y + 30.0f, 0.1f,
                                       this->unk_204 * 20.0f, 0.0f);
                    add_calc(&this->subCamEye.z, sp78->z, 0.1f, this->unk_204 * 20.0f, 0.0f);
                    add_calc(&this->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
                } else {
                    if (this->unk_1A2 == 0) {
                        this->unk_230 += 0.01f;
                    } else {
                        this->unk_230 -= 0.01f;
                    }
                    add_calc(&this->unk_22C, 220.0f, 0.1f, 5.0f, 0.1f);
                    tempSin = sinf(this->unk_230) * (*this).unk_22C;
                    tempCos = cosf(this->unk_230) * (*this).unk_22C;
                    add_calc(&this->subCamEye.x, this->actor.world.pos.x + tempSin, 0.2f, 50.0f, 0.0f);
                    add_calc(&this->subCamEye.y, this->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.0f);
                    add_calc(&this->subCamEye.z, this->actor.world.pos.z + tempCos, 0.2f, 50.0f, 0.0f);
                    add_calc(&this->unk_23C, 0.0f, 0.2f, 0.01f, 0.0f);
                }
            } else {

                if (this->unk_1A2 == 0) {
                    add_calc(&this->unk_23C, -0.5f, 0.2f, 0.05f, 0.0f);
                } else {
                    add_calc(&this->unk_23C, 0.5f, 0.2f, 0.05f, 0.0f);
                }

                add_calc(&this->subCamEye.x, -890.0f, 0.1f, this->unk_204 * 5.0f, 0.1f);
                add_calc(&this->subCamEye.z, -3304.0f, 0.1f, this->unk_204 * 5.0f, 0.1f);
                add_calc(&this->unk_204, 1.0f, 1.0f, 0.05f, 0.0f);
            }

            if (this->unk_1DA == 820) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART,
                            sin_s(this->actor.shape.rot.y) * -50.0f + this->actor.world.pos.x,
                            this->actor.world.pos.y,
                            cos_s(this->actor.shape.rot.y) * -50.0f + this->actor.world.pos.z, 0, 0, 0, 0);
            }
            if (this->unk_1DA == 600) {
                mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->unk_1BC = 0;
                this->subCamId = SUB_CAM_ID_DONE;
                this->csState = 100;
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, -890.0f, -1523.76f, -3304.0f,
                                   0, 0, 0, WARP_DUNGEON_CHILD);
                this->skelAnime.playSpeed = 0.0f;
                Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
            }
            FALLTHROUGH;
        case 100:
            if ((this->unk_1DA < 0x2C6) && (fqrand() < 0.5f)) {
                Vec3f sp68;
                Color_RGBA8 D_808CA568 = { 0, 0, 0, 100 };
                Color_RGBA8 D_808CA56C = { 0, 0, 0, 0 };

                sp68.x = rnd_fx(60.0f) + this->actor.focus.pos.x;
                sp68.y = (fqrand() * 50.0f) + -1498.76f;
                sp68.z = rnd_fx(60.0f) + this->actor.focus.pos.z;
                Effect_SS_Magma2_ct(play, &sp68, &D_808CA568, &D_808CA56C, 5, 1, (s16)(fqrand() * 50.0f) + 50);
            }
            break;
    }
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Kd_Eff_move(PlayState* play) {
    BossDodongoEffect* eff = (BossDodongoEffect*)play->specialEffects;
    Color_RGB8 effectColors[] = { { 255, 128, 0 }, { 255, 0, 0 }, { 255, 255, 0 }, { 255, 0, 0 } };
    s16 colorIndex;
    s16 i;

    for (i = 0; i < BOSS_DODONGO_EFFECT_COUNT; i++, eff++) {
        if (eff->unk_24 != 0) {
            eff->unk_00.x += eff->unk_0C.x;
            eff->unk_00.y += eff->unk_0C.y;
            eff->unk_00.z += eff->unk_0C.z;
            eff->unk_25++;
            eff->unk_0C.x += eff->unk_18.x;
            eff->unk_0C.y += eff->unk_18.y;
            eff->unk_0C.z += eff->unk_18.z;
            if (eff->unk_24 == 1) {
                colorIndex = eff->unk_25 % 4;
                eff->color.r = effectColors[colorIndex].r;
                eff->color.g = effectColors[colorIndex].g;
                eff->color.b = effectColors[colorIndex].b;
                eff->alpha -= 20;
                if (eff->alpha <= 0) {
                    eff->alpha = 0;
                    eff->unk_24 = 0;
                }
            }
        }
    }
}

void Kd_Eff_disp(PlayState* play) {
    s32 pad;
    s16 i;
    u8 materialFlag = 0;
    BossDodongoEffect* eff;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    eff = (BossDodongoEffect*)play->specialEffects;

    OPEN_DISPS(gfxCtx, "../z_boss_dodongo.c", 5228);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < BOSS_DODONGO_EFFECT_COUNT; i++, eff++) {
        if (eff->unk_24 == 1) {
            gDPPipeSync(POLY_XLU_DISP++);

            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_009D50);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b, eff->alpha);
            Matrix_translate(eff->unk_00.x, eff->unk_00.y, eff->unk_00.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(eff->unk_2C, eff->unk_2C, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_dodongo.c", 5253);
            gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_009DD0);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_dodongo.c", 5258);
}
