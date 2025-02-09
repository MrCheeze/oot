s32 Demo_Ik_Get_npcdemopnt_index(s32 params) {
    s32 ret;

    if (params == 0) {
        ret = 5;
    } else if (params == 1) {
        ret = 6;
    } else {
        ret = 7;
    }
    return ret;
}

void Demo_Ik_inArmer_SetSound_Drop(DemoIk* this) {
    switch (this->actor.params) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
                Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_LAND1_DEMO, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
            break;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, 10.0f)) {
                Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_LAND3_DEMO, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
            break;
        case 2:
            if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
                Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_LAND2_DEMO, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
            break;
    }
}

void Birth_Effect_In_Demo_Ik_inArmer(DemoIk* this, PlayState* play) {
    static Vec3f off_set[] = {
        { -14.0f, 5.0f, 5.0f },  { -20.0f, 12.0f, 0.0f }, { -5.0f, 10.0f, -1.0f }, { -10.0f, 8.0f, 14.0f },
        { -3.0f, 10.0f, 7.0f },  { -10.0f, 11.0f, 0.0f }, { 9.0f, 10.0f, -8.0f },  { 4.0f, 10.0f, 3.0f },
        { -6.0f, 13.0f, -5.0f }, { 1.0f, 9.0f, 3.0f },    { -10.0f, 9.0f, 1.0f },
    };
    s32 i;
    s32 cueChannel = Demo_Ik_Get_npcdemopnt_index(this->actor.params);

    if (Demo_Ik_Check_npcdemopnt(play, 5, cueChannel)) {
        Vec3f pos;
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        s32 startIndex;
        s32 endIndex;

        if (cueChannel == 5) {
            startIndex = 0;
            endIndex = 4;
        } else if (cueChannel == 7) {
            startIndex = 4;
            endIndex = 8;
        } else {
            startIndex = 8;
            endIndex = 11;
        }
        for (i = startIndex; i < endIndex; i++) {
            pos.x = off_set[i].x + this->actor.world.pos.x;
            pos.y = off_set[i].y + this->actor.world.pos.y;
            pos.z = off_set[i].z + this->actor.world.pos.z;
            _Effect_SS_Db_ct(play, &pos, &zeroVec, &zeroVec, 10, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
        }
    }
}

void Demo_Ik_Set_DemoStartPosAngle(DemoIk* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Ik_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void Demo_Ik_inArmer_Init(DemoIk* this, PlayState* play) {
    s32 pad[3];
    SkeletonHeader* skeleton;
    AnimationHeader* animation;
    f32 phi_f0;

    switch (this->actor.params) {
        case 0:
            skeleton = &object_ik_Skel_000C90;
            animation = &object_ik_Anim_000C6C;
            phi_f0 = 30.0f;
            break;
        case 1:
            skeleton = &object_ik_Skel_000660;
            animation = &object_ik_Anim_000634;
            phi_f0 = 10.0f;
            break;
        default:
            skeleton = &object_ik_Skel_000380;
            animation = &object_ik_Anim_00035C;
            phi_f0 = 20.0f;
            // No break is required for matching
    }
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, phi_f0);
    Skeleton_Info2_M_ct(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE, 0.0f);
}

void Demo_Ik_setup_Wait(DemoIk* this) {
    this->actionMode = 0;
    this->drawMode = 0;
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Ik_setup_Stick(DemoIk* this, PlayState* play) {
    En_Ik_End_Movement_byAnimation(this);
    Demo_Ik_Set_DemoStartPosAngle(this, play, Demo_Ik_Get_npcdemopnt_index(this->actor.params));
    this->actionMode = 1;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void Demo_Ik_setup_Drop(DemoIk* this) {
    this->actionMode = 2;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void Demo_Ik_Check_DemoMode(DemoIk* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Ik_Get_npcdemopnt(play, Demo_Ik_Get_npcdemopnt_index(this->actor.params));

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    Demo_Ik_setup_Wait(this);
                    break;
                case 2:
                    Demo_Ik_setup_Stick(this, play);
                    break;
                case 3:
                    Demo_Ik_setup_Drop(this);
                    break;
                case 4:
                    Actor_delete(&this->actor);
                    break;
                case 5:
                case 6:
                    break;
                default:
                    // "there is no such action"
                    PRINTF("Demo_Ik_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Ik_main_wait(DemoIk* this, PlayState* play) {
    Demo_Ik_Check_DemoMode(this, play);
}

void Demo_Ik_main_stick(DemoIk* this, PlayState* play) {
    Demo_Ik_BGcheck(this, play);
    Demo_Ik_Check_DemoMode(this, play);
}

void Demo_Ik_main_drop(DemoIk* this, PlayState* play) {
    Demo_Ik_Animation_Base(this);
    Demo_Ik_inArmer_SetSound_Drop(this);
    Demo_Ik_Movement_byAnimation(this, play);
    Demo_Ik_BGcheck(this, play);
    Birth_Effect_In_Demo_Ik_inArmer(this, play);
    Demo_Ik_Check_DemoMode(this, play);
}

void Demo_Ik_inArmer_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoIk* this = (DemoIk*)thisx;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 385);
    if (limbIndex == 1) {
        switch (this->actor.params) {
            case 0:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inArmer.c", 390);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
                break;
            case 2:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inArmer.c", 396);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
                break;
        }
    }
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 404);
}

void Demo_Ik_draw_normal(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 422);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    _texture_z_light_fog_prim(gfxCtx);
    _texture_z_light_fog_prim_xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Demo_Ik_Setcolor(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Demo_Ik_Setcolor(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Demo_Ik_Setcolor(gfxCtx, 255, 255, 255, 20, 40, 30));
    Si2_draw(play, skelAnime->skeleton, skelAnime->jointTable, NULL, Demo_Ik_inArmer_after, this);
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 444);
}
