/**
 * @file fault.c
 *
 * This file implements the screen that may be viewed when the game crashes.
 * This is the second version of the crash screen, originally used in Majora's Mask.
 *
 * When the game crashes, a red bar will be drawn to the top-left of the screen, indicating that the
 * crash screen is available for use. Once this bar appears, it is possible to open the crash screen
 * with the following button combination:
 *
 * (L & R & Z) + DPad-Up + C-Down + C-Up + DPad-Down + DPad-Left + C-Left + C-Right + DPad-Right + (B & A & START)
 *
 * When entering this button combination, buttons that are &'d together must all be pressed together.
 * The L & R presses and B & A presses may be interchanged in the order they are pressed.
 *
 * "Clients" may be registered with the crash screen to extend its functionality. There are
 * two kinds of client, "Client" and "AddressConverterClient". Clients contribute one or
 * more pages to the crash debugger, while Address Converter Clients allow the crash screen to look up
 * the virtual addresses of dynamically allocated overlays.
 *
 * The crash screen has multiple pages:
 *  - Thread Context
 *      This page shows information about the thread on which the program crashed. It displays
 *      the cause of the crash, state of general-purpose registers, state of floating-point registers
 *      and the floating-point status register. If a floating-point exception caused the crash, it will
 *      be displayed next to the floating-point status register.
 *  - Stack Trace
 *      This page displays a full backtrace from the crashing function back to the start of the thread. It
 *      displays the Program Counter for each function and, if applicable, the Virtual Program Counter
 *      for relocated functions in overlays.
 *  - Client Pages
 *      After the stack trace page, currently registered clients are processed and their pages are displayed.
 *  - Memory Dump
 *      This page implements a scrollable memory dump.
 *  - End Screen
 *      This page informs you that there are no more pages to display.
 *
 * To navigate the pages, DPad-Right or A may be used to advance to the next page, and L toggles whether to
 * automatically scroll to the next page after some time has passed.
 * DPad-Up may be pressed to enable sending fault pages over osSyncPrintf as well as displaying them on-screen.
 * DPad-Down disables sending fault pages over osSyncPrintf.
 */
#pragma increment_block_number "gc-eu:160 gc-eu-mq:160 gc-eu-mq-dbg:144 gc-jp:160 gc-jp-ce:160 gc-jp-mq:160 gc-us:160" \
                               "gc-us-mq:160 ique-cn:160 hiratsu3:160"

#include "global.h"
#include "alloca.h"
#include "fault.h"
#include "stack.h"
#include "terminal.h"

void DbStartDebugger(void);
void faultprint_SyncPrintMode(u32 enabled);
void faultprint_FillRectangle(s32 xStart, s32 yStart, s32 xEnd, s32 yEnd, u16 color);
void faultprint_ClearScreen(void);
void faultprint_SetNextPageCallback(void (*callback)(void));
void faultprint_SetFrameBuffer(void* fb, u16 w, u16 h);

const char* cpuExceptions[] = {
    "Interrupt",
    "TLB modification",
    "TLB exception on load",
    "TLB exception on store",
    "Address error on load",
    "Address error on store",
    "Bus error on inst.",
    "Bus error on data",
    "System call exception",
    "Breakpoint exception",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap exception",
    "Virtual coherency on inst.",
    "Floating point exception",
    "Watchpoint exception",
    "Virtual coherency on data",
};

const char* fpuExceptions[] = {
    "Unimplemented operation", "Invalid operation", "Division by zero", "Overflow", "Underflow", "Inexact operation",
};

FaultMgr* this;
u8 key_waiting;
STACK(faultStack, 0x600);
StackEntry fault_stackcheck;
FaultMgr debugger;

typedef struct FaultClientTask {
    /* 0x00 */ s32 (*callback)(void*, void*);
    /* 0x04 */ void* arg0;
    /* 0x08 */ void* arg1;
    /* 0x0C */ s32 ret;
    /* 0x10 */ OSMesgQueue* queue;
    /* 0x14 */ OSMesg msg;
} FaultClientTask; // size = 0x18

void my_msleep(u32 msec) {
    u64 cycles = (msec * OS_CPU_COUNTER) / 1000ull;

    csleep(cycles);
}

void fault_callback_proc(void* arg) {
    FaultClientTask* task = (FaultClientTask*)arg;

    // Run the callback
    if (task->callback != NULL) {
        task->ret = task->callback(task->arg0, task->arg1);
    }

    // Send completion notification
    if (task->queue != NULL) {
        osSendMesg(task->queue, task->msg, OS_MESG_BLOCK);
    }
}

void fault_callfunc_param(FaultClientTask* task) {
    OSMesgQueue queue;
    OSMesg msg;
    OSMesg recMsg;
    OSThread* thread = NULL;
    OSTimer timer;
    u32 timerMsgVal = 666;

    osCreateMesgQueue(&queue, &msg, 1);
    task->queue = &queue;
    task->msg = NULL;

    if (this->clientThreadSp != NULL) {
        // Run the fault client callback on a separate thread
        thread = alloca(sizeof(OSThread));

        osCreateThread(thread, THREAD_ID_FAULT, fault_callback_proc, task, this->clientThreadSp,
                       THREAD_PRI_FAULT_CLIENT);
        osStartThread(thread);
    } else {
        // Run the fault client callback on this thread
        fault_callback_proc(task);
    }

    // Await done
    while (true) {
        // Wait for 1 second
        osSetTimer(&timer, OS_USEC_TO_CYCLES(1000000), 0, &queue, (OSMesg)timerMsgVal);
        osRecvMesg(&queue, &recMsg, OS_MESG_BLOCK);

        if (recMsg != (OSMesg)666) {
            break;
        }

        if (!key_waiting) {
            task->ret = -1;
            break;
        }
    }

    osStopTimer(&timer);

    // Destroy thread if a thread was used
    if (thread != NULL) {
        osStopThread(thread);
        osDestroyThread(thread);
    }
}

s32 fault_callfunc(void* callback, void* arg0, void* arg1) {
    FaultClientTask task;

    task.callback = callback;
    task.arg0 = arg0;
    task.arg1 = arg1;
    task.ret = 0;
    fault_callfunc_param(&task);
    return task.ret;
}

/**
 * Registers a fault client.
 *
 * Clients contribute at least one page to the crash screen, drawn by `callback`.
 * Arguments are passed on to the callback through `arg0` and `arg1`.
 *
 * The callback is intended to be
 * `void (*callback)(void* arg0, void* arg1)`
 */
void fault_AddClient(FaultClient* client, void* callback, void* arg0, void* arg1) {
    OSIntMask mask;
    s32 alreadyExists = false;

    mask = osSetIntMask(OS_IM_NONE);

    // Ensure the client is not already registered
    {
        FaultClient* iterClient = this->clients;

        while (iterClient != NULL) {
            if (iterClient == client) {
                alreadyExists = true;
                goto end;
            }
            iterClient = iterClient->next;
        }
    }

    client->callback = callback;
    client->arg0 = arg0;
    client->arg1 = arg1;
    client->next = this->clients;
    this->clients = client;

end:
    osSetIntMask(mask);
    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) T("fault_AddClient: %08x は既にリスト中にある\n",
                                          "fault_AddClient: %08x is already in the list\n") VT_RST,
                     client);
    }
}

/**
 * Removes a fault client so that the page is no longer displayed if a crash occurs.
 */
void fault_RemoveClient(FaultClient* client) {
    FaultClient* iterClient = this->clients;
    FaultClient* lastClient = NULL;
    OSIntMask mask;
    s32 listIsEmpty = false;

    mask = osSetIntMask(OS_IM_NONE);

    while (iterClient != NULL) {
        if (iterClient == client) {
            if (lastClient != NULL) {
                lastClient->next = client->next;
            } else {
                this->clients = client;
                if (this->clients) {
                    this->clients = client->next;
                } else {
                    listIsEmpty = true;
                }
            }
            break;
        }
        lastClient = iterClient;
        iterClient = iterClient->next;
    }

    osSetIntMask(mask);

    if (listIsEmpty) {
        osSyncPrintf(VT_COL(RED, WHITE) T("fault_RemoveClient: %08x リスト不整合です\n",
                                          "fault_RemoveClient: %08x list inconsistency\n") VT_RST,
                     client);
    }
}

/**
 * Registers an address converter client. This enables the crash screen to look up virtual
 * addresses of overlays relocated during runtime. Address conversion is carried out by
 * `callback`, which either returns a virtual address or NULL if the address could not
 * be converted.
 *
 * The callback is intended to be
 * `uintptr_t (*callback)(uintptr_t addr, void* arg)`
 * The callback may return 0 if it could not convert the address
 * The callback may return -1 to be unregistered
 */
void fault_AddressConverterAddClient(FaultAddrConvClient* client, void* callback, void* arg) {
    OSIntMask mask;
    s32 alreadyExists = false;

    mask = osSetIntMask(OS_IM_NONE);

    // Ensure the client is not already registered
    {
        FaultAddrConvClient* iterClient = this->addrConvClients;

        while (iterClient != NULL) {
            if (iterClient == client) {
                alreadyExists = true;
                goto end;
            }
            iterClient = iterClient->next;
        }
    }

    client->callback = callback;
    client->arg = arg;
    client->next = this->addrConvClients;
    this->addrConvClients = client;

end:
    osSetIntMask(mask);
    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) T("fault_AddressConverterAddClient: %08x は既にリスト中にある\n",
                                          "fault_AddressConverterAddClient: %08x is already in the list\n") VT_RST,
                     client);
    }
}

void fault_AddressConverterRemoveClient(FaultAddrConvClient* client) {
    FaultAddrConvClient* iterClient = this->addrConvClients;
    FaultAddrConvClient* lastClient = NULL;
    OSIntMask mask;
    s32 listIsEmpty = false;

    mask = osSetIntMask(OS_IM_NONE);

    while (iterClient != NULL) {
        if (iterClient == client) {
            if (lastClient != NULL) {
                lastClient->next = client->next;
            } else {
                this->addrConvClients = client;

                if (this->addrConvClients != NULL) {
                    this->addrConvClients = client->next;
                } else {
                    listIsEmpty = true;
                }
            }
            break;
        }
        lastClient = iterClient;
        iterClient = iterClient->next;
    }

    osSetIntMask(mask);

    if (listIsEmpty) {
        osSyncPrintf(VT_COL(RED, WHITE) T("fault_AddressConverterRemoveClient: %08x は既にリスト中にある\n",
                                          "fault_AddressConverterRemoveClient: %08x is already in the list\n") VT_RST,
                     client);
    }
}

/**
 * Converts `addr` to a virtual address via the registered
 * address converter clients
 */
uintptr_t fault_AddressConvert(uintptr_t addr) {
    s32 ret;
    FaultAddrConvClient* client = this->addrConvClients;

    while (client != NULL) {
        if (client->callback != NULL) {
            ret = fault_callfunc(client->callback, (void*)addr, client->arg);
            if (ret == -1) {
                fault_AddressConverterRemoveClient(client);
            } else if (ret != 0) {
                return (uintptr_t)ret;
            }
        }
        client = client->next;
    }

    return 0;
}

void WaitTime(u32 msec) {
    my_msleep(msec);
}

#ifndef AVOID_UB
void padmgr_RequestPadData(Input* inputs, s32 gameRequest);
#endif

void callback_get_pads_default(Input* inputs) {
    //! @bug This function is not called correctly, it is missing a leading PadMgr* argument. This
    //! renders the crash screen unusable.
    //! In Majora's Mask, PadMgr functions were changed to not require this argument, and this was
    //! likely just not addressed when backporting.
#ifndef AVOID_UB
    padmgr_RequestPadData(inputs, false);
#else
    // Guarantee crashing behavior: false -> NULL, previous value in a2 is more often non-zero than zero
    padmgr_RequestPadData((PadMgr*)inputs, NULL, true);
#endif
}

void fault_get_pads(void) {
    this->padCallback(this->inputs);
}

/**
 * Awaits user input
 *
 * L toggles auto-scroll
 * DPad-Up enables osSyncPrintf output
 * DPad-Down disables osSyncPrintf output
 * A and DPad-Right continues and returns true
 * DPad-Left continues and returns false
 */
u32 fault_KeyWait2(void) {
    Input* input = &this->inputs[0];
    s32 count = 600;
    u32 pressedBtn;

    while (true) {
        WaitTime(1000 / 60);
        fault_get_pads();

        pressedBtn = input->press.button;

        if (pressedBtn == BTN_L) {
            this->autoScroll = !this->autoScroll;
        }

        if (this->autoScroll) {
            if (count-- < 1) {
                return false;
            }
        } else {
            if (pressedBtn == BTN_A || pressedBtn == BTN_DRIGHT) {
                return false;
            }

            if (pressedBtn == BTN_DLEFT) {
                return true;
            }

            if (pressedBtn == BTN_DUP) {
                faultprint_SyncPrintMode(true);
            }

            if (pressedBtn == BTN_DDOWN) {
                faultprint_SyncPrintMode(false);
            }
        }
    }
}

void fault_KeyWait(void) {
    key_waiting = true;
    fault_KeyWait2();
    key_waiting = false;
}

void QuarterRectangle(s32 x, s32 y, s32 w, s32 h, u16 color) {
    faultprint_FillRectangle(x, y, x + w - 1, y + h - 1, color);
}

void ClearScreen(void) {
    faultprint_SetForegroundColor(GPACK_RGBA5551(255, 255, 255, 1));
    faultprint_SetBackgroundColor(GPACK_RGBA5551(0, 0, 0, 1));
    faultprint_ClearScreen();
    faultprint_SetBackgroundColor(GPACK_RGBA5551(0, 0, 0, 0));
}

void ClearScreenRed(void) {
    faultprint_SetForegroundColor(GPACK_RGBA5551(255, 255, 255, 1));
    faultprint_SetBackgroundColor(GPACK_RGBA5551(240, 0, 0, 1));
    faultprint_ClearScreen();
    faultprint_SetBackgroundColor(GPACK_RGBA5551(0, 0, 0, 0));
}

void ColorBox(u16 color) {
    QuarterRectangle(22, 16, 8, 1, color);
}

void PrintFPURegs(s32 idx, f32* value) {
    u32 raw = *(u32*)value;
    s32 exp = ((raw & 0x7F800000) >> 23) - 127;

    if ((exp > -127 && exp <= 127) || raw == 0) {
        faultprint_Printf("F%02d:%14.7e ", idx, *value);
    } else {
        // Print subnormal floats as their ieee-754 hex representation
        faultprint_Printf("F%02d:  %08x(16) ", idx, raw);
    }
}

void PrintFPURegsOnIndy(s32 idx, f32* value) {
    u32 raw = *(u32*)value;
    s32 exp = ((raw & 0x7F800000) >> 23) - 127;

    if ((exp > -127 && exp <= 127) || raw == 0) {
        osSyncPrintf("F%02d:%14.7e ", idx, *value);
    } else {
        osSyncPrintf("F%02d:  %08x(16) ", idx, *(u32*)value);
    }
}

void PrintFPUFlags(u32 value) {
    s32 i;
    u32 flag = FPCSR_CE;

    faultprint_Printf("FPCSR:%08xH ", value);

    // Go through each of the six causes and print the name of
    // the first cause that is set
    for (i = 0; i < ARRAY_COUNT(fpuExceptions); i++) {
        if (value & flag) {
            faultprint_Printf("(%s)", fpuExceptions[i]);
            break;
        }
        flag >>= 1;
    }
    faultprint_Printf("\n");
}

void PrintFPUFlagsOnIndy(u32 value) {
    s32 i;
    u32 flag = FPCSR_CE;

    osSyncPrintf("FPCSR:%08xH  ", value);
    for (i = 0; i < ARRAY_COUNT(fpuExceptions); i++) {
        if (value & flag) {
            osSyncPrintf("(%s)\n", fpuExceptions[i]);
            break;
        }
        flag >>= 1;
    }
}

void PrintDebugInfo(OSThread* thread) {
    __OSThreadContext* ctx;
    s16 causeStrIdx = _SHIFTR((u32)thread->context.cause, 2, 5);

    if (causeStrIdx == (EXC_WATCH >> CAUSE_EXCSHIFT)) {
        causeStrIdx = 16;
    }
    if (causeStrIdx == (EXC_VCED >> CAUSE_EXCSHIFT)) {
        causeStrIdx = 17;
    }

    faultprint_ClearScreen();
    faultprint_SetMargin(-2, 4);
    faultprint_Locate(22, 20);

    ctx = &thread->context;
    faultprint_Printf("THREAD:%d (%d:%s)\n", thread->id, causeStrIdx, cpuExceptions[causeStrIdx]);
    faultprint_SetMargin(-1, 0);

    faultprint_Printf("PC:%08xH SR:%08xH VA:%08xH\n", (u32)ctx->pc, (u32)ctx->sr, (u32)ctx->badvaddr);
    faultprint_Printf("AT:%08xH V0:%08xH V1:%08xH\n", (u32)ctx->at, (u32)ctx->v0, (u32)ctx->v1);
    faultprint_Printf("A0:%08xH A1:%08xH A2:%08xH\n", (u32)ctx->a0, (u32)ctx->a1, (u32)ctx->a2);
    faultprint_Printf("A3:%08xH T0:%08xH T1:%08xH\n", (u32)ctx->a3, (u32)ctx->t0, (u32)ctx->t1);
    faultprint_Printf("T2:%08xH T3:%08xH T4:%08xH\n", (u32)ctx->t2, (u32)ctx->t3, (u32)ctx->t4);
    faultprint_Printf("T5:%08xH T6:%08xH T7:%08xH\n", (u32)ctx->t5, (u32)ctx->t6, (u32)ctx->t7);
    faultprint_Printf("S0:%08xH S1:%08xH S2:%08xH\n", (u32)ctx->s0, (u32)ctx->s1, (u32)ctx->s2);
    faultprint_Printf("S3:%08xH S4:%08xH S5:%08xH\n", (u32)ctx->s3, (u32)ctx->s4, (u32)ctx->s5);
    faultprint_Printf("S6:%08xH S7:%08xH T8:%08xH\n", (u32)ctx->s6, (u32)ctx->s7, (u32)ctx->t8);
    faultprint_Printf("T9:%08xH GP:%08xH SP:%08xH\n", (u32)ctx->t9, (u32)ctx->gp, (u32)ctx->sp);
    faultprint_Printf("S8:%08xH RA:%08xH LO:%08xH\n\n", (u32)ctx->s8, (u32)ctx->ra, (u32)ctx->lo);

    PrintFPUFlags(ctx->fpcsr);
    faultprint_Printf("\n");

    PrintFPURegs(0, &ctx->fp0.f.f_even);
    PrintFPURegs(2, &ctx->fp2.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(4, &ctx->fp4.f.f_even);
    PrintFPURegs(6, &ctx->fp6.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(8, &ctx->fp8.f.f_even);
    PrintFPURegs(10, &ctx->fp10.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(12, &ctx->fp12.f.f_even);
    PrintFPURegs(14, &ctx->fp14.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(16, &ctx->fp16.f.f_even);
    PrintFPURegs(18, &ctx->fp18.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(20, &ctx->fp20.f.f_even);
    PrintFPURegs(22, &ctx->fp22.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(24, &ctx->fp24.f.f_even);
    PrintFPURegs(26, &ctx->fp26.f.f_even);
    faultprint_Printf("\n");
    PrintFPURegs(28, &ctx->fp28.f.f_even);
    PrintFPURegs(30, &ctx->fp30.f.f_even);
    faultprint_Printf("\n");

    faultprint_SetMargin(0, 0);
}

void PrintDebugInfoOnIndy(OSThread* thread) {
    __OSThreadContext* ctx;
    s16 causeStrIdx = _SHIFTR((u32)thread->context.cause, 2, 5);

    if (causeStrIdx == (EXC_WATCH >> CAUSE_EXCSHIFT)) {
        causeStrIdx = 16;
    }
    if (causeStrIdx == (EXC_VCED >> CAUSE_EXCSHIFT)) {
        causeStrIdx = 17;
    }

    ctx = &thread->context;
    osSyncPrintf("\n");
    osSyncPrintf("THREAD ID:%d (%d:%s)\n", thread->id, causeStrIdx, cpuExceptions[causeStrIdx]);

    osSyncPrintf("PC:%08xH   SR:%08xH   VA:%08xH\n", (u32)ctx->pc, (u32)ctx->sr, (u32)ctx->badvaddr);
    osSyncPrintf("AT:%08xH   V0:%08xH   V1:%08xH\n", (u32)ctx->at, (u32)ctx->v0, (u32)ctx->v1);
    osSyncPrintf("A0:%08xH   A1:%08xH   A2:%08xH\n", (u32)ctx->a0, (u32)ctx->a1, (u32)ctx->a2);
    osSyncPrintf("A3:%08xH   T0:%08xH   T1:%08xH\n", (u32)ctx->a3, (u32)ctx->t0, (u32)ctx->t1);
    osSyncPrintf("T2:%08xH   T3:%08xH   T4:%08xH\n", (u32)ctx->t2, (u32)ctx->t3, (u32)ctx->t4);
    osSyncPrintf("T5:%08xH   T6:%08xH   T7:%08xH\n", (u32)ctx->t5, (u32)ctx->t6, (u32)ctx->t7);
    osSyncPrintf("S0:%08xH   S1:%08xH   S2:%08xH\n", (u32)ctx->s0, (u32)ctx->s1, (u32)ctx->s2);
    osSyncPrintf("S3:%08xH   S4:%08xH   S5:%08xH\n", (u32)ctx->s3, (u32)ctx->s4, (u32)ctx->s5);
    osSyncPrintf("S6:%08xH   S7:%08xH   T8:%08xH\n", (u32)ctx->s6, (u32)ctx->s7, (u32)ctx->t8);
    osSyncPrintf("T9:%08xH   GP:%08xH   SP:%08xH\n", (u32)ctx->t9, (u32)ctx->gp, (u32)ctx->sp);
    osSyncPrintf("S8:%08xH   RA:%08xH   LO:%08xH\n", (u32)ctx->s8, (u32)ctx->ra, (u32)ctx->lo);
    osSyncPrintf("\n");

    PrintFPUFlagsOnIndy(ctx->fpcsr);
    osSyncPrintf("\n");

    PrintFPURegsOnIndy(0, &ctx->fp0.f.f_even);
    PrintFPURegsOnIndy(2, &ctx->fp2.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(4, &ctx->fp4.f.f_even);
    PrintFPURegsOnIndy(6, &ctx->fp6.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(8, &ctx->fp8.f.f_even);
    PrintFPURegsOnIndy(10, &ctx->fp10.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(12, &ctx->fp12.f.f_even);
    PrintFPURegsOnIndy(14, &ctx->fp14.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(16, &ctx->fp16.f.f_even);
    PrintFPURegsOnIndy(18, &ctx->fp18.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(20, &ctx->fp20.f.f_even);
    PrintFPURegsOnIndy(22, &ctx->fp22.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(24, &ctx->fp24.f.f_even);
    PrintFPURegsOnIndy(26, &ctx->fp26.f.f_even);
    osSyncPrintf("\n");
    PrintFPURegsOnIndy(28, &ctx->fp28.f.f_even);
    PrintFPURegsOnIndy(30, &ctx->fp30.f.f_even);
    osSyncPrintf("\n");
}

/**
 * Iterates through the active thread queue for a user thread with either
 * the CPU break or Fault flag set.
 */
OSThread* FindFaultedThread(void) {
    OSThread* thread = __osGetActiveQueue();

    // OS_PRIORITY_THREADTAIL indicates the end of the thread queue
    while (thread->priority != OS_PRIORITY_THREADTAIL) {
        if (thread->priority > OS_PRIORITY_IDLE && thread->priority < OS_PRIORITY_APPMAX &&
            (thread->flags & (OS_FLAG_CPU_BREAK | OS_FLAG_FAULT))) {
            return thread;
        }
        thread = thread->tlnext;
    }
    return NULL;
}

void KeyWaitAuto(void) {
    s32 pad;
    OSTime start = osGetTime();

    do {
        WaitTime(1000 / 60);
    } while ((osGetTime() - start) <= OS_USEC_TO_CYCLES(5000000)); // 5 seconds

    this->autoScroll = true;
}

/**
 * Waits for the following button combination to be entered before returning:
 *
 * (L & R & Z) + DPad-Up + C-Down + C-Up + DPad-Down + DPad-Left + C-Left + C-Right + DPad-Right + (B & A & START)
 */
void KeyWait(void) {
    Input* input = &this->inputs[0];
    s32 state;
    u32 s1;
    u32 s2;
    u32 pressedBtn;
    u32 curBtn;

    if (1) {}
    if (1) {}

    // "KeyWaitB (L R Z Up Down Up Down Left Left Right Right B A START)"
    osSyncPrintf(VT_FGCOL(WHITE) T("KeyWaitB (ＬＲＺ ", "KeyWaitB (L R Z ") VT_FGCOL(WHITE) T("上", "Up ")
                     VT_FGCOL(YELLOW) T("下 ", "Down ") VT_FGCOL(YELLOW) T("上", "Up ") VT_FGCOL(WHITE)
                         T("下 ", "Down ") VT_FGCOL(WHITE) T("左", "Left ") VT_FGCOL(YELLOW) T("左 ", "Left ")
                             VT_FGCOL(YELLOW) T("右", "Right ") VT_FGCOL(WHITE) T("右 ", "Right ") VT_FGCOL(GREEN)
                                 T("Ｂ", "B ") VT_FGCOL(BLUE) T("Ａ", "A ")
                                     VT_FGCOL(RED) "START" VT_FGCOL(WHITE) ")" VT_RST "\n");
    // "KeyWaitB'(L R Left Right +START)"
    osSyncPrintf(VT_FGCOL(WHITE) T("KeyWaitB'(ＬＲ左", "KeyWaitB'(L R Left ") VT_FGCOL(YELLOW) T("右 +", "Right +")
                     VT_FGCOL(RED) "START" VT_FGCOL(WHITE) ")" VT_RST "\n");

    faultprint_SetForegroundColor(GPACK_RGBA5551(255, 255, 255, 1));
    faultprint_SetBackgroundColor(GPACK_RGBA5551(0, 0, 0, 1));

    state = 0;
    s1 = 0;
    s2 = 1;

    while (state != 11) {
        WaitTime(1000 / 60);
        fault_get_pads();

        pressedBtn = input->press.button;
        curBtn = input->cur.button;

        if (curBtn == 0 && s1 == s2) {
            s1 = 0;
        } else if (pressedBtn != 0) {
            if (s1 == s2) {
                state = 0;
            }

            switch (state) {
                case 0:
                    if (curBtn == (BTN_Z | BTN_L | BTN_R) && pressedBtn == BTN_Z) {
                        state = s2;
                        s1 = s2;
                    }
                    break;
                case 1:
                    if (pressedBtn == BTN_DUP) {
                        state = 2;
                    } else {
                        state = 0;
                    }
                    break;
                case 2:
                    if (pressedBtn == BTN_CDOWN) {
                        state = 3;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 3:
                    if (pressedBtn == BTN_CUP) {
                        state = 4;
                    } else {
                        state = 0;
                    }
                    break;
                case 4:
                    if (pressedBtn == BTN_DDOWN) {
                        state = 5;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 5:
                    if (pressedBtn == BTN_DLEFT) {
                        state = 6;
                    } else {
                        state = 0;
                    }
                    break;
                case 6:
                    if (pressedBtn == BTN_CLEFT) {
                        state = 7;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 7:
                    if (pressedBtn == BTN_CRIGHT) {
                        state = 8;
                    } else {
                        state = 0;
                    }
                    break;
                case 8:
                    if (pressedBtn == BTN_DRIGHT) {
                        state = 9;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 9:
                    if (pressedBtn == (BTN_A | BTN_B)) {
                        state = 10;
                    } else if (pressedBtn == BTN_A) {
                        state = 0x5B;
                    } else if (pressedBtn == BTN_B) {
                        state = 0x5C;
                    } else {
                        state = 0;
                    }
                    break;
                case 0x5B:
                    if (pressedBtn == BTN_B) {
                        state = 10;
                    } else {
                        state = 0;
                    }
                    break;
                case 0x5C:
                    if (pressedBtn == BTN_A) {
                        state = 10;
                    } else {
                        state = 0;
                    }
                    break;
                case 10:
                    if (pressedBtn == BTN_START) {
                        state = 11;
                    } else {
                        state = 0;
                    }
                    break;
            }
        }

        osWritebackDCacheAll();
    }
}

void PrintDump(const char* title, uintptr_t addr, u32 arg2) {
    uintptr_t alignedAddr = addr;
    u32* writeAddr;
    s32 y;
    s32 x;

    // Ensure address is within the bounds of RDRAM (InteractiveDump has already done this)
    if (alignedAddr < K0BASE) {
        alignedAddr = K0BASE;
    }
    // 8MB RAM, leave room to display 0x100 bytes on the final page
    //! @bug The loop below draws 22 * 4 * 4 = 0x160 bytes per page. Due to this, by scrolling further than
    //! 0x807FFEA0 some invalid bytes are read from outside of 8MB RDRAM space. This does not cause a crash,
    //! however the values it displays are meaningless. On N64 hardware these invalid addresses are read as 0.
    if (alignedAddr > K0BASE + 0x800000 - 0x100) {
        alignedAddr = K0BASE + 0x800000 - 0x100;
    }

    // Ensure address is word-aligned
    alignedAddr &= ~3;
    writeAddr = (u32*)alignedAddr;

    // Reset screen
    ClearScreen();
    faultprint_SetMargin(-2, 0);

    faultprint_PosPrintf(36, 18, "%s %08x", title != NULL ? title : "PrintDump", alignedAddr);

    // Draw memory page contents
    if (alignedAddr >= K0BASE && alignedAddr < K2BASE) {
        for (y = 0; y < 22; y++) {
            faultprint_PosPrintf(24, 28 + y * 9, "%06x", writeAddr);
            for (x = 0; x < 4; x++) {
                faultprint_PosPrintf(82 + x * 52, 28 + y * 9, "%08x", *writeAddr++);
            }
        }
    }

    faultprint_SetMargin(0, 0);
}

/**
 * Draws the memory dump page.
 *
 * DPad-Up scrolls up.
 * DPad-Down scrolls down.
 * Holding Z while scrolling speeds up scrolling by a factor of 0x10.
 * Holding B while scrolling speeds up scrolling by a factor of 0x100.
 *
 * L toggles auto-scrolling pages.
 * START and A move on to the next page.
 *
 * @param pc Program counter, pressing C-Up jumps to this address
 * @param sp Stack pointer, pressing C-Down jumps to this address
 * @param cLeftJump Unused parameter, pressing C-Left jumps to this address
 * @param cRightJump Unused parameter, pressing C-Right jumps to this address
 */
void InteractiveDump(uintptr_t pc, uintptr_t sp, uintptr_t cLeftJump, uintptr_t cRightJump) {
    Input* input = &this->inputs[0];
    uintptr_t addr = pc;
    s32 scrollCountdown;
    u32 off;

    do {
        scrollCountdown = 0;
        // Ensure address is within the bounds of RDRAM
        if (addr < K0BASE) {
            addr = K0BASE;
        }
        // 8MB RAM, leave room to display 0x100 bytes on the final page
        if (addr > K0BASE + 0x800000 - 0x100) {
            addr = K0BASE + 0x800000 - 0x100;
        }

        // Align the address to 0x10 bytes and draw the page contents
        addr &= ~0xF;
        PrintDump("Dump", addr, 0);
        scrollCountdown = 600;

        while (this->autoScroll) {
            // Count down until it's time to move on to the next page
            if (scrollCountdown == 0) {
                return;
            }

            scrollCountdown--;
            WaitTime(1000 / 60);
            fault_get_pads();
            if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
                // Disable auto-scrolling
                this->autoScroll = false;
            }
        }

        // Wait for input
        do {
            WaitTime(1000 / 60);
            fault_get_pads();
        } while (input->press.button == 0);

        // Move to next page
        //! @bug DPad-Right does not move to the next page, unlike when on any other page
        // START moving to the next page is unique to this page.
        if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->cur.button, BTN_A)) {
            return;
        }

        // Memory dump controls

        off = 0x10;
        if (CHECK_BTN_ALL(input->cur.button, BTN_Z)) {
            off *= 0x10;
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_B)) {
            off *= 0x100;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            addr -= off;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            addr += off;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
            addr = pc;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            addr = sp;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
            addr = cLeftJump;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
            addr = cRightJump;
        }
    } while (!CHECK_BTN_ALL(input->press.button, BTN_L));

    // Resume auto-scroll and move to next page
    this->autoScroll = true;
}

/**
 * Searches a single function's stack frame for the function it was called from.
 * There are two cases that must be covered: Leaf and non-leaf functions.
 *
 * A leaf function is one that does not call any other function, in this case the
 * return address need not be saved to the stack. Since a leaf function does not
 * call other functions, only the function the stack trace begins in could possibly
 * be a leaf function, in which case the return address is in the thread context's
 * $ra already, as it never left.
 *
 * The procedure is therefore
 *  - Iterate instructions
 *  - Once jr $ra is found, set pc to $ra
 *  - Done after delay slot
 *
 * A non-leaf function calls other functions, it is necessary for the return address
 * to be saved to the stack. In these functions, it is important to keep track of the
 * stack frame size of each function.
 *
 * The procedure is therefore
 *  - Iterate instructions
 *  - If lw $ra <imm>($sp) is found, fetch the saved $ra from stack memory
 *  - If addiu $sp, $sp, <imm> is found, modify $sp by the immediate value
 *  - If jr $ra is found, set pc to $ra
 *  - Done after delay slot
 *
 * Note that searching for one jr $ra is sufficient, as only leaf functions can have
 * multiple jr $ra in the same function.
 *
 * There is also additional handling for eret and j. Neither of these instructions
 * appear in IDO compiled C, however do show up in the exception handler. It is not
 * possible to backtrace through an eret as an interrupt can occur at any time, so
 * there is no choice but to give up here. For j instructions, they can be followed
 * and the backtrace may continue as normal.
 */
void PrintStackTrace3(uintptr_t* spPtr, uintptr_t* pcPtr, uintptr_t* raPtr) {
    uintptr_t sp = *spPtr;
    uintptr_t pc = *pcPtr;
    uintptr_t ra = *raPtr;
    s32 count = 0x10000; // maximum number of instructions to search through
    u32 lastInsn;
    u32 insn;
    u16 insnHi;
    s16 insnLo;
    u32 imm;

    // ensure $sp and $ra are aligned and valid pointers, if they aren't a stack
    // trace cannot be generated
    if (sp % 4 != 0 || !IS_KSEG0(sp) || ra % 4 != 0 || !IS_KSEG0(ra)) {
        *raPtr = *pcPtr = *spPtr = 0;
        return;
    }

    // ensure pc is aligned and a valid pointer, if not a stack trace cannot
    // be generated
    if (pc % 4 != 0 || !IS_KSEG0(pc)) {
        *pcPtr = ra;
        return;
    }

    lastInsn = 0;
    while (true) {
        insn = *(u32*)K0_TO_K1(pc);
        insnHi = insn >> 16;
        insnLo = insn & 0xFFFF;
        imm = insnLo;

        if (insnHi == 0x8FBF) {
            // lw $ra, <imm>($sp)
            // read return address saved on the stack
            ra = *(uintptr_t*)K0_TO_K1(sp + imm);
        } else if (insnHi == 0x27BD) {
            // addiu $sp, $sp, <imm>
            // stack pointer increment or decrement
            sp += imm;
        } else if (insn == 0x42000018) {
            // eret
            // cannot backtrace through an eret, give up
            ra = pc = sp = 0;
            goto done;
        }
        if (lastInsn == 0x03E00008) {
            // jr $ra
            // return to previous function
            pc = ra;
            goto done;
        } else if (lastInsn >> 26 == 2) {
            // j <target>
            // extract jump target
            pc = pc >> 28 << 28 | lastInsn << 6 >> 4;
            goto done;
        }

        lastInsn = insn;
        pc += sizeof(u32);
        if (count == 0) {
            break;
        }
        count--;
    }
    // Hit the maximum number of instructions to search, give up
    ra = pc = sp = 0;

done:
    *spPtr = sp;
    *pcPtr = pc;
    *raPtr = ra;
}

/**
 * Draws the stack trace page contents for the specified thread
 */
void fault_PrintStackTrace(OSThread* thread, s32 x, s32 y, s32 height) {
    s32 line;
    uintptr_t sp = thread->context.sp;
    uintptr_t ra = thread->context.ra;
    uintptr_t pc = thread->context.pc;
    uintptr_t addr;

    faultprint_PosPrintf(x, y, "SP       PC       (VPC)");

    // Backtrace from the current function to the start of the thread
    for (line = 1; line < height && (ra != 0 || sp != 0) && pc != (uintptr_t)__osCleanupThread; line++) {
        faultprint_PosPrintf(x, y + line * 8, "%08x %08x", sp, pc);
        // Convert relocated address to virtual address if applicable
        addr = fault_AddressConvert(pc);
        if (addr != 0) {
            faultprint_Printf(" -> %08x", addr);
        }
        // Search one function for the previous function
        PrintStackTrace3(&sp, &pc, &ra);
    }
}

void PrintStackTraceOnIndy(OSThread* thread, s32 height) {
    s32 line;
    uintptr_t sp = thread->context.sp;
    uintptr_t ra = thread->context.ra;
    uintptr_t pc = thread->context.pc;
    uintptr_t addr;
    s32 pad;

    osSyncPrintf("STACK TRACE\nSP       PC       (VPC)\n");
    for (line = 1; line < height && (ra != 0 || sp != 0) && pc != (uintptr_t)__osCleanupThread; line++) {
        osSyncPrintf("%08x %08x", sp, pc);
        addr = fault_AddressConvert(pc);
        if (addr != 0) {
            osSyncPrintf(" -> %08x", addr);
        }
        osSyncPrintf("\n");
        PrintStackTrace3(&sp, &pc, &ra);
    }
}

void DebugBreak(OSThread* thread) {
    thread->context.cause = 0;
    thread->context.fpcsr = 0;
    thread->context.pc += sizeof(u32);
    *(u32*)thread->context.pc = 0x0000000D; // write in a break instruction
    osWritebackDCache((void*)thread->context.pc, 4);
    osInvalICache((void*)thread->context.pc, 4);
    osStartThread(thread);
}

void SetUpFrameBuffer(void) {
    void* fb;

    osViSetYScale(1.0f);
    osViSetMode(&osViModeNtscLan1);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_DITHER_FILTER_ON);
    osViBlack(false);

    if (this->fb != NULL) {
        fb = this->fb;
    } else {
        fb = osViGetNextFramebuffer();
        if ((uintptr_t)fb == K0BASE) {
            fb = (void*)(PHYS_TO_K0(osMemSize) - sizeof(u16[SCREEN_HEIGHT][SCREEN_WIDTH]));
        }
    }

    osViSwapBuffer(fb);
    faultprint_SetFrameBuffer(fb, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/**
 * Runs all registered fault clients. Each fault client displays a page
 * on the crash screen.
 */
void fault_CallBackFunc(void) {
    FaultClient* client = this->clients;
    s32 idx = 0;

    while (client != NULL) {
        if (client->callback != NULL) {
            ClearScreen();
            faultprint_SetMargin(-2, 0);
            faultprint_Printf(FAULT_COLOR(DARK_GRAY) "CallBack (%d) %08x %08x %08x\n" FAULT_COLOR(WHITE), idx++, client,
                         client->arg0, client->arg1);
            faultprint_SetMargin(0, 0);
            fault_callfunc(client->callback, client->arg0, client->arg1);
            fault_KeyWait();
            SetUpFrameBuffer();
        }
        client = client->next;
    }
}

void DebugRetrace(void) {
    fault_get_pads();
}

#define FAULT_MSG_CPU_BREAK ((OSMesg)1)
#define FAULT_MSG_FAULT ((OSMesg)2)
#define FAULT_MSG_UNK ((OSMesg)3)

void DebugProcess(void* arg) {
    OSMesg msg;
    OSThread* faultedThread;
    s32 pad;

    // Direct OS event messages to the fault event queue
    osSetEventMesg(OS_EVENT_CPU_BREAK, &this->queue, FAULT_MSG_CPU_BREAK);
    osSetEventMesg(OS_EVENT_FAULT, &this->queue, FAULT_MSG_FAULT);

    while (true) {
        do {
            // Wait for a thread to hit a fault
            osRecvMesg(&this->queue, &msg, OS_MESG_BLOCK);

            if (msg == FAULT_MSG_CPU_BREAK) {
                this->msgId = (u32)FAULT_MSG_CPU_BREAK;
                osSyncPrintf(T("フォルトマネージャ:OS_EVENT_CPU_BREAKを受信しました\n",
                               "Fault Manager: OS_EVENT_CPU_BREAK received\n"));
            } else if (msg == FAULT_MSG_FAULT) {
                this->msgId = (u32)FAULT_MSG_FAULT;
                osSyncPrintf(
                    T("フォルトマネージャ:OS_EVENT_FAULTを受信しました\n", "Fault Manager: OS_EVENT_FAULT received\n"));
            } else if (msg == FAULT_MSG_UNK) {
                DebugRetrace();
                faultedThread = NULL;
                continue;
            } else {
                this->msgId = (u32)FAULT_MSG_UNK;
                osSyncPrintf(T("フォルトマネージャ:不明なメッセージを受信しました\n",
                               "Fault Manager: Unknown message received\n"));
            }

            faultedThread = __osGetCurrFaultedThread();
            osSyncPrintf("__osGetCurrFaultedThread()=%08x\n", faultedThread);

            if (faultedThread == NULL) {
                faultedThread = FindFaultedThread();
                osSyncPrintf("FindFaultedThread()=%08x\n", faultedThread);
            }
        } while (faultedThread == NULL);

        // Disable floating-point related exceptions
        __osSetFpcCsr(__osGetFpcCsr() & ~(FPCSR_EV | FPCSR_EZ | FPCSR_EO | FPCSR_EU | FPCSR_EI));
        this->faultedThread = faultedThread;

        while (!this->faultHandlerEnabled) {
            WaitTime(1000);
        }
        WaitTime(1000 / 2);

        // Show fault framebuffer
        SetUpFrameBuffer();

        if (this->autoScroll) {
            KeyWaitAuto();
        } else {
            // Draw error bar signifying the crash screen is available
            ColorBox(GPACK_RGBA5551(255, 0, 0, 1));
            KeyWait();
        }

        // Set auto-scrolling and default colors
        this->autoScroll = true;
        faultprint_SetForegroundColor(GPACK_RGBA5551(255, 255, 255, 1));
        faultprint_SetBackgroundColor(GPACK_RGBA5551(0, 0, 0, 0));

        // Draw pages
        do {
            // Thread context page
            PrintDebugInfo(faultedThread);
            PrintDebugInfoOnIndy(faultedThread);
            fault_KeyWait();
            // Stack trace page
            ClearScreen();
            faultprint_PosPrintf(120, 16, "STACK TRACE");
            fault_PrintStackTrace(faultedThread, 36, 24, 22);
            PrintStackTraceOnIndy(faultedThread, 50);
            fault_KeyWait();
            // Client pages
            fault_CallBackFunc();
            // Memory dump page
            InteractiveDump(faultedThread->context.pc - 0x100, (uintptr_t)faultedThread->context.sp, 0, 0);
            // End page
            ClearScreenRed();
            faultprint_PosPrintf(64, 80, "    CONGRATURATIONS!    ");
            faultprint_PosPrintf(64, 90, "All Pages are displayed.");
            faultprint_PosPrintf(64, 100, "       THANK YOU!       ");
            faultprint_PosPrintf(64, 110, " You are great debugger!");
            fault_KeyWait();
        } while (!this->exit);

        while (!this->exit) {}

        DebugBreak(faultedThread);
    }
}

void DbSetFrameBuffer(void* fb, u16 w, u16 h) {
    this->fb = fb;
    faultprint_SetFrameBuffer(fb, w, h);
}

void DbStartDebugger(void) {
    this = &debugger;
    bzero(this, sizeof(FaultMgr));
    faultprint_Initial();
    faultprint_SetNextPageCallback(fault_KeyWait);
    this->exit = false;
    this->msgId = 0;
    this->faultHandlerEnabled = false;
    this->faultedThread = NULL;
    this->padCallback = callback_get_pads_default;
    this->clients = NULL;
    this->autoScroll = false;
    debugger.faultHandlerEnabled = true;
    osCreateMesgQueue(&this->queue, &this->msg, 1);
    stackcheck_init(&fault_stackcheck, faultStack, STACK_TOP(faultStack), 0, 0x100, "fault");
    osCreateThread(&this->thread, THREAD_ID_FAULT, DebugProcess, NULL, STACK_TOP(faultStack),
                   THREAD_PRI_FAULT);
    osStartThread(&this->thread);
}

/**
 * Fault page for Hungup crashes. Displays the thread id and two messages
 * specified in arguments to `fault_HungUpMsg`.
 */
void fault_HungUp_print(const char* exp1, const char* exp2) {
    osSyncPrintf("HungUp on Thread %d\n", osGetThreadId(NULL));
    osSyncPrintf("%s\n", exp1 != NULL ? exp1 : "(NULL)");
    osSyncPrintf("%s\n", exp2 != NULL ? exp2 : "(NULL)");
    faultprint_Printf("HungUp on Thread %d\n", osGetThreadId(NULL));
    faultprint_Printf("%s\n", exp1 != NULL ? exp1 : "(NULL)");
    faultprint_Printf("%s\n", exp2 != NULL ? exp2 : "(NULL)");
}

/**
 * Immediately crashes the current thread, for cases where an irrecoverable
 * error occurs. The parameters specify two messages detailing the error, one
 * or both may be NULL.
 */
NORETURN void fault_HungUpMsg(const char* exp1, const char* exp2) {
    FaultClient client;
    s32 pad;

    fault_AddClient(&client, fault_HungUp_print, (void*)exp1, (void*)exp2);
    *(u32*)0x11111111 = 0; // trigger an exception via unaligned memory access

    // Since the above line triggers an exception and transfers execution to the fault handler
    // this function does not return and the rest of the function is unreachable.
    UNREACHABLE();
}

/**
 * Like `fault_HungUpMsg`, however provides a fixed message containing
 * filename and line number
 */
NORETURN void fault_HungUp(const char* file, int line) {
    char msg[256];

    sprintf(msg, "HungUp %s:%d", file, line);
    fault_HungUpMsg(msg, NULL);
}
