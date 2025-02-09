void DemoEc_InitMaskShopOwner(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gHappyMaskSalesmanSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_MASK_SHOP_OWNER;
    this->drawConfig = EC_DRAW_MASK_SHOP_OWNER;
}

void DemoEc_UpdateMaskShopOwner(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawMaskShopOwner(DemoEc* this, PlayState* play) {
    DemoEc_DrawSkeleton(this, play, gHappyMaskSalesmanEyeClosedTex, NULL, NULL, NULL);
}
