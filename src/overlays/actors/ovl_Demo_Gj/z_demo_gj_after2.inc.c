// Inits the three cylinders with `sCylinderInit2`
void DemoGj_InitDestructableRubble2(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 16, 0, NULL);
    DemoGj_InitCylinder(this, play, &this->cylinders[0], &sCylinderInit2);
    DemoGj_InitCylinder(this, play, &this->cylinders[1], &sCylinderInit2);
    DemoGj_InitCylinder(this, play, &this->cylinders[2], &sCylinderInit2);
}

#if DEBUG_FEATURES
void DemoGj_DoNothing2(DemoGj* this, PlayState* play) {
}
#endif

// Moves the ColliderCylinder's relative to the actor's position.
void func_8097B450(DemoGj* this, PlayState* play) {
    ColliderCylinder* cylinder0 = &this->cylinders[0];
    ColliderCylinder* cylinder1 = &this->cylinders[1];
    ColliderCylinder* cylinder2 = &this->cylinders[2];
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 pad;
    s16 theta = this->dyna.actor.world.rot.y;
    f32 cos_theta = Math_CosS(theta);
    f32 sin_theta = Math_SinS(theta);

    cylinder0->dim.pos.z = actorPos->z - (35.0f * sin_theta);
    cylinder0->dim.pos.x = actorPos->x + (35.0f * cos_theta);
    cylinder0->dim.pos.y = actorPos->y;

    cylinder1->dim.pos.z = actorPos->z - (-10.0f * sin_theta);
    cylinder1->dim.pos.x = actorPos->x + (-10.0f * cos_theta);
    cylinder1->dim.pos.y = actorPos->y;

    cylinder2->dim.pos.z = actorPos->z - (-55.0f * sin_theta);
    cylinder2->dim.pos.x = actorPos->x + (-55.0f * cos_theta);
    cylinder2->dim.pos.y = actorPos->y;
}

void DemoGj_SetCylindersAsAC2(DemoGj* this, PlayState* play) {
    s32 pad[2];
    Collider* cylinder0 = &this->cylinders[0].base;
    Collider* cylinder1 = &this->cylinders[1].base;
    Collider* cylinder2 = &this->cylinders[2].base;
    s32 pad2[3];

    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder0);
    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder1);
    CollisionCheck_SetAC(play, &play->colChkCtx, cylinder2);
}

// Does the same as `DemoGj_HasCylinderAnyExploded`
s32 DemoGj_HasCylinderAnyExploded2(DemoGj* this, PlayState* play) {
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

void DemoGj_DirectedExplosion2(DemoGj* this, PlayState* play, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;

    DemoGj_Explode(this, play, &pos, direction);
}

void func_8097B6C4(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, play, &gGanonsCastleRubble3Col);
        this->updateMode = 19;
        this->drawConfig = 17;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2
 */
void func_8097B750(DemoGj* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HasCylinderAnyExploded2(this, play)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedExplosion2(this, play, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, play);
        DemoGj_DirectedExplosion2(this, play, &vec2);

        Actor_Kill(thisx);
    }

    func_8097B450(this, play);
    DemoGj_SetCylindersAsAC2(this, play);
}

// func_8097B864
void DemoGj_Update16(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097B6C4(this, play);
}

// func_8097B894
void DemoGj_Update19(DemoGj* this, PlayState* play) {
    func_8097B750(this, play);
#if DEBUG_FEATURES
    DemoGj_DoNothing2(this, play);
#endif
}

void DemoGj_DemoGj_InitDestructableRubble2(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble3DL);
}
