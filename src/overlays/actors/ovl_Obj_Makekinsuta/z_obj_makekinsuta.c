/*
 * File: z_obj_makekinsuta.c
 * Overlay: ovl_Obj_Makekinsuta
 * Description: Skulltula Sprouting from Bean Spot
 */

#include "z_obj_makekinsuta.h"
#include "global.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Obj_Makekinsuta_actor_ct(Actor* thisx, PlayState* play);
void Obj_Makekinsuta_actor_move(Actor* thisx, PlayState* play);

static void mv_stop(ObjMakekinsuta* this, PlayState* play);
static void mv_end(ObjMakekinsuta* this, PlayState* play);

ActorProfile Obj_Makekinsuta_Profile = {
    /**/ ACTOR_OBJ_MAKEKINSUTA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjMakekinsuta),
    /**/ Obj_Makekinsuta_actor_ct,
    /**/ Cheap_non_move,
    /**/ Obj_Makekinsuta_actor_move,
    /**/ NULL,
};

void Obj_Makekinsuta_actor_ct(Actor* thisx, PlayState* play) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    if (PARAMS_GET_NOSHIFT(this->actor.params, 13, 2) == 0x4000) {
        PRINTF_COLOR_BLUE();
        // "Gold Star Enemy(arg_data %x)"
        PRINTF("金スタ発生敵(arg_data %x)\n", this->actor.params);
        PRINTF_RST();
    } else {
        PRINTF_COLOR_WARNING();
        // "Invalid Argument (arg_data %x)(%s %d)"
        PRINTF("引数不正 (arg_data %x)(%s %d)\n", this->actor.params, "../z_obj_makekinsuta.c", 119);
        PRINTF_RST();
    }
    this->actionFunc = mv_stop;
}

static void mv_stop(ObjMakekinsuta* this, PlayState* play) {
    if (this->unk_152 != 0) {
        if (this->timer >= 60 && !Actor_player_demo_check(GET_PLAYER(play))) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_SW, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, (this->actor.params | 0x8000));
            this->actionFunc = mv_end;
        } else {
            this->timer++;
        }
    } else {
        this->timer = 0;
    }
}

static void mv_end(ObjMakekinsuta* this, PlayState* play) {
}

void Obj_Makekinsuta_actor_move(Actor* thisx, PlayState* play) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    this->actionFunc(this, play);
}
