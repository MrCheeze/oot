void func_8098652C(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 15;
}

void func_80986570(DemoIm* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 7.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        u32 sfxId = NA_SE_PL_WALK_GROUND;

        sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Audio_PlaySfxGeneral(sfxId, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

void func_809865F8(DemoIm* this, PlayState* play, s32 arg2) {
    s32 pad[2];

    if (arg2 != 0) {
        f32* unk_278 = &this->unk_278;

        if (*unk_278 >= 0.0f) {
            if (this->unk_27C == 0) {
                Vec3f* thisPos = &this->actor.world.pos;
                s16 shapeRotY = this->actor.shape.rot.y;
                f32 spawnPosX = thisPos->x + (Math_SinS(shapeRotY) * 30.0f);
                f32 spawnPosY = thisPos->y;
                f32 spawnPosZ = thisPos->z + (Math_CosS(shapeRotY) * 30.0f);

                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, spawnPosX, spawnPosY, spawnPosZ, 0xFA0,
                            this->actor.shape.rot.y, 0, ARROW_CS_NUT);
                this->unk_27C = 1;
            }
        } else {
            *unk_278 += 1.0f;
        }
    }
}

void func_80986700(DemoIm* this) {
    this->action = 15;
    this->drawConfig = 0;
}

void func_80986710(DemoIm* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(this, play, 5);
    this->action = 16;
    this->drawConfig = 1;
}

void func_80986794(DemoIm* this) {
    Animation_Change(&this->skelAnime, &gImpaThrowDekuNutAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gImpaThrowDekuNutAnim), ANIMMODE_ONCE, -8.0f);
    this->action = 17;
    this->drawConfig = 1;
}

void func_8098680C(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80986700(this);
                    break;
                case 2:
                    func_80986710(this, play);
                    break;
                case 10:
                    func_80986794(this);
                    break;
                case 11:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    PRINTF("Demo_Im_Spot00_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_809868E8(DemoIm* this, PlayState* play) {
    func_8098680C(this, play);
}

void func_80986908(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_8098680C(this, play);
}

void func_80986948(DemoIm* this, PlayState* play) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, play);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80986570(this, play);
    func_80984BE0(this);
    func_809865F8(this, play, sp24);
    func_8098680C(this, play);
}
