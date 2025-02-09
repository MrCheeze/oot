#ifndef VARIABLES_H
#define VARIABLES_H

#include "z64.h"
#include "libc64/os_malloc.h"
#include "segment_symbols.h"

extern Mtx D_01000000;

extern void* osRomBase;
extern s32 osTvType;
extern u32 osRomType;
extern u32 osVersion;
extern s32 osResetType;
extern s32 osCicId;
extern u32 osMemSize;
extern u8 osAppNMIBuffer[0x40];

extern u32 __osBbIsBb;
extern u32 __osBbEepromSize;
extern u32 __osBbPakSize;
extern u32 __osBbFlashSize;
extern u32 __osBbEepromAddress;
extern u32 __osBbPakAddress[4];
extern u32 __osBbFlashAddress;
extern u32 __osBbSramSize;
extern u32 __osBbSramAddress;
extern u32 __osBbHackFlags;

extern s8 vidirty;
extern vu8 __viblack;
extern u8 viextendvstart;
extern u32 vispecial;
extern f32 vixscale;
extern f32 viyscale;
extern OSPiHandle* carthandle;
extern s32 z_locale_mode;
extern u32 __osPiAccessQueueEnabled;
extern OSViMode osViModePalLan1;
extern s32 osViClock;
extern u32 __osShutdown;
extern OSHWIntr __OSGlobalIntMask;
extern __OSThreadTail __osThreadTail;
extern OSThread* __osRunQueue;
extern OSThread* __osActiveQueue;
extern OSThread* __osRunningThread;
extern OSThread* __osFaultedThread;
extern OSPiHandle* __osPiTable;
extern OSPiHandle* __osCurrentHandle[];
extern OSTimer* __osTimerList;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeMpalLan1;
extern OSViContext* __osViCurr;
extern OSViContext* __osViNext;
extern OSViMode osViModeFpalLan1;
extern u32 __additional_scanline;
extern const char __Creator__[];
extern const char __DateTime__[];
extern const char __MakeOption__[];
extern OSMesgQueue _PiMsgQ;
extern OSViMode vimode;
extern u8 vimode_no;
extern OSMesgQueue __osPiAccessQueue;
extern OSPiHandle __Dom1SpeedParam;
extern OSPiHandle __Dom2SpeedParam;
extern OSTime __osCurrentTime;
extern u32 __osBaseCounter;
extern u32 __osViIntrCount;
extern u32 __osTimerCounter;
extern EffectSsOverlay effect_ss2_dlftbls[EFFECT_SS_TYPE_MAX];
extern Gfx Actor_change_render_mode[];
extern ActorOverlay actor_dlftbls[ACTOR_ID_MAX]; // original name: "actor_dlftbls" 801162A0
extern s32 actor_dlftbls_num; // original name: "MaxProfile"
extern s32 debug_camera_sw;
extern GameStateOverlay game_dlftbls[GAMESTATE_ID_MAX];
extern s32 gZeldaArenaLogSeverity;
extern MapData rom_map_exp_data_tbl;
extern s16 Warashibe_item[3];
extern s16 Warashibe_before_item[3];

extern Gfx dl_noop[];

extern u16 sram_save_address[];
// 4 16-colors palettes
extern u64 gMojiFontTLUTs[4][4]; // original name: "moji_tlut"
extern u64 gMojiFontTex[]; // original name: "font_ff"
extern u8 LargeMapMark_MarkScaleMode;

#if DEBUG_FEATURES
extern u32 gIsCtrlr2Valid;
#endif
extern s16* WAVEMEM_TABLE[9];
extern f32 PCENTTABLE[256];
extern f32 PCENTTABLE2[256];
extern f32 PITCHTABLE[];
extern u8 DEFAULT_VTABLE[16];
extern u8 DEFAULT_GTABLE[16];
extern EnvelopePoint DEFAULT_ENV[4];
extern NoteSubEu NA_CHINIT_TABLE;
extern NoteSubEu NA_SVCINIT_TABLE;
extern u16 CDELAYTABLE[64];
extern s16 NOISEBOOK[];
extern f32 PhoneLeft[128];
extern f32 WideLeft[128];
extern f32 StereoLeft[128];
extern s16 LSF_TABLE[16 * 8];
extern s16 HSF_TABLE[15 * 8];
extern s32 AUDIO_SYSTEM_READY;
extern u8 _se_handle_extend_senum[7];
extern u8 _se_handle_sounds[4][7];
extern u8 _se_handle_objsounds[4][7];
extern u8 na_mofer_picth_up_table[16];
extern u8* na_frog_tbl_ptr;
extern OcarinaNote* na_oca_rec_buf_ptr;
extern u8* na_oca_rec_buf2_ptr;
extern OcarinaSongButtons na_oca_note[];
extern SfxParams* se_indi_info_adrs[7];
extern char D_80133390[];
extern char D_80133398[];
extern u8 _se_ent_writepoint;
extern u8 _se_ent_readpoint;
extern SfxBankEntry* EntrySeStatus[7];
extern u8 SeEntryBufMax[];
extern u8 se_scene_mode;
extern u16 se_bgm_mute_flag;
extern Vec3f _dummy_zero_f;
extern f32 _dummy_one;
extern s8 _dummy_zero_s8;
#if DEBUG_FEATURES
extern u8 D_801333F0;
extern u8 gAudioSfxSwapOff;
extern u8 D_801333F8;
#endif
extern u8 SeqFlagWritePoint;
extern u8 SeqFlagReadPoint;
extern u8 SeqStartCancel;
#if DEBUG_FEATURES
extern u8 gAudioDebugPrintSeqCmd;
#endif
extern u8 SndOutTbl[];
extern u8 SndSpecNum;
extern u8 SpecBusyFlag;
extern AudioSpec NA_SPEC_CONFIG[18];
extern s32 loadfragment_verbose;
extern s32 gSystemArenaLogSeverity;
extern u8 __osPfsInodeCacheBank;
extern s32 __osPfsLastChannel;

extern TempoData AGC;
extern AudioHeapInitSizes gAudioHeapInitSizes;
extern s16 ocarina_number[];
extern AudioTable AudiobankHeaderStart;
extern u8 AudiomapHeaderStart[];
extern u8 AudioseqHeaderStart[];
extern AudioTable AudiowaveHeaderStart;

extern u8 DEMOCAM_SW;
extern u16 E_demosetflg;
extern char E_demokanri[20];
extern u8 E_demo_no;
extern u16 start_flame_bak2;
extern u16 start_flame_bak3;
extern u16 start_flame_bak4;

extern LightningStrike Eleki;
// TODO: These variables are here for BSS ordering but ideally they should not
// be extern. This could be fixed by putting more stuff (e.g. struct definitions)
// between Eleki and E_lf_flag.
extern s16 E_eleki_alpha;
extern s16 E_lens_x;
extern s16 E_lens_y;
extern u8 E_lf_flag;
extern Vec3f E_lf_position;
extern s16 E_lf_wscale;
extern f32 E_lf_alpha;
extern s16 E_lf_rectalpha;
extern MapData* map_exp_data_tbl_p;
extern f32 LargeMapMark_MarkScale;
extern u32 LargeMapMark_NeedDraw;
extern PauseMapMarksData* LargeMapMark_lmarkpos_p;

extern PreNmiBuff* z_nmibuf;
extern Scheduler _sched;
extern PadMgr padmgr;
extern IrqMgr _irqmgr;
extern volatile OSTime audio_cpu_time;
extern volatile OSTime graph_cpu_time;
extern volatile OSTime audio_rsp_time;
extern volatile OSTime graph_rsp_time;
extern volatile OSTime rdp_time;
extern volatile OSTime frame_time;
extern volatile OSTime audio_cpu_start_time;
extern volatile OSTime audio_cpu_sum_time;
extern volatile OSTime audio_rsp_sum_time;
extern volatile OSTime graph_rsp_sum_time;
extern volatile OSTime other_rsp_sum_time;
extern volatile OSTime other_rsp_time;
extern volatile OSTime rdp_sum_time;

extern SfxBankEntry EntrySeHandle_0[9];
extern SfxBankEntry EntrySeHandle_1[12];
extern SfxBankEntry EntrySeHandle_2[22];
extern SfxBankEntry EntrySeHandle_3[20];
extern SfxBankEntry EntrySeHandle_4[8];
extern SfxBankEntry EntrySeHandle_5[3];
extern SfxBankEntry EntrySeHandle_6[5];
extern ActiveSfx play_next[7][MAX_CHANNELS_PER_BANK]; // total size = 0xA8
extern u8 se_handle_lock_flag[];
extern u16 gAudioSfxSwapSource[10];
extern u16 gAudioSfxSwapTarget[10];
extern u8 gAudioSfxSwapMode[10];
extern ActiveSequence SeqInterWork[4];
extern AudioContext AG;
extern AudioCustomUpdateFunction NA_VFRAME_CALLBACK;

extern OSPifRam __osContPifRam;
extern u8 __osContLastCmd;
extern u8 __osMaxControllers;
extern __OSInode __osPfsInodeCache;
extern OSPifRam __osPfsPifRam;
extern u16 sys_zb[SCREEN_HEIGHT][SCREEN_WIDTH]; // 0x25800 bytes
extern u64 sys_fifo_buffer[0x3000]; // 0x18000 bytes
extern u64 sys_yield_buffer[OS_YIELD_DATA_SIZE / sizeof(u64)]; // 0xC00 bytes
extern u64 sys_dram_stack[SP_DRAM_STACK_SIZE64]; // 0x400 bytes
extern GfxPool sys_dynamic[2]; // 0x24820 bytes
extern u8 AUDIOHP[0x38000]; // 0x38000 bytes

#endif
