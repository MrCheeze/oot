static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit AcInfoShieldDataElem[2] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC3FFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC3FFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit AcInfoShieldData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    AcInfoShieldDataElem,
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x00, 0x40 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, EN_IK_DMGEFF_SPARKS_NO_DMG),
    /* Deku stick    */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Slingshot     */ DMG_ENTRY(1, EN_IK_DMGEFF_PROJECTILE),
    /* Explosive     */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Boomerang     */ DMG_ENTRY(0, EN_IK_DMGEFF_SPARKS_NO_DMG),
    /* Normal arrow  */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Hammer swing  */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Hookshot      */ DMG_ENTRY(0, EN_IK_DMGEFF_SPARKS_NO_DMG),
    /* Kokiri sword  */ DMG_ENTRY(1, EN_IK_DMGEFF_DAMAGE),
    /* Master sword  */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Giant's Knife */ DMG_ENTRY(4, EN_IK_DMGEFF_DAMAGE),
    /* Fire arrow    */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Ice arrow     */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Light arrow   */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Unk arrow 1   */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Unk arrow 2   */ DMG_ENTRY(2, EN_IK_DMGEFF_PROJECTILE),
    /* Unk arrow 3   */ DMG_ENTRY(15, EN_IK_DMGEFF_PROJECTILE),
    /* Fire magic    */ DMG_ENTRY(0, EN_IK_DMGEFF_ELEMENTAL_MAGIC),
    /* Ice magic     */ DMG_ENTRY(0, EN_IK_DMGEFF_ELEMENTAL_MAGIC),
    /* Light magic   */ DMG_ENTRY(0, EN_IK_DMGEFF_ELEMENTAL_MAGIC),
    /* Shield        */ DMG_ENTRY(0, EN_IK_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, EN_IK_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, EN_IK_DMGEFF_DAMAGE),
    /* Giant spin    */ DMG_ENTRY(4, EN_IK_DMGEFF_DAMAGE),
    /* Master spin   */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Kokiri jump   */ DMG_ENTRY(2, EN_IK_DMGEFF_DAMAGE),
    /* Giant jump    */ DMG_ENTRY(8, EN_IK_DMGEFF_DAMAGE),
    /* Master jump   */ DMG_ENTRY(4, EN_IK_DMGEFF_DAMAGE),
    /* Unknown 1     */ DMG_ENTRY(10, EN_IK_DMGEFF_DAMAGE),
    /* Unblockable   */ DMG_ENTRY(0, EN_IK_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, EN_IK_DMGEFF_DAMAGE),
    /* Unknown 2     */ DMG_ENTRY(0, EN_IK_DMGEFF_NONE),
};

void En_ik_actor_set_process(EnIk* this, EnIkActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Ik_inFight_Init(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;
    s32 pad;
    EffectBlureInit1 blureInit;

    thisx->update = En_Ik_move;
    thisx->draw = En_Ik_display;
    thisx->flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;

    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, thisx, &OcInfoData);
    ClObjTris_ct(play, &this->shieldCollider);
    ClObjTris_set5_nzm(play, &this->shieldCollider, thisx, &AcInfoShieldData, this->shieldColliderItems);
    ClObjSwrd_ct(play, &this->axeCollider);
    ClObjSwrd_set5(play, &this->axeCollider, thisx, &AtInfoData);

    thisx->colChkInfo.damageTable = &btl_data;
    thisx->colChkInfo.mass = MASS_HEAVY;
    this->isBreakingProp = false;
    thisx->colChkInfo.health = 30;
    thisx->gravity = -1.0f;
    this->switchFlag = IK_GET_SWITCH_FLAG(thisx);
    thisx->params = IK_GET_ARMOR_TYPE(thisx);

    if (thisx->params == IK_TYPE_NABOORU) {
        thisx->colChkInfo.health += 20;
        thisx->naviEnemyId = NAVI_ENEMY_IRON_KNUCKLE_NABOORU;
    } else {
        Actor_set_scale(thisx, 0.012f);
        thisx->naviEnemyId = NAVI_ENEMY_IRON_KNUCKLE;
        Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_ENEMY);
    }

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p2StartColor[0] = blureInit.p2StartColor[1] =
        blureInit.p2StartColor[2] = blureInit.p1EndColor[0] = blureInit.p1EndColor[1] = blureInit.p2EndColor[0] =
            blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;

    blureInit.p2StartColor[3] = 64;
    blureInit.p1StartColor[3] = 200;
    blureInit.p1StartColor[2] = blureInit.p1EndColor[2] = 150;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;

    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    EffectAdd(play, &this->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    mode_wait_init(this);

    if (this->switchFlag != 0xFF) {
        if (Actor_Environment_sw_Check(play, this->switchFlag)) {
            Actor_delete(thisx);
        }
    } else if (thisx->params != 0 && Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
        Actor_delete(thisx);
    }
}

s32 def_chk(EnIk* this, PlayState* play) {
    if (((this->armorStatusFlag != 0) || (this->actor.params == IK_TYPE_NABOORU)) &&
        (PlayerSwingCheck(play, &this->actor, 100.0f, 0x2710, 0x4000, this->actor.shape.rot.y) != 0) &&
        (play->gameplayFrames & 1)) {
        mode_defense_init(this);
        return true;
    } else {
        return false;
    }
}

Actor* Search(PlayState* play, Actor* actor) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (prop != NULL) {
        if ((prop == actor) || (prop->id != ACTOR_BG_JYA_IRONOBJ)) {
            prop = prop->next;
            continue;
        } else if (Actor_actor_distance_direction_check(actor, prop, 80.0f, 0x2710)) {
            return prop;
        }

        prop = prop->next;
    }

    return NULL;
}

static void mode_wait_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleStandUpAnim);
    f32 startFrame;

    if (this->actor.params >= IK_TYPE_BLACK) {
        startFrame = endFrame - 1.0f;
    } else {
        startFrame = 0.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleStandUpAnim, 0.0f, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);
    this->unk_2F8 = 3;
    this->actor.speed = 0.0f;
    En_ik_actor_set_process(this, mode_wait);
}

static void mode_wait(EnIk* this, PlayState* play) {
    Vec3f sparksPos;

    if (this->bodyCollider.base.acFlags & AC_HIT) {
        sparksPos = this->actor.world.pos;
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
        sparksPos.y += 30.0f;
        SetSparkFlash(play, &sparksPos);
        this->skelAnime.playSpeed = 1.0f;
        Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
    }

    if (this->skelAnime.curFrame == 5.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_WAKEUP);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
        mode_walk_init(this);
    }
}

static void mode_wait2_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&object_ik_Anim_00DD50);

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
    this->unk_2F8 = 4;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &object_ik_Anim_00DD50, 0.0f, 0.0f, endFrame, ANIMMODE_LOOP, 4.0f);
    En_ik_actor_set_process(this, mode_wait2);
}

static void mode_wait2(EnIk* this, PlayState* play) {
    s32 detectionThreshold = (this->armorStatusFlag == 0) ? 0xAAA : 0x3FFC;
    s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(yawDiff) <= detectionThreshold) && (this->actor.xzDistToPlayer < 100.0f) &&
        (ABS(this->actor.yDistToPlayer) < 150.0f)) {
        if ((play->gameplayFrames & 1)) {
            mode_attack_init(this);
        } else {
            mode_attack2_init(this);
        }
    } else if ((ABS(yawDiff) <= 0x4000) && (ABS(this->actor.yDistToPlayer) < 150.0f)) {
        mode_walk_init(this);
    } else {
        mode_walk_init(this);
    }

    def_chk(this, play);
    Skeleton_Info2_anime_play(&this->skelAnime);
}

static void mode_walk_init(EnIk* this) {
    this->unk_2F8 = 5;

    if (this->armorStatusFlag == 0) {
        Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleWalkAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gIronKnuckleWalkAnim), ANIMMODE_LOOP, -4.0f);
        this->actor.speed = 0.9f;
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleRunAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gIronKnuckleRunAnim), ANIMMODE_LOOP, -4.0f);
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_DASH);
        this->actor.speed = 2.5f;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_ik_actor_set_process(this, mode_walk);
}

static void mode_walk(EnIk* this, PlayState* play) {
    s16 temp_t0;
    s16 targetYaw;
    s16 yawDiff;
    s16 footstepFrame1;
    s16 footstepFrame2;
    s16 stepVal;

    if (this->armorStatusFlag == 0) {
        temp_t0 = 0xAAA;
        stepVal = 0x320;
        footstepFrame1 = 0;
        footstepFrame2 = 16;
    } else {
        temp_t0 = 0x3FFC;
        stepVal = 0x4B0;
        footstepFrame1 = 2;
        footstepFrame2 = 9;
    }

    targetYaw = this->actor.wallYaw - this->actor.shape.rot.y;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS(targetYaw) >= 0x4000)) {
        targetYaw = (this->actor.yawTowardsPlayer > 0) ? this->actor.wallYaw - 0x4000 : this->actor.wallYaw + 0x4000;
        add_calc_short_angle2(&this->actor.world.rot.y, targetYaw, 1, stepVal, 0);
    } else {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, stepVal, 0);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(yawDiff) <= temp_t0) && (this->actor.xzDistToPlayer < 100.0f)) {
        if (ABS(this->actor.yDistToPlayer) < 150.0f) {
            if (play->gameplayFrames & 1) {
                mode_attack_init(this);
            } else {
                mode_attack2_init(this);
            }
        }
    }

    if (Search(play, &this->actor) != NULL) {
        mode_attack2_init(this);
        this->isBreakingProp = true;
    } else {
        temp_t0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (ABS(temp_t0) > 0x4000) {
            this->unk_300--;

            if (this->unk_300 == 0) {
                mode_attack3_init(this);
            }
        } else {
            this->unk_300 = 40;
        }
    }

    def_chk(this, play);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (((s16)this->skelAnime.curFrame == footstepFrame1) || ((s16)this->skelAnime.curFrame == footstepFrame2)) {
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_WALK);
    }
}

static void mode_attack_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleVerticalAttackAnim);

    this->unk_2FF = 1;
    this->unk_2F8 = 6;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleVerticalAttackAnim, 1.5f, 0.0f, endFrame, ANIMMODE_ONCE, -4.0f);
    En_ik_actor_set_process(this, mode_attack);
}

static void mode_attack(EnIk* this, PlayState* play) {
    Vec3f sparksPos;

    if (this->skelAnime.curFrame == 15.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);
    } else if (this->skelAnime.curFrame == 21.0f) {
        sparksPos.x = this->actor.world.pos.x + sin_s(this->actor.shape.rot.y + 0x6A4) * 70.0f;
        sparksPos.z = this->actor.world.pos.z + cos_s(this->actor.shape.rot.y + 0x6A4) * 70.0f;
        sparksPos.y = this->actor.world.pos.y;

        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_HIT_GND);
        setDamageCamera(&play->mainCamera, 2, 25, 5);
        z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 255, 20, 150);
        CollisionCheckSetSparkFlashBlue_NoSE(play, &sparksPos);
    }

    if ((this->skelAnime.curFrame > 17.0f) && (this->skelAnime.curFrame < 23.0f)) {
        this->unk_2FE = 1;
    } else {
        if ((this->armorStatusFlag != 0) && (this->skelAnime.curFrame < 10.0f)) {
            add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
        this->unk_2FE = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack_end_init(this);
    }
}

void mode_attack_end_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleAxeStuckAnim);

    this->unk_2FE = 0;
    this->animationTimer = (s8)endFrame;
    this->unk_2F8 = 7;
    this->unk_2FF = this->unk_2FE;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleAxeStuckAnim, 1.0f, 0.0f, endFrame, ANIMMODE_LOOP, -4.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_PULLOUT);
    En_ik_actor_set_process(this, mode_attack_end);
}

void mode_attack_end(EnIk* this, PlayState* play) {
    f32 endFrame;

    if (Skeleton_Info2_anime_play(&this->skelAnime) || (--this->animationTimer == 0)) {
        if (this->unk_2F8 == 8) {
            mode_wait2_init(this);
        } else {
            endFrame = Si2_anime_end_frame(&gIronKnuckleRecoverFromVerticalAttackAnim);
            this->unk_2F8 = 8;
            Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleRecoverFromVerticalAttackAnim, 1.5f, 0.0f, endFrame,
                             ANIMMODE_ONCE_INTERP, -4.0f);
        }
    }
}

// Happens when Player is in front of Iron Knuckle or when Iron Knuckle encounters ACTOR_BG_JYA_IRONOBJ
static void mode_attack2_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleHorizontalAttackAnim);

    this->unk_2FF = 2;
    this->unk_300 = 0;
    this->unk_2F8 = 6;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleHorizontalAttackAnim, 0.0f, 0.0f, endFrame, ANIMMODE_ONCE_INTERP,
                     -6.0f);
    this->isBreakingProp = false;
    En_ik_actor_set_process(this, mode_attack2);
}

static void mode_attack2(EnIk* this, PlayState* play) {
    f32 playSpeed;

    this->unk_300 += 0x1C2;
    playSpeed = sin_s(this->unk_300);
    this->skelAnime.playSpeed = ABS(playSpeed);

    if (this->skelAnime.curFrame > 11.0f) {
        this->unk_2FF = 3;
    }

    if (((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 9.0f)) ||
        ((this->skelAnime.curFrame > 13.0f) && (this->skelAnime.curFrame < 18.0f))) {
        if (!this->isBreakingProp && (this->armorStatusFlag != 0) && (this->skelAnime.curFrame < 10.0f)) {
            add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }

        if (this->unk_2FE < 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);
        }

        this->unk_2FE = 1;
    } else {
        this->unk_2FE = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack2_end_init(this);
    }
}

void mode_attack2_end_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleRecoverFromHorizontalAttackAnim);

    this->unk_2FF = this->unk_2FE = 0;
    this->unk_2F8 = 8;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleRecoverFromHorizontalAttackAnim, 1.5f, 0.0f, endFrame,
                     ANIMMODE_ONCE_INTERP, -4.0f);
    En_ik_actor_set_process(this, mode_attack2_end);
}

void mode_attack2_end(EnIk* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait2_init(this);
        def_chk(this, play);
    }
}

// Attack pattern when player is behind Iron Knuckle or attacks Iron Knuckle from behind
void mode_attack3_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleHorizontalAttackAnim);

    this->unk_2F8 = 1;
    this->unk_2FF = 3;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleHorizontalAttackAnim, 0.5f, 13.0f, endFrame, ANIMMODE_ONCE_INTERP,
                     -4.0f);
    En_ik_actor_set_process(this, mode_attack3);
}

static void mode_attack3(EnIk* this, PlayState* play) {
    chase_s2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x7D0);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if ((this->skelAnime.curFrame > 13.0f) && (this->skelAnime.curFrame < 18.0f)) {
        if (this->unk_2FE < 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);
        }
        this->unk_2FE = 1;
    } else {
        this->unk_2FE = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack2_end_init(this);
        def_chk(this, play);
    }
}

static void mode_defense_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleBlockAnim);

    this->unk_2FE = 0;
    this->unk_2F8 = 9;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleBlockAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE_INTERP, -4.0f);
    En_ik_actor_set_process(this, mode_defense);
}

static void mode_defense(EnIk* this, PlayState* play) {
    CollisionCheck_setAC(play, &play->colChkCtx, &this->shieldCollider.base);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4000) &&
            (this->actor.xzDistToPlayer < 100.0f) && (ABS(this->actor.yDistToPlayer) < 150.0f)) {
            if ((play->gameplayFrames & 1)) {
                mode_attack_init(this);
            } else {
                mode_attack2_init(this);
            }
        } else {
            mode_wait2_init(this);
        }
    }
}

static void mode_dam_init(EnIk* this) {
    s16 yaw;
    s16 yawDiff;

    yaw = search_position_angleY(&this->actor.world.pos, &this->bodyCollider.base.ac->world.pos);
    this->unk_2F8 = 0;
    yawDiff = yaw - this->actor.shape.rot.y;

    if (ABS(yawDiff) <= 0x4000) {
        Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleFrontHitAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gIronKnuckleFrontHitAnim), ANIMMODE_ONCE, -4.0f);
        this->actor.speed = -6.0f;
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleBackHitAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gIronKnuckleBackHitAnim), ANIMMODE_ONCE, -4.0f);
        this->actor.speed = 6.0f;
    }

    this->unk_2FE = 0;
    En_ik_actor_set_process(this, mode_dam);
}

static void mode_dam(EnIk* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);

    if (Part_break(&this->actor, &this->bodyBreak, play, this->actor.params + 4)) {
        this->bodyBreak.val = BODYBREAK_STATUS_FINISHED;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4000) {
            mode_wait2_init(this);
            def_chk(this, play);
        } else {
            mode_attack3_init(this);
        }
    }
}

static void mode_down_init(EnIk* this) {
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleDeathAnim);

    this->unk_2FE = 0;
    this->unk_2F8 = 2;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleDeathAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE, -4.0f);
    this->animationTimer = 24;
    Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_DEAD);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_CUTBODY);
    En_ik_actor_set_process(this, mode_down);
}

static void mode_down(EnIk* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if ((this->actor.colChkInfo.health == 0) && (this->animationTimer != 0)) {
            s32 i;
            Vec3f pos;
            Vec3f sp7C = { 0.0f, 0.5f, 0.0f };

            this->animationTimer--;

            for (i = 0xC - (this->animationTimer >> 1); i >= 0; i--) {
                pos.x = this->actor.world.pos.x + rnd_fx(120.0f);
                pos.z = this->actor.world.pos.z + rnd_fx(120.0f);
                pos.y = this->actor.world.pos.y + 20.0f + rnd_fx(50.0f);

                _Effect_SS_Db_ct(play, &pos, &sp7C, &sp7C, 100, 0, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
            }

            if (this->animationTimer == 0) {
                Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xB0);

                if (this->switchFlag != 0xFF) {
                    Actor_Environment_sw_On(play, this->switchFlag);
                }

                Actor_delete(&this->actor);
            }
        }
    } else if (this->skelAnime.curFrame == 23.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_WALK);
    }
}

void En_Ik_damage_proc(EnIk* this, PlayState* play) {

    if ((this->unk_2F8 == 3) || (this->unk_2F8 == 2)) {
        return;
    }

    if (this->shieldCollider.base.acFlags & AC_BOUNCED) {
        f32 frames = Si2_anime_end_frame(&gIronKnuckleBlockAnim) - 2.0f;

        if (this->skelAnime.curFrame < frames) {
            this->skelAnime.curFrame = frames;
        }

        this->shieldCollider.base.acFlags &= ~AC_BOUNCED;
        this->bodyCollider.base.acFlags &= ~AC_HIT;
    } else if (this->bodyCollider.base.acFlags & AC_HIT) {
        s16 pad;
        u8 prevHealth;
        s32 damageEffect;
        Vec3f sparksPos = this->actor.world.pos;

        sparksPos.y += 50.0f;

        Hit_bit_set(&this->actor, &this->bodyCollider.elem, true);

        this->damageEffect = this->actor.colChkInfo.damageEffect;
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if ((this->damageEffect == EN_IK_DMGEFF_NONE) || (this->damageEffect == EN_IK_DMGEFF_SPARKS_NO_DMG) ||
            ((this->armorStatusFlag == 0) && (this->damageEffect == EN_IK_DMGEFF_PROJECTILE))) {
            if (this->damageEffect != EN_IK_DMGEFF_NONE) {
                // spawn sparks and don't damage
                CollisionCheckSetSparkFlashBlue(play, &sparksPos);
            }
            return;
        }

        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);

        prevHealth = this->actor.colChkInfo.health;
        hp_down(&this->actor);

        if (this->actor.params != IK_TYPE_NABOORU) {
            if ((prevHealth > 10) && (this->actor.colChkInfo.health <= 10)) {
                this->armorStatusFlag = ARMOR_BROKEN;
                Part_Break_init(&this->bodyBreak, 3, play);
            }
        } else if (this->actor.colChkInfo.health <= 10) {
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_BOSS);
            Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EN_LAST_DAMAGE);
#if !OOT_PAL_N64
            if (this->switchFlag != 0xFF) {
                Actor_Environment_sw_On(play, this->switchFlag);
            }
#endif
            return;
        } else if (prevHealth == 50) {
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        }

        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this);
            Actor_info_finish(play, &this->actor);
            return;
        }

        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x7D0, 0);

        if ((this->actor.params == IK_TYPE_NABOORU) && (fqrand() < 0.5f)) {
            if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 0x4000) {
                mode_attack3_init(this);
            }
        }

        if ((this->actor.params != IK_TYPE_NABOORU) && (this->armorStatusFlag != 0)) {
            if ((prevHealth > 10) && (this->actor.colChkInfo.health <= 10)) {
                Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_DAMAGE);
                Actor_SE_set(&this->actor, NA_SE_EN_NUTS_CUTBODY);
            }

            mode_dam_init(this);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
            Actor_SE_set(&this->actor, NA_SE_EN_IRONNACK_DAMAGE);
            CollisionCheckSetSparkFlashBlue_NoSE(play, &sparksPos);
        }
    }
}

void En_Ik_move(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    u8 prevInvincibilityTimer;

    this->drawArmorFlag = this->armorStatusFlag;
    En_Ik_damage_proc(this, play);

    if ((this->actor.params == IK_TYPE_NABOORU) && (this->actor.colChkInfo.health <= 10)) {
        En_Ik_Chenge_FightToDemo(&this->actor, play);
    } else {
        this->actionFunc(this, play);

        if (this->axeCollider.base.atFlags & AT_HIT) {
            this->axeCollider.base.atFlags &= ~AT_HIT;

            if (this->axeCollider.base.at == &player->actor) {
                prevInvincibilityTimer = player->invincibilityTimer;

                if (player->invincibilityTimer <= 0) {
                    if (player->invincibilityTimer <= -40) {
                        player->invincibilityTimer = 0;
                    } else {
                        player->invincibilityTimer = 0;
                        play->damagePlayer(play, -64);
                        this->unk_2FE = 0;
                    }
                }

                Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
                player->invincibilityTimer = prevInvincibilityTimer;
            }
        }

        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);

        this->actor.focus.pos = this->actor.world.pos;
        this->actor.focus.pos.y += 45.0f;

        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);

        if ((this->actor.colChkInfo.health > 0) && (this->actor.colorFilterTimer == 0) && (this->unk_2F8 >= 2)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if (this->unk_2FE > 0) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->axeCollider.base);
        }

        if (this->unk_2F8 == 9) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->shieldCollider.base);
        }
    }
}

static Gfx* set_col(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* displayList;
    Gfx* displayListHead;

    displayList = GRAPH_ALLOC(gfxCtx, 4 * sizeof(Gfx));
    displayListHead = displayList;

    gDPPipeSync(displayListHead++);
    gDPSetPrimColor(displayListHead++, 0, 0, primR, primG, primB, 255);
    gDPSetEnvColor(displayListHead++, envR, envG, envB, 255);
    gSPEndDisplayList(displayListHead++);

    return displayList;
}

s32 en_ik_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* this = (EnIk*)thisx;

    if (limbIndex == IRON_KNUCKLE_LIMB_HELMET_ARMOR) {
        if (this->actor.params != IK_TYPE_NABOORU) {
            *dList = gIronKnuckleHelmetDL;
        }
    } else if (limbIndex == IRON_KNUCKLE_LIMB_HEAD) {
        if (this->actor.params != IK_TYPE_NABOORU) {
            *dList = gIronKnuckleGerudoHeadDL;
        }
    } else if ((limbIndex == IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT) ||
               (limbIndex == IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK)) {
        if (this->drawArmorFlag & ARMOR_BROKEN) {
            *dList = NULL;
        }
    } else if ((limbIndex == IRON_KNUCKLE_LIMB_TORSO) || (limbIndex == IRON_KNUCKLE_LIMB_WAIST)) {
        if (!(this->drawArmorFlag & ARMOR_BROKEN)) {
            *dList = NULL;
        }
    }

    return false;
}

// unused
static Vec3f local_foot = { 300.0f, 0.0f, 0.0f };

static Vec3f sword_top[] = {
    { 800.0f, -200.0f, -5200.0f },
    { 0.0f, 0.0f, 0.0f },
    { -200.0f, -2200.0f, -200.0f },
    { -6000.0f, 2000.0f, -3000.0f },
};

static Vec3f shield_data[] = {
    { -3000.0, -700.0, -5000.0 },
    { -3000.0, -700.0, 2000.0 },
    { 4000.0, -700.0, 2000.0 },
};

static Vec3f shield_data2[] = {
    { 4000.0, -700.0, 2000.0 },
    { 4000.0, -700.0, -5000.0 },
    { -3000.0, -700.0, -5000.0 },
};

void en_ik_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f spF4;
    Vec3f spE8;
    EnIk* this = (EnIk*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ik_inFight.c", 1201);

    if (this->armorStatusFlag & ARMOR_BROKEN) {
        Part_Break_Get(&this->bodyBreak, limbIndex, IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT,
                          IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK, IRON_KNUCKLE_LIMB_TORSO, dList,
                          BODYBREAK_OBJECT_SLOT_DEFAULT);
    }
    if (limbIndex == IRON_KNUCKLE_LIMB_HELMET_ARMOR) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ik_inFight.c", 1217);
        if (this->actor.params != IK_TYPE_NABOORU) {
            gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleHelmetMarkingDL);
        } else {
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
        }
    } else if (limbIndex == IRON_KNUCKLE_LIMB_AXE) {
        s32 i;
        Vec3f sp9C[3];
        Vec3f sp78[3];

        Matrix_Position(&sword_top[0], &this->axeCollider.dim.quad[1]);
        Matrix_Position(&sword_top[1], &this->axeCollider.dim.quad[0]);
        Matrix_Position(&sword_top[2], &this->axeCollider.dim.quad[3]);
        Matrix_Position(&sword_top[3], &this->axeCollider.dim.quad[2]);
        CollisionCheck_Uty_setSword4Pos(&this->axeCollider, &this->axeCollider.dim.quad[0], &this->axeCollider.dim.quad[1],
                                 &this->axeCollider.dim.quad[2], &this->axeCollider.dim.quad[3]);
        Matrix_Position(&sword_top[0], &spF4);
        Matrix_Position(&sword_top[1], &spE8);
        if (this->unk_2FE > 0) {
            EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIdx), &spF4, &spE8);
        } else if (this->unk_2FE == 0) {
            EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIdx));
            this->unk_2FE = -1;
        }
        if (this->unk_2F8 == 9) {
            for (i = 0; i < ARRAY_COUNT(sp78); i++) {
                Matrix_Position(&shield_data[i], &sp9C[i]);
                Matrix_Position(&shield_data2[i], &sp78[i]);
            }

            CollisionCheck_Uty_setTrisPos(&this->shieldCollider, 0, &sp9C[0], &sp9C[1], &sp9C[2]);
            CollisionCheck_Uty_setTrisPos(&this->shieldCollider, 1, &sp78[0], &sp78[1], &sp78[2]);
        }
    }

    switch (limbIndex) {
        case IRON_KNUCKLE_LIMB_UPPER_LEFT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ik_inFight.c", 1270);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;

        case IRON_KNUCKLE_LIMB_UPPER_RIGHT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ik_inFight.c", 1275);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;

        case IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT:
            if (!(this->drawArmorFlag & ARMOR_BROKEN)) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ik_inFight.c", 1281);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
            }
            break;

        case IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK:
            if (!(this->drawArmorFlag & ARMOR_BROKEN)) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ik_inFight.c", 1288);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            }
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ik_inFight.c", 1294);
}

void En_Ik_display(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ik_inFight.c", 1309);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if (this->actor.params == IK_TYPE_NABOORU) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 245, 225, 155, 30, 30, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 255, 40, 0, 40, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 255, 255, 255, 20, 40, 30));
    } else if (this->actor.params == IK_TYPE_SILVER) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 245, 255, 205, 30, 35, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 185, 135, 25, 20, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 255, 255, 255, 30, 40, 20));
    } else if (this->actor.params == IK_TYPE_BLACK) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 55, 65, 55, 0, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 255, 255, 255, 180, 180, 180));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
    }

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_ik_display1, en_ik_display2, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ik_inFight.c", 1351);
}
