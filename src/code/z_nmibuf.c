#include "global.h"

#define COLD_RESET 0
#define NMI 1

void z_nmibuf_init(PreNmiBuff* this) {
    this->resetting = false;

    if (osResetType == COLD_RESET) {
        this->resetCount = 0;
        this->duration = 0;
    } else {
        this->resetCount++;
        this->duration += this->resetTime;
    }

    this->resetTime = 0;
}

void z_nmibuf_prenmi(PreNmiBuff* this) {
    this->resetting = true;
    this->resetTime = osGetTime();
}

u32 z_nmibuf_isprenmi(PreNmiBuff* this) {
    return this->resetting;
}
