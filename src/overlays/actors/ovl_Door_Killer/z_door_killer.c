/*
 * File: z_door_killer.c
 * Overlay: ovl_Door_Killer
 * Description: Fake doors which attack player
 */

#include "z_door_killer.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"
#include "assets/objects/object_haka_door/object_haka_door.h"
#include "assets/objects/object_door_killer/object_door_killer.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum DoorKillerBehaviour {
    /* 0 */ DOOR_KILLER_DOOR,
    /* 1 */ DOOR_KILLER_RUBBLE_PIECE_1,
    /* 2 */ DOOR_KILLER_RUBBLE_PIECE_2,
    /* 3 */ DOOR_KILLER_RUBBLE_PIECE_3,
    /* 4 */ DOOR_KILLER_RUBBLE_PIECE_4
} DoorKillerBehaviour;

void Door_Killer_Actor_ct(Actor* thisx, PlayState* play2);
void Door_Killer_Actor_dt(Actor* thisx, PlayState* play);
void Door_Killer_Actor_move(Actor* thisx, PlayState* play);
static void move_close(DoorKiller* this, PlayState* play);
static void move_wait(DoorKiller* this, PlayState* play);
void Door_Killer_Actor_draw(Actor* thisx, PlayState* play);
void Door_Killer_Part_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Door_Killer_Profile = {
    /**/ ACTOR_DOOR_KILLER,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DOOR_KILLER,
    /**/ sizeof(DoorKiller),
    /**/ Door_Killer_Actor_ct,
    /**/ Door_Killer_Actor_dt,
    /**/ Door_Killer_Actor_move,
    /**/ NULL,
};

static ColliderCylinderInit DoorKillerOcInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x0001FFEE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit CrossSphElemDt_base[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 100 }, 100 },
    },
};

static ColliderJntSphInit CrossSphDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    CrossSphElemDt_base,
};

static DoorKillerTextureEntry door_killer_status[4] = {
    { OBJECT_HIDAN_OBJECTS, gFireTempleDoorKillerTex },
    { OBJECT_MIZU_OBJECTS, object_mizu_objects_Tex_0035C0 },
    { OBJECT_HAKA_DOOR, object_haka_door_Tex_000000 },
    { OBJECT_GAMEPLAY_KEEP, gWoodenDoorTex },
};

void Door_Killer_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    f32 randF;
    DoorKiller* this = (DoorKiller*)thisx;
    s32 objectSlot;
    s32 i;

    // Look in the object bank for one of the four objects containing door textures
    objectSlot = -1;
    //! @bug If no objectSlot is found, `door_killer_status` will be read out of bounds
    for (i = 0; objectSlot < 0; i++) {
        objectSlot = Object_Exchange_bank_check(&play->objectCtx, door_killer_status[i].objectId);
        this->textureEntryIndex = i;
    }
    PRINTF("bank_ID = %d\n", objectSlot);
    PRINTF("status = %d\n", this->textureEntryIndex);
    this->requiredObjectSlot = objectSlot;
    this->texture = door_killer_status[this->textureEntryIndex].texture;

    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_set_scale(&this->actor, 0.01f);
    this->timer = 0;
    this->hasHitPlayerOrGround = 0;
    this->openAnim = 0;
    this->playerIsOpening = false;

    switch ((u8)PARAMS_GET_U(this->actor.params, 0, 8)) {
        case DOOR_KILLER_DOOR:
            // `jointTable` is used for both the `jointTable` and `morphTable` args here. Because this actor doesn't
            // play any animations it does not cause problems, but it would need to be changed otherwise.
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_door_killer_Skel_001BC8, NULL, this->jointTable,
                               this->jointTable, 9);
            this->actionFunc = move_wait;
            move_wait(this, play);

            // manually set the overall rotation of the door
            this->jointTable[1].x = this->jointTable[1].z = 0x4000;

            // Set a cylinder collider to detect link attacks and larger sphere collider to detect explosions
            ClObjPipe_ct(play, &this->colliderCylinder);
            ClObjPipe_set5(play, &this->colliderCylinder, &this->actor, &DoorKillerOcInfoData);
            ClObjJntSph_ct(play, &this->colliderJntSph);
            ClObjJntSph_set5_nzm(play, &this->colliderJntSph, &this->actor, &CrossSphDt_base, this->colliderJntSphItems);
            this->colliderJntSph.elements[0].dim.worldSphere.radius = 80;
            this->colliderJntSph.elements[0].dim.worldSphere.center.x = (s16)this->actor.world.pos.x;
            this->colliderJntSph.elements[0].dim.worldSphere.center.y = (s16)this->actor.world.pos.y + 50;
            this->colliderJntSph.elements[0].dim.worldSphere.center.z = (s16)this->actor.world.pos.z;

            // If tied to a switch flag and that switch flag is already set, kill the actor.
            if ((PARAMS_GET_U(this->actor.params, 8, 6) != 0x3F) &&
                Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
                Actor_delete(&this->actor);
            }
            break;
        case DOOR_KILLER_RUBBLE_PIECE_1:
        case DOOR_KILLER_RUBBLE_PIECE_2:
        case DOOR_KILLER_RUBBLE_PIECE_3:
        case DOOR_KILLER_RUBBLE_PIECE_4:
            this->actionFunc = move_wait;
            move_wait(this, play);

            this->actor.gravity = -0.6f;
            this->actor.minVelocityY = -6.0f;

            // Random trajectories for rubble pieces
            randF = rnd_fx(8.0f);
            this->actor.velocity.z = rnd_f(8.0f);
            this->actor.velocity.x = (cos_s(this->actor.world.rot.y) * randF) +
                                     (sin_s(this->actor.world.rot.y) * this->actor.velocity.z);
            this->actor.velocity.z = (-sin_s(this->actor.world.rot.y) * randF) +
                                     (cos_s(this->actor.world.rot.y) * this->actor.velocity.z);
            this->actor.velocity.y = rnd_f(4.0f) + 4.0f;

            // These are used as the x,y,z rotational velocities in move_part
            this->actor.world.rot.x = rnd_fx(0x1000);
            this->actor.world.rot.y = rnd_fx(0x1000);
            this->actor.world.rot.z = rnd_fx(0x1000);
            this->timer = 80;
            break;
    }
}

void Door_Killer_Actor_dt(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    if (PARAMS_GET_U(thisx->params, 0, 8) == DOOR_KILLER_DOOR) {
        ClObjPipe_dt(play, &this->colliderCylinder);
        ClObjJntSph_dt_nzf(play, &this->colliderJntSph);
    }
}

void make_bom_effect(Actor* thisx, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x, thisx->world.pos.y + 9.0f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_1);
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x + 7.88f, thisx->world.pos.y + 39.8f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_2);
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x - 15.86f, thisx->world.pos.y + 61.98f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_3);
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x + 3.72f, thisx->world.pos.y + 85.1f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_4);
}

/**
 * action function for the individual door pieces that spawn and fall down when the door is destroyed
 */
void move_part(DoorKiller* this, PlayState* play) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }

    this->actor.velocity.x *= 0.98f;
    this->actor.velocity.z *= 0.98f;

    // world.rot is repurposed to be the rotation velocity for the rubble pieces
    this->actor.shape.rot.x += this->actor.world.rot.x;
    this->actor.shape.rot.y += this->actor.world.rot.y;
    this->actor.shape.rot.z += this->actor.world.rot.z;

    if (this->timer != 0) {
        this->timer--;
    } else {
        Actor_delete(&this->actor);
    }
    Actor_position_move(&this->actor);
}

s32 ClObjxACHit_local(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;
    if ((this->colliderCylinder.base.acFlags & AC_HIT) && (this->colliderCylinder.elem.acHitElem != NULL)) {
        return true;
    }
    return false;
}

void ClObjxACHit_local_after(DoorKiller* this, PlayState* play) {
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderCylinder);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
}

void move_bomb(DoorKiller* this, PlayState* play) {
    s32 switchFlag = PARAMS_GET_U(this->actor.params, 8, 6);

    // Can set a switch flag on death based on params
    if (switchFlag != 0x3F) {
        Actor_Environment_sw_On(play, switchFlag);
    }
    Actor_delete(&this->actor);
}

/**
 * After slamming on the floor, rise back upright
 */
void move_attack1(DoorKiller* this, PlayState* play) {
    s32 i;
    s16 rotation;

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = move_close;
        this->timer = 16;
        ClObjxACHit_local_after(this, play);
        return;
    }

    this->actor.shape.rot.x = (this->timer >= 8) ? (this->timer * 0x800) - 0x4000 : 0;

    if (this->timer >= 12) {
        rotation = (-this->timer * -500) - 8000;
    } else if (this->timer >= 8) {
        rotation = -2000;
    } else if (this->timer >= 5) {
        rotation = (this->timer * -500) + 2000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = -rotation;
    }

    if (this->timer < 8) {
        rotation = sin_s(this->timer * 0x2000) * this->timer * 100.0f;
        for (i = 2; i < 9; i++) {
            this->jointTable[i].y = rotation;
        }
    }
}

/**
 * After wobbling, fall over and slam onto the floor, damaging the player if they are in the way. Uses manual distance
 * check for damaging player, not AT system.
 */
void move_attack0(DoorKiller* this, PlayState* play) {
    s32 i;
    s16 rotation;

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = move_attack1;
        this->timer = 16;
        return;
    }

    this->actor.shape.rot.x = (this->timer >= 4) ? 0x8000 + (-this->timer * 0x1000) : 0x4000;

    if (this->timer >= 6) {
        rotation = (-this->timer * -500) - 4000;
    } else if (this->timer >= 4) {
        rotation = -1000;
    } else if (this->timer >= 3) {
        rotation = (this->timer * -500) + 1000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = rotation;
    }

    if (this->timer == 4) {
        // spawn 20 random dust particles just before slamming down
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 1.0f, 0.0f };
        Vec3f pos;
        s32 j;
        f32 randF;

        for (j = 0; j != 20; j++) {
            pos.y = 0.0f;
            randF = rnd_fx(40.0f);
            pos.z = rnd_f(100.0f);
            pos.x = (cos_s(this->actor.world.rot.y) * randF) + (sin_s(this->actor.world.rot.y) * pos.z);
            pos.z = (-sin_s(this->actor.world.rot.y) * randF) + (cos_s(this->actor.world.rot.y) * pos.z);
            velocity.x = pos.x * 0.2f;
            velocity.z = pos.z * 0.2f;
            accel.x = -(velocity.x) * 0.1f;
            accel.z = -(velocity.z) * 0.1f;
            pos.x += this->actor.world.pos.x;
            pos.y += this->actor.world.pos.y;
            pos.z += this->actor.world.pos.z;
            Effect_SS_Dust_sc_ct(play, &pos, &velocity, &accel, 300, 30);
        }
    }
    if (!(this->hasHitPlayerOrGround & 1)) {
        Vec3f playerPosRelToDoor;
        Player* player = GET_PLAYER(play);
        Actor_search_position_project_distanceXZ(&this->actor, &playerPosRelToDoor, &player->actor.world.pos);
        if ((fabsf(playerPosRelToDoor.y) < 20.0f) && (fabsf(playerPosRelToDoor.x) < 20.0f) &&
            (playerPosRelToDoor.z < 100.0f) && (playerPosRelToDoor.z > 0.0f)) {
            this->hasHitPlayerOrGround |= 1;
            Actor_player_power_damage_AT_set(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f, 0x10);
            Actor_SE_set(&this->actor, NA_SE_EN_KDOOR_HIT);
            player_SE_set(player, NA_SE_PL_BODY_HIT);
        }
    }
    if (!(this->hasHitPlayerOrGround & 1) && (this->timer == 2)) {
        this->hasHitPlayerOrGround |= 1;
        Actor_SE_set(&this->actor, NA_SE_EN_KDOOR_HIT_GND);
    }
}

/**
 * Wobble around, signifying the door is about to fall over. Does not set AC and so cannot be destroyed during this.
 */
void move_dance(DoorKiller* this, PlayState* play) {
    s16 rotation;
    s32 i;

    if ((this->timer == 16) || (this->timer == 8)) {
        Actor_SE_set(&this->actor, NA_SE_EN_KDOOR_WAVE);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = move_attack0;
        this->timer = 8;
        this->hasHitPlayerOrGround &= ~1;
        return;
    }

    rotation = sin_s(this->timer * 0x2000) * this->timer * 100.0f;
    for (i = 2; i < 9; i++) {
        this->jointTable[i].y = rotation;
    }
    rotation = (u16)(s32)(-cos_s(this->timer * 0x1000) * 1000.0f) + 1000;
    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = rotation;
    }
}

/**
 * Idle while the player attempts to open the door and then begin to wobble
 */
void move_dance_w(DoorKiller* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        this->timer = 16;
        this->actionFunc = move_dance;
    }
}

static void move_close(DoorKiller* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f playerPosRelToDoor;
    s16 angleToFacingPlayer;

    Actor_search_position_project_distanceXZ(&this->actor, &playerPosRelToDoor, &player->actor.world.pos);

    // playerIsOpening is set by player
    if (this->playerIsOpening) {
        this->actionFunc = move_dance_w;
        this->timer = 10;
        this->playerIsOpening = false;
        return;
    }

    if (ClObjxACHit_local(&this->actor, play)) {
        // AC cylinder: wobble if hit by most weapons, die if hit by explosives or hammer
        if (this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_RANGED | DMG_SLASH | DMG_DEKU_STICK)) {
            this->timer = 16;
            this->actionFunc = move_dance;
        } else if (this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_HAMMER_SWING | DMG_EXPLOSIVE)) {
            make_bom_effect(&this->actor, play);
            this->actionFunc = move_bomb;
            Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
        }
    } else if (BlastVsMyCheck(play, &this->colliderJntSph.base) != NULL) {
        // AC sphere: die if hit by explosive
        make_bom_effect(&this->actor, play);
        this->actionFunc = move_bomb;
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
    } else if (!player_demo_check(play) && (fabsf(playerPosRelToDoor.y) < 20.0f) &&
               (fabsf(playerPosRelToDoor.x) < 20.0f) && (playerPosRelToDoor.z < 50.0f) &&
               (playerPosRelToDoor.z > 0.0f)) {
        // Set player properties to make the door openable if within range
        angleToFacingPlayer = player->actor.shape.rot.y - this->actor.shape.rot.y;
        if (playerPosRelToDoor.z > 0.0f) {
            angleToFacingPlayer = 0x8000 - angleToFacingPlayer;
        }
        if (ABS(angleToFacingPlayer) < 0x3000) {
            player->doorType = PLAYER_DOORTYPE_FAKE;
            player->doorDirection = (playerPosRelToDoor.z >= 0.0f) ? 1.0f : -1.0f;
            player->doorActor = &this->actor;
        }
    }

    ClObjxACHit_local_after(this, play);
}

/**
 * Grabs the virtual address of the texture from the relevant door object
 */
void set_texture(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->requiredObjectSlot].segment);
    this->texture = SEGMENTED_TO_VIRTUAL(this->texture);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->actor.objectSlot].segment);
}

/**
 * Gets the correct door texture, defines the appropriate draw fucntion and action function based on type behaviour
 * (door or rubble).
 */
static void move_wait(DoorKiller* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        set_texture(&this->actor, play);
        switch (PARAMS_GET_U(this->actor.params, 0, 8)) {
            case DOOR_KILLER_DOOR:
                this->actionFunc = move_close;
                this->actor.draw = Door_Killer_Actor_draw;
                break;
            case DOOR_KILLER_RUBBLE_PIECE_1:
            case DOOR_KILLER_RUBBLE_PIECE_2:
            case DOOR_KILLER_RUBBLE_PIECE_3:
            case DOOR_KILLER_RUBBLE_PIECE_4:
                this->actionFunc = move_part;
                this->actor.draw = Door_Killer_Part_Actor_draw;
                break;
        }
    }
}

void Door_Killer_Actor_move(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    this->actionFunc(this, play);
}

void Door_Killer_Actor_draw_common(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;
    void* doorTexture = this->texture;

    OPEN_DISPS(play->state.gfxCtx, "../z_door_killer.c", 883);
    gSPSegment(POLY_OPA_DISP++, 0x08, doorTexture);
    CLOSE_DISPS(play->state.gfxCtx, "../z_door_killer.c", 885);
}

void Door_Killer_Actor_draw(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Door_Killer_Actor_draw_common(&this->actor, play);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, NULL);
}

void Door_Killer_Part_Actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = { object_door_killer_DL_001250, object_door_killer_DL_001550, object_door_killer_DL_0017B8,
                             object_door_killer_DL_001A58 };
    s32 rubblePieceIndex = PARAMS_GET_U(thisx->params, 0, 8) - 1;
    DoorKiller* this = (DoorKiller*)thisx;

    if ((this->timer >= 20) || ((this->timer & 1) == 0)) {
        Door_Killer_Actor_draw_common(thisx, play);
        Cheap_gfx_display(play, shape_model[rubblePieceIndex]);
    }
}
