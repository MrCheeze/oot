void Demo_Geff_main_init_Hahen(DemoGeff* this, PlayState* play) {
    Vec3f* thisScale = &this->actor.scale;

    this->action = 1;
    this->drawConfig = 1;

    switch (this->actor.params) {
        case 0:
        case 3:
        case 6:
            thisScale->x = (kREG(7) * 0.01f) + 0.3f;
            thisScale->y = (kREG(8) * 0.01f) + 0.3f;
            thisScale->z = (kREG(9) * 0.01f) + 0.3f;
            break;
        case 1:
        case 4:
        case 7:
            thisScale->x = (kREG(10) * 0.01f) + 0.15f;
            thisScale->y = (kREG(11) * 0.01f) + 0.29f;
            thisScale->z = (kREG(12) * 0.01f) + 0.12f;
            break;
        default:
            thisScale->x = (kREG(13) * 0.01f) + 0.1f;
            thisScale->y = (kREG(14) * 0.01f) + 0.15f;
            thisScale->z = (kREG(15) * 0.01f) + 0.2f;
            break;
    }
}

void Demo_Geff_Search_Gt(DemoGeff* this, PlayState* play) {
    s32 targetParams = 2;
    Actor* propIt;

    if (this->demoGt == NULL) {
        propIt = play->actorCtx.actorLists[ACTORCAT_PROP].head;
        if ((this->actor.params != 0) && (this->actor.params != 1) && (this->actor.params != 2)) {
            targetParams = 2;
        } else {
            targetParams = 1;
        }
        while (propIt != NULL) {
            if (propIt->id == ACTOR_DEMO_GT && propIt->params == targetParams) {
                this->deltaPosX = this->actor.world.pos.x - propIt->world.pos.x;
                this->deltaPosY = this->actor.world.pos.y - propIt->world.pos.y;
                this->deltaPosZ = this->actor.world.pos.z - propIt->world.pos.z;
                this->demoGt = (DemoGt*)propIt;
            }
            propIt = propIt->next;
        }
    }
}

void Demo_Geff_ResetPos(DemoGeff* this, PlayState* play) {
    DemoGt* demoGt = this->demoGt;
    s16 params = this->actor.params;

    if (demoGt != NULL && (params != 6) && (params != 7) && (params != 8)) {
        this->actor.world.pos.x = demoGt->dyna.actor.world.pos.x + this->deltaPosX;
        this->actor.world.pos.y = demoGt->dyna.actor.world.pos.y + this->deltaPosY;
        this->actor.world.pos.z = demoGt->dyna.actor.world.pos.z + this->deltaPosZ;
    }
}

void Demo_Geff_main_Hahen_Wait(DemoGeff* this, PlayState* play) {
    Demo_Geff_Search_Gt(this, play);
    Demo_Geff_ResetPos(this, play);
#if DEBUG_FEATURES
    Demo_Geff_main_init_Hahen(this, play);
#endif
}

void Demo_Geff_draw_Hahen(DemoGeff* this, PlayState* play) {
    Demo_Geff_draw_normal_1(play, gGanonRubbleDL);
}
