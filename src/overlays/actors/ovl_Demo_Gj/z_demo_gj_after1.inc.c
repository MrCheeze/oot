// Inits the three cylinders with `sCylinderInit1`
void DemoGj_InitDestructableRubble1(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 15, 0, NULL);
    DemoGj_InitCylinder(this, play, &this->cylinders[0], &sCylinderInit1);
    DemoGj_InitCylinder(this, play, &this->cylinders[1], &sCylinderInit1);
    DemoGj_InitCylinder(this, play, &this->cylinders[2], &sCylinderInit1);
}

#if DEBUG_FEATURES
void DemoGj_DoNothing1(DemoGj* this, PlayState* play) {
}
#endif

/*
 * Moves the ColliderCylinder's relative to the actor's position.
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1
 */
void func_8097AEE8(DemoGj* this, PlayState* play) {
    ColliderCylinder* cylinder0 = &this->cylinders[0];
    ColliderCylinder* cylinder1 = &this->cylinders[1];
    ColliderCylinder* cylinder2 = &this->cylinders[2];
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 pad;
    s16 theta = this->dyna.actor.world.rot.y;
    f32 cos_theta = Math_CosS(theta);
    f32 sin_theta = Math_SinS(theta);

    cylinder0->dim.pos.z = actorPos->z + (20.0f * cos_theta) - (-20.0f * sin_theta);
    cylinder0->dim.pos.x = actorPos->x + (20.0f * sin_theta) + (-20.0f * cos_theta);
    cylinder0->dim.pos.y = actorPos->y;

    cylinder1->dim.pos.z = actorPos->z + (-20.0f * cos_theta) - (20.0f * sin_theta);
    cylinder1->dim.pos.x = actorPos->x + (-20.0f * sin_theta) + (20.0f * cos_theta);
    cylinder1->dim.pos.y = actorPos->y;

    cylinder2->dim.pos.z = actorPos->z + (-60.0f * cos_theta) - (60.0f * sin_theta);
    cylinder2->dim.pos.x = actorPos->x + (-60.0f * sin_theta) + (60.0f * cos_theta);
    cylinder2->dim.pos.y = actorPos->y;
}

void DemoGj_SetCylindersAsAC(DemoGj* this, PlayState* play) {
    s32 pad[2];
    Collider* cylinder0 = &this->cylinders[0].base;
    Collider* cylinder1 = &this->cylinders[1].base;
    Collider* cylinder2 = &this->cylinders[2].base;
    s32 pad2[3];

    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder0);
    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder1);
    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder2);
}

void DemoGj_DirectedExplosion(DemoGj* this, PlayState* play, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;

    DemoGj_Explode(this, play, &pos, direction);
}

void func_8097B128(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, play, &gGanonsCastleRubble2Col);
        this->updateMode = 18;
        this->drawConfig = 16;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

s32 DemoGj_HasCylinderAnyExploded(DemoGj* this, PlayState* play) {
    if (DemoGj_HitByExplosion(this, play, &this->cylinders[0])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, play, &this->cylinders[1])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, play, &this->cylinders[2])) {
        return true;
    }
    return false;
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1
 */
void func_8097B22C(DemoGj* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HasCylinderAnyExploded(this, play)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedExplosion(this, play, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedExplosion(this, play, &vec2);

        Actor_Kill(thisx);
    }

    func_8097AEE8(this, play);
    DemoGj_SetCylindersAsAC(this, play);
}

// func_8097B340
void DemoGj_Update15(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097B128(this, play);
}

// func_8097B370
void DemoGj_Update18(DemoGj* this, PlayState* play) {
    func_8097B22C(this, play);
#if DEBUG_FEATURES
    DemoGj_DoNothing1(this, play);
#endif
}

void DemoGj_DrawDestructableRubble1(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble2DL);
}
