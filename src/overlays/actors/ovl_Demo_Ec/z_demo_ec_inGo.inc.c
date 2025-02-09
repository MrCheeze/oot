void Demo_Ec_main_init_Go(DemoEc* this, PlayState* play) {
    s32 pad[2];
    AnimationHeader* animation;
    f32 goronScale;
    Vec3f* scale = &this->actor.scale;

    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gGoronSkel);
    Demo_Ec_Change_AnimeBank(this, play);

    if (this->actor.params == 30) {
        animation = &gDemoEcGoronAnim;
        goronScale = 1.0f;
    } else if (this->actor.params == 31) {
        animation = &gDemoEcGoron2Anim;
        goronScale = 1.0f;
    } else if (this->actor.params == 32) {
        animation = &gDemoEcGoronAnim;
        goronScale = 15.0f;
    } else {
        goronScale = 5.0f;
        animation = &object_gm_Anim_0002B8;
    }

    Demo_Ec_Change_Anime(this, animation, 0, 0.0f, false);

    scale->x *= goronScale;
    scale->y *= goronScale;
    scale->z *= goronScale;

    Demo_Ec_Start2_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_GORON;
    this->drawConfig = EC_DRAW_GORON;
}

void Demo_Ec_main_Go_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Go(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inGo_eye[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inGo_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, gGoronCsMouthNeutralTex, NULL, NULL);
}
