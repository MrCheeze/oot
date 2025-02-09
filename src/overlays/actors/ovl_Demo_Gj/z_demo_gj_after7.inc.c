// Inits the first cylinder (only that one) with `sCylinderInit3`
void DemoGj_InitDestructableRubbleTall(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 17, 0, NULL);
    DemoGj_InitCylinder(this, play, &this->cylinders[0], &sCylinderInit3);
}

#if DEBUG_FEATURES
void DemoGj_DoNothing3(DemoGj* this, PlayState* play) {
}
#endif

void DemoGj_DirectedDoubleExplosion(DemoGj* this, PlayState* play, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;
    DemoGj_Explode(this, play, &pos, direction);

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y + 100.0f;
    pos.z = this->dyna.actor.world.pos.z;
    DemoGj_Explode(this, play, &pos, direction);
}

void func_8097B9BC(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, play, &gGanonsCastleRubbleTallCol);
        this->updateMode = 20;
        this->drawConfig = 18;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL
 */
void func_8097BA48(DemoGj* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    ColliderCylinder* cylinder = &this->cylinders[0];
    s32 pad[2];

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HitByExplosion(this, play, cylinder)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedDoubleExplosion(this, play, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedDoubleExplosion(this, play, &vec2);

        Actor_Kill(thisx);
    }

    Collider_UpdateCylinder(thisx, cylinder);
    CollisionCheck_SetAC(play, &play->colChkCtx, &cylinder->base);
}

// func_8097BB78
void DemoGj_Update17(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097B9BC(this, play);
}

// func_8097BBA8
void DemoGj_Update20(DemoGj* this, PlayState* play) {
    func_8097BA48(this, play);
#if DEBUG_FEATURES
    DemoGj_DoNothing3(this, play);
#endif
}

void DemoGj_DemoGj_InitDestructableRubbleTall(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubbleTallDL);
}
