#ifndef FAULT_H
#define FAULT_H

#include "ultra64.h"
#include "attributes.h"
#include "padmgr.h"

#if !PLATFORM_N64
// These are the same as the 3-bit ansi color codes
#define FAULT_COLOR_BLACK      0
#define FAULT_COLOR_RED        1
#define FAULT_COLOR_GREEN      2
#define FAULT_COLOR_YELLOW     3
#define FAULT_COLOR_BLUE       4
#define FAULT_COLOR_MAGENTA    5
#define FAULT_COLOR_CYAN       6
#define FAULT_COLOR_WHITE      7
// Additional color codes
#define FAULT_COLOR_DARK_GRAY  8
#define FAULT_COLOR_LIGHT_GRAY 9

#define FAULT_COLOR_STRINGIFY(s) #s
#define FAULT_COLOR_EXPAND_AND_STRINGIFY(s) FAULT_COLOR_STRINGIFY(s)

#define FAULT_ESC '\x1A'
#define FAULT_COLOR(n) "\x1A" FAULT_COLOR_EXPAND_AND_STRINGIFY(FAULT_COLOR_ ## n)
#endif

typedef struct FaultClient {
    /* 0x00 */ struct FaultClient* next;
    /* 0x04 */ void* callback;
    /* 0x08 */ void* arg0;
    /* 0x0C */ void* arg1;
} FaultClient; // size = 0x10

#if !PLATFORM_N64
typedef struct FaultAddrConvClient {
    /* 0x00 */ struct FaultAddrConvClient* next;
    /* 0x04 */ void* callback;
    /* 0x08 */ void* arg;
} FaultAddrConvClient; // size = 0xC
#endif

// Initialization

void DbStartDebugger(void);

// Fatal Errors

NORETURN void fault_HungUpMsg(const char* exp1, const char* exp2);
NORETURN void fault_HungUp(const char* file, int line);

// Client Registration

void fault_AddClient(FaultClient* client, void* callback, void* arg0, void* arg1);
void fault_RemoveClient(FaultClient* client);

#if PLATFORM_GC
void fault_AddressConverterAddClient(FaultAddrConvClient* client, void* callback, void* arg);
void fault_AddressConverterRemoveClient(FaultAddrConvClient* client);
#endif

// For use in Fault Client callbacks

void DbSetFrameBuffer(void* fb, u16 w, u16 h);

void fault_KeyWait(void);

void faultprint_Locate(s32 x, s32 y);
s32 faultprint_Printf(const char* fmt, ...);
void faultprint_PosPrintf(s32 x, s32 y, const char* fmt, ...);

#if PLATFORM_N64

void func_800AE1F8(void);

// Not implemented. Silently noop-ing is fine, these are not essential for functionality.
#define faultprint_Color(color) (void)0
#define faultprint_SetMargin(padW, padH) (void)0

#else

void faultprint_Initial(void);
void faultprint_SetForegroundColor(u16 color);
void faultprint_SetBackgroundColor(u16 color);
void faultprint_Color(u16 color);
void faultprint_SetMargin(s8 padW, s8 padH);
s32 faultprint_Vprintf(const char* fmt, va_list args);

#endif

#if PLATFORM_N64

extern vs32 gFaultExit;
extern vs32 gFaultMsgId;
extern vs32 gFaultDisplayEnable;
extern volatile OSThread* gFaultFaultedThread;

#define FAULT_MSG_ID gFaultMsgId

#else

typedef struct FaultMgr {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char unk_1B0[0x600];
    /* 0x7B0 */ OSMesgQueue queue;
    /* 0x7C8 */ OSMesg msg;
    /* 0x7CC */ u8 exit;
    /* 0x7CD */ u8 msgId;
    /* 0x7CE */ u8 faultHandlerEnabled;
    /* 0x7CF */ u8 autoScroll;
    /* 0x7D0 */ OSThread* faultedThread;
    /* 0x7D4 */ void (*padCallback)(Input* inputs);
    /* 0x7D8 */ FaultClient* clients;
    /* 0x7DC */ FaultAddrConvClient* addrConvClients;
    /* 0x7E0 */ char unk_7E0[0x4];
    /* 0x7E4 */ Input inputs[MAXCONTROLLERS];
    /* 0x844 */ u16* fb;
    /* 0x848 */ void* clientThreadSp;
} FaultMgr; // size = 0x850

extern FaultMgr debugger;

#define FAULT_MSG_ID debugger.msgId

#endif

#endif
