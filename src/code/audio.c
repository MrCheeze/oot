#include "global.h"

u8 handle[] = {
    BANK_PLAYER, BANK_ITEM, BANK_ENV, BANK_ENEMY, BANK_SYSTEM, BANK_OCARINA, BANK_VOICE,
};

void audio_StopAllSoundEffect(void) {
    u8* bankIdPtr;

    for (bankIdPtr = &handle[0]; bankIdPtr < (handle + ARRAY_COUNT(handle)); bankIdPtr++) {
        Nai_StopAllHandleFx(*bankIdPtr);
    }
}
