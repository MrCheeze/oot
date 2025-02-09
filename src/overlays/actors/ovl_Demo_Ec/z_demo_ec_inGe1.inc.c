void Demo_Ec_main_init_Ge1(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gGerudoWhiteSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcGerudoAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_GERUDO;
    this->drawConfig = EC_DRAW_GERUDO;
}

void Demo_Ec_main_Ge1_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

Gfx* Demo_Ec_Get_Zura_Ge1(DemoEc* this) {
    switch (this->actor.params) {
        case 16:
            return gGerudoWhiteHairstyleBobDL;
        case 17:
            return gGerudoWhiteHairstyleStraightFringeDL;
        case 18:
            return gGerudoWhiteHairstyleSpikyDL;
        default:
            PRINTF(VT_FGCOL(RED) "かつらが無い!!!!!!!!!!!!!!!!\n" VT_RST);
            return NULL;
    }
}

void Demo_Ec_AfterDraw_Ge1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    DemoEc* this = (DemoEc*)thisx;
    Gfx* postLimbDList;

    if (limbIndex == 15) {
        postLimbDList = Demo_Ec_Get_Zura_Ge1(this);
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(postLimbDList));
    }
}

void Demo_Ec_draw_normal_Ge1(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inGe1_eye[] = {
        gGerudoWhiteEyeOpenTex,
        gGerudoWhiteEyeHalfTex,
        gGerudoWhiteEyeClosedTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inGe1_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, NULL, NULL, Demo_Ec_AfterDraw_Ge1);
}
