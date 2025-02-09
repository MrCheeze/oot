#include "global.h"
#include "terminal.h"

NORETURN void Freeze(void) {
    PRINTF(VT_FGCOL(RED) "\n**** Freeze!! ****\n" VT_RST);
    for (;;) {
        msleep(1000);
    }
}
