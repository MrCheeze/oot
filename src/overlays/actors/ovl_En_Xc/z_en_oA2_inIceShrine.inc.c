void func_80B3EE64(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_SERENADE;
}

void func_80B3EE74(EnXc* this, PlayState* play) {
    if (EnXc_SerenadeCS(this, play)) {
        this->action = SHEIK_ACTION_30;
    }
}

void func_80B3EEA4(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_30, SHEIK_ACTION_31);
}

void func_80B3EEC8(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_31, SHEIK_ACTION_32);
}

void func_80B3EEEC(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_32, SHEIK_ACTION_33);
}

void func_80B3EF10(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_33, SHEIK_ACTION_34);
}

void func_80B3EF34(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_34, SHEIK_ACTION_35);
}

void func_80B3EF58(EnXc* this) {
    func_80B3C7D4(this, SHEIK_ACTION_35, SHEIK_ACTION_36, SHEIK_ACTION_34);
}

void func_80B3EF80(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_36, SHEIK_ACTION_37);
}

void func_80B3EFA4(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_37, SHEIK_ACTION_38);
}

void func_80B3EFC8(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_38, SHEIK_ACTION_39);
}

void func_80B3EFEC(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_39, SHEIK_ACTION_40);
}

void func_80B3F010(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Animation_Change(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_41;
        this->timer = 0.0f;
    }
}

void func_80B3F0B8(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_41, SHEIK_ACTION_42);
}

void func_80B3F0DC(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_42, SHEIK_ACTION_43);
}

void func_80B3F100(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_43, SHEIK_ACTION_44);
}

void EnXc_Serenade(EnXc* this, PlayState* play) {
    func_80B3EE74(this, play);
}

void EnXc_ActionFunc30(EnXc* this, PlayState* play) {
    EnXc_ActionFunc21(this, play);
    func_80B3EEA4(this);
}

void EnXc_ActionFunc31(EnXc* this, PlayState* play) {
    EnXc_ActionFunc6(this, play);
    func_80B3C588(this, play, 4);
    func_80B3EEC8(this);
}

void EnXc_ActionFunc32(EnXc* this, PlayState* play) {
    EnXc_ActionFunc7(this, play);
    func_80B3EEEC(this);
}

void EnXc_ActionFunc33(EnXc* this, PlayState* play) {
    EnXc_ActionFunc8(this, play);
    func_80B3EF10(this);
}

void EnXc_ActionFunc34(EnXc* this, PlayState* play) {
    EnXc_ActionFunc9(this, play);
    func_80B3EF34(this);
}

void EnXc_ActionFunc35(EnXc* this, PlayState* play) {
    EnXc_ActionFunc10(this, play);
    func_80B3EF58(this);
}

void EnXc_ActionFunc36(EnXc* this, PlayState* play) {
    EnXc_ActionFunc11(this, play);
    func_80B3EF80(this);
}

void EnXc_ActionFunc37(EnXc* this, PlayState* play) {
    EnXc_ActionFunc12(this, play);
    func_80B3EFA4(this);
}

void EnXc_ActionFunc38(EnXc* this, PlayState* play) {
    EnXc_ActionFunc13(this, play);
    func_80B3EFC8(this);
}

void EnXc_ActionFunc39(EnXc* this, PlayState* play) {
    EnXc_ReverseAccelerate(this, play);
    func_80B3EFEC(this);
}

void EnXc_ActionFunc40(EnXc* this, PlayState* play) {
    func_80B3D710(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    func_80B3F010(this);
}

void EnXc_ActionFunc41(EnXc* this, PlayState* play) {
    EnXc_HaltAndWaitToThrowNut(this, play);
    func_80B3F0B8(this);
}

void EnXc_ActionFunc42(EnXc* this, PlayState* play) {
    EnXc_ThrowNut(this, play);
    func_80B3F0DC(this);
}

void EnXc_ActionFunc43(EnXc* this, PlayState* play) {
    EnXc_Delete(this, play);
    func_80B3F100(this);
}

void EnXc_ActionFunc44(EnXc* this, PlayState* play) {
}
