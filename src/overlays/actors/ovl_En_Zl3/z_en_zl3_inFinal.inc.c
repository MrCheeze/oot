void func_80B54EF4(EnZl3* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B54F18(EnZl3* this, PlayState* play) {
    if (this->unk_2F8 == 0) {
        f32 posX = this->actor.world.pos.x;
        f32 posY = this->actor.world.pos.y + (kREG(5) + -26.0f);
        f32 posZ = this->actor.world.pos.z;

        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000, 0,
                           WARP_PURPLE_CRYSTAL);
        this->unk_2F8 = 1;
    }
}

void func_80B54FB4(EnZl3* this, PlayState* play) {
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    func_80B54E14(this, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    EnZl3_setEyeIndex(this, 4);
    EnZl3_setMouthIndex(this, 2);
    this->action = 1;
    this->drawConfig = 1;
    func_80B54F18(this, play);
    this->actor.shape.rot.z = 0;
    this->unk_3C4 = this->actor.world.rot.z;
    this->actor.world.rot.z = this->actor.shape.rot.z;
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void func_80B55054(EnZl3* this) {
    if (this->unk_328 != 0) {
        Actor* child = this->actor.child;

        if (child != NULL) {
            f32* temp_v0 = &this->unk_2EC;

            if (*temp_v0 < 19.0f) {
                s32 pad;

                ((DoorWarp1*)child)->crystalAlpha = (20.0f - *temp_v0) * 12.75f;
                *temp_v0 += 1.0f;
            } else {
                Actor_Kill(child);
                this->actor.child = NULL;
            }
        }
    }
}

void func_80B550F0(EnZl3* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        child->world.pos.x = this->actor.world.pos.x;
        child->world.pos.y = this->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = this->actor.world.pos.z;
    }
}

void func_80B55144(EnZl3* this) {
    static f32 D_80B5A484 = 0.0f;

    if (D_80B5A484 < 2.0f) {
        D_80B5A484 += 1.0f;
        EnZl3_setEyeIndex(this, 2);
    } else if (D_80B5A484 < 4.0f) {
        D_80B5A484 += 1.0f;
        EnZl3_setEyeIndex(this, 1);
    } else {
        EnZl3_UpdateEyes(this);
    }
}

void func_80B551E0(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    this->action = 1;
}

void func_80B55220(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    this->action = 2;
    EnZl3_setMouthIndex(this, 0);
}

void func_80B55268(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    this->action = 3;
}

void func_80B552A8(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_0099A0, 0, 0.0f, 0);
    }
}

void func_80B552DC(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_00A598, 2, -8.0f, 0);
    func_80B54EF4(this);
    EnZl3_setMouthIndex(this, 2);
    this->action = 4;
    func_80B53468();
}

void func_80B55334(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_00AACC, 0, 0.0f, 0);
    }
}

void func_80B55368(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_00A334, 2, -8.0f, 0);
    EnZl3_setMouthIndex(this, 0);
    this->action = 5;
}

void func_80B553B4(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
    }
}

void func_80B553E8(EnZl3* this, PlayState* play) {
    func_80B59AD0(this, play);
}

void func_80B55408(EnZl3* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        Actor_Kill(child);
    }
    Actor_Kill(&this->actor);
}

void func_80B55444(EnZl3* this, PlayState* play) {
    s32 temp_v0 = func_80B5396C(this);

    if (temp_v0 >= 0) {
        s32 unk_2F0 = this->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    func_80B551E0(this);
                    break;
                case 1:
                    EnZl3_setEyeIndex(this, 3);
                    func_80B54EF4(this);
                    break;
                case 3:
                    func_80B55220(this);
                    break;
                case 4:
                    func_80B55268(this);
                    break;
                case 5:
                    func_80B552DC(this);
                    break;
                case 6:
                    func_80B55368(this);
                    break;
                case 7:
                    func_80B553E8(this, play);
                    break;
                case 2:
                    func_80B55408(this);
                    break;
                case 8:
                    this->unk_328 = 1;
                    FALLTHROUGH;
                default:
                    PRINTF("En_Zl3_inFinal_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->unk_2F0 = temp_v0;
        }
    }
}

void func_80B55550(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateSkelAnime(this);
    func_80B550F0(this);
    func_80B55444(this, play);
}

void func_80B555A4(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_setEyeIndex(this, 2);
    func_80B550F0(this);
    func_80B55054(this);
    func_80B55444(this, play);
}

void func_80B55604(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    func_80B55144(this);
    func_80B552A8(this, EnZl3_UpdateSkelAnime(this));
    func_80B55054(this);
    func_80B55444(this, play);
}

void func_80B5566C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55334(this, EnZl3_UpdateSkelAnime(this));
    func_80B55444(this, play);
}

void func_80B556CC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B553B4(this, EnZl3_UpdateSkelAnime(this));
    func_80B55444(this, play);
}

void func_80B5572C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B55444(this, play);
}
