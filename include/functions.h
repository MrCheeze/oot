#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "z64.h"
#include "macros.h"

void boot(void);
void mainx(void* arg);
void idleproc(void* arg);
void viBlack(u32 black);
void viRetrace(void);
#if !PLATFORM_IQUE
void slidma(uintptr_t romStart, u8* dst, size_t size);
#else
void gzip_decompress(uintptr_t romStart, u8* dst, size_t size);
#endif
void z_locale_init(void);
void z_locale_cleanup(void);
#if DEBUG_FEATURES
void isPrintfInit(void);
#endif
void rmonPrintf(const char* fmt, ...);
#if DEBUG_FEATURES
void* is_proutSyncPrintf(void* arg, const char* str, size_t count);
NORETURN void func_80002384(const char* exp, const char* file, int line);
#endif
OSPiHandle* osDriveRomInit(void);
void Mio0_Decompress(u8* src, u8* dst);

void flg_set(PlayState* play);
void DLFTBL_link(GameStateOverlay* overlayEntry);
void DLFTBL_unlink(GameStateOverlay* overlayEntry);

void actor_dlftbls_show_info(void);
void actor_dlftbls_init(void);
void actor_dlftbls_cleanup(void);

void z_common_data_init(void);
s32 func_800635D0(s32);
void new_Debug_mode(void);
void Debug_Print_write(u8 x, u8 y, const char* text);
void Debug_Print2_write(u8 x, u8 y, u8 colorIndex, const char* text);
#if DEBUG_FEATURES
void Regs_UpdateEditor(Input* input);
#endif
void Debug_mode_output(GraphicsContext* gfxCtx);

void* Memcpy(void* dest, const void* src, s32 len);

u16 get_sa_message(PlayState* play);
u16 get_elf_message(PlayState* play);
u16 get_mask_message(PlayState* play, u32 maskReactionSet);
void event_ct(PlayState* play);
void eventbit_set(PlayState* play, s16 flag);
void eventbit_reset(PlayState* play, s16 flag);
s32 eventbit_check(PlayState* play, s16 flag);

s32 getkadr(s32 character);
#if PLATFORM_IQUE
void kanfont_get_NESCHN(Font* font, u16 character, u16 codePointIndex);
#endif
void kanfont_get(Font* font, u16 character, u16 codePointIndex);
void kanfont_get_NES(Font* font, u8 character, u16 codePointIndex);
void kanfont_get2(Font* font, u16 icon);
void kscope_kanfont_get(Font* font);

void initial_LifeMeterColorAnimation(PlayState* play);
void LifeMeterColorAnimation(PlayState* play);
void DrawLifeMeter(PlayState* play);
void LifeMeterZoom(PlayState* play);
u32 Life_Caution_Check(void);

void MapMarkInit(PlayState* play);
void MapMarkCleanup(PlayState* play);
void MapMarkDisplay(PlayState* play);
void z_nmibuf_init(PreNmiBuff* this);
void z_nmibuf_prenmi(PreNmiBuff* this);
u32 z_nmibuf_isprenmi(PreNmiBuff* this);
void nulltask(void);

Path* get_path_data(PlayState* play, s16 index, s16 max);
f32 path_move(Actor* actor, Path* path, s16 waypoint, s16* yaw);
void get_path_goal_position(Path* path, Vec3f* dest);

void prenmi_init(GameState* thisx);

void Room_Move(PlayState* play, Room* room, Input* input, s32 arg3);
void prerender_draw2(Gfx** gfxP, void* tex, void* tlut, u16 width, u16 height, u8 fmt, u8 siz, u16 tlutMode,
                           u16 tlutCount, f32 offsetX, f32 offsetY);
void Room_Info_ct(PlayState* play, Room* room);
u32 Room_Info_init(PlayState* play, RoomContext* roomCtx);
s32 Room_Info_exchange_start(PlayState* play, RoomContext* roomCtx, s32 roomNum);
s32 Room_Info_exchange_check(PlayState* play, RoomContext* roomCtx);
void Room_Draw(PlayState* play, Room* room, u32 flags);
void Room_Info_old_room_clear(PlayState* play, RoomContext* roomCtx);
void sample_cleanup(GameState* thisx);
void sample_init(GameState* thisx);

void Skin_Mesh2_setPosCalcNormal(MtxF* mtx, SkinVertex* skinVertices, SkinLimbModif* modifEntry, Vtx* vtxBuf, Vec3f* pos);
void Skin_Mesh2_disp(GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, s32 arg3, s32 drawFlags);
void Skin_gfx_mesh2_disp(GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, Gfx* dlistOverride, s32 drawFlags);
void Skin_disp2(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw, s32 setTranslation);
void Skin_disp3(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation);
void Skin_disp4(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6);
void Skin_disp5(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6, s32 drawFlags);
void Skin_MatrixPosition2_gfx(Skin* skin, s32 limbIndex, Vec3f* offset, Vec3f* dst);
void Skin_AnimationWorkBuffer2_ct(PlayState* play, Skin* skin, SkeletonHeader* skeletonHeader, AnimationHeader* animationHeader);
void Skin_AnimationWorkBuffer2_dt(PlayState* play, Skin* skin);
s32 Skin_AnimationWorkBuffer2_setupData(Skin* skin, MtxF* limbMatrices, Actor* actor, s32 setTranslation);

void save_initialize(void);
void save_initialize999(void);
void sram_load_check(SramContext* sramCtx);
void sram_save(SramContext* sramCtx);
void sram_start_load(FileSelectState* fileSelect, SramContext* sramCtx);
void sram_start_save(FileSelectState* fileSelect, SramContext* sramCtx);
void sram_start_clear(FileSelectState* fileSelect, SramContext* sramCtx);
void sram_start_copy(FileSelectState* fileSelect, SramContext* sramCtx);
void sram_sound_save(SramContext* sramCtx);
void sram_initialize(GameState* gameState, SramContext* sramCtx);
void sram_title_ct(GameState* gameState, SramContext* sramCtx);
void sram_ct(PlayState* play, SramContext* sramCtx);
void ssDMNInitial(s32 addr, u8 handleType, u8 handleDomain, u8 handleLatency, u8 handlePageSize, u8 handleRelDuration,
                 u8 handlePulse, u32 handleSpeed);
void ssDMNReadWrite(void* dramAddr, size_t size, s32 direction);
void ssSRAMReadWrite(s32 addr, void* dramAddr, size_t size, s32 direction);

void disp_vimode(OSViMode* osViMode);
void make_osvimode2(ViMode* viMode, s32 type, s32 tvType, s32 loRes, s32 antialiasOff, s32 modeN, s32 fb16Bit,
                      s32 width, s32 height, s32 leftAdjust, s32 rightAdjust, s32 upperAdjust, s32 lowerAdjust);
void z_vimode_setreg(ViMode* viMode);
void z_vimode_getreg(ViMode* viMode);
void z_vimode_init(ViMode* viMode);
void z_vimode_cleanup(ViMode* viMode);
void z_vimode_SetSpecialFeatures(ViMode* viMode, s32 viFeatures);
void z_vimode_move(ViMode* viMode, Input* input);
void initfunc(void);
void fbdemo_cleanup(TransitionTile* this);
TransitionTile* fbdemo_init(TransitionTile* this, s32 cols, s32 rows);
void fbdemo_draw(TransitionTile* this, Gfx** gfxP);
void fbdemo_move(TransitionTile* this);
void fbdemo_triforce_startup(void* thisx);
void* fbdemo_triforce_init(void* thisx);
void fbdemo_triforce_cleanup(void* thisx);
void fbdemo_triforce_move(void* thisx, s32 updateRate);
void fbdemo_triforce_setcolor_rgba8888(void* thisx, u32 color);
void fbdemo_triforce_settype(void* thisx, s32 type);
void fbdemo_triforce_draw(void* thisx, Gfx** gfxP);
s32 fbdemo_triforce_is_finish(void* thisx);
void fbdemo_wipe1_startup(void* thisx);
void* fbdemo_wipe1_init(void* thisx);
void fbdemo_wipe1_cleanup(void* thisx);
void fbdemo_wipe1_move(void* thisx, s32 updateRate);
void fbdemo_wipe1_draw(void* thisx, Gfx** gfxP);
s32 fbdemo_wipe1_is_finish(void* thisx);
void fbdemo_wipe1_settype(void* thisx, s32 type);
void fbdemo_wipe1_setcolor_rgba8888(void* thisx, u32 color);
void fbdemo_wipe3_startup(void* thisx);
void* fbdemo_wipe3_init(void* thisx);
void fbdemo_wipe3_cleanup(void* thisx);
void fbdemo_wipe3_move(void* thisx, s32 updateRate);
void fbdemo_wipe3_draw(void* thisx, Gfx** gfxP);
s32 fbdemo_wipe3_is_finish(void* thisx);
void fbdemo_wipe3_settype(void* thisx, s32 type);
void fbdemo_wipe3_setcolor_rgba8888(void* thisx, u32 color);
void fbdemo_wipe3_setaltcolor_rgba8888(void* thisx, u32 color);
void fbdemo_fade_startup(void* thisx);
void* fbdemo_fade_init(void* thisx);
void fbdemo_fade_cleanup(void* thisx);
void fbdemo_fade_move(void* thisx, s32 updateRate);
void fbdemo_fade_draw(void* thisx, Gfx** gfxP);
s32 fbdemo_fade_is_finish(void* thisx);
void fbdemo_fade_setcolor_rgba8888(void* thisx, u32 color);
void fbdemo_fade_settype(void* thisx, s32 type);

void DebugCamera_Init(DebugCam* debugCam, Camera* cameraPtr);
void DebugCamera_Enable(DebugCam* debugCam, Camera* cam);
void DebugactionCameraWork(DebugCam* debugCam, Camera* cam);
void DebugCamera_Reset(Camera* cam, DebugCam* debugCam);
void Grou_Bspline(f32 u, Vec3f* pos, f32* roll, f32* viewAngle, f32* point0, f32* point1, f32* point2, f32* point3);
s32 Grou_Dospline(Vec3f* pos, f32* roll, f32* fov, CutsceneCameraPoint* point, s16* keyFrame, f32* curFrame);

s32 Game_play_getDoorAngle(PlayState* this, Actor* actor, s16* yaw);
s32 Gama_play_position_in_water(PlayState* this, Vec3f* pos);
void PreRender_setup_savebuf(PreRender* this, u32 width, u32 height, void* fbuf, void* zbuf, void* cvg);
void PreRender_init(PreRender* this);
void PreRender_setup_renderbuf(PreRender* this, u32 width, u32 height, void* fbuf, void* zbuf);
void PreRender_cleanup(PreRender* this);
void PreRender_TransBuffer1_env(PreRender* this, Gfx** gfxP, void* buf, void* bufSave, u32 r, u32 g, u32 b, u32 a);
void PreRender_TransBuffer1(PreRender* this, Gfx** gfxP, void* fbuf, void* fbufSave);
void PreRender_saveZBuffer(PreRender* this, Gfx** gfxP);
void PreRender_saveFrameBuffer(PreRender* this, Gfx** gfxP);
void PreRender_saveCVG(PreRender* this, Gfx** gfxP);
void PreRender_loadZBuffer(PreRender* this, Gfx** gfxP);
void PreRender_loadFrameBuffer(PreRender* this, Gfx** gfxP);
void PreRender_loadFrameBufferCopy(PreRender* this, Gfx** gfxP);
void PreRender_loadFrameBufferCopyX(PreRender* this, Gfx** gfxP);
void PreRender_ConvertFrameBuffer2(PreRender* this);
void debug_filter(Gfx** gfxP);
void game_debug_draw_last(GameState* gameState, GraphicsContext* gfxCtx);
void game_draw_first(GraphicsContext* gfxCtx);
void game_get_controller(GameState* gameState);
void game_main(GameState* gameState);
void game_init_hyral(GameState* gameState, size_t size);
void game_resize_hyral(GameState* gameState, size_t size);
void game_ct(GameState* gameState, GameStateFunc init, GraphicsContext* gfxCtx);
void game_dt(GameState* gameState);
GameStateFunc game_get_next_game_init(GameState* gameState);
u32 game_is_doing(GameState* gameState);
#if DEBUG_FEATURES
void* GameState_Alloc(GameState* gameState, size_t size, const char* file, int line);
void* gamealloc_mallocDebug(GameAlloc* this, u32 size, const char* file, int line);
#endif
void* gamealloc_malloc(GameAlloc* this, u32 size);
void gamealloc_free(GameAlloc* this, void* data);
void gamealloc_cleanup(GameAlloc* this);
void gamealloc_init(GameAlloc* this);
void graph_setup_double_buffer(GraphicsContext* gfxCtx);
GameStateOverlay* game_get_next_game_dlftbl(GameState* gameState);
void graph_ct(GraphicsContext* gfxCtx);
void graph_dt(GraphicsContext* gfxCtx);
void graph_task_set00(GraphicsContext* gfxCtx);
void graph_main(GraphicsContext* gfxCtx, GameState* gameState);
void graph_proc(void*);

ListAlloc* listalloc_init(ListAlloc* this);
void* listalloc_malloc(ListAlloc* this, u32 size);
void listalloc_free(ListAlloc* this, void* data);
void listalloc_cleanup(ListAlloc* this);
void mainproc(void* arg);
void sys_cfb_init(s32 n64dd);
void* sys_cfb_getptr(s32 idx);
void* sys_cfb_get_bottom(void);

u64* ucode_GetRspBootTextStart(void);
size_t ucode_GetRspBootTextSize(void);
u64* ucode_GetPolyTextStart(void);
u64* ucode_GetPolyDataStart(void);
NORETURN void Freeze(void);
void func_800D31F0(void);
void func_800D3210(void);
void* DebugArena_Malloc(u32 size);
void* DebugArena_MallocR(u32 size);
void* DebugArena_Realloc(void* ptr, u32 newSize);
void DebugArena_Free(void* ptr);
void* DebugArena_Calloc(u32 num, u32 size);
void DebugArena_GetSizes(u32* outMaxFree, u32* outFree, u32* outAlloc);
void DebugArena_Check(void);
void DebugArena_Init(void* start, u32 size);
void DebugArena_Cleanup(void);
s32 DebugArena_IsInitialized(void);
#if DEBUG_FEATURES
void DebugArena_CheckPointer(void* ptr, u32 size, const char* name, const char* action);
void* DebugArena_MallocDebug(u32 size, const char* file, int line);
void* DebugArena_MallocRDebug(u32 size, const char* file, int line);
void* DebugArena_ReallocDebug(void* ptr, u32 newSize, const char* file, int line);
void DebugArena_FreeDebug(void* ptr, const char* file, int line);
void DebugArena_Display(void);
#endif

void showRspRdp(void);
void KillRspRdp(void);
void* LoadFragment2(uintptr_t vromStart, uintptr_t vromEnd, void* vramStart, void* vramEnd);
void MtxConv_F2L(Mtx* m1, MtxF* m2);
void MtxConv_L2F(MtxF* m1, Mtx* m2);
void DoRelocation(void* allocatedRamAddr, OverlayRelocationSection* ovlRelocs, void* vramStart);
size_t LoadFragmentFix2(uintptr_t vromStart, uintptr_t vromEnd, void* vramStart, void* vramEnd, void* allocatedRamAddr);
// ? __nw__FUi(?);
// ? __dl__FPv(?);
// ? func_800FCAB4(?);
void osInitializeCPP(void* start, u32 size);

f32 absf(f32);

void save_area_ct(PlayState* play);

void first_game_init(GameState* thisx);
void first_game_cleanup(GameState* thisx);
void title_init(GameState* thisx);
void title_cleanup(GameState* thisx);
void select_init(GameState* thisx);
void select_cleanup(GameState* thisx);
void opening_init(GameState* thisx);
void opening_cleanup(GameState* thisx);
void file_choose_init(GameState* thisx);
void file_choose_cleanup(GameState* thisx);

#endif
