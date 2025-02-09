void Demo_Ec_main_init_Dk2(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &object_daiku_Skel_007958);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcCarpenterAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_CARPENTER;
    this->drawConfig = EC_DRAW_CARPENTER;
}

void Demo_Ec_main_Dk2_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

s32 Demo_Ec_BeforeDraw_Dk2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                                     Gfx** gfx) {
    DemoEc* this = (DemoEc*)thisx;

    if (limbIndex == 1) {
        gDPPipeSync((*gfx)++);
        switch (this->actor.params) {
            case 10:
                gDPSetEnvColor((*gfx)++, 170, 10, 70, 255);
                break;
            case 11:
                gDPSetEnvColor((*gfx)++, 170, 200, 255, 255);
                break;
            case 12:
                gDPSetEnvColor((*gfx)++, 0, 230, 70, 255);
                break;
            case 13:
                gDPSetEnvColor((*gfx)++, 200, 0, 150, 255);
                break;
        }
    }

    return false;
}

Gfx* Demo_Ec_Get_Zura_Dk2(DemoEc* this) {
    switch (this->actor.params) {
        case 10:
            return object_daiku_DL_005BD0;
        case 11:
            return object_daiku_DL_005AC0;
        case 12:
            return object_daiku_DL_005990;
        case 13:
            return object_daiku_DL_005880;
        default:
            PRINTF(VT_FGCOL(RED) "かつらが無い!!!!!!!!!!!!!!!!\n" VT_RST);
            return NULL;
    }
}

void Demo_Ec_AfterDraw_Dk2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    DemoEc* this = (DemoEc*)thisx;
    Gfx* postLimbDList;

    if (limbIndex == 15) {
        postLimbDList = Demo_Ec_Get_Zura_Dk2(this);
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(postLimbDList));
    }
}

void Demo_Ec_draw_normal_Dk2(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, NULL, NULL, Demo_Ec_BeforeDraw_Dk2, Demo_Ec_AfterDraw_Dk2);
}
