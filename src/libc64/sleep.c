#include "global.h"

void csleep(OSTime cycles) {
    OSMesgQueue mq;
    OSMesg msg;
    OSTimer timer;

    osCreateMesgQueue(&mq, &msg, 1);
    osSetTimer(&timer, cycles, 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);
}

void nsleep(u32 nsec) {
    csleep(OS_NSEC_TO_CYCLES(nsec));
}

void usleep(u32 usec) {
    csleep(OS_USEC_TO_CYCLES(usec));
}

// originally "msleep"
void msleep(u32 ms) {
    csleep((ms * OS_CPU_COUNTER) / 1000ull);
}

void sleep(u32 sec) {
    csleep(sec * OS_CPU_COUNTER);
}
