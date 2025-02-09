void En_Zl3_inFinal_Set_PainSound(EnZl3* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void Birth_Cryst_In_En_Zl3_inFinal(EnZl3* this, PlayState* play) {
    if (this->unk_2F8 == 0) {
        f32 posX = this->actor.world.pos.x;
        f32 posY = this->actor.world.pos.y + (kREG(5) + -26.0f);
        f32 posZ = this->actor.world.pos.z;

        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000, 0,
                           WARP_PURPLE_CRYSTAL);
        this->unk_2F8 = 1;
    }
}

void En_Zl3_Actor_inFinal_Init(EnZl3* this, PlayState* play) {
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    En_Zl3_setNum_eye_pattern(this, 4);
    En_Zl3_set_mouth_pattern(this, 2);
    this->action = 1;
    this->drawConfig = 1;
    Birth_Cryst_In_En_Zl3_inFinal(this, play);
    this->actor.shape.rot.z = 0;
    this->unk_3C4 = this->actor.world.rot.z;
    this->actor.world.rot.z = this->actor.shape.rot.z;
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void Disappear_Cryst_In_En_Zl3_inFinal(EnZl3* this) {
    if (this->unk_328 != 0) {
        Actor* child = this->actor.child;

        if (child != NULL) {
            f32* temp_v0 = &this->unk_2EC;

            if (*temp_v0 < 19.0f) {
                s32 pad;

                ((DoorWarp1*)child)->crystalAlpha = (20.0f - *temp_v0) * 12.75f;
                *temp_v0 += 1.0f;
            } else {
                Actor_delete(child);
                this->actor.child = NULL;
            }
        }
    }
}

void SetPos_Cryst_In_En_Zl3_inFinal(EnZl3* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        child->world.pos.x = this->actor.world.pos.x;
        child->world.pos.y = this->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = this->actor.world.pos.z;
    }
}

void En_Zl3_inFinal_SetFace_Lookganon(EnZl3* this) {
    static f32 timer = 0.0f;

    if (timer < 2.0f) {
        timer += 1.0f;
        En_Zl3_setNum_eye_pattern(this, 2);
    } else if (timer < 4.0f) {
        timer += 1.0f;
        En_Zl3_setNum_eye_pattern(this, 1);
    } else {
        En_Zl3_set_eye_pattern(this);
    }
}

void En_Zl3_inFinal_setup_Lookhand(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    this->action = 1;
}

void En_Zl3_inFinal_setup_Swoop(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    this->action = 2;
    En_Zl3_set_mouth_pattern(this, 0);
}

void En_Zl3_inFinal_setup_Lookganon(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    this->action = 3;
}

void En_Zl3_inFinal_Check_Animation_Lookganon(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0099A0, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal_setup_Lookup(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_00A598, 2, -8.0f, 0);
    En_Zl3_inFinal_Set_PainSound(this);
    En_Zl3_set_mouth_pattern(this, 2);
    this->action = 4;
    En_Zl3_Set_BGMStart();
}

void En_Zl3_inFinal_Check_Animation_Lookup(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_00AACC, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal_setup_Stand(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_00A334, 2, -8.0f, 0);
    En_Zl3_set_mouth_pattern(this, 0);
    this->action = 5;
}

void En_Zl3_inFinal_Check_Animation_Stand(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal_setup_Walk(EnZl3* this, PlayState* play) {
    En_Zl3_Setup_FinalToEscape(this, play);
}

void En_Zl3_inFinal_Delete(EnZl3* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        Actor_delete(child);
    }
    Actor_delete(&this->actor);
}

void En_Zl3_inFinal_Check_DemoMode(EnZl3* this, PlayState* play) {
    s32 temp_v0 = En_Zl3_Get_Command_fromGannon(this);

    if (temp_v0 >= 0) {
        s32 unk_2F0 = this->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    En_Zl3_inFinal_setup_Lookhand(this);
                    break;
                case 1:
                    En_Zl3_setNum_eye_pattern(this, 3);
                    En_Zl3_inFinal_Set_PainSound(this);
                    break;
                case 3:
                    En_Zl3_inFinal_setup_Swoop(this);
                    break;
                case 4:
                    En_Zl3_inFinal_setup_Lookganon(this);
                    break;
                case 5:
                    En_Zl3_inFinal_setup_Lookup(this);
                    break;
                case 6:
                    En_Zl3_inFinal_setup_Stand(this);
                    break;
                case 7:
                    En_Zl3_inFinal_setup_Walk(this, play);
                    break;
                case 2:
                    En_Zl3_inFinal_Delete(this);
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

void En_Zl3_inFinal_main_lookhand(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_Animation_Base(this);
    SetPos_Cryst_In_En_Zl3_inFinal(this);
    En_Zl3_inFinal_Check_DemoMode(this, play);
}

void En_Zl3_inFinal_main_swoop(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_setNum_eye_pattern(this, 2);
    SetPos_Cryst_In_En_Zl3_inFinal(this);
    Disappear_Cryst_In_En_Zl3_inFinal(this);
    En_Zl3_inFinal_Check_DemoMode(this, play);
}

void En_Zl3_inFinal_main_lookganon(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_inFinal_SetFace_Lookganon(this);
    En_Zl3_inFinal_Check_Animation_Lookganon(this, En_Zl3_Animation_Base(this));
    Disappear_Cryst_In_En_Zl3_inFinal(this);
    En_Zl3_inFinal_Check_DemoMode(this, play);
}

void En_Zl3_inFinal_main_lookup(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal_Check_Animation_Lookup(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal_Check_DemoMode(this, play);
}

void En_Zl3_inFinal_main_stand(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal_Check_Animation_Stand(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal_Check_DemoMode(this, play);
}

void En_Zl3_inFinal_main_walk(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal_Check_DemoMode(this, play);
}
