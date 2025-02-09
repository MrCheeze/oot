/*
 * File: z_en_torch.c
 * Overlay: ovl_En_Torch
 * Description: Spawns a chest with the appropriate contents then unloads. Used in grottos.
 */

#include "z_en_torch.h"

#define FLAGS 0

void En_Torch_Actor_ct(Actor* thisx, PlayState* play);

ActorProfile En_Torch_Profile = {
    /**/ ACTOR_EN_TORCH,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnTorch),
    /**/ En_Torch_Actor_ct,
    /**/ NULL,
    /**/ NULL,
    /**/ NULL,
};

static u8 item_type_data[] = {
    GI_RUPEE_BLUE, GI_RUPEE_RED, GI_RUPEE_GOLD, GI_BOMBS_20, GI_BOMBS_1, GI_BOMBS_1, GI_BOMBS_1, GI_BOMBS_1,
};

void En_Torch_Actor_ct(Actor* thisx, PlayState* play) {
    EnTorch* this = (EnTorch*)thisx;
    s8 returnData = z_common_data.respawn[RESPAWN_MODE_RETURN].data;

    /* Spawn chest with desired contents.
       Contents are passed to en_torch from grotto params via Save Context. */
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOX, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                (item_type_data[(returnData >> 0x5) & 0x7] << 0x5) | 0x5000 | (returnData & 0x1F));

    Actor_delete(&this->actor);
}
