void Demo_Im_Spot00_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 15;
}

void Demo_Im_Spot00_Set_ThrowSound(DemoIm* this, PlayState* play) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 7.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        u32 sfxId = NA_SE_PL_WALK_GROUND;

        sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Nai_FxFlagEntry(sfxId, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void Birth_Door_Deku_In_Demo_Im_Spot00(DemoIm* this, PlayState* play, s32 arg2) {
    s32 pad[2];

    if (arg2 != 0) {
        f32* unk_278 = &this->unk_278;

        if (*unk_278 >= 0.0f) {
            if (this->unk_27C == 0) {
                Vec3f* thisPos = &this->actor.world.pos;
                s16 shapeRotY = this->actor.shape.rot.y;
                f32 spawnPosX = thisPos->x + (sin_s(shapeRotY) * 30.0f);
                f32 spawnPosY = thisPos->y;
                f32 spawnPosZ = thisPos->z + (cos_s(shapeRotY) * 30.0f);

                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ARROW, spawnPosX, spawnPosY, spawnPosZ, 0xFA0,
                            this->actor.shape.rot.y, 0, ARROW_CS_NUT);
                this->unk_27C = 1;
            }
        } else {
            *unk_278 += 1.0f;
        }
    }
}

void Demo_Im_Spot00_setup_Demo_Wait(DemoIm* this) {
    this->action = 15;
    this->drawConfig = 0;
}

void Demo_Im_Spot00_setup_Demo_Greet(DemoIm* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    Demo_Im_Set_DemoStartPosAngle(this, play, 5);
    this->action = 16;
    this->drawConfig = 1;
}

void Demo_Im_Spot00_setup_Demo_Away(DemoIm* this) {
    Skeleton_Info2_init(&this->skelAnime, &gImpaThrowDekuNutAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gImpaThrowDekuNutAnim), ANIMMODE_ONCE, -8.0f);
    this->action = 17;
    this->drawConfig = 1;
}

void Demo_Im_Spot00_Check_DemoMode(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    Demo_Im_Spot00_setup_Demo_Wait(this);
                    break;
                case 2:
                    Demo_Im_Spot00_setup_Demo_Greet(this, play);
                    break;
                case 10:
                    Demo_Im_Spot00_setup_Demo_Away(this);
                    break;
                case 11:
                    Actor_delete(&this->actor);
                    break;
                default:
                    PRINTF("Demo_Im_Spot00_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Im_Spot00_main_wait(DemoIm* this, PlayState* play) {
    Demo_Im_Spot00_Check_DemoMode(this, play);
}

void Demo_Im_Spot00_main_greet(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Spot00_Check_DemoMode(this, play);
}

void Demo_Im_Spot00_main_away(DemoIm* this, PlayState* play) {
    s32 sp24;

    Demo_Im_BGcheck(this, play);
    sp24 = Demo_Im_Animation_Base(this);
    Demo_Im_Spot00_Set_ThrowSound(this, play);
    Demo_Im_set_eye_pattern(this);
    Birth_Door_Deku_In_Demo_Im_Spot00(this, play, sp24);
    Demo_Im_Spot00_Check_DemoMode(this, play);
}
