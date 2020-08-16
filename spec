/*
 * ROM spec file
 */

#define SEGMENT_SCENE 0x02000000
#define SEGMENT_ROOM 0x03000000

beginseg
    name "makerom"
    include "build/asm/rom_header.o"
    include "build/asm/ipl3.o"
    include "build/asm/entry.o"
endseg

beginseg
    name "boot"
    address 0x80000460
    include "build/src/boot/boot_main.o"
    include "build/src/boot/idle.o"
    include "build/src/boot/viconfig.o"
    include "build/src/boot/z_std_dma.o"
    include "build/src/boot/yaz0.o"
    include "build/src/boot/z_locale.o"
    include "build/src/boot/assert.o"
    include "build/src/boot/is_debug.o"
    include "build/src/libultra_boot_O2/osDriveRomInit.o"
    include "build/asm/mio0.o"
    include "build/src/boot/stackcheck.o"
    include "build/src/boot/logutils.o"
    include "build/src/libultra_boot_O2/sprintf.o"
    include "build/src/libultra_boot_O1/piacs.o"
    include "build/src/libultra_boot_O1/osSendMesg.o"
    include "build/src/libultra_boot_O1/osStopThread.o"
    include "build/src/libultra_boot_O1/osViExtendVStart.o"
    include "build/src/libultra_boot_O1/osViModePalLan1.o"
    include "build/src/libultra_boot_O1/osRecvMesg.o"
    include "build/src/libultra_boot_O1/initialize.o"
    include "build/asm/libm.o"
    include "build/asm/exceptasm.o"
    include "build/src/libultra_boot_O1/__osDequeueThread.o"
    include "build/src/libultra_boot_O1/osDestroyThread.o"
    include "build/asm/bzero.o"
    include "build/src/libultra_boot_O1/osCreateThread.o"
    include "build/asm/__osSetSR.o"
    include "build/asm/__osGetSR.o"
    include "build/asm/osWritebackDCache.o"
    include "build/src/libultra_boot_O2/osViGetNextFramebuffer.o"
    include "build/src/libultra_boot_O2/pimgr.o"
    include "build/src/libultra_boot_O2/__osDevMgrMain.o"
    include "build/src/libultra_boot_O2/__osPiRawStartDma.o"
    include "build/src/libultra_boot_O1/osVirtualToPhysical.o"
    include "build/src/libultra_boot_O2/osViBlack.o"
    include "build/src/libultra_boot_O2/__osSiRawReadIo.o"
    include "build/src/libultra_boot_O1/osGetThreadId.o"
    include "build/asm/osSetIntMask.o"
    include "build/src/libultra_boot_O2/osViSetMode.o"
    include "build/asm/__osProbeTLB.o"
    include "build/src/libultra_boot_O1/osGetMemSize.o"
    include "build/src/libultra_boot_O1/osSetEventMesg.o"
    include "build/src/libultra_boot_O2/_Printf.o"
    include "build/asm/osUnmapTLBAll.o"
    include "build/src/libultra_boot_O2/osEPiStartDma.o"
    include "build/src/libultra_boot_O2/string.o"
    include "build/asm/osInvalICache.o"
    include "build/src/libultra_boot_O1/osCreateMesgQueue.o"
    include "build/asm/osInvalDCache.o"
    include "build/src/libultra_boot_O2/__osSiDeviceBusy.o"
    include "build/src/libultra_boot_O1/osJamMesg.o"
    include "build/src/libultra_boot_O1/osSetThreadPri.o"
    include "build/src/libultra_boot_O1/osGetThreadPri.o"
    include "build/src/libultra_boot_O2/__osEPiRawReadIo.o"
    include "build/src/libultra_boot_O2/osViSwapBuffer.o"
    include "build/src/libultra_boot_O2/__osEPiRawStartDma.o"
    include "build/asm/bcmp.o"
    include "build/src/libultra_boot_O1/osGetTime.o"
    include "build/src/libultra_boot_O1/timerintr.o"
    include "build/asm/osGetCount.o"
    include "build/src/libultra_boot_O1/__osSetGlobalIntMask.o"
    include "build/asm/__osSetCompare.o"
    include "build/asm/bcopy.o"
    include "build/src/libultra_boot_O1/__osResetGlobalIntMask.o"
    include "build/asm/__osDisableInt.o"
    include "build/asm/__osRestoreInt.o"
    include "build/src/libultra_boot_O2/osViModeNtscLan1.o"
    include "build/src/libultra_boot_O2/osViModeMpalLan1.o"
    include "build/src/libultra_boot_O2/__osViInit.o"
    include "build/src/libultra_boot_O2/__osViSwapContext.o"
    include "build/src/libultra_boot_O2/osPiGetCmdQueue.o"
    include "build/src/libultra_boot_O2/osEPiReadIo.o"
    include "build/src/libultra_boot_O2/osViSetSpecialFeatures.o"
    include "build/src/libultra_boot_O2/osCartRomInit.o"
    include "build/src/libultra_boot_O2/osViModeFpalLan1.o"
    include "build/asm/__osSetFpcCsr.o"
    include "build/asm/__osGetFpcCsr.o"
    include "build/src/libultra_boot_O2/osEPiWriteIo.o"
    include "build/asm/osMapTLBRdb.o"
    include "build/src/libultra_boot_O1/osYieldThread.o"
    include "build/asm/__osGetCause.o"
    include "build/src/libultra_boot_O2/__osEPiRawWriteIo.o"
    include "build/src/libultra_boot_O2/_Litob.o"
    include "build/src/libultra_boot_O2/ldiv.o"
    include "build/src/libultra_boot_O2/_Ldtob.o"
    include "build/src/boot/build.o"
    include "build/src/libultra_boot_O2/__osSiRawWriteIo.o"
    include "build/src/libultra_boot_O2/vimgr.o"
    include "build/src/libultra_boot_O2/__osViGetCurrentContext.o"
    include "build/src/libultra_boot_O1/osStartThread.o"
    include "build/src/libultra_boot_O2/osViSetYScale.o"
    include "build/src/libultra_boot_O2/osViSetXScale.o"
    include "build/src/libultra_boot_O1/__osSetHWIntrRoutine.o"
    include "build/src/libultra_boot_O1/__osGetHWIntrRoutine.o"
    include "build/asm/__osSetWatchLo.o"
    include "build/data/rsp_boot.text.o"
endseg

beginseg
    name "dmadata"
    include "build/asm/dmadata.o"
endseg

beginseg
    name "Audiobank"
    address 0x10 // fake RAM address to avoid map lookup inaccuracies
    include "build/baserom/Audiobank.o"
endseg

beginseg
    name "Audioseq"
    include "build/baserom/Audioseq.o"
endseg

beginseg
    name "Audiotable"
    include "build/baserom/Audiotable.o"
endseg

beginseg
    name "link_animetion"
    include "build/baserom/link_animetion.o"
endseg

beginseg
    name "icon_item_static"
    romalign 0x1000
    include "build/assets/textures/icon_item_static/icon_item_static.o"
endseg

beginseg
    name "icon_item_24_static"
    romalign 0x1000
    include "build/assets/textures/icon_item_24_static/icon_item_24_static.o"
endseg

beginseg
    name "icon_item_field_static"
    romalign 0x1000
    include "build/assets/textures/icon_item_field_static/icon_item_field_static.o"
endseg

beginseg
    name "icon_item_dungeon_static"
    romalign 0x1000
    include "build/assets/textures/icon_item_dungeon_static/icon_item_dungeon_static.o"
endseg

beginseg
    name "icon_item_gameover_static"
    romalign 0x1000
    include "build/baserom/icon_item_gameover_static.o"
endseg

beginseg
    name "icon_item_nes_static"
    romalign 0x1000
    include "build/baserom/icon_item_nes_static.o"
    //include "build/assets/textures/icon_item_nes_static/icon_item_nes_static.o"
endseg

beginseg
    name "icon_item_ger_static"
    romalign 0x1000
    include "build/baserom/icon_item_ger_static.o"
endseg

beginseg
    name "icon_item_fra_static"
    romalign 0x1000
    include "build/baserom/icon_item_fra_static.o"
endseg

beginseg
    name "item_name_static"
    romalign 0x1000
    include "build/baserom/item_name_static.o"
endseg

beginseg
    name "map_name_static"
    romalign 0x1000
    include "build/baserom/map_name_static.o"
endseg

beginseg
    name "do_action_static"
    romalign 0x1000
    include "build/baserom/do_action_static.o"
endseg

beginseg
    name "message_static"
    romalign 0x1000
    include "build/baserom/message_static.o"
endseg

beginseg
    name "message_texture_static"
    romalign 0x1000
    include "build/baserom/message_texture_static.o"
endseg

beginseg
    name "nes_font_static"
    romalign 0x1000
    include "build/baserom/nes_font_static.o"
endseg

beginseg
    name "nes_message_data_static"
    romalign 0x1000
    include "build/baserom/nes_message_data_static.o"
endseg

beginseg
    name "ger_message_data_static"
    romalign 0x1000
    include "build/baserom/ger_message_data_static.o"
endseg

beginseg
    name "fra_message_data_static"
    romalign 0x1000
    include "build/baserom/fra_message_data_static.o"
endseg

beginseg
    name "staff_message_data_static"
    romalign 0x1000
    include "build/baserom/staff_message_data_static.o"
endseg

beginseg
    name "map_grand_static"
    romalign 0x1000
    include "build/baserom/map_grand_static.o"
endseg

beginseg
    name "map_48x85_static"
    romalign 0x1000
    include "build/baserom/map_48x85_static.o"
endseg

beginseg
    name "map_i_static"
    romalign 0x1000
    include "build/baserom/map_i_static.o"
endseg

beginseg
    name "code"
    after "dmadata"
    include "build/src/code/z_en_a_keep.o"
    include "build/src/code/z_en_item00.o"
    include "build/data/z_en_item00.data.o"
    include "build/data/z_en_item00.bss.o"
    include "build/src/code/z_eff_blure.o"
    include "build/src/code/z_eff_shield_particle.o"
    include "build/src/code/z_eff_spark.o"
    include "build/src/code/z_eff_ss_dead.o"
    include "build/src/code/z_effect.o"
    include "build/src/code/z_effect_soft_sprite.o"
    include "build/src/code/z_effect_soft_sprite_old_init.o"
    include "build/data/z_effect_soft_sprite_old_init.data.o"
    include "build/src/code/z_effect_soft_sprite_dlftbls.o"
    include "build/src/code/flg_set.o"
    include "build/src/code/z_DLF.o"
    include "build/src/code/z_actor.o"
    include "build/src/code/z_actor_dlftbls.o"
    include "build/src/code/z_bgcheck.o"
    include "build/data/z_bgcheck.data.o"
    include "build/data/z_bgcheck.rodata.o"
    include "build/data/z_bgcheck.bss.o"
    include "build/src/code/code_800430A0.o"
    include "build/src/code/code_80043480.o"
    include "build/src/code/z_camera.o"
    include "build/data/z_camera.bss.o"
    include "build/src/code/z_collision_btltbls.o"
    include "build/src/code/z_collision_check.o"
    include "build/data/z_collision_check.bss.o"
    include "build/src/code/z_common_data.o"
    include "build/src/code/z_debug.o"
    include "build/src/code/z_debug_display.o"
    include "build/src/code/z_demo.o"
    include "build/src/code/code_80069420.o"
    include "build/src/code/z_draw.o"
    include "build/src/code/code_8006BA00.o"
    include "build/src/code/z_elf_message.o"
    include "build/src/code/z_face_reaction.o"
    include "build/src/code/code_8006C3A0.o"
    include "build/src/code/code_8006C510.o"
    include "build/src/code/z_fcurve_data_skelanime.o"
    include "build/src/code/z_game_dlftbls.o"
    include "build/src/code/z_horse.o"
    include "build/src/code/z_jpeg.o"
    include "build/src/code/z_kaleido_setup.o"
    include "build/src/code/z_kanfont.o"
    include "build/src/code/z_kankyo.o"
    include "build/data/z_kankyo.data.o"
    include "build/data/z_kankyo.bss.o"
    include "build/src/code/z_lib.o"
    include "build/src/code/z_lifemeter.o"
    include "build/data/z_lifemeter.data.o"
    include "build/src/code/z_lights.o"
    include "build/data/z_lights.data.o"
    include "build/src/code/z_malloc.o"
    include "build/src/code/z_map_mark.o"
    include "build/src/code/z_moji.o"
    include "build/src/code/z_prenmi_buff.o"
    include "build/src/code/z_msgevent.o"
    include "build/src/code/z_olib.o"
    include "build/src/code/z_onepointdemo.o"
    include "build/data/z_onepointdemo.data.o"
    include "build/src/code/z_map_exp.o"
    include "build/src/code/z_map_data.o"
    include "build/src/code/z_parameter.o"
    include "build/src/code/z_path.o"
    include "build/src/code/code_8008E6A0.o"
    include "build/src/code/z_player_lib.o"
    include "build/data/z_player_lib.data.o"
    include "build/data/z_player_lib.bss.o"
    include "build/src/code/z_prenmi.o"
    include "build/src/code/z_quake.o"
    include "build/src/code/z_rcp.o"
    include "build/src/code/z_room.o"
    include "build/src/code/z_sample.o"
    include "build/src/code/code_80097A00.o"
    include "build/src/code/z_scene.o"
    include "build/src/code/z_scene_table.o"
    include "build/src/code/z_skelanime.o"
    include "build/src/code/z_skin.o"
    include "build/data/z_skin.bss.o"
    include "build/src/code/z_skin_awb.o"
    include "build/src/code/z_skin_matrix.o"
    include "build/src/code/z_sram.o"
    include "build/data/z_sram.data.o"
    include "build/src/code/code_800A9D40.o"
    include "build/src/code/code_800A9F30.o"
    include "build/data/z_text.data.o"
    include "build/src/code/z_view.o"
    include "build/src/code/z_vimode.o"
    include "build/src/code/code_800ACE70.o"
    include "build/src/code/z_vismono.o"
    include "build/src/code/code_800AD920.o"
    include "build/src/code/z_vr_box.o"
    include "build/data/z_vr_box.data.o"
    include "build/src/code/z_vr_box_draw.o"
    include "build/src/code/z_player_call.o"
    include "build/src/code/z_fbdemo.o"
    include "build/src/code/z_fbdemo_triforce.o"
    include "build/src/code/z_fbdemo_wipe1.o"
    include "build/src/code/z_fbdemo_circle.o"
    include "build/src/code/z_fbdemo_fade.o"
    include "build/src/code/shrink_window.o"
    include "build/src/code/db_camera.o"
    include "build/data/db_camera.rodata.o"
    include "build/data/db_camera.data.o"
    include "build/data/db_camera.bss.o"
    include "build/src/code/code_800BB0A0.o"
    include "build/src/code/mempak.o"
    include "build/src/code/z_kaleido_manager.o"
    include "build/src/code/z_kaleido_scope_call.o"
    include "build/src/code/z_play.o"
    include "build/src/code/PreRender.o"
    include "build/src/code/TwoHeadArena.o"
    include "build/src/code/code_800C3C20.o"
    include "build/src/code/audioMgr.o"
    include "build/src/code/title_setup.o"
    include "build/src/code/game.o"
    include "build/src/code/gamealloc.o"
    include "build/src/code/graph.o"
    include "build/src/code/listalloc.o"
    include "build/src/code/main.o"
    include "build/src/code/padmgr.o"
    include "build/src/code/sched.o"
    include "build/src/code/speed_meter.o"
    include "build/src/code/sys_cfb.o"
    include "build/src/code/sys_math.o"
    include "build/src/code/sys_math3d.o"
    include "build/src/code/sys_math_atan.o"
    include "build/src/code/sys_matrix.o"
    include "build/src/code/sys_ucode.o"
    include "build/src/code/code_800D2E30.o"
    include "build/data/code_800D2E30.data.o"
    include "build/src/code/code_800D31A0.o"
    include "build/src/code/irqmgr.o"
    include "build/src/code/debug_malloc.o"
    include "build/src/code/fault.o"
    include "build/data/fault.bss.o"
    include "build/src/code/fault_drawer.o"
    include "build/data/fault_drawer.bss.o"
    include "build/asm/code_800D71F0.o"
    include "build/src/code/ucode_disas.o"
    include "build/src/code/code_800DACC0.o"
    include "build/data/code_800DACC0.data.o"
    include "build/data/code_800DACC0.bss.o"
    include "build/src/code/audio_heap.o"
    include "build/src/code/code_800E11F0.o"
    include "build/data/code_800E11F0.data.o"
    include "build/data/code_800E11F0.rodata.o"
    include "build/src/libultra_code/code_800E6840.o"
    include "build/src/libultra_code/osAiSetNextBuffer.o"
    include "build/src/code/audio_playback.o"
    include "build/src/code/audio_effects.o"
    include "build/data/audio_effects.data.o"
    include "build/src/code/audio_seqplayer.o"
    include "build/src/code/code_800EC960.o"
    include "build/data/code_800EC960.data.o"
    include "build/src/code/code_800F7260.o"
    include "build/data/code_800F7260.data.o"
    include "build/src/code/code_800F9280.o"
    include "build/data/code_800F9280.data.o"
    include "build/data/code_800F9280.rodata.o"
    include "build/src/code/logseverity.o"
    include "build/src/code/gfxprint.o"
    include "build/src/code/code_800FBCE0.o"
    include "build/src/code/loadfragment2.o"
    include "build/src/code/mtxuty-cvt.o"
    include "build/src/code/relocation.o"
    include "build/src/code/code_800FC620.o"
    include "build/src/code/padutils.o"
    include "build/src/code/padsetup.o"
    include "build/src/code/code_800FCE80.o"
    include "build/asm/fp.o"
    include "build/src/code/system_malloc.o"
    include "build/src/code/code_800FD970.o"
    include "build/src/code/__osMalloc.o"
    include "build/src/code/printutils.o"
    include "build/src/code/sleep.o"
    include "build/src/code/jpegutils.o"
    include "build/src/code/jpegdecoder.o"
    include "build/src/libultra_code/osPfsFreeBlocks.o"
    include "build/asm/guScale.o"
    include "build/src/libultra_code/sinf.o"
    include "build/src/libultra_code/sins.o"
    include "build/src/libultra_code/sptask.o"
    include "build/src/libultra_code/osRumblePak.o"
    include "build/src/libultra_code/__osSiCreateAccessQueue.o"
    include "build/src/libultra_code/osContInit.o"
    include "build/src/libultra_code/osContStartReadData.o"
    include "build/src/libultra_code/guPerspectiveF.o"
    include "build/src/libultra_code/__osSpRawStartDma.o"
    include "build/src/libultra_code/__osSiRawStartDma.o"
    include "build/src/libultra_code/osSpTaskYield.o"
    include "build/src/libultra_code/pfsreadwritefile.o"
    include "build/src/libultra_code/__osPfsGetStatus.o"
    include "build/asm/guMtxIdentF.o"
    include "build/src/libultra_code/guLookAt.o"
    include "build/src/libultra_code/pfsallocatefile.o"
    include "build/src/libultra_code/osStopTimer.o"
    include "build/src/libultra_code/contpfs.o"
    include "build/src/libultra_code/code_80102FA0.o"
    include "build/src/libultra_code/osPfsFindFile.o"
    include "build/src/libultra_code/sqrtf.o"
    include "build/src/libultra_code/osAfterPreNMI.o"
    include "build/src/libultra_code/contquery.o"
    include "build/src/libultra_code/guLookAtHilite.o"
    include "build/src/libultra_code/sp.o"
    include "build/asm/guMtxIdent.o"
    include "build/src/libultra_code/guPosition.o"
    include "build/src/libultra_code/osSpTaskYielded.o"
    include "build/src/libultra_code/rotate.o"
    include "build/src/libultra_code/osAiSetFrequency.o"
    include "build/src/libultra_code/__osGetActiveQueue.o"
    include "build/asm/guNormalize.o"
    include "build/src/libultra_code/osDpGetStatus.o"
    include "build/src/libultra_code/osDpSetStatus.o"
    include "build/src/libultra_code/osPfsDeleteFile.o"
    include "build/src/libultra_code/ortho.o"
    include "build/src/libultra_code/cosf.o"
    include "build/data/__libm_qnan_f.rodata.o"
    include "build/src/libultra_code/coss.o"
    include "build/src/libultra_code/osViSetEvent.o"
    include "build/src/libultra_code/osPfsIsPlug.o"
    include "build/src/libultra_code/guS2DInitBg.o"
    include "build/src/libultra_code/__osPfsSelectBank.o"
    include "build/src/libultra_code/osContSetCh.o"
    include "build/src/libultra_code/code_80104D60.o"
    include "build/src/libultra_code/pfsinitpak.o"
    include "build/src/libultra_code/pfschecker.o"
    include "build/src/libultra_code/osAiGetLength.o"
    include "build/asm/guTranslate.o"
    include "build/src/libultra_code/__osContRamWrite.o"
    include "build/src/libultra_code/__osContRamRead.o"
    include "build/src/libultra_code/__osContAddressCrc.o"
    include "build/src/libultra_code/osSetTimer.o"
    include "build/src/libultra_code/__osSpGetStatus.o"
    include "build/src/libultra_code/__osSpSetStatus.o"
    include "build/asm/osWritebackDCacheAll.o"
    include "build/src/libultra_code/__osGetCurrFaultedThread.o"
    include "build/asm/guMtxF2L.o"
    include "build/src/libultra_code/llcvt.o"
    include "build/src/libultra_code/osViGetCurrentFramebuffer.o"
    include "build/src/libultra_code/__osSpSetPc.o"
    include "build/src/libultra_code/sqrt.o"
    include "build/src/libultra_code/absf.o"
    include "build/src/code/code_801067F0.o"
    include "build/src/code/code_80106860.o"
    include "build/src/code/code_801068B0.o"
    include "build/data/z_message_PAL.rodata.o"
    include "build/src/code/z_message_PAL.o"
    include "build/data/z_message_PAL.bss.o"
    include "build/src/code/code_80110450.o"
    include "build/src/code/z_construct.o"
    include "build/data/rsp.text.o"
    include "build/data/rsp.rodata.o"
endseg

beginseg
    name "buffers"
    align 0x40
    include "build/src/buffers/zbuffer.o"
    include "build/src/buffers/gfxbuffers.o"
    include "build/src/buffers/heaps.o"
endseg

beginseg
    name "ovl_title"
    address 0x80800000
    include "build/src/overlays/gamestates/ovl_title/z_title.o"
    include "build/src/overlays/gamestates/ovl_title/ovl_title_reloc.o"
endseg

beginseg
    name "ovl_select"
    include "build/src/overlays/gamestates/ovl_select/z_select.o"
    include "build/src/overlays/gamestates/ovl_select/ovl_select_reloc.o"
endseg

beginseg
    name "ovl_opening"
    include "build/src/overlays/gamestates/ovl_opening/z_opening.o"
    include "build/src/overlays/gamestates/ovl_opening/ovl_opening_reloc.o"
endseg

beginseg
    name "ovl_file_choose"
    include "build/src/overlays/gamestates/ovl_file_choose/z_file_nameset_PAL.o"
    include "build/data/overlays/gamestates/z_file_nameset_PAL.data.o"
    include "build/data/overlays/gamestates/z_file_nameset_PAL.rodata.o"
    include "build/data/overlays/gamestates/z_file_nameset_PAL.bss.o"
    include "build/src/overlays/gamestates/ovl_file_choose/z_file_choose.o"
    include "build/data/overlays/gamestates/z_file_choose.rodata.o"
    include "build/data/overlays/gamestates/z_file_choose.reloc.o"
endseg

beginseg
    name "ovl_kaleido_scope"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_collect.o"
    include "build/data/overlays/actors/z_kaleido_collect.data.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_debug.o"
    include "build/data/overlays/actors/z_kaleido_debug.data.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_equipment.o"
    include "build/data/overlays/actors/z_kaleido_equipment.data.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_item.o"
    include "build/data/overlays/actors/z_kaleido_item.data.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_map_PAL.o"
    include "build/data/overlays/actors/z_kaleido_map_PAL.data.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_kaleido_scope_PAL.o"
    include "build/data/overlays/actors/z_kaleido_scope_PAL.data.o"
    include "build/data/overlays/actors/z_kaleido_scope_PAL.bss.o"
    include "build/src/overlays/actors/ovl_kaleido_scope/z_lmap_mark.o"
    include "build/data/overlays/actors/z_lmap_mark.data.o"
    include "build/data/overlays/actors/z_lmap_mark.rodata.o"
    include "build/data/overlays/actors/z_kaleido_scope.reloc.o"
endseg

beginseg
    name "ovl_player_actor"
    include "build/src/overlays/actors/ovl_player_actor/z_player.o"
    include "build/data/overlays/actors/z_player.data.o"
    include "build/data/overlays/actors/z_player.bss.o"
    include "build/data/overlays/actors/z_player.reloc.o"
endseg

beginseg
    name "ovl_map_mark_data"
    include "build/asm/overlays/data/ovl_map_mark_data/z_map_mark_data.o"
endseg

beginseg
    name "ovl_En_Test"
    include "build/src/overlays/actors/ovl_En_Test/z_en_test.o"
    include "build/data/overlays/actors/z_en_test.data.o"
    include "build/data/overlays/actors/z_en_test.reloc.o"
endseg

beginseg
    name "ovl_Arms_Hook"
    include "build/src/overlays/actors/ovl_Arms_Hook/z_arms_hook.o"
    include "build/src/overlays/actors/ovl_Arms_Hook/ovl_Arms_Hook_reloc.o"
endseg

beginseg
    name "ovl_Arrow_Fire"
    include "build/src/overlays/actors/ovl_Arrow_Fire/z_arrow_fire.o"
    include "build/src/overlays/actors/ovl_Arrow_Fire/ovl_Arrow_Fire_reloc.o"
endseg

beginseg
    name "ovl_Arrow_Ice"
    include "build/src/overlays/actors/ovl_Arrow_Ice/z_arrow_ice.o"
    include "build/src/overlays/actors/ovl_Arrow_Ice/ovl_Arrow_Ice_reloc.o"
endseg

beginseg
    name "ovl_Arrow_Light"
    include "build/src/overlays/actors/ovl_Arrow_Light/z_arrow_light.o"
    include "build/src/overlays/actors/ovl_Arrow_Light/ovl_Arrow_Light_reloc.o"
endseg

beginseg
    name "ovl_Bg_Bdan_Objects"
    include "build/src/overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.o"
    include "build/src/overlays/actors/ovl_Bg_Bdan_Objects/ovl_Bg_Bdan_Objects_reloc.o"
endseg

beginseg
    name "ovl_Bg_Bdan_Switch"
    include "build/src/overlays/actors/ovl_Bg_Bdan_Switch/z_bg_bdan_switch.o"
    include "build/src/overlays/actors/ovl_Bg_Bdan_Switch/ovl_Bg_Bdan_Switch_reloc.o"
endseg

beginseg
    name "ovl_Bg_Bom_Guard"
    include "build/src/overlays/actors/ovl_Bg_Bom_Guard/z_bg_bom_guard.o"
    include "build/src/overlays/actors/ovl_Bg_Bom_Guard/ovl_Bg_Bom_Guard_reloc.o"
endseg

beginseg
    name "ovl_Bg_Bombwall"
    include "build/src/overlays/actors/ovl_Bg_Bombwall/z_bg_bombwall.o"
    include "build/data/overlays/actors/z_bg_bombwall.data.o"
    include "build/data/overlays/actors/z_bg_bombwall.reloc.o"
endseg

beginseg
    name "ovl_Bg_Bowl_Wall"
    include "build/src/overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.o"
    include "build/data/overlays/actors/z_bg_bowl_wall.data.o"
    include "build/data/overlays/actors/z_bg_bowl_wall.reloc.o"
endseg

beginseg
    name "ovl_Bg_Breakwall"
    include "build/src/overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.o"
    include "build/data/overlays/actors/z_bg_breakwall.data.o"
    include "build/data/overlays/actors/z_bg_breakwall.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ddan_Jd"
    include "build/src/overlays/actors/ovl_Bg_Ddan_Jd/z_bg_ddan_jd.o"
    include "build/data/overlays/actors/z_bg_ddan_jd.data.o"
    include "build/data/overlays/actors/z_bg_ddan_jd.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ddan_Kd"
    include "build/src/overlays/actors/ovl_Bg_Ddan_Kd/z_bg_ddan_kd.o"
    include "build/src/overlays/actors/ovl_Bg_Ddan_Kd/ovl_Bg_Ddan_Kd_reloc.o"
endseg

beginseg
    name "ovl_Bg_Dodoago"
    include "build/src/overlays/actors/ovl_Bg_Dodoago/z_bg_dodoago.o"
    include "build/data/overlays/actors/z_bg_dodoago.data.o"
    include "build/data/overlays/actors/z_bg_dodoago.bss.o"
    include "build/data/overlays/actors/z_bg_dodoago.reloc.o"
endseg

beginseg
    name "ovl_Bg_Dy_Yoseizo"
    include "build/src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.o"
    include "build/data/overlays/actors/z_bg_dy_yoseizo.data.o"
    include "build/data/overlays/actors/z_bg_dy_yoseizo.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ganon_Otyuka"
    include "build/src/overlays/actors/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.o"
    include "build/data/overlays/actors/z_bg_ganon_otyuka.data.o"
    include "build/data/overlays/actors/z_bg_ganon_otyuka.reloc.o"
endseg

beginseg
    name "ovl_Bg_Gate_Shutter"
    include "build/src/overlays/actors/ovl_Bg_Gate_Shutter/z_bg_gate_shutter.o"
    include "build/src/overlays/actors/ovl_Bg_Gate_Shutter/ovl_Bg_Gate_Shutter_reloc.o"
endseg

beginseg
    name "ovl_Bg_Gjyo_Bridge"
    include "build/src/overlays/actors/ovl_Bg_Gjyo_Bridge/z_bg_gjyo_bridge.o"
    include "build/src/overlays/actors/ovl_Bg_Gjyo_Bridge/ovl_Bg_Gjyo_Bridge_reloc.o"
endseg

beginseg
    name "ovl_Bg_Gnd_Darkmeiro"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Darkmeiro/z_bg_gnd_darkmeiro.o"
    include "build/data/overlays/actors/z_bg_gnd_darkmeiro.data.o"
    include "build/data/overlays/actors/z_bg_gnd_darkmeiro.reloc.o"
endseg

beginseg
    name "ovl_Bg_Gnd_Firemeiro"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Firemeiro/z_bg_gnd_firemeiro.o"
    include "build/data/overlays/actors/z_bg_gnd_firemeiro.data.o"
    include "build/data/overlays/actors/z_bg_gnd_firemeiro.reloc.o"
endseg

beginseg
    name "ovl_Bg_Gnd_Iceblock"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Iceblock/z_bg_gnd_iceblock.o"
    include "build/data/overlays/actors/z_bg_gnd_iceblock.data.o"
    include "build/data/overlays/actors/z_bg_gnd_iceblock.rodata.o"
    include "build/data/overlays/actors/z_bg_gnd_iceblock.bss.o"
    include "build/data/overlays/actors/z_bg_gnd_iceblock.reloc.o"
endseg

beginseg
    name "ovl_Bg_Gnd_Nisekabe"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Nisekabe/z_bg_gnd_nisekabe.o"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Nisekabe/ovl_Bg_Gnd_Nisekabe_reloc.o"
endseg

beginseg
    name "ovl_Bg_Gnd_Soulmeiro"
    include "build/src/overlays/actors/ovl_Bg_Gnd_Soulmeiro/z_bg_gnd_soulmeiro.o"
    include "build/data/overlays/actors/z_bg_gnd_soulmeiro.data.o"
    include "build/data/overlays/actors/z_bg_gnd_soulmeiro.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka"
    include "build/src/overlays/actors/ovl_Bg_Haka/z_bg_haka.o"
    include "build/data/overlays/actors/z_bg_haka.data.o"
    include "build/data/overlays/actors/z_bg_haka.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Gate"
    include "build/src/overlays/actors/ovl_Bg_Haka_Gate/z_bg_haka_gate.o"
    include "build/data/overlays/actors/z_bg_haka_gate.data.o"
    include "build/data/overlays/actors/z_bg_haka_gate.bss.o"
    include "build/data/overlays/actors/z_bg_haka_gate.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Huta"
    include "build/src/overlays/actors/ovl_Bg_Haka_Huta/z_bg_haka_huta.o"
    include "build/data/overlays/actors/z_bg_haka_huta.data.o"
    include "build/data/overlays/actors/z_bg_haka_huta.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Megane"
    include "build/src/overlays/actors/ovl_Bg_Haka_Megane/z_bg_haka_megane.o"
    include "build/src/overlays/actors/ovl_Bg_Haka_Megane/ovl_Bg_Haka_Megane_reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_MeganeBG"
    include "build/src/overlays/actors/ovl_Bg_Haka_MeganeBG/z_bg_haka_meganebg.o"
    include "build/data/overlays/actors/z_bg_haka_meganebg.data.o"
    include "build/data/overlays/actors/z_bg_haka_meganebg.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Sgami"
    include "build/src/overlays/actors/ovl_Bg_Haka_Sgami/z_bg_haka_sgami.o"
    include "build/data/overlays/actors/z_bg_haka_sgami.data.o"
    include "build/data/overlays/actors/z_bg_haka_sgami.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Ship"
    include "build/src/overlays/actors/ovl_Bg_Haka_Ship/z_bg_haka_ship.o"
    include "build/data/overlays/actors/z_bg_haka_ship.data.o"
    include "build/data/overlays/actors/z_bg_haka_ship.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Trap"
    include "build/src/overlays/actors/ovl_Bg_Haka_Trap/z_bg_haka_trap.o"
    include "build/src/overlays/actors/ovl_Bg_Haka_Trap/ovl_Bg_Haka_Trap_reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Tubo"
    include "build/src/overlays/actors/ovl_Bg_Haka_Tubo/z_bg_haka_tubo.o"
    include "build/src/overlays/actors/ovl_Bg_Haka_Tubo/ovl_Bg_Haka_Tubo_reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Water"
    include "build/src/overlays/actors/ovl_Bg_Haka_Water/z_bg_haka_water.o"
    include "build/data/overlays/actors/z_bg_haka_water.data.o"
    include "build/data/overlays/actors/z_bg_haka_water.reloc.o"
endseg

beginseg
    name "ovl_Bg_Haka_Zou"
    include "build/src/overlays/actors/ovl_Bg_Haka_Zou/z_bg_haka_zou.o"
    include "build/data/overlays/actors/z_bg_haka_zou.data.o"
    include "build/data/overlays/actors/z_bg_haka_zou.reloc.o"
endseg

beginseg
    name "ovl_Bg_Heavy_Block"
    include "build/src/overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.o"
    include "build/src/overlays/actors/ovl_Bg_Heavy_Block/ovl_Bg_Heavy_Block_reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Curtain"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.o"
    include "build/data/overlays/actors/z_bg_hidan_curtain.data.o"
    include "build/data/overlays/actors/z_bg_hidan_curtain.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Dalm"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Dalm/z_bg_hidan_dalm.o"
    include "build/data/overlays/actors/z_bg_hidan_dalm.data.o"
    include "build/data/overlays/actors/z_bg_hidan_dalm.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Firewall"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Firewall/z_bg_hidan_firewall.o"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Firewall/ovl_Bg_Hidan_Firewall_reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Fslift"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Fslift/z_bg_hidan_fslift.o"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Fslift/ovl_Bg_Hidan_Fslift_reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Fwbig"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Fwbig/z_bg_hidan_fwbig.o"
    include "build/data/overlays/actors/z_bg_hidan_fwbig.data.o"
    include "build/data/overlays/actors/z_bg_hidan_fwbig.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Hamstep"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Hamstep/z_bg_hidan_hamstep.o"
    include "build/data/overlays/actors/z_bg_hidan_hamstep.data.o"
    include "build/data/overlays/actors/z_bg_hidan_hamstep.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Hrock"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Hrock/z_bg_hidan_hrock.o"
    include "build/data/overlays/actors/z_bg_hidan_hrock.data.o"
    include "build/data/overlays/actors/z_bg_hidan_hrock.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Kousi"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Kousi/z_bg_hidan_kousi.o"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Kousi/ovl_Bg_Hidan_Kousi_reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Kowarerukabe"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Kowarerukabe/z_bg_hidan_kowarerukabe.o"
    include "build/data/overlays/actors/z_bg_hidan_kowarerukabe.data.o"
    include "build/data/overlays/actors/z_bg_hidan_kowarerukabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Rock"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Rock/z_bg_hidan_rock.o"
    include "build/data/overlays/actors/z_bg_hidan_rock.data.o"
    include "build/data/overlays/actors/z_bg_hidan_rock.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Rsekizou"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Rsekizou/z_bg_hidan_rsekizou.o"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Rsekizou/ovl_Bg_Hidan_Rsekizou_reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Sekizou"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Sekizou/z_bg_hidan_sekizou.o"
    include "build/data/overlays/actors/z_bg_hidan_sekizou.data.o"
    include "build/data/overlays/actors/z_bg_hidan_sekizou.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Sima"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Sima/z_bg_hidan_sima.o"
    include "build/data/overlays/actors/z_bg_hidan_sima.data.o"
    include "build/data/overlays/actors/z_bg_hidan_sima.reloc.o"
endseg

beginseg
    name "ovl_Bg_Hidan_Syoku"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Syoku/z_bg_hidan_syoku.o"
    include "build/src/overlays/actors/ovl_Bg_Hidan_Syoku/ovl_Bg_Hidan_Syoku_reloc.o"
endseg

beginseg
    name "ovl_Bg_Ice_Objects"
    include "build/src/overlays/actors/ovl_Bg_Ice_Objects/z_bg_ice_objects.o"
    include "build/data/overlays/actors/z_bg_ice_objects.data.o"
    include "build/data/overlays/actors/z_bg_ice_objects.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ice_Shelter"
    include "build/src/overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.o"
    include "build/data/overlays/actors/z_bg_ice_shelter.data.o"
    include "build/data/overlays/actors/z_bg_ice_shelter.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ice_Shutter"
    include "build/src/overlays/actors/ovl_Bg_Ice_Shutter/z_bg_ice_shutter.o"
    include "build/data/overlays/actors/z_bg_ice_shutter.data.o"
    include "build/data/overlays/actors/z_bg_ice_shutter.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ice_Turara"
    include "build/src/overlays/actors/ovl_Bg_Ice_Turara/z_bg_ice_turara.o"
    include "build/data/overlays/actors/z_bg_ice_turara.data.o"
    include "build/data/overlays/actors/z_bg_ice_turara.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ingate"
    include "build/src/overlays/actors/ovl_Bg_Ingate/z_bg_ingate.o"
    include "build/src/overlays/actors/ovl_Bg_Ingate/ovl_Bg_Ingate_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_1flift"
    include "build/src/overlays/actors/ovl_Bg_Jya_1flift/z_bg_jya_1flift.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_1flift/ovl_Bg_Jya_1flift_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Amishutter"
    include "build/src/overlays/actors/ovl_Bg_Jya_Amishutter/z_bg_jya_amishutter.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_Amishutter/ovl_Bg_Jya_Amishutter_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Bigmirror"
    include "build/src/overlays/actors/ovl_Bg_Jya_Bigmirror/z_bg_jya_bigmirror.o"
    include "build/data/overlays/actors/z_bg_jya_bigmirror.data.o"
    include "build/data/overlays/actors/z_bg_jya_bigmirror.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Block"
    include "build/src/overlays/actors/ovl_Bg_Jya_Block/z_bg_jya_block.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_Block/ovl_Bg_Jya_Block_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Bombchuiwa"
    include "build/src/overlays/actors/ovl_Bg_Jya_Bombchuiwa/z_bg_jya_bombchuiwa.o"
    include "build/data/overlays/actors/z_bg_jya_bombchuiwa.data.o"
    include "build/data/overlays/actors/z_bg_jya_bombchuiwa.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Bombiwa"
    include "build/src/overlays/actors/ovl_Bg_Jya_Bombiwa/z_bg_jya_bombiwa.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_Bombiwa/ovl_Bg_Jya_Bombiwa_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Cobra"
    include "build/src/overlays/actors/ovl_Bg_Jya_Cobra/z_bg_jya_cobra.o"
    include "build/data/overlays/actors/z_bg_jya_cobra.data.o"
    include "build/data/overlays/actors/z_bg_jya_cobra.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Goroiwa"
    include "build/src/overlays/actors/ovl_Bg_Jya_Goroiwa/z_bg_jya_goroiwa.o"
    include "build/data/overlays/actors/z_bg_jya_goroiwa.data.o"
    include "build/data/overlays/actors/z_bg_jya_goroiwa.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Haheniron"
    include "build/src/overlays/actors/ovl_Bg_Jya_Haheniron/z_bg_jya_haheniron.o"
    include "build/data/overlays/actors/z_bg_jya_haheniron.data.o"
    include "build/data/overlays/actors/z_bg_jya_haheniron.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Ironobj"
    include "build/src/overlays/actors/ovl_Bg_Jya_Ironobj/z_bg_jya_ironobj.o"
    include "build/data/overlays/actors/z_bg_jya_ironobj.data.o"
    include "build/data/overlays/actors/z_bg_jya_ironobj.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Kanaami"
    include "build/src/overlays/actors/ovl_Bg_Jya_Kanaami/z_bg_jya_kanaami.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_Kanaami/ovl_Bg_Jya_Kanaami_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Lift"
    include "build/src/overlays/actors/ovl_Bg_Jya_Lift/z_bg_jya_lift.o"
    include "build/src/overlays/actors/ovl_Bg_Jya_Lift/ovl_Bg_Jya_Lift_reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Megami"
    include "build/src/overlays/actors/ovl_Bg_Jya_Megami/z_bg_jya_megami.o"
    include "build/data/overlays/actors/z_bg_jya_megami.data.o"
    include "build/data/overlays/actors/z_bg_jya_megami.reloc.o"
endseg

beginseg
    name "ovl_Bg_Jya_Zurerukabe"
    include "build/src/overlays/actors/ovl_Bg_Jya_Zurerukabe/z_bg_jya_zurerukabe.o"
    include "build/data/overlays/actors/z_bg_jya_zurerukabe.data.o"
    include "build/data/overlays/actors/z_bg_jya_zurerukabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Menkuri_Eye"
    include "build/src/overlays/actors/ovl_Bg_Menkuri_Eye/z_bg_menkuri_eye.o"
    include "build/data/overlays/actors/z_bg_menkuri_eye.data.o"
    include "build/data/overlays/actors/z_bg_menkuri_eye.bss.o"
    include "build/data/overlays/actors/z_bg_menkuri_eye.reloc.o"
endseg

beginseg
    name "ovl_Bg_Menkuri_Kaiten"
    include "build/src/overlays/actors/ovl_Bg_Menkuri_Kaiten/z_bg_menkuri_kaiten.o"
    include "build/src/overlays/actors/ovl_Bg_Menkuri_Kaiten/ovl_Bg_Menkuri_Kaiten_reloc.o"
endseg

beginseg
    name "ovl_Bg_Menkuri_Nisekabe"
    include "build/src/overlays/actors/ovl_Bg_Menkuri_Nisekabe/z_bg_menkuri_nisekabe.o"
    include "build/src/overlays/actors/ovl_Bg_Menkuri_Nisekabe/ovl_Bg_Menkuri_Nisekabe_reloc.o"
endseg

beginseg
    name "ovl_Bg_Mizu_Bwall"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Bwall/z_bg_mizu_bwall.o"
    include "build/data/overlays/actors/z_bg_mizu_bwall.data.o"
    include "build/data/overlays/actors/z_bg_mizu_bwall.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mizu_Movebg"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Movebg/z_bg_mizu_movebg.o"
    include "build/data/overlays/actors/z_bg_mizu_movebg.data.o"
    include "build/data/overlays/actors/z_bg_mizu_movebg.bss.o"
    include "build/data/overlays/actors/z_bg_mizu_movebg.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mizu_Shutter"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Shutter/z_bg_mizu_shutter.o"
    include "build/data/overlays/actors/z_bg_mizu_shutter.data.o"
    include "build/data/overlays/actors/z_bg_mizu_shutter.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mizu_Uzu"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Uzu/z_bg_mizu_uzu.o"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Uzu/ovl_Bg_Mizu_Uzu_reloc.o"
endseg

beginseg
    name "ovl_Bg_Mizu_Water"
    include "build/src/overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.o"
    include "build/data/overlays/actors/z_bg_mizu_water.data.o"
    include "build/data/overlays/actors/z_bg_mizu_water.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mjin"
    include "build/src/overlays/actors/ovl_Bg_Mjin/z_bg_mjin.o"
    include "build/src/overlays/actors/ovl_Bg_Mjin/ovl_Bg_Mjin_reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Bigst"
    include "build/src/overlays/actors/ovl_Bg_Mori_Bigst/z_bg_mori_bigst.o"
    include "build/data/overlays/actors/z_bg_mori_bigst.data.o"
    include "build/data/overlays/actors/z_bg_mori_bigst.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Elevator"
    include "build/src/overlays/actors/ovl_Bg_Mori_Elevator/z_bg_mori_elevator.o"
    include "build/data/overlays/actors/z_bg_mori_elevator.data.o"
    include "build/data/overlays/actors/z_bg_mori_elevator.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Hashigo"
    include "build/src/overlays/actors/ovl_Bg_Mori_Hashigo/z_bg_mori_hashigo.o"
    include "build/data/overlays/actors/z_bg_mori_hashigo.data.o"
    include "build/data/overlays/actors/z_bg_mori_hashigo.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Hashira4"
    include "build/src/overlays/actors/ovl_Bg_Mori_Hashira4/z_bg_mori_hashira4.o"
    include "build/data/overlays/actors/z_bg_mori_hashira4.data.o"
    include "build/data/overlays/actors/z_bg_mori_hashira4.bss.o"
    include "build/data/overlays/actors/z_bg_mori_hashira4.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Hineri"
    include "build/src/overlays/actors/ovl_Bg_Mori_Hineri/z_bg_mori_hineri.o"
    include "build/src/overlays/actors/ovl_Bg_Mori_Hineri/ovl_Bg_Mori_Hineri_reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Idomizu"
    include "build/src/overlays/actors/ovl_Bg_Mori_Idomizu/z_bg_mori_idomizu.o"
    include "build/data/overlays/actors/z_bg_mori_idomizu.data.o"
    include "build/data/overlays/actors/z_bg_mori_idomizu.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Kaitenkabe"
    include "build/src/overlays/actors/ovl_Bg_Mori_Kaitenkabe/z_bg_mori_kaitenkabe.o"
    include "build/data/overlays/actors/z_bg_mori_kaitenkabe.data.o"
    include "build/data/overlays/actors/z_bg_mori_kaitenkabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Mori_Rakkatenjo"
    include "build/src/overlays/actors/ovl_Bg_Mori_Rakkatenjo/z_bg_mori_rakkatenjo.o"
    include "build/data/overlays/actors/z_bg_mori_rakkatenjo.data.o"
    include "build/data/overlays/actors/z_bg_mori_rakkatenjo.reloc.o"
endseg

beginseg
    name "ovl_Bg_Po_Event"
    include "build/src/overlays/actors/ovl_Bg_Po_Event/z_bg_po_event.o"
    include "build/data/overlays/actors/z_bg_po_event.data.o"
    include "build/data/overlays/actors/z_bg_po_event.bss.o"
    include "build/data/overlays/actors/z_bg_po_event.reloc.o"
endseg

beginseg
    name "ovl_Bg_Po_Syokudai"
    include "build/src/overlays/actors/ovl_Bg_Po_Syokudai/z_bg_po_syokudai.o"
    include "build/data/overlays/actors/z_bg_po_syokudai.data.o"
    include "build/data/overlays/actors/z_bg_po_syokudai.reloc.o"
endseg

beginseg
    name "ovl_Bg_Pushbox"
    include "build/src/overlays/actors/ovl_Bg_Pushbox/z_bg_pushbox.o"
    include "build/src/overlays/actors/ovl_Bg_Pushbox/ovl_Bg_Pushbox_reloc.o"
endseg

beginseg
    name "ovl_Bg_Relay_Objects"
    include "build/src/overlays/actors/ovl_Bg_Relay_Objects/z_bg_relay_objects.o"
    include "build/data/overlays/actors/z_bg_relay_objects.data.o"
    include "build/data/overlays/actors/z_bg_relay_objects.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot00_Break"
    include "build/src/overlays/actors/ovl_Bg_Spot00_Break/z_bg_spot00_break.o"
    include "build/src/overlays/actors/ovl_Bg_Spot00_Break/ovl_Bg_Spot00_Break_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot00_Hanebasi"
    include "build/src/overlays/actors/ovl_Bg_Spot00_Hanebasi/z_bg_spot00_hanebasi.o"
    include "build/data/overlays/actors/z_bg_spot00_hanebasi.data.o"
    include "build/data/overlays/actors/z_bg_spot00_hanebasi.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot01_Fusya"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Fusya/z_bg_spot01_fusya.o"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Fusya/ovl_Bg_Spot01_Fusya_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot01_Idohashira"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idohashira/z_bg_spot01_idohashira.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idohashira/ovl_Bg_Spot01_Idohashira_reloc.o"
#else
    include "build/data/overlays/actors/z_bg_spot01_idohashira.reloc.o"
#endif
endseg

beginseg
    name "ovl_Bg_Spot01_Idomizu"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idomizu/z_bg_spot01_idomizu.o"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idomizu/ovl_Bg_Spot01_Idomizu_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot01_Idosoko"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idosoko/z_bg_spot01_idosoko.o"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Idosoko/ovl_Bg_Spot01_Idosoko_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot01_Objects2"
    include "build/src/overlays/actors/ovl_Bg_Spot01_Objects2/z_bg_spot01_objects2.o"
    include "build/data/overlays/actors/z_bg_spot01_objects2.data.o"
    include "build/data/overlays/actors/z_bg_spot01_objects2.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot02_Objects"
    include "build/src/overlays/actors/ovl_Bg_Spot02_Objects/z_bg_spot02_objects.o"
    include "build/data/overlays/actors/z_bg_spot02_objects.data.o"
    include "build/data/overlays/actors/z_bg_spot02_objects.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot03_Taki"
    include "build/src/overlays/actors/ovl_Bg_Spot03_Taki/z_bg_spot03_taki.o"
    include "build/data/overlays/actors/z_bg_spot03_taki.data.o"
    include "build/data/overlays/actors/z_bg_spot03_taki.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot05_Soko"
    include "build/src/overlays/actors/ovl_Bg_Spot05_Soko/z_bg_spot05_soko.o"
    include "build/src/overlays/actors/ovl_Bg_Spot05_Soko/ovl_Bg_Spot05_Soko_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot06_Objects"
    include "build/src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.o"
    include "build/data/overlays/actors/z_bg_spot06_objects.data.o"
    include "build/data/overlays/actors/z_bg_spot06_objects.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot07_Taki"
    include "build/src/overlays/actors/ovl_Bg_Spot07_Taki/z_bg_spot07_taki.o"
    include "build/data/overlays/actors/z_bg_spot07_taki.data.o"
    include "build/data/overlays/actors/z_bg_spot07_taki.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot08_Bakudankabe"
    include "build/src/overlays/actors/ovl_Bg_Spot08_Bakudankabe/z_bg_spot08_bakudankabe.o"
    include "build/data/overlays/actors/z_bg_spot08_bakudankabe.data.o"
    include "build/data/overlays/actors/z_bg_spot08_bakudankabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot08_Iceblock"
    include "build/src/overlays/actors/ovl_Bg_Spot08_Iceblock/z_bg_spot08_iceblock.o"
    include "build/data/overlays/actors/z_bg_spot08_iceblock.data.o"
    include "build/data/overlays/actors/z_bg_spot08_iceblock.rodata.o"
    include "build/data/overlays/actors/z_bg_spot08_iceblock.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot09_Obj"
    include "build/src/overlays/actors/ovl_Bg_Spot09_Obj/z_bg_spot09_obj.o"
    include "build/src/overlays/actors/ovl_Bg_Spot09_Obj/ovl_Bg_Spot09_Obj_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot11_Bakudankabe"
    include "build/src/overlays/actors/ovl_Bg_Spot11_Bakudankabe/z_bg_spot11_bakudankabe.o"
    include "build/data/overlays/actors/z_bg_spot11_bakudankabe.data.o"
    include "build/data/overlays/actors/z_bg_spot11_bakudankabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot11_Oasis"
    include "build/src/overlays/actors/ovl_Bg_Spot11_Oasis/z_bg_spot11_oasis.o"
    include "build/data/overlays/actors/z_bg_spot11_oasis.data.o"
    include "build/data/overlays/actors/z_bg_spot11_oasis.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot12_Gate"
    include "build/src/overlays/actors/ovl_Bg_Spot12_Gate/z_bg_spot12_gate.o"
    include "build/src/overlays/actors/ovl_Bg_Spot12_Gate/ovl_Bg_Spot12_Gate_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot12_Saku"
    include "build/src/overlays/actors/ovl_Bg_Spot12_Saku/z_bg_spot12_saku.o"
    include "build/src/overlays/actors/ovl_Bg_Spot12_Saku/ovl_Bg_Spot12_Saku_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot15_Rrbox"
    include "build/src/overlays/actors/ovl_Bg_Spot15_Rrbox/z_bg_spot15_rrbox.o"
    include "build/data/overlays/actors/z_bg_spot15_rrbox.data.o"
    include "build/data/overlays/actors/z_bg_spot15_rrbox.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot15_Saku"
    include "build/src/overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.o"
    include "build/data/overlays/actors/z_bg_spot15_saku.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot16_Bombstone"
    include "build/src/overlays/actors/ovl_Bg_Spot16_Bombstone/z_bg_spot16_bombstone.o"
    include "build/data/overlays/actors/z_bg_spot16_bombstone.data.o"
    include "build/data/overlays/actors/z_bg_spot16_bombstone.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot16_Doughnut"
    include "build/src/overlays/actors/ovl_Bg_Spot16_Doughnut/z_bg_spot16_doughnut.o"
    include "build/src/overlays/actors/ovl_Bg_Spot16_Doughnut/ovl_Bg_Spot16_Doughnut_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot17_Bakudankabe"
    include "build/src/overlays/actors/ovl_Bg_Spot17_Bakudankabe/z_bg_spot17_bakudankabe.o"
    include "build/data/overlays/actors/z_bg_spot17_bakudankabe.data.o"
    include "build/data/overlays/actors/z_bg_spot17_bakudankabe.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot17_Funen"
    include "build/src/overlays/actors/ovl_Bg_Spot17_Funen/z_bg_spot17_funen.o"
    include "build/src/overlays/actors/ovl_Bg_Spot17_Funen/ovl_Bg_Spot17_Funen_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot18_Basket"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Basket/z_bg_spot18_basket.o"
    include "build/data/overlays/actors/z_bg_spot18_basket.data.o"
    include "build/data/overlays/actors/z_bg_spot18_basket.reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot18_Futa"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Futa/z_bg_spot18_futa.o"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Futa/ovl_Bg_Spot18_Futa_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot18_Obj"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Obj/z_bg_spot18_obj.o"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Obj/ovl_Bg_Spot18_Obj_reloc.o"
endseg

beginseg
    name "ovl_Bg_Spot18_Shutter"
    include "build/src/overlays/actors/ovl_Bg_Spot18_Shutter/z_bg_spot18_shutter.o"
    include "build/data/overlays/actors/z_bg_spot18_shutter.data.o"
    include "build/data/overlays/actors/z_bg_spot18_shutter.reloc.o"
endseg

beginseg
    name "ovl_Bg_Sst_Floor"
    include "build/src/overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.o"
    include "build/data/overlays/actors/z_bg_sst_floor.data.o"
    include "build/data/overlays/actors/z_bg_sst_floor.reloc.o"
endseg

beginseg
    name "ovl_Bg_Toki_Hikari"
    include "build/src/overlays/actors/ovl_Bg_Toki_Hikari/z_bg_toki_hikari.o"
    include "build/data/overlays/actors/z_bg_toki_hikari.data.o"
    include "build/data/overlays/actors/z_bg_toki_hikari.reloc.o"
endseg

beginseg
    name "ovl_Bg_Toki_Swd"
    include "build/src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_1.o"
    include "build/src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_2.o"
    include "build/src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_3.o"
    include "build/src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd.o"
    include "build/src/overlays/actors/ovl_Bg_Toki_Swd/ovl_Bg_Toki_Swd_reloc.o"
endseg

beginseg
    name "ovl_Bg_Treemouth"
    include "build/src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.o"
    include "build/src/overlays/actors/ovl_Bg_Treemouth/ovl_Bg_Treemouth_reloc.o"
endseg

beginseg
    name "ovl_Bg_Umajump"
    include "build/src/overlays/actors/ovl_Bg_Umajump/z_bg_umajump.o"
    include "build/src/overlays/actors/ovl_Bg_Umajump/ovl_Bg_Umajump_reloc.o"
endseg

beginseg
    name "ovl_Bg_Vb_Sima"
    include "build/src/overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.o"
    include "build/data/overlays/actors/z_bg_vb_sima.data.o"
    include "build/data/overlays/actors/z_bg_vb_sima.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ydan_Hasi"
    include "build/src/overlays/actors/ovl_Bg_Ydan_Hasi/z_bg_ydan_hasi.o"
    include "build/src/overlays/actors/ovl_Bg_Ydan_Hasi/ovl_Bg_Ydan_Hasi_reloc.o"
endseg

beginseg
    name "ovl_Bg_Ydan_Maruta"
    include "build/src/overlays/actors/ovl_Bg_Ydan_Maruta/z_bg_ydan_maruta.o"
    include "build/data/overlays/actors/z_bg_ydan_maruta.data.o"
    include "build/data/overlays/actors/z_bg_ydan_maruta.reloc.o"
endseg

beginseg
    name "ovl_Bg_Ydan_Sp"
    include "build/src/overlays/actors/ovl_Bg_Ydan_Sp/z_bg_ydan_sp.o"
    include "build/data/overlays/actors/z_bg_ydan_sp.data.o"
    include "build/data/overlays/actors/z_bg_ydan_sp.reloc.o"
endseg

beginseg
    name "ovl_Bg_Zg"
    include "build/src/overlays/actors/ovl_Bg_Zg/z_bg_zg.o"
    include "build/src/overlays/actors/ovl_Bg_Zg/ovl_Bg_Zg_reloc.o"
endseg

beginseg
    name "ovl_Boss_Dodongo"
    include "build/src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.o"
    include "build/data/overlays/actors/z_boss_dodongo.data.o"
    include "build/data/overlays/actors/z_boss_dodongo.reloc.o"
endseg

beginseg
    name "ovl_Boss_Fd"
    include "build/src/overlays/actors/ovl_Boss_Fd/z_boss_fd.o"
    include "build/data/overlays/actors/z_boss_fd.data.o"
    include "build/data/overlays/actors/z_boss_fd.reloc.o"
endseg

beginseg
    name "ovl_Boss_Fd2"
    include "build/src/overlays/actors/ovl_Boss_Fd2/z_boss_fd2.o"
    include "build/data/overlays/actors/z_boss_fd2.data.o"
    include "build/data/overlays/actors/z_boss_fd2.reloc.o"
endseg

beginseg
    name "ovl_Boss_Ganon"
    include "build/src/overlays/actors/ovl_Boss_Ganon/z_boss_ganon.o"
    include "build/data/overlays/actors/z_boss_ganon.data.o"
    include "build/data/overlays/actors/z_boss_ganon.bss.o"
    include "build/data/overlays/actors/z_boss_ganon.reloc.o"
endseg

beginseg
    name "ovl_Boss_Ganon2"
    include "build/src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.o"
    include "build/data/overlays/actors/z_boss_ganon2.data.o"
    include "build/data/overlays/actors/z_boss_ganon2.bss.o"
    include "build/data/overlays/actors/z_boss_ganon2.reloc.o"
endseg

beginseg
    name "ovl_Boss_Ganondrof"
    include "build/src/overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.o"
    include "build/data/overlays/actors/z_boss_ganondrof.data.o"
    include "build/data/overlays/actors/z_boss_ganondrof.reloc.o"
endseg

beginseg
    name "ovl_Boss_Goma"
    include "build/src/overlays/actors/ovl_Boss_Goma/z_boss_goma.o"
    include "build/data/overlays/actors/z_boss_goma.data.o"
    include "build/data/overlays/actors/z_boss_goma.reloc.o"
endseg

beginseg
    name "ovl_Boss_Mo"
    include "build/src/overlays/actors/ovl_Boss_Mo/z_boss_mo.o"
    include "build/data/overlays/actors/z_boss_mo.data.o"
    include "build/data/overlays/actors/z_boss_mo.bss.o"
    include "build/data/overlays/actors/z_boss_mo.reloc.o"
endseg

beginseg
    name "ovl_Boss_Sst"
    include "build/src/overlays/actors/ovl_Boss_Sst/z_boss_sst.o"
    include "build/data/overlays/actors/z_boss_sst.data.o"
    include "build/data/overlays/actors/z_boss_sst.bss.o"
    include "build/data/overlays/actors/z_boss_sst.reloc.o"
endseg

beginseg
    name "ovl_Boss_Tw"
    include "build/src/overlays/actors/ovl_Boss_Tw/z_boss_tw.o"
    include "build/data/overlays/actors/z_boss_tw.data.o"
    include "build/data/overlays/actors/z_boss_tw.bss.o"
    include "build/data/overlays/actors/z_boss_tw.reloc.o"
endseg

beginseg
    name "ovl_Boss_Va"
    include "build/src/overlays/actors/ovl_Boss_Va/z_boss_va.o"
    include "build/data/overlays/actors/z_boss_va.data.o"
    include "build/data/overlays/actors/z_boss_va.bss.o"
    include "build/data/overlays/actors/z_boss_va.reloc.o"
endseg

beginseg
    name "ovl_Demo_6K"
    include "build/src/overlays/actors/ovl_Demo_6K/z_demo_6k.o"
    include "build/data/overlays/actors/z_demo_6k.data.o"
    include "build/data/overlays/actors/z_demo_6k.reloc.o"
endseg

beginseg
    name "ovl_Demo_Du"
    include "build/src/overlays/actors/ovl_Demo_Du/z_demo_du.o"
    include "build/data/overlays/actors/z_demo_du.data.o"
    include "build/data/overlays/actors/z_demo_du.reloc.o"
endseg

beginseg
    name "ovl_Demo_Ec"
    include "build/src/overlays/actors/ovl_Demo_Ec/z_demo_ec.o"
    include "build/data/overlays/actors/z_demo_ec.data.o"
    include "build/data/overlays/actors/z_demo_ec.reloc.o"
endseg

beginseg
    name "ovl_Demo_Effect"
    include "build/src/overlays/actors/ovl_Demo_Effect/z_demo_effect.o"
    include "build/data/overlays/actors/z_demo_effect.data.o"
    include "build/data/overlays/actors/z_demo_effect.reloc.o"
endseg

beginseg
    name "ovl_Demo_Ext"
    include "build/src/overlays/actors/ovl_Demo_Ext/z_demo_ext.o"
    include "build/data/overlays/actors/z_demo_ext.data.o"
    include "build/data/overlays/actors/z_demo_ext.reloc.o"
endseg

beginseg
    name "ovl_Demo_Geff"
    include "build/src/overlays/actors/ovl_Demo_Geff/z_demo_geff.o"
    include "build/src/overlays/actors/ovl_Demo_Geff/ovl_Demo_Geff_reloc.o"
endseg

beginseg
    name "ovl_Demo_Gj"
    include "build/src/overlays/actors/ovl_Demo_Gj/z_demo_gj.o"
    include "build/data/overlays/actors/z_demo_gj.data.o"
    include "build/data/overlays/actors/z_demo_gj.reloc.o"
endseg

beginseg
    name "ovl_Demo_Go"
    include "build/src/overlays/actors/ovl_Demo_Go/z_demo_go.o"
    include "build/src/overlays/actors/ovl_Demo_Go/ovl_Demo_Go_reloc.o"
endseg

beginseg
    name "ovl_Demo_Gt"
    include "build/src/overlays/actors/ovl_Demo_Gt/z_demo_gt.o"
    include "build/data/overlays/actors/z_demo_gt.data.o"
    include "build/data/overlays/actors/z_demo_gt.reloc.o"
endseg

beginseg
    name "ovl_Demo_Ik"
    include "build/src/overlays/actors/ovl_Demo_Ik/z_demo_ik.o"
    include "build/data/overlays/actors/z_demo_ik.data.o"
    include "build/data/overlays/actors/z_demo_ik.reloc.o"
endseg

beginseg
    name "ovl_Demo_Im"
    include "build/src/overlays/actors/ovl_Demo_Im/z_demo_im.o"
    include "build/src/overlays/actors/ovl_Demo_Im/ovl_Demo_Im_reloc.o"
endseg

beginseg
    name "ovl_Demo_Kankyo"
    include "build/src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.o"
    include "build/data/overlays/actors/z_demo_kankyo.data.o"
    include "build/data/overlays/actors/z_demo_kankyo.bss.o"
    include "build/data/overlays/actors/z_demo_kankyo.reloc.o"
endseg

beginseg
    name "ovl_Demo_Kekkai"
    include "build/src/overlays/actors/ovl_Demo_Kekkai/z_demo_kekkai.o"
    include "build/data/overlays/actors/z_demo_kekkai.data.o"
    include "build/data/overlays/actors/z_demo_kekkai.reloc.o"
endseg

beginseg
    name "ovl_Demo_Sa"
    include "build/src/overlays/actors/ovl_Demo_Sa/z_demo_sa.o"
    include "build/src/overlays/actors/ovl_Demo_Sa/ovl_Demo_Sa_reloc.o"
endseg

beginseg
    name "ovl_Demo_Shd"
    include "build/src/overlays/actors/ovl_Demo_Shd/z_demo_shd.o"
    include "build/data/overlays/actors/z_demo_shd.data.o"
    include "build/data/overlays/actors/z_demo_shd.reloc.o"
endseg

beginseg
    name "ovl_Demo_Tre_Lgt"
    include "build/src/overlays/actors/ovl_Demo_Tre_Lgt/z_demo_tre_lgt.o"
    include "build/data/overlays/actors/z_demo_tre_lgt.data.o"
    include "build/data/overlays/actors/z_demo_tre_lgt.reloc.o"
endseg

beginseg
    name "ovl_Door_Ana"
    include "build/src/overlays/actors/ovl_Door_Ana/z_door_ana.o"
    include "build/src/overlays/actors/ovl_Door_Ana/ovl_Door_Ana_reloc.o"
endseg

beginseg
    name "ovl_Door_Gerudo"
    include "build/src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.o"
    include "build/src/overlays/actors/ovl_Door_Gerudo/ovl_Door_Gerudo_reloc.o"
endseg

beginseg
    name "ovl_Door_Killer"
    include "build/src/overlays/actors/ovl_Door_Killer/z_door_killer.o"
    include "build/data/overlays/actors/z_door_killer.data.o"
    include "build/data/overlays/actors/z_door_killer.reloc.o"
endseg

beginseg
    name "ovl_Door_Shutter"
    include "build/src/overlays/actors/ovl_Door_Shutter/z_door_shutter.o"
    include "build/data/overlays/actors/z_door_shutter.data.o"
    include "build/data/overlays/actors/z_door_shutter.reloc.o"
endseg

beginseg
    name "ovl_Door_Toki"
    include "build/src/overlays/actors/ovl_Door_Toki/z_door_toki.o"
    include "build/src/overlays/actors/ovl_Door_Toki/ovl_Door_Toki_reloc.o"
endseg

beginseg
    name "ovl_Door_Warp1"
    include "build/src/overlays/actors/ovl_Door_Warp1/z_door_warp1.o"
    include "build/data/overlays/actors/z_door_warp1.data.o"
    include "build/data/overlays/actors/z_door_warp1.bss.o"
    include "build/data/overlays/actors/z_door_warp1.reloc.o"
endseg

beginseg
    name "ovl_Efc_Erupc"
    include "build/src/overlays/actors/ovl_Efc_Erupc/z_efc_erupc.o"
    include "build/data/overlays/actors/z_efc_erupc.data.o"
    include "build/data/overlays/actors/z_efc_erupc.reloc.o"
endseg

beginseg
    name "ovl_Eff_Dust"
    include "build/src/overlays/actors/ovl_Eff_Dust/z_eff_dust.o"
    include "build/data/overlays/actors/z_eff_dust.data.o"
    include "build/data/overlays/actors/z_eff_dust.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Blast"
    include "build/src/overlays/effects/ovl_Effect_Ss_Blast/z_eff_ss_blast.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Blast/ovl_Effect_Ss_Blast_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Bomb"
    include "build/src/overlays/effects/ovl_Effect_Ss_Bomb/z_eff_ss_bomb.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Bomb/ovl_Effect_Ss_Bomb_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Bomb2"
    include "build/src/overlays/effects/ovl_Effect_Ss_Bomb2/z_eff_ss_bomb2.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Bomb2/ovl_Effect_Ss_Bomb2_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Bubble"
    include "build/src/overlays/effects/ovl_Effect_Ss_Bubble/z_eff_ss_bubble.o"
    include "build/data/overlays/effects/z_eff_ss_bubble.data.o"
    include "build/data/overlays/effects/z_eff_ss_bubble.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_D_Fire"
    include "build/src/overlays/effects/ovl_Effect_Ss_D_Fire/z_eff_ss_d_fire.o"
    include "build/data/overlays/effects/z_eff_ss_d_fire.data.o"
    include "build/data/overlays/effects/z_eff_ss_d_fire.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dead_Db"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dead_Db/z_eff_ss_dead_db.o"
    include "build/data/overlays/effects/z_eff_ss_dead_db.data.o"
    include "build/data/overlays/effects/z_eff_ss_dead_db.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dead_Dd"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dead_Dd/z_eff_ss_dead_dd.o"
    include "build/data/overlays/effects/z_eff_ss_dead_dd.data.o"
    include "build/data/overlays/effects/z_eff_ss_dead_dd.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dead_Ds"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dead_Ds/z_eff_ss_dead_ds.o"
    include "build/data/overlays/effects/z_eff_ss_dead_ds.data.o"
    include "build/data/overlays/effects/z_eff_ss_dead_ds.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dead_Sound"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dead_Sound/ovl_Effect_Ss_Dead_Sound_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dt_Bubble"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dt_Bubble/z_eff_ss_dt_bubble.o"
    include "build/data/overlays/effects/z_eff_ss_dt_bubble.data.o"
    include "build/data/overlays/effects/z_eff_ss_dt_bubble.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Dust"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dust/z_eff_ss_dust.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Dust/ovl_Effect_Ss_Dust_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_En_Fire"
    include "build/src/overlays/effects/ovl_Effect_Ss_En_Fire/z_eff_ss_en_fire.o"
    include "build/data/overlays/effects/z_eff_ss_en_fire.data.o"
    include "build/data/overlays/effects/z_eff_ss_en_fire.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_En_Ice"
    include "build/src/overlays/effects/ovl_Effect_Ss_En_Ice/z_eff_ss_en_ice.o"
    include "build/data/overlays/effects/z_eff_ss_en_ice.data.o"
    include "build/data/overlays/effects/z_eff_ss_en_ice.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Extra"
    include "build/src/overlays/effects/ovl_Effect_Ss_Extra/z_eff_ss_extra.o"
    include "build/data/overlays/effects/z_eff_ss_extra.data.o"
    include "build/data/overlays/effects/z_eff_ss_extra.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Fcircle"
    include "build/src/overlays/effects/ovl_Effect_Ss_Fcircle/z_eff_ss_fcircle.o"
    include "build/data/overlays/effects/z_eff_ss_fcircle.data.o"
    include "build/data/overlays/effects/z_eff_ss_fcircle.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Fhg_Flash"
    include "build/src/overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.o"
    include "build/data/overlays/effects/z_eff_ss_fhg_flash.data.o"
    include "build/data/overlays/effects/z_eff_ss_fhg_flash.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Fire_Tail"
    include "build/src/overlays/effects/ovl_Effect_Ss_Fire_Tail/z_eff_ss_fire_tail.o"
    include "build/data/overlays/effects/z_eff_ss_fire_tail.data.o"
    include "build/data/overlays/effects/z_eff_ss_fire_tail.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Fire"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Fire/z_eff_ss_g_fire.o"
    include "build/data/overlays/effects/z_eff_ss_g_fire.data.o"
    include "build/data/overlays/effects/z_eff_ss_g_fire.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Magma"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Magma/z_eff_ss_g_magma.o"
    include "build/data/overlays/effects/z_eff_ss_g_magma.data.o"
    include "build/data/overlays/effects/z_eff_ss_g_magma.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Magma2"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Magma2/z_eff_ss_g_magma2.o"
    include "build/data/overlays/effects/z_eff_ss_g_magma2.data.o"
    include "build/data/overlays/effects/z_eff_ss_g_magma2.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Ripple"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Ripple/z_eff_ss_g_ripple.o"
    include "build/data/overlays/effects/z_eff_ss_g_ripple.data.o"
    include "build/data/overlays/effects/z_eff_ss_g_ripple.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Spk"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Spk/z_eff_ss_g_spk.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Spk/ovl_Effect_Ss_G_Spk_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_G_Splash"
    include "build/src/overlays/effects/ovl_Effect_Ss_G_Splash/z_eff_ss_g_splash.o"
    include "build/data/overlays/effects/z_eff_ss_g_splash.data.o"
    include "build/data/overlays/effects/z_eff_ss_g_splash.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Hahen"
    include "build/src/overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.o"
    include "build/data/overlays/effects/z_eff_ss_hahen.data.o"
    include "build/data/overlays/effects/z_eff_ss_hahen.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_HitMark"
    include "build/src/overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.o"
    include "build/data/overlays/effects/z_eff_ss_hitmark.data.o"
    include "build/data/overlays/effects/z_eff_ss_hitmark.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Ice_Piece"
    include "build/src/overlays/effects/ovl_Effect_Ss_Ice_Piece/z_eff_ss_ice_piece.o"
    include "build/data/overlays/effects/z_eff_ss_ice_piece.data.o"
    include "build/data/overlays/effects/z_eff_ss_ice_piece.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Ice_Smoke"
    include "build/src/overlays/effects/ovl_Effect_Ss_Ice_Smoke/z_eff_ss_ice_smoke.o"
    include "build/data/overlays/effects/z_eff_ss_ice_smoke.data.o"
    include "build/data/overlays/effects/z_eff_ss_ice_smoke.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_K_Fire"
    include "build/src/overlays/effects/ovl_Effect_Ss_K_Fire/z_eff_ss_k_fire.o"
    include "build/data/overlays/effects/z_eff_ss_k_fire.data.o"
    include "build/data/overlays/effects/z_eff_ss_k_fire.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Kakera"
    include "build/src/overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.o"
    include "build/data/overlays/effects/z_eff_ss_kakera.data.o"
    include "build/data/overlays/effects/z_eff_ss_kakera.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_KiraKira"
    include "build/src/overlays/effects/ovl_Effect_Ss_KiraKira/z_eff_ss_kirakira.o"
    include "build/data/overlays/effects/z_eff_ss_kirakira.data.o"
    include "build/data/overlays/effects/z_eff_ss_kirakira.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Lightning"
    include "build/src/overlays/effects/ovl_Effect_Ss_Lightning/z_eff_ss_lightning.o"
    include "build/data/overlays/effects/z_eff_ss_lightning.data.o"
    include "build/data/overlays/effects/z_eff_ss_lightning.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Sibuki"
    include "build/src/overlays/effects/ovl_Effect_Ss_Sibuki/z_eff_ss_sibuki.o"
    include "build/data/overlays/effects/z_eff_ss_sibuki.data.o"
    include "build/data/overlays/effects/z_eff_ss_sibuki.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Sibuki2"
    include "build/src/overlays/effects/ovl_Effect_Ss_Sibuki2/z_eff_ss_sibuki2.o"
    include "build/data/overlays/effects/z_eff_ss_sibuki2.data.o"
    include "build/data/overlays/effects/z_eff_ss_sibuki2.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Solder_Srch_Ball"
    include "build/src/overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.o"
    include "build/src/overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/ovl_Effect_Ss_Solder_Srch_Ball_reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Stick"
    include "build/src/overlays/effects/ovl_Effect_Ss_Stick/z_eff_ss_stick.o"
    include "build/data/overlays/effects/z_eff_ss_stick.data.o"
    include "build/data/overlays/effects/z_eff_ss_stick.reloc.o"
endseg

beginseg
    name "ovl_Effect_Ss_Stone1"
    include "build/src/overlays/effects/ovl_Effect_Ss_Stone1/z_eff_ss_stone1.o"
    include "build/data/overlays/effects/z_eff_ss_stone1.data.o"
    include "build/data/overlays/effects/z_eff_ss_stone1.reloc.o"
endseg

beginseg
    name "ovl_Elf_Msg"
    include "build/src/overlays/actors/ovl_Elf_Msg/z_elf_msg.o"
    include "build/data/overlays/actors/z_elf_msg.data.o"
    include "build/data/overlays/actors/z_elf_msg.reloc.o"
endseg

beginseg
    name "ovl_Elf_Msg2"
    include "build/src/overlays/actors/ovl_Elf_Msg2/z_elf_msg2.o"
    include "build/data/overlays/actors/z_elf_msg2.data.o"
    include "build/data/overlays/actors/z_elf_msg2.reloc.o"
endseg

beginseg
    name "ovl_En_Am"
    include "build/src/overlays/actors/ovl_En_Am/z_en_am.o"
    include "build/data/overlays/actors/z_en_am.data.o"
    include "build/data/overlays/actors/z_en_am.reloc.o"
endseg

beginseg
    name "ovl_En_Ani"
    include "build/src/overlays/actors/ovl_En_Ani/z_en_ani.o"
    include "build/src/overlays/actors/ovl_En_Ani/ovl_En_Ani_reloc.o"
endseg

beginseg
    name "ovl_En_Anubice"
    include "build/src/overlays/actors/ovl_En_Anubice/z_en_anubice.o"
    include "build/data/overlays/actors/z_en_anubice.data.o"
    include "build/data/overlays/actors/z_en_anubice.reloc.o"
endseg

beginseg
    name "ovl_En_Anubice_Fire"
    include "build/src/overlays/actors/ovl_En_Anubice_Fire/z_en_anubice_fire.o"
    include "build/data/overlays/actors/z_en_anubice_fire.data.o"
    include "build/data/overlays/actors/z_en_anubice_fire.reloc.o"
endseg

beginseg
    name "ovl_En_Anubice_Tag"
    include "build/src/overlays/actors/ovl_En_Anubice_Tag/z_en_anubice_tag.o"
    include "build/src/overlays/actors/ovl_En_Anubice_Tag/ovl_En_Anubice_Tag_reloc.o"
endseg

beginseg
    name "ovl_En_Arow_Trap"
    include "build/src/overlays/actors/ovl_En_Arow_Trap/z_en_arow_trap.o"
    include "build/src/overlays/actors/ovl_En_Arow_Trap/ovl_En_Arow_Trap_reloc.o"
endseg

beginseg
    name "ovl_En_Arrow"
    include "build/src/overlays/actors/ovl_En_Arrow/z_en_arrow.o"
    include "build/data/overlays/actors/z_en_arrow.data.o"
    include "build/data/overlays/actors/z_en_arrow.reloc.o"
endseg

beginseg
    name "ovl_En_Attack_Niw"
    include "build/src/overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.o"
    include "build/data/overlays/actors/z_en_attack_niw.data.o"
    include "build/data/overlays/actors/z_en_attack_niw.reloc.o"
endseg

beginseg
    name "ovl_En_Ba"
    include "build/src/overlays/actors/ovl_En_Ba/z_en_ba.o"
    include "build/data/overlays/actors/z_en_ba.data.o"
    include "build/data/overlays/actors/z_en_ba.reloc.o"
endseg

beginseg
    name "ovl_En_Bb"
    include "build/src/overlays/actors/ovl_En_Bb/z_en_bb.o"
    include "build/data/overlays/actors/z_en_bb.data.o"
    include "build/data/overlays/actors/z_en_bb.reloc.o"
endseg

beginseg
    name "ovl_En_Bdfire"
    include "build/src/overlays/actors/ovl_En_Bdfire/z_en_bdfire.o"
    include "build/data/overlays/actors/z_en_bdfire.data.o"
    include "build/data/overlays/actors/z_en_bdfire.reloc.o"
endseg

beginseg
    name "ovl_En_Bigokuta"
    include "build/src/overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.o"
    include "build/data/overlays/actors/z_en_bigokuta.data.o"
    include "build/data/overlays/actors/z_en_bigokuta.reloc.o"
endseg

beginseg
    name "ovl_En_Bili"
    include "build/src/overlays/actors/ovl_En_Bili/z_en_bili.o"
    include "build/data/overlays/actors/z_en_bili.data.o"
    include "build/data/overlays/actors/z_en_bili.reloc.o"
endseg

beginseg
    name "ovl_En_Bird"
    include "build/src/overlays/actors/ovl_En_Bird/z_en_bird.o"
    include "build/data/overlays/actors/z_en_bird.reloc.o"
endseg

beginseg
    name "ovl_En_Blkobj"
    include "build/src/overlays/actors/ovl_En_Blkobj/z_en_blkobj.o"
    include "build/src/overlays/actors/ovl_En_Blkobj/ovl_En_Blkobj_reloc.o"
endseg

beginseg
    name "ovl_En_Bom"
    include "build/src/overlays/actors/ovl_En_Bom/z_en_bom.o"
    include "build/src/overlays/actors/ovl_En_Bom/ovl_En_Bom_reloc.o"
endseg

beginseg
    name "ovl_En_Bom_Bowl_Man"
    include "build/src/overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.o"
    include "build/data/overlays/actors/z_en_bom_bowl_man.data.o"
    include "build/data/overlays/actors/z_en_bom_bowl_man.reloc.o"
endseg

beginseg
    name "ovl_En_Bom_Bowl_Pit"
    include "build/src/overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.o"
    include "build/data/overlays/actors/z_en_bom_bowl_pit.data.o"
    include "build/data/overlays/actors/z_en_bom_bowl_pit.reloc.o"
endseg

beginseg
    name "ovl_En_Bom_Chu"
    include "build/src/overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.o"
    include "build/data/overlays/actors/z_en_bom_chu.data.o"
    include "build/data/overlays/actors/z_en_bom_chu.reloc.o"
endseg

beginseg
    name "ovl_En_Bombf"
    include "build/src/overlays/actors/ovl_En_Bombf/z_en_bombf.o"
    include "build/src/overlays/actors/ovl_En_Bombf/ovl_En_Bombf_reloc.o"
endseg

beginseg
    name "ovl_En_Boom"
    include "build/src/overlays/actors/ovl_En_Boom/z_en_boom.o"
    include "build/src/overlays/actors/ovl_En_Boom/ovl_En_Boom_reloc.o"
endseg

beginseg
    name "ovl_En_Box"
    include "build/src/overlays/actors/ovl_En_Box/z_en_box.o"
    include "build/data/overlays/actors/z_en_box.data.o"
    include "build/data/overlays/actors/z_en_box.bss.o"
    include "build/data/overlays/actors/z_en_box.reloc.o"
endseg

beginseg
    name "ovl_En_Brob"
    include "build/src/overlays/actors/ovl_En_Brob/z_en_brob.o"
    include "build/data/overlays/actors/z_en_brob.data.o"
    include "build/data/overlays/actors/z_en_brob.reloc.o"
endseg

beginseg
    name "ovl_En_Bubble"
    include "build/src/overlays/actors/ovl_En_Bubble/z_en_bubble.o"
    include "build/data/overlays/actors/z_en_bubble.data.o"
    include "build/data/overlays/actors/z_en_bubble.reloc.o"
endseg

beginseg
    name "ovl_En_Butte"
    include "build/src/overlays/actors/ovl_En_Butte/z_en_butte.o"
    include "build/data/overlays/actors/z_en_butte.data.o"
    include "build/data/overlays/actors/z_en_butte.reloc.o"
endseg

beginseg
    name "ovl_En_Bw"
    include "build/src/overlays/actors/ovl_En_Bw/z_en_bw.o"
    include "build/data/overlays/actors/z_en_bw.data.o"
    include "build/data/overlays/actors/z_en_bw.reloc.o"
endseg

beginseg
    name "ovl_En_Bx"
    include "build/src/overlays/actors/ovl_En_Bx/z_en_bx.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_En_Bx/ovl_En_Bx_reloc.o"
#else
    include "build/data/overlays/actors/z_en_bx.reloc.o"
#endif
endseg

beginseg
    name "ovl_En_Changer"
    include "build/src/overlays/actors/ovl_En_Changer/z_en_changer.o"
    include "build/data/overlays/actors/z_en_changer.data.o"
    include "build/data/overlays/actors/z_en_changer.reloc.o"
endseg

beginseg
    name "ovl_En_Clear_Tag"
    include "build/src/overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.o"
    include "build/data/overlays/actors/z_en_clear_tag.data.o"
    include "build/data/overlays/actors/z_en_clear_tag.bss.o"
    include "build/data/overlays/actors/z_en_clear_tag.reloc.o"
endseg

beginseg
    name "ovl_En_Cow"
    include "build/src/overlays/actors/ovl_En_Cow/z_en_cow.o"
    include "build/src/overlays/actors/ovl_En_Cow/ovl_En_Cow_reloc.o"
endseg

beginseg
    name "ovl_En_Crow"
    include "build/src/overlays/actors/ovl_En_Crow/z_en_crow.o"
    include "build/data/overlays/actors/z_en_crow.data.o"
    include "build/data/overlays/actors/z_en_crow.reloc.o"
endseg

beginseg
    name "ovl_En_Cs"
    include "build/src/overlays/actors/ovl_En_Cs/z_en_cs.o"
    include "build/data/overlays/actors/z_en_cs.data.o"
    include "build/data/overlays/actors/z_en_cs.reloc.o"
endseg

beginseg
    name "ovl_En_Daiku"
    include "build/src/overlays/actors/ovl_En_Daiku/z_en_daiku.o"
    include "build/data/overlays/actors/z_en_daiku.data.o"
    include "build/data/overlays/actors/z_en_daiku.reloc.o"
endseg

beginseg
    name "ovl_En_Daiku_Kakariko"
    include "build/src/overlays/actors/ovl_En_Daiku_Kakariko/z_en_daiku_kakariko.o"
    include "build/data/overlays/actors/z_en_daiku_kakariko.data.o"
    include "build/data/overlays/actors/z_en_daiku_kakariko.reloc.o"
endseg

beginseg
    name "ovl_En_Dekubaba"
    include "build/src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.o"
    include "build/data/overlays/actors/z_en_dekubaba.data.o"
    include "build/data/overlays/actors/z_en_dekubaba.reloc.o"
endseg

beginseg
    name "ovl_En_Dekunuts"
    include "build/src/overlays/actors/ovl_En_Dekunuts/z_en_dekunuts.o"
    include "build/data/overlays/actors/z_en_dekunuts.data.o"
    include "build/data/overlays/actors/z_en_dekunuts.reloc.o"
endseg

beginseg
    name "ovl_En_Dh"
    include "build/src/overlays/actors/ovl_En_Dh/z_en_dh.o"
    include "build/data/overlays/actors/z_en_dh.data.o"
    include "build/data/overlays/actors/z_en_dh.reloc.o"
endseg

beginseg
    name "ovl_En_Dha"
    include "build/src/overlays/actors/ovl_En_Dha/z_en_dha.o"
    include "build/src/overlays/actors/ovl_En_Dha/ovl_En_Dha_reloc.o"
endseg

beginseg
    name "ovl_En_Diving_Game"
    include "build/src/overlays/actors/ovl_En_Diving_Game/z_en_diving_game.o"
    include "build/data/overlays/actors/z_en_diving_game.data.o"
    include "build/data/overlays/actors/z_en_diving_game.reloc.o"
endseg

beginseg
    name "ovl_En_Dns"
    include "build/src/overlays/actors/ovl_En_Dns/z_en_dns.o"
    include "build/data/overlays/actors/z_en_dns.data.o"
    include "build/data/overlays/actors/z_en_dns.reloc.o"
endseg

beginseg
    name "ovl_En_Dnt_Demo"
    include "build/src/overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.o"
    include "build/data/overlays/actors/z_en_dnt_demo.data.o"
    include "build/data/overlays/actors/z_en_dnt_demo.reloc.o"
endseg

beginseg
    name "ovl_En_Dnt_Jiji"
    include "build/src/overlays/actors/ovl_En_Dnt_Jiji/z_en_dnt_jiji.o"
    include "build/data/overlays/actors/z_en_dnt_jiji.data.o"
    include "build/data/overlays/actors/z_en_dnt_jiji.reloc.o"
endseg

beginseg
    name "ovl_En_Dnt_Nomal"
    include "build/src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.o"
    include "build/data/overlays/actors/z_en_dnt_nomal.data.o"
    include "build/data/overlays/actors/z_en_dnt_nomal.reloc.o"
endseg

beginseg
    name "ovl_En_Dodojr"
    include "build/src/overlays/actors/ovl_En_Dodojr/z_en_dodojr.o"
    include "build/data/overlays/actors/z_en_dodojr.data.o"
    include "build/data/overlays/actors/z_en_dodojr.reloc.o"
endseg

beginseg
    name "ovl_En_Dodongo"
    include "build/src/overlays/actors/ovl_En_Dodongo/z_en_dodongo.o"
    include "build/data/overlays/actors/z_en_dodongo.data.o"
    include "build/data/overlays/actors/z_en_dodongo.reloc.o"
endseg

beginseg
    name "ovl_En_Dog"
    include "build/src/overlays/actors/ovl_En_Dog/z_en_dog.o"
    include "build/src/overlays/actors/ovl_En_Dog/ovl_En_Dog_reloc.o"
endseg

beginseg
    name "ovl_En_Door"
    include "build/src/overlays/actors/ovl_En_Door/z_en_door.o"
    include "build/data/overlays/actors/z_en_door.data.o"
    include "build/data/overlays/actors/z_en_door.reloc.o"
endseg

beginseg
    name "ovl_En_Ds"
    include "build/src/overlays/actors/ovl_En_Ds/z_en_ds.o"
    include "build/src/overlays/actors/ovl_En_Ds/ovl_En_Ds_reloc.o"
endseg

beginseg
    name "ovl_En_Du"
    include "build/src/overlays/actors/ovl_En_Du/z_en_du.o"
    include "build/data/overlays/actors/z_en_du.data.o"
    include "build/data/overlays/actors/z_en_du.reloc.o"
endseg

beginseg
    name "ovl_En_Dy_Extra"
    include "build/src/overlays/actors/ovl_En_Dy_Extra/z_en_dy_extra.o"
    include "build/data/overlays/actors/z_en_dy_extra.data.o"
    include "build/data/overlays/actors/z_en_dy_extra.reloc.o"
endseg

beginseg
    name "ovl_En_Eg"
    include "build/src/overlays/actors/ovl_En_Eg/z_en_eg.o"
    include "build/src/overlays/actors/ovl_En_Eg/ovl_En_Eg_reloc.o"
endseg

beginseg
    name "ovl_En_Eiyer"
    include "build/src/overlays/actors/ovl_En_Eiyer/z_en_eiyer.o"
    include "build/data/overlays/actors/z_en_eiyer.data.o"
    include "build/data/overlays/actors/z_en_eiyer.reloc.o"
endseg

beginseg
    name "ovl_En_Elf"
    include "build/src/overlays/actors/ovl_En_Elf/z_en_elf.o"
    include "build/data/overlays/actors/z_en_elf.data.o"
    include "build/data/overlays/actors/z_en_elf.reloc.o"
endseg

beginseg
    name "ovl_En_Encount1"
    include "build/src/overlays/actors/ovl_En_Encount1/z_en_encount1.o"
    include "build/data/overlays/actors/z_en_encount1.data.o"
    include "build/data/overlays/actors/z_en_encount1.reloc.o"
endseg

beginseg
    name "ovl_En_Encount2"
    include "build/src/overlays/actors/ovl_En_Encount2/z_en_encount2.o"
    include "build/data/overlays/actors/z_en_encount2.data.o"
    include "build/data/overlays/actors/z_en_encount2.reloc.o"
endseg

beginseg
    name "ovl_En_Ex_Item"
    include "build/src/overlays/actors/ovl_En_Ex_Item/z_en_ex_item.o"
    include "build/data/overlays/actors/z_en_ex_item.data.o"
    include "build/data/overlays/actors/z_en_ex_item.reloc.o"
endseg

beginseg
    name "ovl_En_Ex_Ruppy"
    include "build/src/overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.o"
    include "build/data/overlays/actors/z_en_ex_ruppy.data.o"
    include "build/data/overlays/actors/z_en_ex_ruppy.reloc.o"
endseg

beginseg
    name "ovl_En_Fd"
    include "build/src/overlays/actors/ovl_En_Fd/z_en_fd.o"
    include "build/data/overlays/actors/z_en_fd.data.o"
    include "build/data/overlays/actors/z_en_fd.reloc.o"
endseg

beginseg
    name "ovl_En_Fd_Fire"
    include "build/src/overlays/actors/ovl_En_Fd_Fire/z_en_fd_fire.o"
    include "build/data/overlays/actors/z_en_fd_fire.data.o"
    include "build/data/overlays/actors/z_en_fd_fire.reloc.o"
endseg

beginseg
    name "ovl_En_Fhg_Fire"
    include "build/src/overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.o"
    include "build/data/overlays/actors/z_en_fhg_fire.data.o"
    include "build/data/overlays/actors/z_en_fhg_fire.reloc.o"
endseg

beginseg
    name "ovl_En_Fire_Rock"
    include "build/src/overlays/actors/ovl_En_Fire_Rock/z_en_fire_rock.o"
    include "build/data/overlays/actors/z_en_fire_rock.data.o"
    include "build/data/overlays/actors/z_en_fire_rock.reloc.o"
endseg

beginseg
    name "ovl_En_Firefly"
    include "build/src/overlays/actors/ovl_En_Firefly/z_en_firefly.o"
    include "build/data/overlays/actors/z_en_firefly.data.o"
    include "build/data/overlays/actors/z_en_firefly.reloc.o"
endseg

beginseg
    name "ovl_En_Fish"
    include "build/src/overlays/actors/ovl_En_Fish/z_en_fish.o"
    include "build/data/overlays/actors/z_en_fish.data.o"
    include "build/data/overlays/actors/z_en_fish.reloc.o"
endseg

beginseg
    name "ovl_En_Floormas"
    include "build/src/overlays/actors/ovl_En_Floormas/z_en_floormas.o"
    include "build/src/overlays/actors/ovl_En_Floormas/ovl_En_Floormas_reloc.o"
endseg

beginseg
    name "ovl_En_Fr"
    include "build/src/overlays/actors/ovl_En_Fr/z_en_fr.o"
    include "build/data/overlays/actors/z_en_fr.data.o"
    include "build/data/overlays/actors/z_en_fr.reloc.o"
endseg

beginseg
    name "ovl_En_Fu"
    include "build/src/overlays/actors/ovl_En_Fu/z_en_fu.o"
    include "build/src/overlays/actors/ovl_En_Fu/ovl_En_Fu_reloc.o"
endseg

beginseg
    name "ovl_En_Fw"
    include "build/src/overlays/actors/ovl_En_Fw/z_en_fw.o"
    include "build/data/overlays/actors/z_en_fw.data.o"
    include "build/data/overlays/actors/z_en_fw.reloc.o"
endseg

beginseg
    name "ovl_En_Fz"
    include "build/src/overlays/actors/ovl_En_Fz/z_en_fz.o"
    include "build/data/overlays/actors/z_en_fz.data.o"
    include "build/data/overlays/actors/z_en_fz.reloc.o"
endseg

beginseg
    name "ovl_En_G_Switch"
    include "build/src/overlays/actors/ovl_En_G_Switch/z_en_g_switch.o"
    include "build/data/overlays/actors/z_en_g_switch.data.o"
    include "build/data/overlays/actors/z_en_g_switch.reloc.o"
endseg

beginseg
    name "ovl_En_Ganon_Mant"
    include "build/src/overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.o"
    include "build/data/overlays/actors/z_en_ganon_mant.data.o"
    include "build/data/overlays/actors/z_en_ganon_mant.reloc.o"
endseg

beginseg
    name "ovl_En_Ganon_Organ"
    include "build/src/overlays/actors/ovl_En_Ganon_Organ/z_en_ganon_organ.o"
    include "build/data/overlays/actors/z_en_ganon_organ.data.o"
    include "build/data/overlays/actors/z_en_ganon_organ.reloc.o"
endseg

beginseg
    name "ovl_En_Gb"
    include "build/src/overlays/actors/ovl_En_Gb/z_en_gb.o"
    include "build/data/overlays/actors/z_en_gb.data.o"
    include "build/data/overlays/actors/z_en_gb.reloc.o"
endseg

beginseg
    name "ovl_En_Ge1"
    include "build/src/overlays/actors/ovl_En_Ge1/z_en_ge1.o"
    include "build/data/overlays/actors/z_en_ge1.data.o"
    include "build/data/overlays/actors/z_en_ge1.reloc.o"
endseg

beginseg
    name "ovl_En_Ge2"
    include "build/src/overlays/actors/ovl_En_Ge2/z_en_ge2.o"
    include "build/data/overlays/actors/z_en_ge2.data.o"
    include "build/data/overlays/actors/z_en_ge2.reloc.o"
endseg

beginseg
    name "ovl_En_Ge3"
    include "build/src/overlays/actors/ovl_En_Ge3/z_en_ge3.o"
    include "build/data/overlays/actors/z_en_ge3.data.o"
    include "build/data/overlays/actors/z_en_ge3.reloc.o"
endseg

beginseg
    name "ovl_En_GeldB"
    include "build/src/overlays/actors/ovl_En_GeldB/z_en_geldb.o"
    include "build/data/overlays/actors/z_en_geldb.data.o"
    include "build/data/overlays/actors/z_en_geldb.reloc.o"
endseg

beginseg
    name "ovl_En_GirlA"
    include "build/src/overlays/actors/ovl_En_GirlA/z_en_girla.o"
    include "build/src/overlays/actors/ovl_En_GirlA/ovl_En_GirlA_reloc.o"
endseg

beginseg
    name "ovl_En_Gm"
    include "build/src/overlays/actors/ovl_En_Gm/z_en_gm.o"
    include "build/data/overlays/actors/z_en_gm.data.o"
    include "build/data/overlays/actors/z_en_gm.reloc.o"
endseg

beginseg
    name "ovl_En_Go"
    include "build/src/overlays/actors/ovl_En_Go/z_en_go.o"
    include "build/data/overlays/actors/z_en_go.data.o"
    include "build/data/overlays/actors/z_en_go.rodata.o"
    include "build/data/overlays/actors/z_en_go.reloc.o"
endseg

beginseg
    name "ovl_En_Go2"
    include "build/src/overlays/actors/ovl_En_Go2/z_en_go2.o"
    include "build/data/overlays/actors/z_en_go2.data.o"
    include "build/data/overlays/actors/z_en_go2.reloc.o"
endseg

beginseg
    name "ovl_En_Goma"
    include "build/src/overlays/actors/ovl_En_Goma/z_en_goma.o"
    include "build/data/overlays/actors/z_en_goma.data.o"
    include "build/data/overlays/actors/z_en_goma.reloc.o"
endseg

beginseg
    name "ovl_En_Goroiwa"
    include "build/src/overlays/actors/ovl_En_Goroiwa/z_en_goroiwa.o"
    include "build/data/overlays/actors/z_en_goroiwa.data.o"
    include "build/data/overlays/actors/z_en_goroiwa.reloc.o"
endseg

beginseg
    name "ovl_En_Gs"
    include "build/src/overlays/actors/ovl_En_Gs/z_en_gs.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_En_Gs/ovl_En_Gs_reloc.o"
#else
    include "build/data/overlays/actors/z_en_gs.reloc.o"
#endif
endseg

beginseg
    name "ovl_En_Guest"
    include "build/src/overlays/actors/ovl_En_Guest/z_en_guest.o"
    include "build/src/overlays/actors/ovl_En_Guest/ovl_En_Guest_reloc.o"
endseg

beginseg
    name "ovl_En_Hata"
    include "build/src/overlays/actors/ovl_En_Hata/z_en_hata.o"
    include "build/src/overlays/actors/ovl_En_Hata/ovl_En_Hata_reloc.o"
endseg

beginseg
    name "ovl_En_Heishi1"
    include "build/src/overlays/actors/ovl_En_Heishi1/z_en_heishi1.o"
    include "build/src/overlays/actors/ovl_En_Heishi1/ovl_En_Heishi1_reloc.o"
endseg

beginseg
    name "ovl_En_Heishi2"
    include "build/src/overlays/actors/ovl_En_Heishi2/z_en_heishi2.o"
    include "build/src/overlays/actors/ovl_En_Heishi2/ovl_En_Heishi2_reloc.o"
endseg

beginseg
    name "ovl_En_Heishi3"
    include "build/src/overlays/actors/ovl_En_Heishi3/z_en_heishi3.o"
    include "build/src/overlays/actors/ovl_En_Heishi3/ovl_En_Heishi3_reloc.o"
endseg

beginseg
    name "ovl_En_Heishi4"
    include "build/src/overlays/actors/ovl_En_Heishi4/z_en_heishi4.o"
    include "build/data/overlays/actors/z_en_heishi4.data.o"
    include "build/data/overlays/actors/z_en_heishi4.reloc.o"
endseg

beginseg
    name "ovl_En_Hintnuts"
    include "build/src/overlays/actors/ovl_En_Hintnuts/z_en_hintnuts.o"
    include "build/src/overlays/actors/ovl_En_Hintnuts/ovl_En_Hintnuts_reloc.o"
endseg

beginseg
    name "ovl_En_Holl"
    include "build/src/overlays/actors/ovl_En_Holl/z_en_holl.o"
    include "build/data/overlays/actors/z_en_holl.data.o"
    include "build/data/overlays/actors/z_en_holl.reloc.o"
endseg

beginseg
    name "ovl_En_Honotrap"
    include "build/src/overlays/actors/ovl_En_Honotrap/z_en_honotrap.o"
    include "build/data/overlays/actors/z_en_honotrap.data.o"
    include "build/data/overlays/actors/z_en_honotrap.reloc.o"
endseg

beginseg
    name "ovl_En_Horse"
    include "build/src/overlays/actors/ovl_En_Horse/z_en_horse.o"
    include "build/data/overlays/actors/z_en_horse.data.o"
    include "build/data/overlays/actors/z_en_horse.reloc.o"
endseg

beginseg
    name "ovl_En_Horse_Game_Check"
    include "build/src/overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.o"
    include "build/data/overlays/actors/z_en_horse_game_check.data.o"
    include "build/data/overlays/actors/z_en_horse_game_check.reloc.o"
endseg

beginseg
    name "ovl_En_Horse_Ganon"
    include "build/src/overlays/actors/ovl_En_Horse_Ganon/z_en_horse_ganon.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_En_Horse_Ganon/ovl_En_Horse_Ganon_reloc.o"
#else
    include "build/data/overlays/actors/z_en_horse_ganon.reloc.o"
#endif
endseg

beginseg
    name "ovl_En_Horse_Link_Child"
    include "build/src/overlays/actors/ovl_En_Horse_Link_Child/z_en_horse_link_child.o"
    include "build/data/overlays/actors/z_en_horse_link_child.data.o"
    include "build/data/overlays/actors/z_en_horse_link_child.reloc.o"
endseg

beginseg
    name "ovl_En_Horse_Normal"
    include "build/src/overlays/actors/ovl_En_Horse_Normal/z_en_horse_normal.o"
    include "build/data/overlays/actors/z_en_horse_normal.data.o"
    include "build/data/overlays/actors/z_en_horse_normal.reloc.o"
endseg

beginseg
    name "ovl_En_Horse_Zelda"
    include "build/src/overlays/actors/ovl_En_Horse_Zelda/z_en_horse_zelda.o"
    include "build/data/overlays/actors/z_en_horse_zelda.data.o"
    include "build/data/overlays/actors/z_en_horse_zelda.reloc.o"
endseg

beginseg
    name "ovl_En_Hs"
    include "build/src/overlays/actors/ovl_En_Hs/z_en_hs.o"
    include "build/data/overlays/actors/z_en_hs.data.o"
    include "build/data/overlays/actors/z_en_hs.reloc.o"
endseg

beginseg
    name "ovl_En_Hs2"
    include "build/src/overlays/actors/ovl_En_Hs2/z_en_hs2.o"
    include "build/data/overlays/actors/z_en_hs2.data.o"
    include "build/data/overlays/actors/z_en_hs2.reloc.o"
endseg

beginseg
    name "ovl_En_Hy"
    include "build/src/overlays/actors/ovl_En_Hy/z_en_hy.o"
    include "build/data/overlays/actors/z_en_hy.data.o"
    include "build/data/overlays/actors/z_en_hy.reloc.o"
endseg

beginseg
    name "ovl_En_Ice_Hono"
    include "build/src/overlays/actors/ovl_En_Ice_Hono/z_en_ice_hono.o"
    include "build/data/overlays/actors/z_en_ice_hono.data.o"
    include "build/data/overlays/actors/z_en_ice_hono.reloc.o"
endseg

beginseg
    name "ovl_En_Ik"
    include "build/src/overlays/actors/ovl_En_Ik/z_en_ik.o"
    include "build/data/overlays/actors/z_en_ik.data.o"
    include "build/data/overlays/actors/z_en_ik.bss.o"
    include "build/data/overlays/actors/z_en_ik.reloc.o"
endseg

beginseg
    name "ovl_En_In"
    include "build/src/overlays/actors/ovl_En_In/z_en_in.o"
    include "build/data/overlays/actors/z_en_in.data.o"
    include "build/data/overlays/actors/z_en_in.reloc.o"
endseg

beginseg
    name "ovl_En_Insect"
    include "build/src/overlays/actors/ovl_En_Insect/z_en_insect.o"
    include "build/data/overlays/actors/z_en_insect.reloc.o"
endseg

beginseg
    name "ovl_En_Ishi"
    include "build/src/overlays/actors/ovl_En_Ishi/z_en_ishi.o"
    include "build/data/overlays/actors/z_en_ishi.data.o"
    include "build/data/overlays/actors/z_en_ishi.reloc.o"
endseg

beginseg
    name "ovl_En_It"
    include "build/src/overlays/actors/ovl_En_It/z_en_it.o"
    include "build/src/overlays/actors/ovl_En_It/ovl_En_It_reloc.o"
endseg

beginseg
    name "ovl_En_Jj"
    include "build/src/overlays/actors/ovl_En_Jj/z_en_jj.o"
    include "build/data/overlays/actors/z_en_jj.data.o"
    include "build/data/overlays/actors/z_en_jj.reloc.o"
endseg

beginseg
    name "ovl_En_Js"
    include "build/src/overlays/actors/ovl_En_Js/z_en_js.o"
    include "build/data/overlays/actors/z_en_js.data.o"
    include "build/data/overlays/actors/z_en_js.reloc.o"
endseg

beginseg
    name "ovl_En_Jsjutan"
    include "build/src/overlays/actors/ovl_En_Jsjutan/z_en_jsjutan.o"
    include "build/data/overlays/actors/z_en_jsjutan.data.o"
    include "build/data/overlays/actors/z_en_jsjutan.bss.o"
    include "build/data/overlays/actors/z_en_jsjutan.reloc.o"
endseg

beginseg
    name "ovl_En_Kakasi"
    include "build/src/overlays/actors/ovl_En_Kakasi/z_en_kakasi.o"
    include "build/data/overlays/actors/z_en_kakasi.data.o"
    include "build/data/overlays/actors/z_en_kakasi.reloc.o"
endseg

beginseg
    name "ovl_En_Kakasi2"
    include "build/src/overlays/actors/ovl_En_Kakasi2/z_en_kakasi2.o"
    include "build/data/overlays/actors/z_en_kakasi2.data.o"
    include "build/data/overlays/actors/z_en_kakasi2.reloc.o"
endseg

beginseg
    name "ovl_En_Kakasi3"
    include "build/src/overlays/actors/ovl_En_Kakasi3/z_en_kakasi3.o"
    include "build/data/overlays/actors/z_en_kakasi3.data.o"
    include "build/data/overlays/actors/z_en_kakasi3.reloc.o"
endseg

beginseg
    name "ovl_En_Kanban"
    include "build/src/overlays/actors/ovl_En_Kanban/z_en_kanban.o"
    include "build/data/overlays/actors/z_en_kanban.data.o"
    include "build/data/overlays/actors/z_en_kanban.reloc.o"
endseg

beginseg
    name "ovl_En_Karebaba"
    include "build/src/overlays/actors/ovl_En_Karebaba/z_en_karebaba.o"
    include "build/data/overlays/actors/z_en_karebaba.data.o"
    include "build/data/overlays/actors/z_en_karebaba.reloc.o"
endseg

beginseg
    name "ovl_En_Ko"
    include "build/src/overlays/actors/ovl_En_Ko/z_en_ko.o"
    include "build/data/overlays/actors/z_en_ko.data.o"
    include "build/data/overlays/actors/z_en_ko.reloc.o"
endseg

beginseg
    name "ovl_En_Kusa"
    include "build/src/overlays/actors/ovl_En_Kusa/z_en_kusa.o"
    include "build/data/overlays/actors/z_en_kusa.data.o"
    include "build/data/overlays/actors/z_en_kusa.reloc.o"
endseg

beginseg
    name "ovl_En_Kz"
    include "build/src/overlays/actors/ovl_En_Kz/z_en_kz.o"
    include "build/src/overlays/actors/ovl_En_Kz/ovl_En_Kz_reloc.o"
endseg

beginseg
    name "ovl_En_Light"
    include "build/src/overlays/actors/ovl_En_Light/z_en_light.o"
    include "build/data/overlays/actors/z_en_light.data.o"
    include "build/data/overlays/actors/z_en_light.reloc.o"
endseg

beginseg
    name "ovl_En_Lightbox"
    include "build/src/overlays/actors/ovl_En_Lightbox/z_en_lightbox.o"
    include "build/src/overlays/actors/ovl_En_Lightbox/ovl_En_Lightbox_reloc.o"
endseg

beginseg
    name "ovl_En_M_Fire1"
    include "build/src/overlays/actors/ovl_En_M_Fire1/z_en_m_fire1.o"
    include "build/src/overlays/actors/ovl_En_M_Fire1/ovl_En_M_Fire1_reloc.o"
endseg

beginseg
    name "ovl_En_M_Thunder"
    include "build/src/overlays/actors/ovl_En_M_Thunder/z_en_m_thunder.o"
    include "build/data/overlays/actors/z_en_m_thunder.data.o"
    include "build/data/overlays/actors/z_en_m_thunder.reloc.o"
endseg

beginseg
    name "ovl_En_Ma1"
    include "build/src/overlays/actors/ovl_En_Ma1/z_en_ma1.o"
    include "build/src/overlays/actors/ovl_En_Ma1/ovl_En_Ma1_reloc.o"
endseg

beginseg
    name "ovl_En_Ma2"
    include "build/src/overlays/actors/ovl_En_Ma2/z_en_ma2.o"
    include "build/src/overlays/actors/ovl_En_Ma2/ovl_En_Ma2_reloc.o"
endseg

beginseg
    name "ovl_En_Ma3"
    include "build/src/overlays/actors/ovl_En_Ma3/z_en_ma3.o"
    include "build/src/overlays/actors/ovl_En_Ma3/ovl_En_Ma3_reloc.o"
endseg

beginseg
    name "ovl_En_Mag"
    include "build/src/overlays/actors/ovl_En_Mag/z_en_mag.o"
    include "build/data/overlays/actors/z_en_mag.data.o"
    include "build/data/overlays/actors/z_en_mag.reloc.o"
endseg

beginseg
    name "ovl_En_Mb"
    include "build/src/overlays/actors/ovl_En_Mb/z_en_mb.o"
    include "build/data/overlays/actors/z_en_mb.data.o"
    include "build/data/overlays/actors/z_en_mb.reloc.o"
endseg

beginseg
    name "ovl_En_Md"
    include "build/src/overlays/actors/ovl_En_Md/z_en_md.o"
    include "build/src/overlays/actors/ovl_En_Md/ovl_En_Md_reloc.o"
endseg

beginseg
    name "ovl_En_Mk"
    include "build/src/overlays/actors/ovl_En_Mk/z_en_mk.o"
    include "build/data/overlays/actors/z_en_mk.data.o"
    include "build/data/overlays/actors/z_en_mk.reloc.o"
endseg

beginseg
    name "ovl_En_Mm"
    include "build/src/overlays/actors/ovl_En_Mm/z_en_mm.o"
    include "build/data/overlays/actors/z_en_mm.data.o"
    include "build/data/overlays/actors/z_en_mm.reloc.o"
endseg

beginseg
    name "ovl_En_Mm2"
    include "build/src/overlays/actors/ovl_En_Mm2/z_en_mm2.o"
    include "build/data/overlays/actors/z_en_mm2.data.o"
    include "build/data/overlays/actors/z_en_mm2.reloc.o"
endseg

beginseg
    name "ovl_En_Ms"
    include "build/src/overlays/actors/ovl_En_Ms/z_en_ms.o"
    include "build/src/overlays/actors/ovl_En_Ms/ovl_En_Ms_reloc.o"
endseg

beginseg
    name "ovl_En_Mu"
    include "build/src/overlays/actors/ovl_En_Mu/z_en_mu.o"
    include "build/data/overlays/actors/z_en_mu.data.o"
    include "build/data/overlays/actors/z_en_mu.reloc.o"
endseg

beginseg
    name "ovl_En_Nb"
    include "build/src/overlays/actors/ovl_En_Nb/z_en_nb.o"
    include "build/data/overlays/actors/z_en_nb.data.o"
    include "build/data/overlays/actors/z_en_nb.reloc.o"
endseg

beginseg
    name "ovl_En_Niw"
    include "build/src/overlays/actors/ovl_En_Niw/z_en_niw.o"
    include "build/data/overlays/actors/z_en_niw.data.o"
    include "build/data/overlays/actors/z_en_niw.reloc.o"
endseg

beginseg
    name "ovl_En_Niw_Girl"
    include "build/src/overlays/actors/ovl_En_Niw_Girl/z_en_niw_girl.o"
    include "build/data/overlays/actors/z_en_niw_girl.data.o"
    include "build/data/overlays/actors/z_en_niw_girl.reloc.o"
endseg

beginseg
    name "ovl_En_Niw_Lady"
    include "build/src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.o"
    include "build/data/overlays/actors/z_en_niw_lady.data.o"
    include "build/data/overlays/actors/z_en_niw_lady.reloc.o"
endseg

beginseg
    name "ovl_En_Nutsball"
    include "build/src/overlays/actors/ovl_En_Nutsball/z_en_nutsball.o"
    include "build/src/overlays/actors/ovl_En_Nutsball/ovl_En_Nutsball_reloc.o"
endseg

beginseg
    name "ovl_En_Nwc"
    include "build/src/overlays/actors/ovl_En_Nwc/z_en_nwc.o"
    include "build/data/overlays/actors/z_en_nwc.data.o"
    include "build/data/overlays/actors/z_en_nwc.reloc.o"
endseg

beginseg
    name "ovl_En_Ny"
    include "build/src/overlays/actors/ovl_En_Ny/z_en_ny.o"
    include "build/data/overlays/actors/z_en_ny.data.o"
    include "build/data/overlays/actors/z_en_ny.reloc.o"
endseg

beginseg
    name "ovl_En_OE2"
    include "build/src/overlays/actors/ovl_En_OE2/z_en_oe2.o"
    include "build/src/overlays/actors/ovl_En_OE2/ovl_En_OE2_reloc.o"
endseg

beginseg
    name "ovl_En_Okarina_Effect"
    include "build/src/overlays/actors/ovl_En_Okarina_Effect/z_en_okarina_effect.o"
    include "build/src/overlays/actors/ovl_En_Okarina_Effect/ovl_En_Okarina_Effect_reloc.o"
endseg

beginseg
    name "ovl_En_Okarina_Tag"
    include "build/src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag_cutscene_data.o"
    include "build/src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag.o"
    include "build/src/overlays/actors/ovl_En_Okarina_Tag/ovl_En_Okarina_Tag_reloc.o"
endseg

beginseg
    name "ovl_En_Okuta"
    include "build/src/overlays/actors/ovl_En_Okuta/z_en_okuta.o"
    include "build/data/overlays/actors/z_en_okuta.data.o"
    include "build/data/overlays/actors/z_en_okuta.reloc.o"
endseg

beginseg
    name "ovl_En_Ossan"
    include "build/src/overlays/actors/ovl_En_Ossan/z_en_ossan.o"
    include "build/data/overlays/actors/z_en_ossan.data.o"
    include "build/data/overlays/actors/z_en_ossan.reloc.o"
endseg

beginseg
    name "ovl_En_Owl"
    include "build/src/overlays/actors/ovl_En_Owl/z_en_owl.o"
    include "build/data/overlays/actors/z_en_owl.data.o"
    include "build/data/overlays/actors/z_en_owl.reloc.o"
endseg

beginseg
    name "ovl_En_Part"
    include "build/src/overlays/actors/ovl_En_Part/z_en_part.o"
    include "build/data/overlays/actors/z_en_part.data.o"
    include "build/data/overlays/actors/z_en_part.reloc.o"
endseg

beginseg
    name "ovl_En_Peehat"
    include "build/src/overlays/actors/ovl_En_Peehat/z_en_peehat.o"
    include "build/data/overlays/actors/z_en_peehat.data.o"
    include "build/data/overlays/actors/z_en_peehat.reloc.o"
endseg

beginseg
    name "ovl_En_Po_Desert"
    include "build/src/overlays/actors/ovl_En_Po_Desert/z_en_po_desert.o"
    include "build/data/overlays/actors/z_en_po_desert.data.o"
    include "build/data/overlays/actors/z_en_po_desert.reloc.o"
endseg

beginseg
    name "ovl_En_Po_Field"
    include "build/src/overlays/actors/ovl_En_Po_Field/z_en_po_field.o"
    include "build/data/overlays/actors/z_en_po_field.data.o"
    include "build/data/overlays/actors/z_en_po_field.bss.o"
    include "build/data/overlays/actors/z_en_po_field.reloc.o"
endseg

beginseg
    name "ovl_En_Po_Relay"
    include "build/src/overlays/actors/ovl_En_Po_Relay/z_en_po_relay.o"
    include "build/data/overlays/actors/z_en_po_relay.data.o"
    include "build/data/overlays/actors/z_en_po_relay.reloc.o"
endseg

beginseg
    name "ovl_En_Po_Sisters"
    include "build/src/overlays/actors/ovl_En_Po_Sisters/z_en_po_sisters.o"
    include "build/data/overlays/actors/z_en_po_sisters.data.o"
    include "build/data/overlays/actors/z_en_po_sisters.reloc.o"
endseg

beginseg
    name "ovl_En_Poh"
    include "build/src/overlays/actors/ovl_En_Poh/z_en_poh.o"
    include "build/data/overlays/actors/z_en_poh.data.o"
    include "build/data/overlays/actors/z_en_poh.reloc.o"
endseg

beginseg
    name "ovl_En_Pu_box"
    include "build/src/overlays/actors/ovl_En_Pu_box/z_en_pu_box.o"
    include "build/src/overlays/actors/ovl_En_Pu_box/ovl_En_Pu_box_reloc.o"
endseg

beginseg
    name "ovl_En_Rd"
    include "build/src/overlays/actors/ovl_En_Rd/z_en_rd.o"
    include "build/data/overlays/actors/z_en_rd.data.o"
    include "build/data/overlays/actors/z_en_rd.reloc.o"
endseg

beginseg
    name "ovl_En_Reeba"
    include "build/src/overlays/actors/ovl_En_Reeba/z_en_reeba.o"
    include "build/data/overlays/actors/z_en_reeba.data.o"
    include "build/data/overlays/actors/z_en_reeba.reloc.o"
endseg

beginseg
    name "ovl_En_River_Sound"
    include "build/src/overlays/actors/ovl_En_River_Sound/z_en_river_sound.o"
    include "build/data/overlays/actors/z_en_river_sound.data.o"
    include "build/data/overlays/actors/z_en_river_sound.reloc.o"
endseg

beginseg
    name "ovl_En_Rl"
    include "build/src/overlays/actors/ovl_En_Rl/z_en_rl.o"
    include "build/data/overlays/actors/z_en_rl.data.o"
    include "build/data/overlays/actors/z_en_rl.reloc.o"
endseg

beginseg
    name "ovl_En_Rr"
    include "build/src/overlays/actors/ovl_En_Rr/z_en_rr.o"
    include "build/data/overlays/actors/z_en_rr.data.o"
    include "build/data/overlays/actors/z_en_rr.reloc.o"
endseg

beginseg
    name "ovl_En_Ru1"
    include "build/src/overlays/actors/ovl_En_Ru1/z_en_ru1.o"
    include "build/src/overlays/actors/ovl_En_Ru1/ovl_En_Ru1_reloc.o"
endseg

beginseg
    name "ovl_En_Ru2"
    include "build/src/overlays/actors/ovl_En_Ru2/z_en_ru2.o"
    include "build/src/overlays/actors/ovl_En_Ru2/ovl_En_Ru2_reloc.o"
endseg

beginseg
    name "ovl_En_Sa"
    include "build/src/overlays/actors/ovl_En_Sa/z_en_sa.o"
    include "build/data/overlays/actors/z_en_sa.data.o"
    include "build/data/overlays/actors/z_en_sa.reloc.o"
endseg

beginseg
    name "ovl_En_Sb"
    include "build/src/overlays/actors/ovl_En_Sb/z_en_sb.o"
    include "build/src/overlays/actors/ovl_En_Sb/ovl_En_Sb_reloc.o"
endseg

beginseg
    name "ovl_En_Scene_Change"
    include "build/src/overlays/actors/ovl_En_Scene_Change/z_en_scene_change.o"
    include "build/src/overlays/actors/ovl_En_Scene_Change/ovl_En_Scene_Change_reloc.o"
endseg

beginseg
    name "ovl_En_Sda"
    include "build/src/overlays/actors/ovl_En_Sda/z_en_sda.o"
    include "build/data/overlays/actors/z_en_sda.data.o"
    include "build/data/overlays/actors/z_en_sda.bss.o"
    include "build/data/overlays/actors/z_en_sda.reloc.o"
endseg

beginseg
    name "ovl_En_Shopnuts"
    include "build/src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.o"
    include "build/data/overlays/actors/z_en_shopnuts.data.o"
    include "build/data/overlays/actors/z_en_shopnuts.reloc.o"
endseg

beginseg
    name "ovl_En_Si"
    include "build/src/overlays/actors/ovl_En_Si/z_en_si.o"
    include "build/src/overlays/actors/ovl_En_Si/ovl_En_Si_reloc.o"
endseg

beginseg
    name "ovl_En_Siofuki"
    include "build/src/overlays/actors/ovl_En_Siofuki/z_en_siofuki.o"
    include "build/data/overlays/actors/z_en_siofuki.data.o"
    include "build/data/overlays/actors/z_en_siofuki.reloc.o"
endseg

beginseg
    name "ovl_En_Skb"
    include "build/src/overlays/actors/ovl_En_Skb/z_en_skb.o"
    include "build/data/overlays/actors/z_en_skb.data.o"
    include "build/data/overlays/actors/z_en_skb.reloc.o"
endseg

beginseg
    name "ovl_En_Skj"
    include "build/src/overlays/actors/ovl_En_Skj/z_en_skj.o"
    include "build/data/overlays/actors/z_en_skj.data.o"
    include "build/data/overlays/actors/z_en_skj.rodata.o"
    include "build/data/overlays/actors/z_en_skj.bss.o"
    include "build/data/overlays/actors/z_en_skj.reloc.o"
endseg

beginseg
    name "ovl_En_Skjneedle"
    include "build/src/overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.o"
    include "build/src/overlays/actors/ovl_En_Skjneedle/ovl_En_Skjneedle_reloc.o"
endseg

beginseg
    name "ovl_En_Ssh"
    include "build/src/overlays/actors/ovl_En_Ssh/z_en_ssh.o"
    include "build/data/overlays/actors/z_en_ssh.data.o"
    include "build/data/overlays/actors/z_en_ssh.reloc.o"
endseg

beginseg
    name "ovl_En_St"
    include "build/src/overlays/actors/ovl_En_St/z_en_st.o"
    include "build/data/overlays/actors/z_en_st.data.o"
    include "build/data/overlays/actors/z_en_st.reloc.o"
endseg

beginseg
    name "ovl_En_Sth"
    include "build/src/overlays/actors/ovl_En_Sth/z_en_sth.o"
    include "build/data/overlays/actors/z_en_sth.data.o"
    include "build/data/overlays/actors/z_en_sth.reloc.o"
endseg

beginseg
    name "ovl_En_Stream"
    include "build/src/overlays/actors/ovl_En_Stream/z_en_stream.o"
    include "build/data/overlays/actors/z_en_stream.data.o"
    include "build/data/overlays/actors/z_en_stream.reloc.o"
endseg

beginseg
    name "ovl_En_Sw"
    include "build/src/overlays/actors/ovl_En_Sw/z_en_sw.o"
    include "build/data/overlays/actors/z_en_sw.data.o"
    include "build/data/overlays/actors/z_en_sw.bss.o"
    include "build/data/overlays/actors/z_en_sw.reloc.o"
endseg

beginseg
    name "ovl_En_Syateki_Itm"
    include "build/src/overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.o"
    include "build/data/overlays/actors/z_en_syateki_itm.data.o"
    include "build/data/overlays/actors/z_en_syateki_itm.reloc.o"
endseg

beginseg
    name "ovl_En_Syateki_Man"
    include "build/src/overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.o"
    include "build/data/overlays/actors/z_en_syateki_man.data.o"
    include "build/data/overlays/actors/z_en_syateki_man.reloc.o"
endseg

beginseg
    name "ovl_En_Syateki_Niw"
    include "build/src/overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.o"
    include "build/data/overlays/actors/z_en_syateki_niw.data.o"
    include "build/data/overlays/actors/z_en_syateki_niw.reloc.o"
endseg

beginseg
    name "ovl_En_Ta"
    include "build/src/overlays/actors/ovl_En_Ta/z_en_ta.o"
    include "build/data/overlays/actors/z_en_ta.data.o"
    include "build/data/overlays/actors/z_en_ta.reloc.o"
endseg

beginseg
    name "ovl_En_Takara_Man"
    include "build/src/overlays/actors/ovl_En_Takara_Man/z_en_takara_man.o"
    include "build/data/overlays/actors/z_en_takara_man.data.o"
    include "build/data/overlays/actors/z_en_takara_man.reloc.o"
endseg

beginseg
    name "ovl_En_Tana"
    include "build/src/overlays/actors/ovl_En_Tana/z_en_tana.o"
    include "build/src/overlays/actors/ovl_En_Tana/ovl_En_Tana_reloc.o"
endseg

beginseg
    name "ovl_En_Tg"
    include "build/src/overlays/actors/ovl_En_Tg/z_en_tg.o"
    include "build/data/overlays/actors/z_en_tg.data.o"
    include "build/data/overlays/actors/z_en_tg.reloc.o"
endseg

beginseg
    name "ovl_En_Tite"
    include "build/src/overlays/actors/ovl_En_Tite/z_en_tite.o"
    include "build/data/overlays/actors/z_en_tite.data.o"
    include "build/data/overlays/actors/z_en_tite.reloc.o"
endseg

beginseg
    name "ovl_En_Tk"
    include "build/src/overlays/actors/ovl_En_Tk/z_en_tk.o"
    include "build/src/overlays/actors/ovl_En_Tk/ovl_En_Tk_reloc.o"
endseg

beginseg
    name "ovl_En_Torch"
    include "build/src/overlays/actors/ovl_En_Torch/z_en_torch.o"
    include "build/src/overlays/actors/ovl_En_Torch/ovl_En_Torch_reloc.o"
endseg

beginseg
    name "ovl_En_Torch2"
    include "build/src/overlays/actors/ovl_En_Torch2/z_en_torch2.o"
    include "build/data/overlays/actors/z_en_torch2.data.o"
    include "build/data/overlays/actors/z_en_torch2.bss.o"
    include "build/data/overlays/actors/z_en_torch2.reloc.o"
endseg

beginseg
    name "ovl_En_Toryo"
    include "build/src/overlays/actors/ovl_En_Toryo/z_en_toryo.o"
    include "build/data/overlays/actors/z_en_toryo.data.o"
    include "build/data/overlays/actors/z_en_toryo.reloc.o"
endseg

beginseg
    name "ovl_En_Tp"
    include "build/src/overlays/actors/ovl_En_Tp/z_en_tp.o"
    include "build/data/overlays/actors/z_en_tp.data.o"
    include "build/data/overlays/actors/z_en_tp.reloc.o"
endseg

beginseg
    name "ovl_En_Tr"
    include "build/src/overlays/actors/ovl_En_Tr/z_en_tr.o"
    include "build/data/overlays/actors/z_en_tr.data.o"
    include "build/data/overlays/actors/z_en_tr.reloc.o"
endseg

beginseg
    name "ovl_En_Trap"
    include "build/src/overlays/actors/ovl_En_Trap/z_en_trap.o"
    include "build/data/overlays/actors/z_en_trap.data.o"
    include "build/data/overlays/actors/z_en_trap.reloc.o"
endseg

beginseg
    name "ovl_En_Tubo_Trap"
    include "build/src/overlays/actors/ovl_En_Tubo_Trap/z_en_tubo_trap.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_En_Tubo_Trap/ovl_En_Tubo_Trap_reloc.o"
#else
    include "build/data/overlays/actors/z_en_tubo_trap.reloc.o"
#endif
endseg

beginseg
    name "ovl_En_Vali"
    include "build/src/overlays/actors/ovl_En_Vali/z_en_vali.o"
    include "build/data/overlays/actors/z_en_vali.data.o"
    include "build/data/overlays/actors/z_en_vali.reloc.o"
endseg

beginseg
    name "ovl_En_Vase"
    include "build/src/overlays/actors/ovl_En_Vase/z_en_vase.o"
    include "build/src/overlays/actors/ovl_En_Vase/ovl_En_Vase_reloc.o"
endseg

beginseg
    name "ovl_En_Vb_Ball"
    include "build/src/overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.o"
    include "build/data/overlays/actors/z_en_vb_ball.data.o"
    include "build/data/overlays/actors/z_en_vb_ball.reloc.o"
endseg

beginseg
    name "ovl_En_Viewer"
    include "build/src/overlays/actors/ovl_En_Viewer/z_en_viewer.o"
    include "build/data/overlays/actors/z_en_viewer.data.o"
    include "build/data/overlays/actors/z_en_viewer.bss.o"
    include "build/data/overlays/actors/z_en_viewer.reloc.o"
endseg

beginseg
    name "ovl_En_Vm"
    include "build/src/overlays/actors/ovl_En_Vm/z_en_vm.o"
    include "build/data/overlays/actors/z_en_vm.data.o"
    include "build/data/overlays/actors/z_en_vm.reloc.o"
endseg

beginseg
    name "ovl_En_Wall_Tubo"
    include "build/src/overlays/actors/ovl_En_Wall_Tubo/z_en_wall_tubo.o"
    include "build/data/overlays/actors/z_en_wall_tubo.data.o"
    include "build/data/overlays/actors/z_en_wall_tubo.reloc.o"
endseg

beginseg
    name "ovl_En_Wallmas"
    include "build/src/overlays/actors/ovl_En_Wallmas/z_en_wallmas.o"
    include "build/src/overlays/actors/ovl_En_Wallmas/ovl_En_Wallmas_reloc.o"
endseg

beginseg
    name "ovl_En_Weather_Tag"
    include "build/src/overlays/actors/ovl_En_Weather_Tag/z_en_weather_tag.o"
    include "build/data/overlays/actors/z_en_weather_tag.data.o"
    include "build/data/overlays/actors/z_en_weather_tag.reloc.o"
endseg

beginseg
    name "ovl_En_Weiyer"
    include "build/src/overlays/actors/ovl_En_Weiyer/z_en_weiyer.o"
    include "build/data/overlays/actors/z_en_weiyer.data.o"
    include "build/data/overlays/actors/z_en_weiyer.reloc.o"
endseg

beginseg
    name "ovl_En_Wf"
    include "build/src/overlays/actors/ovl_En_Wf/z_en_wf.o"
    include "build/data/overlays/actors/z_en_wf.data.o"
    include "build/data/overlays/actors/z_en_wf.reloc.o"
endseg

beginseg
    name "ovl_En_Wonder_Item"
    include "build/src/overlays/actors/ovl_En_Wonder_Item/z_en_wonder_item.o"
    include "build/data/overlays/actors/z_en_wonder_item.data.o"
    include "build/data/overlays/actors/z_en_wonder_item.bss.o"
    include "build/data/overlays/actors/z_en_wonder_item.reloc.o"
endseg

beginseg
    name "ovl_En_Wonder_Talk"
    include "build/src/overlays/actors/ovl_En_Wonder_Talk/z_en_wonder_talk.o"
    include "build/data/overlays/actors/z_en_wonder_talk.data.o"
    include "build/data/overlays/actors/z_en_wonder_talk.reloc.o"
endseg

beginseg
    name "ovl_En_Wonder_Talk2"
    include "build/src/overlays/actors/ovl_En_Wonder_Talk2/z_en_wonder_talk2.o"
    include "build/data/overlays/actors/z_en_wonder_talk2.data.o"
    include "build/data/overlays/actors/z_en_wonder_talk2.reloc.o"
endseg

beginseg
    name "ovl_En_Wood02"
    include "build/src/overlays/actors/ovl_En_Wood02/z_en_wood02.o"
    include "build/data/overlays/actors/z_en_wood02.data.o"
    include "build/data/overlays/actors/z_en_wood02.bss.o"
    include "build/data/overlays/actors/z_en_wood02.reloc.o"
endseg

beginseg
    name "ovl_En_Xc"
    include "build/src/overlays/actors/ovl_En_Xc/z_en_xc.o"
    include "build/data/overlays/actors/z_en_xc.data.o"
    include "build/data/overlays/actors/z_en_xc.bss.o"
    include "build/data/overlays/actors/z_en_xc.reloc.o"
endseg

beginseg
    name "ovl_En_Yabusame_Mark"
    include "build/src/overlays/actors/ovl_En_Yabusame_Mark/z_en_yabusame_mark.o"
    include "build/data/overlays/actors/z_en_yabusame_mark.data.o"
    include "build/data/overlays/actors/z_en_yabusame_mark.reloc.o"
endseg

beginseg
    name "ovl_En_Yukabyun"
    include "build/src/overlays/actors/ovl_En_Yukabyun/z_en_yukabyun.o"
    include "build/data/overlays/actors/z_en_yukabyun.data.o"
    include "build/data/overlays/actors/z_en_yukabyun.reloc.o"
endseg

beginseg
    name "ovl_En_Zf"
    include "build/src/overlays/actors/ovl_En_Zf/z_en_zf.o"
    include "build/data/overlays/actors/z_en_zf.data.o"
    include "build/data/overlays/actors/z_en_zf.bss.o"
    include "build/data/overlays/actors/z_en_zf.reloc.o"
endseg

beginseg
    name "ovl_En_Zl1"
    include "build/src/overlays/actors/ovl_En_Zl1/z_en_zl1.o"
    include "build/src/overlays/actors/ovl_En_Zl1/ovl_En_Zl1_reloc.o"
endseg

beginseg
    name "ovl_En_Zl2"
    include "build/src/overlays/actors/ovl_En_Zl2/z_en_zl2.o"
#ifdef NON_MATCHING
    include "build/src/overlays/actors/ovl_En_Zl2/ovl_En_Zl2_reloc.o"
#else
    include "build/data/overlays/actors/z_en_zl2.reloc.o"
#endif
endseg

beginseg
    name "ovl_En_Zl3"
    include "build/src/overlays/actors/ovl_En_Zl3/z_en_zl3.o"
    include "build/data/overlays/actors/z_en_zl3.data.o"
    include "build/data/overlays/actors/z_en_zl3.reloc.o"
endseg

beginseg
    name "ovl_En_Zl4"
    include "build/src/overlays/actors/ovl_En_Zl4/z_en_zl4.o"
    include "build/data/overlays/actors/z_en_zl4.data.o"
    include "build/data/overlays/actors/z_en_zl4.reloc.o"
endseg

beginseg
    name "ovl_En_Zo"
    include "build/src/overlays/actors/ovl_En_Zo/z_en_zo.o"
    include "build/data/overlays/actors/z_en_zo.data.o"
    include "build/data/overlays/actors/z_en_zo.reloc.o"
endseg

beginseg
    name "ovl_En_fHG"
    include "build/src/overlays/actors/ovl_En_fHG/z_en_fhg.o"
    include "build/data/overlays/actors/z_en_fhg.data.o"
    include "build/data/overlays/actors/z_en_fhg.reloc.o"
endseg

beginseg
    name "ovl_End_Title"
    include "build/src/overlays/actors/ovl_End_Title/z_end_title.o"
    include "build/src/overlays/actors/ovl_End_Title/ovl_End_Title_reloc.o"
endseg

beginseg
    name "ovl_Fishing"
    include "build/src/overlays/actors/ovl_Fishing/z_fishing.o"
    include "build/data/overlays/actors/z_fishing.data.o"
    include "build/data/overlays/actors/z_fishing.bss.o"
    include "build/data/overlays/actors/z_fishing.reloc.o"
endseg

beginseg
    name "ovl_Item_B_Heart"
    include "build/src/overlays/actors/ovl_Item_B_Heart/z_item_b_heart.o"
    include "build/src/overlays/actors/ovl_Item_B_Heart/ovl_Item_B_Heart_reloc.o"
endseg

beginseg
    name "ovl_Item_Etcetera"
    include "build/src/overlays/actors/ovl_Item_Etcetera/z_item_etcetera.o"
    include "build/src/overlays/actors/ovl_Item_Etcetera/ovl_Item_Etcetera_reloc.o"
endseg

beginseg
    name "ovl_Item_Inbox"
    include "build/src/overlays/actors/ovl_Item_Inbox/z_item_inbox.o"
    include "build/src/overlays/actors/ovl_Item_Inbox/ovl_Item_Inbox_reloc.o"
endseg

beginseg
    name "ovl_Item_Ocarina"
    include "build/src/overlays/actors/ovl_Item_Ocarina/z_item_ocarina.o"
    include "build/data/overlays/actors/z_item_ocarina.data.o"
    include "build/data/overlays/actors/z_item_ocarina.reloc.o"
endseg

beginseg
    name "ovl_Item_Shield"
    include "build/src/overlays/actors/ovl_Item_Shield/z_item_shield.o"
    include "build/data/overlays/actors/z_item_shield.data.o"
    include "build/data/overlays/actors/z_item_shield.reloc.o"
endseg

beginseg
    name "ovl_Magic_Dark"
    include "build/src/overlays/actors/ovl_Magic_Dark/z_magic_dark.o"
    include "build/data/overlays/actors/z_magic_dark.data.o"
    include "build/data/overlays/actors/z_magic_dark.reloc.o"
endseg

beginseg
    name "ovl_Magic_Fire"
    include "build/src/overlays/actors/ovl_Magic_Fire/z_magic_fire.o"
    include "build/data/overlays/actors/z_magic_fire.data.o"
    include "build/data/overlays/actors/z_magic_fire.reloc.o"
endseg

beginseg
    name "ovl_Magic_Wind"
    include "build/src/overlays/actors/ovl_Magic_Wind/z_magic_wind.o"
    include "build/src/overlays/actors/ovl_Magic_Wind/ovl_Magic_Wind_reloc.o"
endseg

beginseg
    name "ovl_Mir_Ray"
    include "build/src/overlays/actors/ovl_Mir_Ray/z_mir_ray.o"
    include "build/data/overlays/actors/z_mir_ray.data.o"
    include "build/data/overlays/actors/z_mir_ray.reloc.o"
endseg

beginseg
    name "ovl_Obj_Bean"
    include "build/src/overlays/actors/ovl_Obj_Bean/z_obj_bean.o"
    include "build/data/overlays/actors/z_obj_bean.data.o"
    include "build/data/overlays/actors/z_obj_bean.reloc.o"
endseg

beginseg
    name "ovl_Obj_Blockstop"
    include "build/src/overlays/actors/ovl_Obj_Blockstop/z_obj_blockstop.o"
    include "build/src/overlays/actors/ovl_Obj_Blockstop/ovl_Obj_Blockstop_reloc.o"
endseg

beginseg
    name "ovl_Obj_Bombiwa"
    include "build/src/overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.o"
    include "build/src/overlays/actors/ovl_Obj_Bombiwa/ovl_Obj_Bombiwa_reloc.o"
endseg

beginseg
    name "ovl_Obj_Comb"
    include "build/src/overlays/actors/ovl_Obj_Comb/z_obj_comb.o"
    include "build/src/overlays/actors/ovl_Obj_Comb/ovl_Obj_Comb_reloc.o"
endseg

beginseg
    name "ovl_Obj_Dekujr"
    include "build/src/overlays/actors/ovl_Obj_Dekujr/z_obj_dekujr.o"
    include "build/src/overlays/actors/ovl_Obj_Dekujr/ovl_Obj_Dekujr_reloc.o"
endseg

beginseg
    name "ovl_Obj_Elevator"
    include "build/src/overlays/actors/ovl_Obj_Elevator/z_obj_elevator.o"
    include "build/src/overlays/actors/ovl_Obj_Elevator/ovl_Obj_Elevator_reloc.o"
endseg

beginseg
    name "ovl_Obj_Hamishi"
    include "build/src/overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.o"
    include "build/data/overlays/actors/z_obj_hamishi.data.o"
    include "build/data/overlays/actors/z_obj_hamishi.reloc.o"
endseg

beginseg
    name "ovl_Obj_Hana"
    include "build/src/overlays/actors/ovl_Obj_Hana/z_obj_hana.o"
    include "build/data/overlays/actors/z_obj_hana.data.o"
    include "build/data/overlays/actors/z_obj_hana.reloc.o"
endseg

beginseg
    name "ovl_Obj_Hsblock"
    include "build/src/overlays/actors/ovl_Obj_Hsblock/z_obj_hsblock.o"
    include "build/data/overlays/actors/z_obj_hsblock.data.o"
    include "build/data/overlays/actors/z_obj_hsblock.reloc.o"
endseg

beginseg
    name "ovl_Obj_Ice_Poly"
    include "build/src/overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.o"
    include "build/data/overlays/actors/z_obj_ice_poly.data.o"
    include "build/data/overlays/actors/z_obj_ice_poly.reloc.o"
endseg

beginseg
    name "ovl_Obj_Kibako"
    include "build/src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.o"
    include "build/data/overlays/actors/z_obj_kibako.data.o"
    include "build/data/overlays/actors/z_obj_kibako.reloc.o"
endseg

beginseg
    name "ovl_Obj_Kibako2"
    include "build/src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.o"
    include "build/data/overlays/actors/z_obj_kibako2.data.o"
    include "build/data/overlays/actors/z_obj_kibako2.reloc.o"
endseg

beginseg
    name "ovl_Obj_Lift"
    include "build/src/overlays/actors/ovl_Obj_Lift/z_obj_lift.o"
    include "build/data/overlays/actors/z_obj_lift.data.o"
    include "build/data/overlays/actors/z_obj_lift.reloc.o"
endseg

beginseg
    name "ovl_Obj_Lightswitch"
    include "build/src/overlays/actors/ovl_Obj_Lightswitch/z_obj_lightswitch.o"
    include "build/data/overlays/actors/z_obj_lightswitch.data.o"
    include "build/data/overlays/actors/z_obj_lightswitch.reloc.o"
endseg

beginseg
    name "ovl_Obj_Makekinsuta"
    include "build/src/overlays/actors/ovl_Obj_Makekinsuta/z_obj_makekinsuta.o"
    include "build/src/overlays/actors/ovl_Obj_Makekinsuta/ovl_Obj_Makekinsuta_reloc.o"
endseg

beginseg
    name "ovl_Obj_Makeoshihiki"
    include "build/src/overlays/actors/ovl_Obj_Makeoshihiki/z_obj_makeoshihiki.o"
    include "build/src/overlays/actors/ovl_Obj_Makeoshihiki/ovl_Obj_Makeoshihiki_reloc.o"
endseg

beginseg
    name "ovl_Obj_Mure"
    include "build/src/overlays/actors/ovl_Obj_Mure/z_obj_mure.o"
    include "build/data/overlays/actors/z_obj_mure.data.o"
    include "build/data/overlays/actors/z_obj_mure.reloc.o"
endseg

beginseg
    name "ovl_Obj_Mure2"
    include "build/src/overlays/actors/ovl_Obj_Mure2/z_obj_mure2.o"
    include "build/data/overlays/actors/z_obj_mure2.data.o"
    include "build/data/overlays/actors/z_obj_mure2.reloc.o"
endseg

beginseg
    name "ovl_Obj_Mure3"
    include "build/src/overlays/actors/ovl_Obj_Mure3/z_obj_mure3.o"
    include "build/data/overlays/actors/z_obj_mure3.data.o"
    include "build/data/overlays/actors/z_obj_mure3.reloc.o"
endseg

beginseg
    name "ovl_Obj_Oshihiki"
    include "build/src/overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.o"
    include "build/data/overlays/actors/z_obj_oshihiki.data.o"
    include "build/data/overlays/actors/z_obj_oshihiki.reloc.o"
endseg

beginseg
    name "ovl_Obj_Roomtimer"
    include "build/src/overlays/actors/ovl_Obj_Roomtimer/z_obj_roomtimer.o"
    include "build/src/overlays/actors/ovl_Obj_Roomtimer/ovl_Obj_Roomtimer_reloc.o"
endseg

beginseg
    name "ovl_Obj_Switch"
    include "build/src/overlays/actors/ovl_Obj_Switch/z_obj_switch.o"
    include "build/data/overlays/actors/z_obj_switch.data.o"
    include "build/data/overlays/actors/z_obj_switch.reloc.o"
endseg

beginseg
    name "ovl_Obj_Syokudai"
    include "build/src/overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.o"
    include "build/data/overlays/actors/z_obj_syokudai.data.o"
    include "build/data/overlays/actors/z_obj_syokudai.bss.o"
    include "build/data/overlays/actors/z_obj_syokudai.reloc.o"
endseg

beginseg
    name "ovl_Obj_Timeblock"
    include "build/src/overlays/actors/ovl_Obj_Timeblock/z_obj_timeblock.o"
    include "build/data/overlays/actors/z_obj_timeblock.data.o"
    include "build/data/overlays/actors/z_obj_timeblock.reloc.o"
endseg

beginseg
    name "ovl_Obj_Tsubo"
    include "build/src/overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.o"
    include "build/data/overlays/actors/z_obj_tsubo.data.o"
    include "build/data/overlays/actors/z_obj_tsubo.reloc.o"
endseg

beginseg
    name "ovl_Obj_Warp2block"
    include "build/src/overlays/actors/ovl_Obj_Warp2block/z_obj_warp2block.o"
    include "build/data/overlays/actors/z_obj_warp2block.data.o"
    include "build/data/overlays/actors/z_obj_warp2block.reloc.o"
endseg

beginseg
    name "ovl_Object_Kankyo"
    include "build/src/overlays/actors/ovl_Object_Kankyo/z_object_kankyo.o"
    include "build/data/overlays/actors/z_object_kankyo.data.o"
    include "build/data/overlays/actors/z_object_kankyo.reloc.o"
endseg

beginseg
    name "ovl_Oceff_Spot"
    include "build/src/overlays/actors/ovl_Oceff_Spot/z_oceff_spot.o"
    include "build/src/overlays/actors/ovl_Oceff_Spot/ovl_Oceff_Spot_reloc.o"
endseg

beginseg
    name "ovl_Oceff_Storm"
    include "build/src/overlays/actors/ovl_Oceff_Storm/z_oceff_storm.o"
    include "build/src/overlays/actors/ovl_Oceff_Storm/ovl_Oceff_Storm_reloc.o"
endseg

beginseg
    name "ovl_Oceff_Wipe"
    include "build/src/overlays/actors/ovl_Oceff_Wipe/z_oceff_wipe.o"
    include "build/src/overlays/actors/ovl_Oceff_Wipe/ovl_Oceff_Wipe_reloc.o"
endseg

beginseg
    name "ovl_Oceff_Wipe2"
    include "build/src/overlays/actors/ovl_Oceff_Wipe2/z_oceff_wipe2.o"
    include "build/src/overlays/actors/ovl_Oceff_Wipe2/ovl_Oceff_Wipe2_reloc.o"
endseg

beginseg
    name "ovl_Oceff_Wipe3"
    include "build/src/overlays/actors/ovl_Oceff_Wipe3/z_oceff_wipe3.o"
    include "build/src/overlays/actors/ovl_Oceff_Wipe3/ovl_Oceff_Wipe3_reloc.o"
endseg

beginseg
    name "ovl_Oceff_Wipe4"
    include "build/src/overlays/actors/ovl_Oceff_Wipe4/z_oceff_wipe4.o"
    include "build/src/overlays/actors/ovl_Oceff_Wipe4/ovl_Oceff_Wipe4_reloc.o"
endseg

beginseg
    name "ovl_Shot_Sun"
    include "build/src/overlays/actors/ovl_Shot_Sun/z_shot_sun.o"
    include "build/src/overlays/actors/ovl_Shot_Sun/ovl_Shot_Sun_reloc.o"
endseg

beginseg
    name "gameplay_keep"
    romalign 0x1000
    include "build/baserom/gameplay_keep.o"
endseg

beginseg
    name "gameplay_field_keep"
    romalign 0x1000
    include "build/baserom/gameplay_field_keep.o"
endseg

beginseg
    name "gameplay_dangeon_keep"
    romalign 0x1000
    include "build/baserom/gameplay_dangeon_keep.o"
endseg

beginseg
    name "gameplay_object_exchange_static"
    romalign 0x1000
    include "build/baserom/gameplay_object_exchange_static.o"
endseg

beginseg
    name "object_link_boy"
    romalign 0x1000
    include "build/baserom/object_link_boy.o"
endseg

beginseg
    name "object_link_child"
    romalign 0x1000
    include "build/baserom/object_link_child.o"
endseg

beginseg
    name "object_box"
    romalign 0x1000
    include "build/baserom/object_box.o"
endseg

beginseg
    name "object_human"
    romalign 0x1000
    include "build/baserom/object_human.o"
endseg

beginseg
    name "object_okuta"
    romalign 0x1000
    include "build/baserom/object_okuta.o"
endseg

beginseg
    name "object_poh"
    romalign 0x1000
    include "build/baserom/object_poh.o"
endseg

beginseg
    name "object_wallmaster"
    romalign 0x1000
    include "build/baserom/object_wallmaster.o"
endseg

beginseg
    name "object_dy_obj"
    romalign 0x1000
    include "build/baserom/object_dy_obj.o"
endseg

beginseg
    name "object_firefly"
    romalign 0x1000
    include "build/baserom/object_firefly.o"
endseg

beginseg
    name "object_dodongo"
    romalign 0x1000
    include "build/baserom/object_dodongo.o"
endseg

beginseg
    name "object_fire"
    romalign 0x1000
    include "build/baserom/object_fire.o"
endseg

beginseg
    name "object_niw"
    romalign 0x1000
    include "build/baserom/object_niw.o"
endseg

beginseg
    name "object_tite"
    romalign 0x1000
    include "build/baserom/object_tite.o"
endseg

beginseg
    name "object_reeba"
    romalign 0x1000
    include "build/baserom/object_reeba.o"
endseg

beginseg
    name "object_peehat"
    romalign 0x1000
    include "build/baserom/object_peehat.o"
endseg

beginseg
    name "object_kingdodongo"
    romalign 0x1000
    include "build/baserom/object_kingdodongo.o"
endseg

beginseg
    name "object_horse"
    romalign 0x1000
    include "build/baserom/object_horse.o"
endseg

beginseg
    name "object_zf"
    romalign 0x1000
    include "build/baserom/object_zf.o"
endseg

beginseg
    name "object_goma"
    romalign 0x1000
    include "build/baserom/object_goma.o"
endseg

beginseg
    name "object_zl1"
    romalign 0x1000
    include "build/baserom/object_zl1.o"
endseg

beginseg
    name "object_gol"
    romalign 0x1000
    include "build/baserom/object_gol.o"
endseg

beginseg
    name "object_bubble"
    romalign 0x1000
    include "build/baserom/object_bubble.o"
endseg

beginseg
    name "object_dodojr"
    romalign 0x1000
    include "build/baserom/object_dodojr.o"
endseg

beginseg
    name "object_torch2"
    romalign 0x1000
    include "build/baserom/object_torch2.o"
endseg

beginseg
    name "object_bl"
    romalign 0x1000
    include "build/baserom/object_bl.o"
endseg

beginseg
    name "object_tp"
    romalign 0x1000
    include "build/baserom/object_tp.o"
endseg

beginseg
    name "object_oA1"
    romalign 0x1000
    include "build/baserom/object_oA1.o"
endseg

beginseg
    name "object_st"
    romalign 0x1000
    include "build/baserom/object_st.o"
endseg

beginseg
    name "object_bw"
    romalign 0x1000
    include "build/baserom/object_bw.o"
endseg

beginseg
    name "object_ei"
    romalign 0x1000
    include "build/baserom/object_ei.o"
endseg

beginseg
    name "object_horse_normal"
    romalign 0x1000
    include "build/baserom/object_horse_normal.o"
endseg

beginseg
    name "object_oB1"
    romalign 0x1000
    include "build/baserom/object_oB1.o"
endseg

beginseg
    name "object_o_anime"
    romalign 0x1000
    include "build/baserom/object_o_anime.o"
endseg

beginseg
    name "object_spot04_objects"
    romalign 0x1000
    include "build/baserom/object_spot04_objects.o"
endseg

beginseg
    name "object_ddan_objects"
    romalign 0x1000
    include "build/baserom/object_ddan_objects.o"
endseg

beginseg
    name "object_hidan_objects"
    romalign 0x1000
    include "build/baserom/object_hidan_objects.o"
endseg

beginseg
    name "object_horse_ganon"
    romalign 0x1000
    include "build/baserom/object_horse_ganon.o"
endseg

beginseg
    name "object_oA2"
    romalign 0x1000
    include "build/baserom/object_oA2.o"
endseg

beginseg
    name "object_spot00_objects"
    romalign 0x1000
    include "build/baserom/object_spot00_objects.o"
endseg

beginseg
    name "object_mb"
    romalign 0x1000
    include "build/baserom/object_mb.o"
endseg

beginseg
    name "object_bombf"
    romalign 0x1000
    include "build/baserom/object_bombf.o"
endseg

beginseg
    name "object_sk2"
    romalign 0x1000
    include "build/baserom/object_sk2.o"
endseg

beginseg
    name "object_oE1"
    romalign 0x1000
    include "build/baserom/object_oE1.o"
endseg

beginseg
    name "object_oE_anime"
    romalign 0x1000
    include "build/baserom/object_oE_anime.o"
endseg

beginseg
    name "object_oE2"
    romalign 0x1000
    include "build/baserom/object_oE2.o"
endseg

beginseg
    name "object_ydan_objects"
    romalign 0x1000
    include "build/baserom/object_ydan_objects.o"
endseg

beginseg
    name "object_gnd"
    romalign 0x1000
    include "build/baserom/object_gnd.o"
endseg

beginseg
    name "object_am"
    romalign 0x1000
    include "build/baserom/object_am.o"
endseg

beginseg
    name "object_dekubaba"
    romalign 0x1000
    include "build/baserom/object_dekubaba.o"
endseg

beginseg
    name "object_oA3"
    romalign 0x1000
    include "build/baserom/object_oA3.o"
endseg

beginseg
    name "object_oA4"
    romalign 0x1000
    include "build/baserom/object_oA4.o"
endseg

beginseg
    name "object_oA5"
    romalign 0x1000
    include "build/baserom/object_oA5.o"
endseg

beginseg
    name "object_oA6"
    romalign 0x1000
    include "build/baserom/object_oA6.o"
endseg

beginseg
    name "object_oA7"
    romalign 0x1000
    include "build/baserom/object_oA7.o"
endseg

beginseg
    name "object_jj"
    romalign 0x1000
    include "build/baserom/object_jj.o"
endseg

beginseg
    name "object_oA8"
    romalign 0x1000
    include "build/baserom/object_oA8.o"
endseg

beginseg
    name "object_oA9"
    romalign 0x1000
    include "build/baserom/object_oA9.o"
endseg

beginseg
    name "object_oB2"
    romalign 0x1000
    include "build/baserom/object_oB2.o"
endseg

beginseg
    name "object_oB3"
    romalign 0x1000
    include "build/baserom/object_oB3.o"
endseg

beginseg
    name "object_oB4"
    romalign 0x1000
    include "build/baserom/object_oB4.o"
endseg

beginseg
    name "object_horse_zelda"
    romalign 0x1000
    include "build/baserom/object_horse_zelda.o"
endseg

beginseg
    name "object_opening_demo1"
    romalign 0x1000
    include "build/baserom/object_opening_demo1.o"
endseg

beginseg
    name "object_warp1"
    romalign 0x1000
    include "build/baserom/object_warp1.o"
endseg

beginseg
    name "object_b_heart"
    romalign 0x1000
    include "build/baserom/object_b_heart.o"
endseg

beginseg
    name "object_dekunuts"
    romalign 0x1000
    include "build/baserom/object_dekunuts.o"
endseg

beginseg
    name "object_oE3"
    romalign 0x1000
    include "build/baserom/object_oE3.o"
endseg

beginseg
    name "object_oE4"
    romalign 0x1000
    include "build/baserom/object_oE4.o"
endseg

beginseg
    name "object_menkuri_objects"
    romalign 0x1000
    include "build/baserom/object_menkuri_objects.o"
endseg

beginseg
    name "object_oE5"
    romalign 0x1000
    include "build/baserom/object_oE5.o"
endseg

beginseg
    name "object_oE6"
    romalign 0x1000
    include "build/baserom/object_oE6.o"
endseg

beginseg
    name "object_oE7"
    romalign 0x1000
    include "build/baserom/object_oE7.o"
endseg

beginseg
    name "object_oE8"
    romalign 0x1000
    include "build/baserom/object_oE8.o"
endseg

beginseg
    name "object_oE9"
    romalign 0x1000
    include "build/baserom/object_oE9.o"
endseg

beginseg
    name "object_oE10"
    romalign 0x1000
    include "build/baserom/object_oE10.o"
endseg

beginseg
    name "object_oE11"
    romalign 0x1000
    include "build/baserom/object_oE11.o"
endseg

beginseg
    name "object_oE12"
    romalign 0x1000
    include "build/baserom/object_oE12.o"
endseg

beginseg
    name "object_vali"
    romalign 0x1000
    include "build/baserom/object_vali.o"
endseg

beginseg
    name "object_oA10"
    romalign 0x1000
    include "build/baserom/object_oA10.o"
endseg

beginseg
    name "object_oA11"
    romalign 0x1000
    include "build/baserom/object_oA11.o"
endseg

beginseg
    name "object_mizu_objects"
    romalign 0x1000
    include "build/baserom/object_mizu_objects.o"
endseg

beginseg
    name "object_fhg"
    romalign 0x1000
    include "build/baserom/object_fhg.o"
endseg

beginseg
    name "object_ossan"
    romalign 0x1000
    include "build/baserom/object_ossan.o"
endseg

beginseg
    name "object_mori_hineri1"
    romalign 0x1000
    include "build/baserom/object_mori_hineri1.o"
endseg

beginseg
    name "object_Bb"
    romalign 0x1000
    include "build/baserom/object_Bb.o"
endseg

beginseg
    name "object_toki_objects"
    romalign 0x1000
    include "build/baserom/object_toki_objects.o"
endseg

beginseg
    name "object_yukabyun"
    romalign 0x1000
    include "build/baserom/object_yukabyun.o"
endseg

beginseg
    name "object_zl2"
    romalign 0x1000
    include "build/baserom/object_zl2.o"
endseg

beginseg
    name "object_mjin"
    romalign 0x1000
    include "build/baserom/object_mjin.o"
endseg

beginseg
    name "object_mjin_flash"
    romalign 0x1000
    include "build/baserom/object_mjin_flash.o"
endseg

beginseg
    name "object_mjin_dark"
    romalign 0x1000
    include "build/baserom/object_mjin_dark.o"
endseg

beginseg
    name "object_mjin_flame"
    romalign 0x1000
    include "build/baserom/object_mjin_flame.o"
endseg

beginseg
    name "object_mjin_ice"
    romalign 0x1000
    include "build/baserom/object_mjin_ice.o"
endseg

beginseg
    name "object_mjin_soul"
    romalign 0x1000
    include "build/baserom/object_mjin_soul.o"
endseg

beginseg
    name "object_mjin_wind"
    romalign 0x1000
    include "build/baserom/object_mjin_wind.o"
endseg

beginseg
    name "object_mjin_oka"
    romalign 0x1000
    include "build/baserom/object_mjin_oka.o"
endseg

beginseg
    name "object_haka_objects"
    romalign 0x1000
    include "build/baserom/object_haka_objects.o"
endseg

beginseg
    name "object_spot06_objects"
    romalign 0x1000
    include "build/baserom/object_spot06_objects.o"
endseg

beginseg
    name "object_ice_objects"
    romalign 0x1000
    include "build/baserom/object_ice_objects.o"
endseg

beginseg
    name "object_relay_objects"
    romalign 0x1000
    include "build/baserom/object_relay_objects.o"
endseg

beginseg
    name "object_mori_hineri1a"
    romalign 0x1000
    include "build/baserom/object_mori_hineri1a.o"
endseg

beginseg
    name "object_mori_hineri2"
    romalign 0x1000
    include "build/baserom/object_mori_hineri2.o"
endseg

beginseg
    name "object_mori_hineri2a"
    romalign 0x1000
    include "build/baserom/object_mori_hineri2a.o"
endseg

beginseg
    name "object_mori_objects"
    romalign 0x1000
    include "build/baserom/object_mori_objects.o"
endseg

beginseg
    name "object_mori_tex"
    romalign 0x1000
    include "build/baserom/object_mori_tex.o"
endseg

beginseg
    name "object_spot08_obj"
    romalign 0x1000
    include "build/baserom/object_spot08_obj.o"
endseg

beginseg
    name "object_warp2"
    romalign 0x1000
    include "build/baserom/object_warp2.o"
endseg

beginseg
    name "object_hata"
    romalign 0x1000
    include "build/baserom/object_hata.o"
endseg

beginseg
    name "object_bird"
    romalign 0x1000
    include "build/baserom/object_bird.o"
endseg

beginseg
    name "object_wood02"
    romalign 0x1000
    include "build/baserom/object_wood02.o"
endseg

beginseg
    name "object_lightbox"
    romalign 0x1000
    include "build/baserom/object_lightbox.o"
endseg

beginseg
    name "object_pu_box"
    romalign 0x1000
    include "build/baserom/object_pu_box.o"
endseg

beginseg
    name "object_trap"
    romalign 0x1000
    include "build/baserom/object_trap.o"
endseg

beginseg
    name "object_vase"
    romalign 0x1000
    include "build/baserom/object_vase.o"
endseg

beginseg
    name "object_im"
    romalign 0x1000
    include "build/baserom/object_im.o"
endseg

beginseg
    name "object_ta"
    romalign 0x1000
    include "build/baserom/object_ta.o"
endseg

beginseg
    name "object_tk"
    romalign 0x1000
    include "build/baserom/object_tk.o"
endseg

beginseg
    name "object_xc"
    romalign 0x1000
    include "build/baserom/object_xc.o"
endseg

beginseg
    name "object_vm"
    romalign 0x1000
    include "build/baserom/object_vm.o"
endseg

beginseg
    name "object_bv"
    romalign 0x1000
    include "build/baserom/object_bv.o"
endseg

beginseg
    name "object_hakach_objects"
    romalign 0x1000
    include "build/baserom/object_hakach_objects.o"
endseg

beginseg
    name "object_efc_crystal_light"
    romalign 0x1000
    include "build/baserom/object_efc_crystal_light.o"
endseg

beginseg
    name "object_efc_fire_ball"
    romalign 0x1000
    include "build/baserom/object_efc_fire_ball.o"
endseg

beginseg
    name "object_efc_flash"
    romalign 0x1000
    include "build/baserom/object_efc_flash.o"
endseg

beginseg
    name "object_efc_lgt_shower"
    romalign 0x1000
    include "build/baserom/object_efc_lgt_shower.o"
endseg

beginseg
    name "object_efc_star_field"
    romalign 0x1000
    include "build/baserom/object_efc_star_field.o"
endseg

beginseg
    name "object_god_lgt"
    romalign 0x1000
    include "build/baserom/object_god_lgt.o"
endseg

beginseg
    name "object_light_ring"
    romalign 0x1000
    include "build/baserom/object_light_ring.o"
endseg

beginseg
    name "object_triforce_spot"
    romalign 0x1000
    include "build/baserom/object_triforce_spot.o"
endseg

beginseg
    name "object_medal"
    romalign 0x1000
    include "build/baserom/object_medal.o"
endseg

beginseg
    name "object_bdan_objects"
    romalign 0x1000
    include "build/baserom/object_bdan_objects.o"
endseg

beginseg
    name "object_sd"
    romalign 0x1000
    include "build/baserom/object_sd.o"
endseg

beginseg
    name "object_rd"
    romalign 0x1000
    include "build/baserom/object_rd.o"
endseg

beginseg
    name "object_po_sisters"
    romalign 0x1000
    include "build/baserom/object_po_sisters.o"
endseg

beginseg
    name "object_heavy_object"
    romalign 0x1000
    include "build/baserom/object_heavy_object.o"
endseg

beginseg
    name "object_gndd"
    romalign 0x1000
    include "build/baserom/object_gndd.o"
endseg

beginseg
    name "object_fd"
    romalign 0x1000
    include "build/baserom/object_fd.o"
endseg

beginseg
    name "object_du"
    romalign 0x1000
    include "build/baserom/object_du.o"
endseg

beginseg
    name "object_fw"
    romalign 0x1000
    include "build/baserom/object_fw.o"
endseg

beginseg
    name "object_horse_link_child"
    romalign 0x1000
    include "build/baserom/object_horse_link_child.o"
endseg

beginseg
    name "object_spot02_objects"
    romalign 0x1000
    include "build/baserom/object_spot02_objects.o"
endseg

beginseg
    name "object_haka"
    romalign 0x1000
    include "build/baserom/object_haka.o"
endseg

beginseg
    name "object_ru1"
    romalign 0x1000
    include "build/baserom/object_ru1.o"
endseg

beginseg
    name "object_syokudai"
    romalign 0x1000
    include "build/baserom/object_syokudai.o"
endseg

beginseg
    name "object_fd2"
    romalign 0x1000
    include "build/baserom/object_fd2.o"
endseg

beginseg
    name "object_dh"
    romalign 0x1000
    include "build/baserom/object_dh.o"
endseg

beginseg
    name "object_rl"
    romalign 0x1000
    include "build/baserom/object_rl.o"
endseg

beginseg
    name "object_efc_tw"
    romalign 0x1000
    include "build/baserom/object_efc_tw.o"
endseg

beginseg
    name "object_demo_tre_lgt"
    romalign 0x1000
    include "build/baserom/object_demo_tre_lgt.o"
endseg

beginseg
    name "object_gi_key"
    romalign 0x1000
    include "build/baserom/object_gi_key.o"
endseg

beginseg
    name "object_mir_ray"
    romalign 0x1000
    include "build/baserom/object_mir_ray.o"
endseg

beginseg
    name "object_brob"
    romalign 0x1000
    include "build/baserom/object_brob.o"
endseg

beginseg
    name "object_gi_jewel"
    romalign 0x1000
    include "build/baserom/object_gi_jewel.o"
endseg

beginseg
    name "object_spot09_obj"
    romalign 0x1000
    include "build/baserom/object_spot09_obj.o"
endseg

beginseg
    name "object_spot18_obj"
    romalign 0x1000
    include "build/baserom/object_spot18_obj.o"
endseg

beginseg
    name "object_bdoor"
    romalign 0x1000
    include "build/baserom/object_bdoor.o"
endseg

beginseg
    name "object_spot17_obj"
    romalign 0x1000
    include "build/baserom/object_spot17_obj.o"
endseg

beginseg
    name "object_shop_dungen"
    romalign 0x1000
    include "build/baserom/object_shop_dungen.o"
endseg

beginseg
    name "object_nb"
    romalign 0x1000
    include "build/baserom/object_nb.o"
endseg

beginseg
    name "object_mo"
    romalign 0x1000
    include "build/baserom/object_mo.o"
endseg

beginseg
    name "object_sb"
    romalign 0x1000
    include "build/baserom/object_sb.o"
endseg

beginseg
    name "object_gi_melody"
    romalign 0x1000
    include "build/baserom/object_gi_melody.o"
endseg

beginseg
    name "object_gi_heart"
    romalign 0x1000
    include "build/baserom/object_gi_heart.o"
endseg

beginseg
    name "object_gi_compass"
    romalign 0x1000
    include "build/baserom/object_gi_compass.o"
endseg

beginseg
    name "object_gi_bosskey"
    romalign 0x1000
    include "build/baserom/object_gi_bosskey.o"
endseg

beginseg
    name "object_gi_medal"
    romalign 0x1000
    include "build/baserom/object_gi_medal.o"
endseg

beginseg
    name "object_gi_nuts"
    romalign 0x1000
    include "build/baserom/object_gi_nuts.o"
endseg

beginseg
    name "object_sa"
    romalign 0x1000
    include "build/baserom/object_sa.o"
endseg

beginseg
    name "object_gi_hearts"
    romalign 0x1000
    include "build/baserom/object_gi_hearts.o"
endseg

beginseg
    name "object_gi_arrowcase"
    romalign 0x1000
    include "build/baserom/object_gi_arrowcase.o"
endseg

beginseg
    name "object_gi_bombpouch"
    romalign 0x1000
    include "build/baserom/object_gi_bombpouch.o"
endseg

beginseg
    name "object_in"
    romalign 0x1000
    include "build/baserom/object_in.o"
endseg

beginseg
    name "object_tr"
    romalign 0x1000
    include "build/baserom/object_tr.o"
endseg

beginseg
    name "object_spot16_obj"
    romalign 0x1000
    include "build/baserom/object_spot16_obj.o"
endseg

beginseg
    name "object_oE1s"
    romalign 0x1000
    include "build/baserom/object_oE1s.o"
endseg

beginseg
    name "object_oE4s"
    romalign 0x1000
    include "build/baserom/object_oE4s.o"
endseg

beginseg
    name "object_os_anime"
    romalign 0x1000
    include "build/baserom/object_os_anime.o"
endseg

beginseg
    name "object_gi_bottle"
    romalign 0x1000
    include "build/baserom/object_gi_bottle.o"
endseg

beginseg
    name "object_gi_stick"
    romalign 0x1000
    include "build/baserom/object_gi_stick.o"
endseg

beginseg
    name "object_gi_map"
    romalign 0x1000
    include "build/baserom/object_gi_map.o"
endseg

beginseg
    name "object_oF1d_map"
    romalign 0x1000
    include "build/baserom/object_oF1d_map.o"
endseg

beginseg
    name "object_ru2"
    romalign 0x1000
    include "build/baserom/object_ru2.o"
endseg

beginseg
    name "object_gi_shield_1"
    romalign 0x1000
    include "build/baserom/object_gi_shield_1.o"
endseg

beginseg
    name "object_dekujr"
    romalign 0x1000
    include "build/baserom/object_dekujr.o"
endseg

beginseg
    name "object_gi_magicpot"
    romalign 0x1000
    include "build/baserom/object_gi_magicpot.o"
endseg

beginseg
    name "object_gi_bomb_1"
    romalign 0x1000
    include "build/baserom/object_gi_bomb_1.o"
endseg

beginseg
    name "object_oF1s"
    romalign 0x1000
    include "build/baserom/object_oF1s.o"
endseg

beginseg
    name "object_ma2"
    romalign 0x1000
    include "build/baserom/object_ma2.o"
endseg

beginseg
    name "object_gi_purse"
    romalign 0x1000
    include "build/baserom/object_gi_purse.o"
endseg

beginseg
    name "object_hni"
    romalign 0x1000
    include "build/baserom/object_hni.o"
endseg

beginseg
    name "object_tw"
    romalign 0x1000
    include "build/baserom/object_tw.o"
endseg

beginseg
    name "object_rr"
    romalign 0x1000
    include "build/baserom/object_rr.o"
endseg

beginseg
    name "object_bxa"
    romalign 0x1000
    include "build/baserom/object_bxa.o"
endseg

beginseg
    name "object_anubice"
    romalign 0x1000
    include "build/baserom/object_anubice.o"
endseg

beginseg
    name "object_gi_gerudo"
    romalign 0x1000
    include "build/baserom/object_gi_gerudo.o"
endseg

beginseg
    name "object_gi_arrow"
    romalign 0x1000
    include "build/baserom/object_gi_arrow.o"
endseg

beginseg
    name "object_gi_bomb_2"
    romalign 0x1000
    include "build/baserom/object_gi_bomb_2.o"
endseg

beginseg
    name "object_gi_egg"
    romalign 0x1000
    include "build/baserom/object_gi_egg.o"
endseg

beginseg
    name "object_gi_scale"
    romalign 0x1000
    include "build/baserom/object_gi_scale.o"
endseg

beginseg
    name "object_gi_shield_2"
    romalign 0x1000
    include "build/baserom/object_gi_shield_2.o"
endseg

beginseg
    name "object_gi_hookshot"
    romalign 0x1000
    include "build/baserom/object_gi_hookshot.o"
endseg

beginseg
    name "object_gi_ocarina"
    romalign 0x1000
    include "build/baserom/object_gi_ocarina.o"
endseg

beginseg
    name "object_gi_milk"
    romalign 0x1000
    include "build/baserom/object_gi_milk.o"
endseg

beginseg
    name "object_ma1"
    romalign 0x1000
    include "build/baserom/object_ma1.o"
endseg

beginseg
    name "object_ganon"
    romalign 0x1000
    include "build/baserom/object_ganon.o"
endseg

beginseg
    name "object_sst"
    romalign 0x1000
    include "build/baserom/object_sst.o"
endseg

beginseg
    name "object_ny"
    romalign 0x1000
    include "build/baserom/object_ny.o"
endseg

beginseg
    name "object_fr"
    romalign 0x1000
    include "build/baserom/object_fr.o"
endseg

beginseg
    name "object_gi_pachinko"
    romalign 0x1000
    include "build/baserom/object_gi_pachinko.o"
endseg

beginseg
    name "object_gi_boomerang"
    romalign 0x1000
    include "build/baserom/object_gi_boomerang.o"
endseg

beginseg
    name "object_gi_bow"
    romalign 0x1000
    include "build/baserom/object_gi_bow.o"
endseg

beginseg
    name "object_gi_glasses"
    romalign 0x1000
    include "build/baserom/object_gi_glasses.o"
endseg

beginseg
    name "object_gi_liquid"
    romalign 0x1000
    include "build/baserom/object_gi_liquid.o"
endseg

beginseg
    name "object_ani"
    romalign 0x1000
    include "build/baserom/object_ani.o"
endseg

beginseg
    name "object_demo_6k"
    romalign 0x1000
    include "build/baserom/object_demo_6k.o"
endseg

beginseg
    name "object_gi_shield_3"
    romalign 0x1000
    include "build/baserom/object_gi_shield_3.o"
endseg

beginseg
    name "object_gi_letter"
    romalign 0x1000
    include "build/baserom/object_gi_letter.o"
endseg

beginseg
    name "object_spot15_obj"
    romalign 0x1000
    include "build/baserom/object_spot15_obj.o"
endseg

beginseg
    name "object_jya_obj"
    romalign 0x1000
    include "build/baserom/object_jya_obj.o"
endseg

beginseg
    name "object_gi_clothes"
    romalign 0x1000
    include "build/baserom/object_gi_clothes.o"
endseg

beginseg
    name "object_gi_bean"
    romalign 0x1000
    include "build/baserom/object_gi_bean.o"
endseg

beginseg
    name "object_gi_fish"
    romalign 0x1000
    include "build/baserom/object_gi_fish.o"
endseg

beginseg
    name "object_gi_saw"
    romalign 0x1000
    include "build/baserom/object_gi_saw.o"
endseg

beginseg
    name "object_gi_hammer"
    romalign 0x1000
    include "build/baserom/object_gi_hammer.o"
endseg

beginseg
    name "object_gi_grass"
    romalign 0x1000
    include "build/baserom/object_gi_grass.o"
endseg

beginseg
    name "object_gi_longsword"
    romalign 0x1000
    include "build/baserom/object_gi_longsword.o"
endseg

beginseg
    name "object_spot01_objects"
    romalign 0x1000
    include "build/baserom/object_spot01_objects.o"
endseg

beginseg
    name "object_md"
    romalign 0x1000
    include "build/baserom/object_md.o"
endseg

beginseg
    name "object_km1"
    romalign 0x1000
    include "build/baserom/object_km1.o"
endseg

beginseg
    name "object_kw1"
    romalign 0x1000
    include "build/baserom/object_kw1.o"
endseg

beginseg
    name "object_zo"
    romalign 0x1000
    include "build/baserom/object_zo.o"
endseg

beginseg
    name "object_kz"
    romalign 0x1000
    include "build/baserom/object_kz.o"
endseg

beginseg
    name "object_umajump"
    romalign 0x1000
    include "build/baserom/object_umajump.o"
endseg

beginseg
    name "object_masterkokiri"
    romalign 0x1000
    include "build/baserom/object_masterkokiri.o"
endseg

beginseg
    name "object_masterkokirihead"
    romalign 0x1000
    include "build/baserom/object_masterkokirihead.o"
endseg

beginseg
    name "object_mastergolon"
    romalign 0x1000
    include "build/baserom/object_mastergolon.o"
endseg

beginseg
    name "object_masterzoora"
    romalign 0x1000
    include "build/baserom/object_masterzoora.o"
endseg

beginseg
    name "object_aob"
    romalign 0x1000
    include "build/baserom/object_aob.o"
endseg

beginseg
    name "object_ik"
    romalign 0x1000
    include "build/baserom/object_ik.o"
endseg

beginseg
    name "object_ahg"
    romalign 0x1000
    include "build/baserom/object_ahg.o"
endseg

beginseg
    name "object_cne"
    romalign 0x1000
    include "build/baserom/object_cne.o"
endseg

beginseg
    name "object_gi_niwatori"
    romalign 0x1000
    include "build/baserom/object_gi_niwatori.o"
endseg

beginseg
    name "object_skj"
    romalign 0x1000
    include "build/baserom/object_skj.o"
endseg

beginseg
    name "object_gi_bottle_letter"
    romalign 0x1000
    include "build/baserom/object_gi_bottle_letter.o"
endseg

beginseg
    name "object_bji"
    romalign 0x1000
    include "build/baserom/object_bji.o"
endseg

beginseg
    name "object_bba"
    romalign 0x1000
    include "build/baserom/object_bba.o"
endseg

beginseg
    name "object_gi_ocarina_0"
    romalign 0x1000
    include "build/baserom/object_gi_ocarina_0.o"
endseg

beginseg
    name "object_ds"
    romalign 0x1000
    include "build/baserom/object_ds.o"
endseg

beginseg
    name "object_ane"
    romalign 0x1000
    include "build/baserom/object_ane.o"
endseg

beginseg
    name "object_boj"
    romalign 0x1000
    include "build/baserom/object_boj.o"
endseg

beginseg
    name "object_spot03_object"
    romalign 0x1000
    include "build/baserom/object_spot03_object.o"
endseg

beginseg
    name "object_spot07_object"
    romalign 0x1000
    include "build/baserom/object_spot07_object.o"
endseg

beginseg
    name "object_fz"
    romalign 0x1000
    include "build/baserom/object_fz.o"
endseg

beginseg
    name "object_bob"
    romalign 0x1000
    include "build/baserom/object_bob.o"
endseg

beginseg
    name "object_ge1"
    romalign 0x1000
    include "build/baserom/object_ge1.o"
endseg

beginseg
    name "object_yabusame_point"
    romalign 0x1000
    include "build/baserom/object_yabusame_point.o"
endseg

beginseg
    name "object_gi_boots_2"
    romalign 0x1000
    include "build/baserom/object_gi_boots_2.o"
endseg

beginseg
    name "object_gi_seed"
    romalign 0x1000
    include "build/baserom/object_gi_seed.o"
endseg

beginseg
    name "object_gnd_magic"
    romalign 0x1000
    include "build/baserom/object_gnd_magic.o"
endseg

beginseg
    name "object_d_elevator"
    romalign 0x1000
    include "build/baserom/object_d_elevator.o"
endseg

beginseg
    name "object_d_hsblock"
    romalign 0x1000
    include "build/baserom/object_d_hsblock.o"
endseg

beginseg
    name "object_d_lift"
    romalign 0x1000
    include "build/baserom/object_d_lift.o"
endseg

beginseg
    name "object_mamenoki"
    romalign 0x1000
    include "build/baserom/object_mamenoki.o"
endseg

beginseg
    name "object_goroiwa"
    romalign 0x1000
    include "build/baserom/object_goroiwa.o"
endseg

beginseg
    name "object_toryo"
    romalign 0x1000
    include "build/baserom/object_toryo.o"
endseg

beginseg
    name "object_daiku"
    romalign 0x1000
    include "build/baserom/object_daiku.o"
endseg

beginseg
    name "object_nwc"
    romalign 0x1000
    include "build/baserom/object_nwc.o"
endseg

beginseg
    name "object_blkobj"
    romalign 0x1000
    include "build/baserom/object_blkobj.o"
endseg

beginseg
    name "object_gm"
    romalign 0x1000
    include "build/baserom/object_gm.o"
endseg

beginseg
    name "object_ms"
    romalign 0x1000
    include "build/baserom/object_ms.o"
endseg

beginseg
    name "object_hs"
    romalign 0x1000
    include "build/baserom/object_hs.o"
endseg

beginseg
    name "object_ingate"
    romalign 0x1000
    include "build/baserom/object_ingate.o"
endseg

beginseg
    name "object_lightswitch"
    romalign 0x1000
    include "build/baserom/object_lightswitch.o"
endseg

beginseg
    name "object_kusa"
    romalign 0x1000
    include "build/baserom/object_kusa.o"
endseg

beginseg
    name "object_tsubo"
    romalign 0x1000
    include "build/baserom/object_tsubo.o"
endseg

beginseg
    name "object_gi_gloves"
    romalign 0x1000
    include "build/baserom/object_gi_gloves.o"
endseg

beginseg
    name "object_gi_coin"
    romalign 0x1000
    include "build/baserom/object_gi_coin.o"
endseg

beginseg
    name "object_kanban"
    romalign 0x1000
    include "build/baserom/object_kanban.o"
endseg

beginseg
    name "object_gjyo_objects"
    romalign 0x1000
    include "build/baserom/object_gjyo_objects.o"
endseg

beginseg
    name "object_owl"
    romalign 0x1000
    include "build/baserom/object_owl.o"
endseg

beginseg
    name "object_mk"
    romalign 0x1000
    include "build/baserom/object_mk.o"
endseg

beginseg
    name "object_fu"
    romalign 0x1000
    include "build/baserom/object_fu.o"
endseg

beginseg
    name "object_gi_ki_tan_mask"
    romalign 0x1000
    include "build/baserom/object_gi_ki_tan_mask.o"
endseg

beginseg
    name "object_gi_redead_mask"
    romalign 0x1000
    include "build/baserom/object_gi_redead_mask.o"
endseg

beginseg
    name "object_gi_skj_mask"
    romalign 0x1000
    include "build/baserom/object_gi_skj_mask.o"
endseg

beginseg
    name "object_gi_rabit_mask"
    romalign 0x1000
    include "build/baserom/object_gi_rabit_mask.o"
endseg

beginseg
    name "object_gi_truth_mask"
    romalign 0x1000
    include "build/baserom/object_gi_truth_mask.o"
endseg

beginseg
    name "object_ganon_objects"
    romalign 0x1000
    include "build/baserom/object_ganon_objects.o"
endseg

beginseg
    name "object_siofuki"
    romalign 0x1000
    include "build/baserom/object_siofuki.o"
endseg

beginseg
    name "object_stream"
    romalign 0x1000
    include "build/baserom/object_stream.o"
endseg

beginseg
    name "object_mm"
    romalign 0x1000
    include "build/baserom/object_mm.o"
endseg

beginseg
    name "object_fa"
    romalign 0x1000
    include "build/baserom/object_fa.o"
endseg

beginseg
    name "object_os"
    romalign 0x1000
    include "build/baserom/object_os.o"
endseg

beginseg
    name "object_gi_eye_lotion"
    romalign 0x1000
    include "build/baserom/object_gi_eye_lotion.o"
endseg

beginseg
    name "object_gi_powder"
    romalign 0x1000
    include "build/baserom/object_gi_powder.o"
endseg

beginseg
    name "object_gi_mushroom"
    romalign 0x1000
    include "build/baserom/object_gi_mushroom.o"
endseg

beginseg
    name "object_gi_ticketstone"
    romalign 0x1000
    include "build/baserom/object_gi_ticketstone.o"
endseg

beginseg
    name "object_gi_brokensword"
    romalign 0x1000
    include "build/baserom/object_gi_brokensword.o"
endseg

beginseg
    name "object_js"
    romalign 0x1000
    include "build/baserom/object_js.o"
endseg

beginseg
    name "object_cs"
    romalign 0x1000
    include "build/baserom/object_cs.o"
endseg

beginseg
    name "object_gi_prescription"
    romalign 0x1000
    include "build/baserom/object_gi_prescription.o"
endseg

beginseg
    name "object_gi_bracelet"
    romalign 0x1000
    include "build/baserom/object_gi_bracelet.o"
endseg

beginseg
    name "object_gi_soldout"
    romalign 0x1000
    include "build/baserom/object_gi_soldout.o"
endseg

beginseg
    name "object_gi_frog"
    romalign 0x1000
    include "build/baserom/object_gi_frog.o"
endseg

beginseg
    name "object_mag"
    romalign 0x1000
    include "build/baserom/object_mag.o"
endseg

beginseg
    name "object_door_gerudo"
    romalign 0x1000
    include "build/baserom/object_door_gerudo.o"
endseg

beginseg
    name "object_gt"
    romalign 0x1000
    include "build/baserom/object_gt.o"
endseg

beginseg
    name "object_efc_erupc"
    romalign 0x1000
    include "build/baserom/object_efc_erupc.o"
endseg

beginseg
    name "object_zl2_anime1"
    romalign 0x1000
    include "build/baserom/object_zl2_anime1.o"
endseg

beginseg
    name "object_zl2_anime2"
    romalign 0x1000
    include "build/baserom/object_zl2_anime2.o"
endseg

beginseg
    name "object_gi_golonmask"
    romalign 0x1000
    include "build/baserom/object_gi_golonmask.o"
endseg

beginseg
    name "object_gi_zoramask"
    romalign 0x1000
    include "build/baserom/object_gi_zoramask.o"
endseg

beginseg
    name "object_gi_gerudomask"
    romalign 0x1000
    include "build/baserom/object_gi_gerudomask.o"
endseg

beginseg
    name "object_ganon2"
    romalign 0x1000
    include "build/baserom/object_ganon2.o"
endseg

beginseg
    name "object_ka"
    romalign 0x1000
    include "build/baserom/object_ka.o"
endseg

beginseg
    name "object_ts"
    romalign 0x1000
    include "build/baserom/object_ts.o"
endseg

beginseg
    name "object_zg"
    romalign 0x1000
    include "build/baserom/object_zg.o"
endseg

beginseg
    name "object_gi_hoverboots"
    romalign 0x1000
    include "build/baserom/object_gi_hoverboots.o"
endseg

beginseg
    name "object_gi_m_arrow"
    romalign 0x1000
    include "build/baserom/object_gi_m_arrow.o"
endseg

beginseg
    name "object_ds2"
    romalign 0x1000
    include "build/baserom/object_ds2.o"
endseg

beginseg
    name "object_ec"
    romalign 0x1000
    include "build/baserom/object_ec.o"
endseg

beginseg
    name "object_fish"
    romalign 0x1000
    include "build/baserom/object_fish.o"
endseg

beginseg
    name "object_gi_sutaru"
    romalign 0x1000
    include "build/baserom/object_gi_sutaru.o"
endseg

beginseg
    name "object_gi_goddess"
    romalign 0x1000
    include "build/baserom/object_gi_goddess.o"
endseg

beginseg
    name "object_ssh"
    romalign 0x1000
    include "build/baserom/object_ssh.o"
endseg

beginseg
    name "object_bigokuta"
    romalign 0x1000
    include "build/baserom/object_bigokuta.o"
endseg

beginseg
    name "object_bg"
    romalign 0x1000
    include "build/baserom/object_bg.o"
endseg

beginseg
    name "object_spot05_objects"
    romalign 0x1000
    include "build/baserom/object_spot05_objects.o"
endseg

beginseg
    name "object_spot12_obj"
    romalign 0x1000
    include "build/baserom/object_spot12_obj.o"
endseg

beginseg
    name "object_bombiwa"
    romalign 0x1000
    include "build/baserom/object_bombiwa.o"
endseg

beginseg
    name "object_hintnuts"
    romalign 0x1000
    include "build/baserom/object_hintnuts.o"
endseg

beginseg
    name "object_rs"
    romalign 0x1000
    include "build/baserom/object_rs.o"
endseg

beginseg
    name "object_spot00_break"
    romalign 0x1000
    include "build/baserom/object_spot00_break.o"
endseg

beginseg
    name "object_gla"
    romalign 0x1000
    include "build/baserom/object_gla.o"
endseg

beginseg
    name "object_shopnuts"
    romalign 0x1000
    include "build/baserom/object_shopnuts.o"
endseg

beginseg
    name "object_geldb"
    romalign 0x1000
    include "build/baserom/object_geldb.o"
endseg

beginseg
    name "object_gr"
    romalign 0x1000
    include "build/baserom/object_gr.o"
endseg

beginseg
    name "object_dog"
    romalign 0x1000
    include "build/baserom/object_dog.o"
endseg

beginseg
    name "object_jya_iron"
    romalign 0x1000
    include "build/baserom/object_jya_iron.o"
endseg

beginseg
    name "object_jya_door"
    romalign 0x1000
    include "build/baserom/object_jya_door.o"
endseg

beginseg
    name "object_spot01_objects2"
    romalign 0x1000
    include "build/baserom/object_spot01_objects2.o"
endseg

beginseg
    name "object_spot11_obj"
    romalign 0x1000
    include "build/baserom/object_spot11_obj.o"
endseg

beginseg
    name "object_kibako2"
    romalign 0x1000
    include "build/baserom/object_kibako2.o"
endseg

beginseg
    name "object_dns"
    romalign 0x1000
    include "build/baserom/object_dns.o"
endseg

beginseg
    name "object_dnk"
    romalign 0x1000
    include "build/baserom/object_dnk.o"
endseg

beginseg
    name "object_gi_fire"
    romalign 0x1000
    include "build/baserom/object_gi_fire.o"
endseg

beginseg
    name "object_gi_insect"
    romalign 0x1000
    include "build/baserom/object_gi_insect.o"
endseg

beginseg
    name "object_gi_butterfly"
    romalign 0x1000
    include "build/baserom/object_gi_butterfly.o"
endseg

beginseg
    name "object_gi_ghost"
    romalign 0x1000
    include "build/baserom/object_gi_ghost.o"
endseg

beginseg
    name "object_gi_soul"
    romalign 0x1000
    include "build/baserom/object_gi_soul.o"
endseg

beginseg
    name "object_bowl"
    romalign 0x1000
    include "build/baserom/object_bowl.o"
endseg

beginseg
    name "object_po_field"
    romalign 0x1000
    include "build/baserom/object_po_field.o"
endseg

beginseg
    name "object_demo_kekkai"
    romalign 0x1000
    include "build/baserom/object_demo_kekkai.o"
endseg

beginseg
    name "object_efc_doughnut"
    romalign 0x1000
    include "build/baserom/object_efc_doughnut.o"
endseg

beginseg
    name "object_gi_dekupouch"
    romalign 0x1000
    include "build/baserom/object_gi_dekupouch.o"
endseg

beginseg
    name "object_ganon_anime1"
    romalign 0x1000
    include "build/baserom/object_ganon_anime1.o"
endseg

beginseg
    name "object_ganon_anime2"
    romalign 0x1000
    include "build/baserom/object_ganon_anime2.o"
endseg

beginseg
    name "object_ganon_anime3"
    romalign 0x1000
    include "build/baserom/object_ganon_anime3.o"
endseg

beginseg
    name "object_gi_rupy"
    romalign 0x1000
    include "build/baserom/object_gi_rupy.o"
endseg

beginseg
    name "object_spot01_matoya"
    romalign 0x1000
    include "build/baserom/object_spot01_matoya.o"
endseg

beginseg
    name "object_spot01_matoyab"
    romalign 0x1000
    include "build/baserom/object_spot01_matoyab.o"
endseg

beginseg
    name "object_po_composer"
    romalign 0x1000
    include "build/baserom/object_po_composer.o"
endseg

beginseg
    name "object_mu"
    romalign 0x1000
    include "build/baserom/object_mu.o"
endseg

beginseg
    name "object_wf"
    romalign 0x1000
    include "build/baserom/object_wf.o"
endseg

beginseg
    name "object_skb"
    romalign 0x1000
    include "build/baserom/object_skb.o"
endseg

beginseg
    name "object_gj"
    romalign 0x1000
    include "build/baserom/object_gj.o"
endseg

beginseg
    name "object_geff"
    romalign 0x1000
    include "build/baserom/object_geff.o"
endseg

beginseg
    name "object_haka_door"
    romalign 0x1000
    include "build/baserom/object_haka_door.o"
endseg

beginseg
    name "object_gs"
    romalign 0x1000
    include "build/baserom/object_gs.o"
endseg

beginseg
    name "object_ps"
    romalign 0x1000
    include "build/baserom/object_ps.o"
endseg

beginseg
    name "object_bwall"
    romalign 0x1000
    include "build/baserom/object_bwall.o"
endseg

beginseg
    name "object_crow"
    romalign 0x1000
    include "build/baserom/object_crow.o"
endseg

beginseg
    name "object_cow"
    romalign 0x1000
    include "build/baserom/object_cow.o"
endseg

beginseg
    name "object_cob"
    romalign 0x1000
    include "build/baserom/object_cob.o"
endseg

beginseg
    name "object_gi_sword_1"
    romalign 0x1000
    include "build/baserom/object_gi_sword_1.o"
endseg

beginseg
    name "object_door_killer"
    romalign 0x1000
    include "build/baserom/object_door_killer.o"
endseg

beginseg
    name "object_ouke_haka"
    romalign 0x1000
    include "build/baserom/object_ouke_haka.o"
endseg

beginseg
    name "object_timeblock"
    romalign 0x1000
    include "build/baserom/object_timeblock.o"
endseg

beginseg
    name "object_zl4"
    romalign 0x1000
    include "build/baserom/object_zl4.o"
endseg

beginseg
    name "g_pn_01"
    romalign 0x1000
    include "build/baserom/g_pn_01.o"
endseg

beginseg
    name "g_pn_02"
    romalign 0x1000
    include "build/baserom/g_pn_02.o"
endseg

beginseg
    name "g_pn_03"
    romalign 0x1000
    include "build/baserom/g_pn_03.o"
endseg

beginseg
    name "g_pn_04"
    romalign 0x1000
    include "build/baserom/g_pn_04.o"
endseg

beginseg
    name "g_pn_05"
    romalign 0x1000
    include "build/baserom/g_pn_05.o"
endseg

beginseg
    name "g_pn_06"
    romalign 0x1000
    include "build/baserom/g_pn_06.o"
endseg

beginseg
    name "g_pn_07"
    romalign 0x1000
    include "build/baserom/g_pn_07.o"
endseg

beginseg
    name "g_pn_08"
    romalign 0x1000
    include "build/baserom/g_pn_08.o"
endseg

beginseg
    name "g_pn_09"
    romalign 0x1000
    include "build/baserom/g_pn_09.o"
endseg

beginseg
    name "g_pn_10"
    romalign 0x1000
    include "build/baserom/g_pn_10.o"
endseg

beginseg
    name "g_pn_11"
    romalign 0x1000
    include "build/baserom/g_pn_11.o"
endseg

beginseg
    name "g_pn_12"
    romalign 0x1000
    include "build/baserom/g_pn_12.o"
endseg

beginseg
    name "g_pn_13"
    romalign 0x1000
    include "build/baserom/g_pn_13.o"
endseg

beginseg
    name "g_pn_14"
    romalign 0x1000
    include "build/baserom/g_pn_14.o"
endseg

beginseg
    name "g_pn_15"
    romalign 0x1000
    include "build/baserom/g_pn_15.o"
endseg

beginseg
    name "g_pn_16"
    romalign 0x1000
    include "build/baserom/g_pn_16.o"
endseg

beginseg
    name "g_pn_17"
    romalign 0x1000
    include "build/baserom/g_pn_17.o"
endseg

beginseg
    name "g_pn_18"
    romalign 0x1000
    include "build/baserom/g_pn_18.o"
endseg

beginseg
    name "g_pn_19"
    romalign 0x1000
    include "build/baserom/g_pn_19.o"
endseg

beginseg
    name "g_pn_20"
    romalign 0x1000
    include "build/baserom/g_pn_20.o"
endseg

beginseg
    name "g_pn_21"
    romalign 0x1000
    include "build/baserom/g_pn_21.o"
endseg

beginseg
    name "g_pn_22"
    romalign 0x1000
    include "build/baserom/g_pn_22.o"
endseg

beginseg
    name "g_pn_23"
    romalign 0x1000
    include "build/baserom/g_pn_23.o"
endseg

beginseg
    name "g_pn_24"
    romalign 0x1000
    include "build/baserom/g_pn_24.o"
endseg

beginseg
    name "g_pn_25"
    romalign 0x1000
    include "build/baserom/g_pn_25.o"
endseg

beginseg
    name "g_pn_26"
    romalign 0x1000
    include "build/baserom/g_pn_26.o"
endseg

beginseg
    name "g_pn_27"
    romalign 0x1000
    include "build/baserom/g_pn_27.o"
endseg

beginseg
    name "g_pn_28"
    romalign 0x1000
    include "build/baserom/g_pn_28.o"
endseg

beginseg
    name "g_pn_29"
    romalign 0x1000
    include "build/baserom/g_pn_29.o"
endseg

beginseg
    name "g_pn_30"
    romalign 0x1000
    include "build/baserom/g_pn_30.o"
endseg

beginseg
    name "g_pn_31"
    romalign 0x1000
    include "build/baserom/g_pn_31.o"
endseg

beginseg
    name "g_pn_32"
    romalign 0x1000
    include "build/baserom/g_pn_32.o"
endseg

beginseg
    name "g_pn_33"
    romalign 0x1000
    include "build/baserom/g_pn_33.o"
endseg

beginseg
    name "g_pn_34"
    romalign 0x1000
    include "build/baserom/g_pn_34.o"
endseg

beginseg
    name "g_pn_35"
    romalign 0x1000
    include "build/baserom/g_pn_35.o"
endseg

beginseg
    name "g_pn_36"
    romalign 0x1000
    include "build/baserom/g_pn_36.o"
endseg

beginseg
    name "g_pn_37"
    romalign 0x1000
    include "build/baserom/g_pn_37.o"
endseg

beginseg
    name "g_pn_38"
    romalign 0x1000
    include "build/baserom/g_pn_38.o"
endseg

beginseg
    name "g_pn_39"
    romalign 0x1000
    include "build/baserom/g_pn_39.o"
endseg

beginseg
    name "g_pn_40"
    romalign 0x1000
    include "build/baserom/g_pn_40.o"
endseg

beginseg
    name "g_pn_41"
    romalign 0x1000
    include "build/baserom/g_pn_41.o"
endseg

beginseg
    name "g_pn_42"
    romalign 0x1000
    include "build/baserom/g_pn_42.o"
endseg

beginseg
    name "g_pn_43"
    romalign 0x1000
    include "build/baserom/g_pn_43.o"
endseg

beginseg
    name "g_pn_44"
    romalign 0x1000
    include "build/baserom/g_pn_44.o"
endseg

beginseg
    name "g_pn_45"
    romalign 0x1000
    include "build/baserom/g_pn_45.o"
endseg

beginseg
    name "g_pn_46"
    romalign 0x1000
    include "build/baserom/g_pn_46.o"
endseg

beginseg
    name "g_pn_47"
    romalign 0x1000
    include "build/baserom/g_pn_47.o"
endseg

beginseg
    name "g_pn_48"
    romalign 0x1000
    include "build/baserom/g_pn_48.o"
endseg

beginseg
    name "g_pn_49"
    romalign 0x1000
    include "build/baserom/g_pn_49.o"
endseg

beginseg
    name "g_pn_50"
    romalign 0x1000
    include "build/baserom/g_pn_50.o"
endseg

beginseg
    name "g_pn_51"
    romalign 0x1000
    include "build/baserom/g_pn_51.o"
endseg

beginseg
    name "g_pn_52"
    romalign 0x1000
    include "build/baserom/g_pn_52.o"
endseg

beginseg
    name "g_pn_53"
    romalign 0x1000
    include "build/baserom/g_pn_53.o"
endseg

beginseg
    name "g_pn_54"
    romalign 0x1000
    include "build/baserom/g_pn_54.o"
endseg

beginseg
    name "g_pn_55"
    romalign 0x1000
    include "build/baserom/g_pn_55.o"
endseg

beginseg
    name "g_pn_56"
    romalign 0x1000
    include "build/baserom/g_pn_56.o"
endseg

beginseg
    name "g_pn_57"
    romalign 0x1000
    include "build/baserom/g_pn_57.o"
endseg

beginseg
    name "z_select_static"
    romalign 0x1000
    include "build/baserom/z_select_static.o"
endseg

beginseg
    name "nintendo_rogo_static"
    romalign 0x1000
    include "build/baserom/nintendo_rogo_static.o"
endseg

beginseg
    name "title_static"
    romalign 0x1000
    include "build/baserom/title_static.o"
endseg

beginseg
    name "parameter_static"
    romalign 0x1000
    include "build/baserom/parameter_static.o"
endseg

beginseg
    name "vr_fine0_static"
    romalign 0x1000
    include "build/baserom/vr_fine0_static.o"
endseg

beginseg
    name "vr_fine0_pal_static"
    romalign 0x1000
    include "build/baserom/vr_fine0_pal_static.o"
endseg

beginseg
    name "vr_fine1_static"
    romalign 0x1000
    include "build/baserom/vr_fine1_static.o"
endseg

beginseg
    name "vr_fine1_pal_static"
    romalign 0x1000
    include "build/baserom/vr_fine1_pal_static.o"
endseg

beginseg
    name "vr_fine2_static"
    romalign 0x1000
    include "build/baserom/vr_fine2_static.o"
endseg

beginseg
    name "vr_fine2_pal_static"
    romalign 0x1000
    include "build/baserom/vr_fine2_pal_static.o"
endseg

beginseg
    name "vr_fine3_static"
    romalign 0x1000
    include "build/baserom/vr_fine3_static.o"
endseg

beginseg
    name "vr_fine3_pal_static"
    romalign 0x1000
    include "build/baserom/vr_fine3_pal_static.o"
endseg

beginseg
    name "vr_cloud0_static"
    romalign 0x1000
    include "build/baserom/vr_cloud0_static.o"
endseg

beginseg
    name "vr_cloud0_pal_static"
    romalign 0x1000
    include "build/baserom/vr_cloud0_pal_static.o"
endseg

beginseg
    name "vr_cloud1_static"
    romalign 0x1000
    include "build/baserom/vr_cloud1_static.o"
endseg

beginseg
    name "vr_cloud1_pal_static"
    romalign 0x1000
    include "build/baserom/vr_cloud1_pal_static.o"
endseg

beginseg
    name "vr_cloud2_static"
    romalign 0x1000
    include "build/baserom/vr_cloud2_static.o"
endseg

beginseg
    name "vr_cloud2_pal_static"
    romalign 0x1000
    include "build/baserom/vr_cloud2_pal_static.o"
endseg

beginseg
    name "vr_cloud3_static"
    romalign 0x1000
    include "build/baserom/vr_cloud3_static.o"
endseg

beginseg
    name "vr_cloud3_pal_static"
    romalign 0x1000
    include "build/baserom/vr_cloud3_pal_static.o"
endseg

beginseg
    name "vr_holy0_static"
    romalign 0x1000
    include "build/baserom/vr_holy0_static.o"
endseg

beginseg
    name "vr_holy0_pal_static"
    romalign 0x1000
    include "build/baserom/vr_holy0_pal_static.o"
endseg

beginseg
    name "vr_holy1_static"
    romalign 0x1000
    include "build/baserom/vr_holy1_static.o"
endseg

beginseg
    name "vr_holy1_pal_static"
    romalign 0x1000
    include "build/baserom/vr_holy1_pal_static.o"
endseg

beginseg
    name "vr_MDVR_static"
    romalign 0x1000
    include "build/baserom/vr_MDVR_static.o"
endseg

beginseg
    name "vr_MDVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_MDVR_pal_static.o"
endseg

beginseg
    name "vr_MNVR_static"
    romalign 0x1000
    include "build/baserom/vr_MNVR_static.o"
endseg

beginseg
    name "vr_MNVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_MNVR_pal_static.o"
endseg

beginseg
    name "vr_RUVR_static"
    romalign 0x1000
    include "build/baserom/vr_RUVR_static.o"
endseg

beginseg
    name "vr_RUVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_RUVR_pal_static.o"
endseg

beginseg
    name "vr_LHVR_static"
    romalign 0x1000
    include "build/baserom/vr_LHVR_static.o"
endseg

beginseg
    name "vr_LHVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_LHVR_pal_static.o"
endseg

beginseg
    name "vr_KHVR_static"
    romalign 0x1000
    include "build/baserom/vr_KHVR_static.o"
endseg

beginseg
    name "vr_KHVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_KHVR_pal_static.o"
endseg

beginseg
    name "vr_K3VR_static"
    romalign 0x1000
    include "build/baserom/vr_K3VR_static.o"
endseg

beginseg
    name "vr_K3VR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_K3VR_pal_static.o"
endseg

beginseg
    name "vr_K4VR_static"
    romalign 0x1000
    include "build/baserom/vr_K4VR_static.o"
endseg

beginseg
    name "vr_K4VR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_K4VR_pal_static.o"
endseg

beginseg
    name "vr_K5VR_static"
    romalign 0x1000
    include "build/baserom/vr_K5VR_static.o"
endseg

beginseg
    name "vr_K5VR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_K5VR_pal_static.o"
endseg

beginseg
    name "vr_SP1a_static"
    romalign 0x1000
    include "build/baserom/vr_SP1a_static.o"
endseg

beginseg
    name "vr_SP1a_pal_static"
    romalign 0x1000
    include "build/baserom/vr_SP1a_pal_static.o"
endseg

beginseg
    name "vr_MLVR_static"
    romalign 0x1000
    include "build/baserom/vr_MLVR_static.o"
endseg

beginseg
    name "vr_MLVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_MLVR_pal_static.o"
endseg

beginseg
    name "vr_KKRVR_static"
    romalign 0x1000
    include "build/baserom/vr_KKRVR_static.o"
endseg

beginseg
    name "vr_KKRVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_KKRVR_pal_static.o"
endseg

beginseg
    name "vr_KR3VR_static"
    romalign 0x1000
    include "build/baserom/vr_KR3VR_static.o"
endseg

beginseg
    name "vr_KR3VR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_KR3VR_pal_static.o"
endseg

beginseg
    name "vr_IPVR_static"
    romalign 0x1000
    include "build/baserom/vr_IPVR_static.o"
endseg

beginseg
    name "vr_IPVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_IPVR_pal_static.o"
endseg

beginseg
    name "vr_KSVR_static"
    romalign 0x1000
    include "build/baserom/vr_KSVR_static.o"
endseg

beginseg
    name "vr_KSVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_KSVR_pal_static.o"
endseg

beginseg
    name "vr_GLVR_static"
    romalign 0x1000
    include "build/baserom/vr_GLVR_static.o"
endseg

beginseg
    name "vr_GLVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_GLVR_pal_static.o"
endseg

beginseg
    name "vr_ZRVR_static"
    romalign 0x1000
    include "build/baserom/vr_ZRVR_static.o"
endseg

beginseg
    name "vr_ZRVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_ZRVR_pal_static.o"
endseg

beginseg
    name "vr_DGVR_static"
    romalign 0x1000
    include "build/baserom/vr_DGVR_static.o"
endseg

beginseg
    name "vr_DGVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_DGVR_pal_static.o"
endseg

beginseg
    name "vr_ALVR_static"
    romalign 0x1000
    include "build/baserom/vr_ALVR_static.o"
endseg

beginseg
    name "vr_ALVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_ALVR_pal_static.o"
endseg

beginseg
    name "vr_NSVR_static"
    romalign 0x1000
    include "build/baserom/vr_NSVR_static.o"
endseg

beginseg
    name "vr_NSVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_NSVR_pal_static.o"
endseg

beginseg
    name "vr_LBVR_static"
    romalign 0x1000
    include "build/baserom/vr_LBVR_static.o"
endseg

beginseg
    name "vr_LBVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_LBVR_pal_static.o"
endseg

beginseg
    name "vr_TTVR_static"
    romalign 0x1000
    include "build/baserom/vr_TTVR_static.o"
endseg

beginseg
    name "vr_TTVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_TTVR_pal_static.o"
endseg

beginseg
    name "vr_FCVR_static"
    romalign 0x1000
    include "build/baserom/vr_FCVR_static.o"
endseg

beginseg
    name "vr_FCVR_pal_static"
    romalign 0x1000
    include "build/baserom/vr_FCVR_pal_static.o"
endseg

beginseg
    name "elf_message_field"
    romalign 0x1000
    include "build/baserom/elf_message_field.o"
endseg

beginseg
    name "elf_message_ydan"
    romalign 0x1000
    include "build/baserom/elf_message_ydan.o"
endseg

beginseg
    name "ydan_scene"
    romalign 0x1000
    include "zelda_scene/Ydan/ydan_main_info.o"
    include "zelda_tool_scene/Ydan/ydan_show.o"
    include "zelda_scene/Ydan/ydan_bgc.o"
    include "zelda_scene/Ydan/ydan_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ydan_room_0"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM0/ydan_0_info.o"
    include "zelda_scene/Ydan/ROOM0/ydan_0_shape.o"
    include "zelda_scene/Ydan/ROOM0/ydan_grp_yda00_00.o"
    include "zelda_scene/Ydan/ROOM0/ydan_grp_yda00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_1"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM1/ydan_1_info.o"
    include "zelda_scene/Ydan/ROOM1/ydan_1_shape.o"
    include "zelda_scene/Ydan/ROOM1/ydan_grp_yda01_00.o"
    include "zelda_scene/Ydan/ROOM1/ydan_grp_yda01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_2"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM2/ydan_2_info.o"
    include "zelda_scene/Ydan/ROOM2/ydan_2_shape.o"
    include "zelda_scene/Ydan/ROOM2/ydan_grp_yda02_00.o"
    include "zelda_scene/Ydan/ROOM2/ydan_grp_yda02_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_3"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM3/ydan_3_info.o"
    include "zelda_scene/Ydan/ROOM3/ydan_3_shape.o"
    include "zelda_scene/Ydan/ROOM3/ydan_grp_yda03_00.o"
    include "zelda_scene/Ydan/ROOM3/ydan_grp_yda03_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_4"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM4/ydan_4_info.o"
    include "zelda_scene/Ydan/ROOM4/ydan_4_shape.o"
    include "zelda_scene/Ydan/ROOM4/ydan_grp_yda04_00.o"
    include "zelda_scene/Ydan/ROOM4/ydan_grp_yda04_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_5"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM5/ydan_5_info.o"
    include "zelda_scene/Ydan/ROOM5/ydan_5_shape.o"
    include "zelda_scene/Ydan/ROOM5/ydan_grp_yda05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_6"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM6/ydan_6_info.o"
    include "zelda_scene/Ydan/ROOM6/ydan_6_shape.o"
    include "zelda_scene/Ydan/ROOM6/ydan_grp_yda06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_7"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM7/ydan_7_info.o"
    include "zelda_scene/Ydan/ROOM7/ydan_7_shape.o"
    include "zelda_scene/Ydan/ROOM7/ydan_grp_yda07_00.o"
    include "zelda_scene/Ydan/ROOM7/ydan_grp_yda07_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_8"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM8/ydan_8_info.o"
    include "zelda_scene/Ydan/ROOM8/ydan_8_shape.o"
    include "zelda_scene/Ydan/ROOM8/ydan_grp_yda08_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_9"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM9/ydan_9_info.o"
    include "zelda_scene/Ydan/ROOM9/ydan_9_shape.o"
    include "zelda_scene/Ydan/ROOM9/ydan_grp_yda09_00.o"
    include "zelda_scene/Ydan/ROOM9/ydan_grp_yda09_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_10"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM10/ydan_10_info.o"
    include "zelda_scene/Ydan/ROOM10/ydan_10_shape.o"
    include "zelda_scene/Ydan/ROOM10/ydan_grp_yda0a_00.o"
    include "zelda_scene/Ydan/ROOM10/ydan_grp_yda0a_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_room_11"
    romalign 0x1000
    include "zelda_scene/Ydan/ROOM11/ydan_11_info.o"
    include "zelda_scene/Ydan/ROOM11/ydan_11_shape.o"
    include "zelda_scene/Ydan/ROOM11/ydan_grp_ydaff_00.o"
    include "zelda_scene/Ydan/ROOM11/ydan_grp_ydaff_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_scene"
    romalign 0x1000
    include "zelda_scene/Ddan/ddan_main_info.o"
    include "zelda_scene/Ddan/ddan_bgc.o"
    include "zelda_scene/Ddan/ddan_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ddan_room_0"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM0/ddan_0_info.o"
    include "zelda_scene/Ddan/ROOM0/ddan_0_shape.o"
    include "zelda_scene/Ddan/ROOM0/ddan_grp_dda00_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_1"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM1/ddan_1_info.o"
    include "zelda_scene/Ddan/ROOM1/ddan_1_shape.o"
    include "zelda_scene/Ddan/ROOM1/ddan_grp_dda01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_2"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM2/ddan_2_info.o"
    include "zelda_scene/Ddan/ROOM2/ddan_2_shape.o"
    include "zelda_scene/Ddan/ROOM2/ddan_grp_dda02_00.o"
    include "zelda_scene/Ddan/ROOM2/ddan_grp_dda02_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_3"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM3/ddan_3_info.o"
    include "zelda_scene/Ddan/ROOM3/ddan_3_shape.o"
    include "zelda_scene/Ddan/ROOM3/ddan_grp_dda03_00.o"
    include "zelda_scene/Ddan/ROOM3/ddan_grp_dda03_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_4"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM4/ddan_4_info.o"
    include "zelda_scene/Ddan/ROOM4/ddan_4_shape.o"
    include "zelda_scene/Ddan/ROOM4/ddan_grp_dda04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_5"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM5/ddan_5_info.o"
    include "zelda_scene/Ddan/ROOM5/ddan_5_shape.o"
    include "zelda_scene/Ddan/ROOM5/ddan_grp_dda05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_6"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM6/ddan_6_info.o"
    include "zelda_scene/Ddan/ROOM6/ddan_6_shape.o"
    include "zelda_scene/Ddan/ROOM6/ddan_grp_dda06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_7"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM7/ddan_7_info.o"
    include "zelda_scene/Ddan/ROOM7/ddan_7_shape.o"
    include "zelda_scene/Ddan/ROOM7/ddan_grp_dda07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_8"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM8/ddan_8_info.o"
    include "zelda_scene/Ddan/ROOM8/ddan_8_shape.o"
    include "zelda_scene/Ddan/ROOM8/ddan_grp_dda08_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_9"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM9/ddan_9_info.o"
    include "zelda_scene/Ddan/ROOM9/ddan_9_shape.o"
    include "zelda_scene/Ddan/ROOM9/ddan_grp_dda09_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_10"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM10/ddan_10_info.o"
    include "zelda_scene/Ddan/ROOM10/ddan_10_shape.o"
    include "zelda_scene/Ddan/ROOM10/ddan_grp_dda0a_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_11"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM11/ddan_11_info.o"
    include "zelda_scene/Ddan/ROOM11/ddan_11_shape.o"
    include "zelda_scene/Ddan/ROOM11/ddan_grp_dda0b_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_12"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM12/ddan_12_info.o"
    include "zelda_scene/Ddan/ROOM12/ddan_12_shape.o"
    include "zelda_scene/Ddan/ROOM12/ddan_grp_dda0c_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_13"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM13/ddan_13_info.o"
    include "zelda_scene/Ddan/ROOM13/ddan_13_shape.o"
    include "zelda_scene/Ddan/ROOM13/ddan_grp_dda0d_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_14"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM14/ddan_14_info.o"
    include "zelda_scene/Ddan/ROOM14/ddan_14_shape.o"
    include "zelda_scene/Ddan/ROOM14/ddan_grp_dda0e_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_15"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM15/ddan_15_info.o"
    include "zelda_scene/Ddan/ROOM15/ddan_15_shape.o"
    include "zelda_scene/Ddan/ROOM15/ddan_grp_dda0f_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_room_16"
    romalign 0x1000
    include "zelda_scene/Ddan/ROOM16/ddan_16_info.o"
    include "zelda_scene/Ddan/ROOM16/ddan_16_shape.o"
    include "zelda_scene/Ddan/ROOM16/ddan_grp_ddaff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_scene"
    romalign 0x1000
    include "zelda_scene/Bdan/bdan_main_info.o"
    include "zelda_tool_scene/Bdan/bdan_demo00.o"
    include "zelda_scene/Bdan/bdan_bgc.o"
    include "zelda_scene/Bdan/bdan_main_info_d0.o"
    include "zelda_scene/Bdan/bdan_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "bdan_room_0"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM0/bdan_0_info.o"
    include "zelda_scene/Bdan/ROOM0/bdan_0_info_d0.o"
    include "zelda_scene/Bdan/ROOM0/bdan_0_shape.o"
    include "zelda_scene/Bdan/ROOM0/bdan_grp_bda00_00.o"
    include "zelda_scene/Bdan/ROOM0/bdan_grp_bda00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_1"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM1/bdan_1_info.o"
    include "zelda_scene/Bdan/ROOM1/bdan_1_info_d0.o"
    include "zelda_scene/Bdan/ROOM1/bdan_1_shape.o"
    include "zelda_scene/Bdan/ROOM1/bdan_grp_bda01_00.o"
    include "zelda_scene/Bdan/ROOM1/bdan_grp_bda01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_2"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM2/bdan_2_info.o"
    include "zelda_scene/Bdan/ROOM2/bdan_2_info_d0.o"
    include "zelda_scene/Bdan/ROOM2/bdan_2_shape.o"
    include "zelda_scene/Bdan/ROOM2/bdan_grp_bda02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_3"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM3/bdan_3_info.o"
    include "zelda_scene/Bdan/ROOM3/bdan_3_info_d0.o"
    include "zelda_scene/Bdan/ROOM3/bdan_3_shape.o"
    include "zelda_scene/Bdan/ROOM3/bdan_grp_bda03_00.o"
    include "zelda_scene/Bdan/ROOM3/bdan_grp_bda03_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_4"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM4/bdan_4_info.o"
    include "zelda_scene/Bdan/ROOM4/bdan_4_info_d0.o"
    include "zelda_scene/Bdan/ROOM4/bdan_4_shape.o"
    include "zelda_scene/Bdan/ROOM4/bdan_grp_bda04_00.o"
    include "zelda_scene/Bdan/ROOM4/bdan_grp_bda04_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_5"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM5/bdan_5_info.o"
    include "zelda_scene/Bdan/ROOM5/bdan_5_info_d0.o"
    include "zelda_scene/Bdan/ROOM5/bdan_5_shape.o"
    include "zelda_scene/Bdan/ROOM5/bdan_grp_bda05_00.o"
    include "zelda_scene/Bdan/ROOM5/bdan_grp_bda05_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_6"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM6/bdan_6_info.o"
    include "zelda_scene/Bdan/ROOM6/bdan_6_info_d0.o"
    include "zelda_scene/Bdan/ROOM6/bdan_6_shape.o"
    include "zelda_scene/Bdan/ROOM6/bdan_grp_bda06_00.o"
    include "zelda_scene/Bdan/ROOM6/bdan_grp_bda06_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_7"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM7/bdan_7_info.o"
    include "zelda_scene/Bdan/ROOM7/bdan_7_info_d0.o"
    include "zelda_scene/Bdan/ROOM7/bdan_7_shape.o"
    include "zelda_scene/Bdan/ROOM7/bdan_grp_bda07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_8"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM8/bdan_8_info.o"
    include "zelda_scene/Bdan/ROOM8/bdan_8_info_d0.o"
    include "zelda_scene/Bdan/ROOM8/bdan_8_shape.o"
    include "zelda_scene/Bdan/ROOM8/bdan_grp_bda08_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_9"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM9/bdan_9_info.o"
    include "zelda_scene/Bdan/ROOM9/bdan_9_info_d0.o"
    include "zelda_scene/Bdan/ROOM9/bdan_9_shape.o"
    include "zelda_scene/Bdan/ROOM9/bdan_grp_bda09_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_10"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM10/bdan_10_info.o"
    include "zelda_scene/Bdan/ROOM10/bdan_10_info_d0.o"
    include "zelda_scene/Bdan/ROOM10/bdan_10_shape.o"
    include "zelda_scene/Bdan/ROOM10/bdan_grp_bda0a_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_11"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM11/bdan_11_info.o"
    include "zelda_scene/Bdan/ROOM11/bdan_11_info_d0.o"
    include "zelda_scene/Bdan/ROOM11/bdan_11_shape.o"
    include "zelda_scene/Bdan/ROOM11/bdan_grp_bda0b_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_12"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM12/bdan_12_info.o"
    include "zelda_scene/Bdan/ROOM12/bdan_12_info_d0.o"
    include "zelda_scene/Bdan/ROOM12/bdan_12_shape.o"
    include "zelda_scene/Bdan/ROOM12/bdan_grp_bda0c_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_13"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM13/bdan_13_info.o"
    include "zelda_scene/Bdan/ROOM13/bdan_13_info_d0.o"
    include "zelda_scene/Bdan/ROOM13/bdan_13_shape.o"
    include "zelda_scene/Bdan/ROOM13/bdan_grp_bda0d_00.o"
    include "zelda_scene/Bdan/ROOM13/bdan_grp_bda0d_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_14"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM14/bdan_14_info.o"
    include "zelda_scene/Bdan/ROOM14/bdan_14_info_d0.o"
    include "zelda_scene/Bdan/ROOM14/bdan_14_shape.o"
    include "zelda_scene/Bdan/ROOM14/bdan_grp_bda0e_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_room_15"
    romalign 0x1000
    include "zelda_scene/Bdan/ROOM15/bdan_15_info.o"
    include "zelda_scene/Bdan/ROOM15/bdan_15_info_d0.o"
    include "zelda_scene/Bdan/ROOM15/bdan_15_shape.o"
    include "zelda_scene/Bdan/ROOM15/bdan_grp_bdaff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_scene"
    romalign 0x1000
    include "zelda_scene/Bmori1/Bmori1_main_info.o"
    include "zelda_scene/Bmori1/Bmori1_bgc.o"
    include "zelda_scene/Bmori1/Bmori1_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "Bmori1_room_0"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM0/Bmori1_0_info.o"
    include "zelda_scene/Bmori1/ROOM0/Bmori1_0_shape.o"
    include "zelda_scene/Bmori1/ROOM0/Bmori1_grp_mor00_00.o"
    include "zelda_scene/Bmori1/ROOM0/Bmori1_grp_mor00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_1"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM1/Bmori1_1_info.o"
    include "zelda_scene/Bmori1/ROOM1/Bmori1_1_shape.o"
    include "zelda_scene/Bmori1/ROOM1/Bmori1_grp_mor01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_2"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM2/Bmori1_2_info.o"
    include "zelda_scene/Bmori1/ROOM2/Bmori1_2_shape.o"
    include "zelda_scene/Bmori1/ROOM2/Bmori1_grp_mor02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_3"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM3/Bmori1_3_info.o"
    include "zelda_scene/Bmori1/ROOM3/Bmori1_3_shape.o"
    include "zelda_scene/Bmori1/ROOM3/Bmori1_grp_mor03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_4"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM4/Bmori1_4_info.o"
    include "zelda_scene/Bmori1/ROOM4/Bmori1_4_shape.o"
    include "zelda_scene/Bmori1/ROOM4/Bmori1_grp_mor04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_5"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM5/Bmori1_5_info.o"
    include "zelda_scene/Bmori1/ROOM5/Bmori1_5_shape.o"
    include "zelda_scene/Bmori1/ROOM5/Bmori1_grp_mor05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_6"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM6/Bmori1_6_info.o"
    include "zelda_scene/Bmori1/ROOM6/Bmori1_6_shape.o"
    include "zelda_scene/Bmori1/ROOM6/Bmori1_grp_mor06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_7"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM7/Bmori1_7_info.o"
    include "zelda_scene/Bmori1/ROOM7/Bmori1_7_shape.o"
    include "zelda_scene/Bmori1/ROOM7/Bmori1_grp_mor07_00.o"
    include "zelda_scene/Bmori1/ROOM7/Bmori1_grp_mor07_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_8"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM8/Bmori1_8_info.o"
    include "zelda_scene/Bmori1/ROOM8/Bmori1_8_shape.o"
    include "zelda_scene/Bmori1/ROOM8/Bmori1_grp_mor08_00.o"
    include "zelda_scene/Bmori1/ROOM8/Bmori1_grp_mor08_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_9"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM9/Bmori1_9_info.o"
    include "zelda_scene/Bmori1/ROOM9/Bmori1_9_shape.o"
    include "zelda_scene/Bmori1/ROOM9/Bmori1_grp_mor09_00.o"
    include "zelda_scene/Bmori1/ROOM9/Bmori1_grp_mor09_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_10"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM10/Bmori1_10_info.o"
    include "zelda_scene/Bmori1/ROOM10/Bmori1_10_shape.o"
    include "zelda_scene/Bmori1/ROOM10/Bmori1_grp_mor0a_00.o"
    include "zelda_scene/Bmori1/ROOM10/Bmori1_grp_mor0a_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_11"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM11/Bmori1_11_info.o"
    include "zelda_scene/Bmori1/ROOM11/Bmori1_11_shape.o"
    include "zelda_scene/Bmori1/ROOM11/Bmori1_grp_mor0b_00.o"
    include "zelda_scene/Bmori1/ROOM11/Bmori1_grp_mor0b_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_12"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM12/Bmori1_12_info.o"
    include "zelda_scene/Bmori1/ROOM12/Bmori1_12_shape.o"
    include "zelda_scene/Bmori1/ROOM12/Bmori1_grp_mor0c_00.o"
    include "zelda_scene/Bmori1/ROOM12/Bmori1_grp_mor0c_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_13"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM13/Bmori1_13_info.o"
    include "zelda_scene/Bmori1/ROOM13/Bmori1_13_shape.o"
    include "zelda_scene/Bmori1/ROOM13/Bmori1_grp_mor0d_00.o"
    include "zelda_scene/Bmori1/ROOM13/Bmori1_grp_mor0d_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_14"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM14/Bmori1_14_info.o"
    include "zelda_scene/Bmori1/ROOM14/Bmori1_14_shape.o"
    include "zelda_scene/Bmori1/ROOM14/Bmori1_grp_mor0e_00.o"
    include "zelda_scene/Bmori1/ROOM14/Bmori1_grp_mor0e_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_15"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM15/Bmori1_15_info.o"
    include "zelda_scene/Bmori1/ROOM15/Bmori1_15_shape.o"
    include "zelda_scene/Bmori1/ROOM15/Bmori1_grp_mor0f_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_16"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM16/Bmori1_16_info.o"
    include "zelda_scene/Bmori1/ROOM16/Bmori1_16_shape.o"
    include "zelda_scene/Bmori1/ROOM16/Bmori1_grp_mor10_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_17"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM17/Bmori1_17_info.o"
    include "zelda_scene/Bmori1/ROOM17/Bmori1_17_shape.o"
    include "zelda_scene/Bmori1/ROOM17/Bmori1_grp_mor11_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_18"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM18/Bmori1_18_info.o"
    include "zelda_scene/Bmori1/ROOM18/Bmori1_18_shape.o"
    include "zelda_scene/Bmori1/ROOM18/Bmori1_grp_mor12_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_19"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM19/Bmori1_19_info.o"
    include "zelda_scene/Bmori1/ROOM19/Bmori1_19_shape.o"
    include "zelda_scene/Bmori1/ROOM19/Bmori1_grp_mor13_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_20"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM20/Bmori1_20_info.o"
    include "zelda_scene/Bmori1/ROOM20/Bmori1_20_shape.o"
    include "zelda_scene/Bmori1/ROOM20/Bmori1_grp_mor14_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_21"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM21/Bmori1_21_info.o"
    include "zelda_scene/Bmori1/ROOM21/Bmori1_21_shape.o"
    include "zelda_scene/Bmori1/ROOM21/Bmori1_grp_mor15_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "Bmori1_room_22"
    romalign 0x1000
    include "zelda_scene/Bmori1/ROOM22/Bmori1_22_info.o"
    include "zelda_scene/Bmori1/ROOM22/Bmori1_22_shape.o"
    include "zelda_scene/Bmori1/ROOM22/Bmori1_grp_morff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_scene"
    romalign 0x1000
    include "zelda_scene/Hidan/HIDAN_main_info.o"
    include "zelda_scene/Hidan/HIDAN_bgc.o"
    include "zelda_scene/Hidan/HIDAN_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "HIDAN_room_0"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM0/HIDAN_0_info.o"
    include "zelda_scene/Hidan/ROOM0/HIDAN_0_shape.o"
    include "zelda_scene/Hidan/ROOM0/HIDAN_grp_Hid00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_1"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM1/HIDAN_1_info.o"
    include "zelda_scene/Hidan/ROOM1/HIDAN_1_shape.o"
    include "zelda_scene/Hidan/ROOM1/HIDAN_grp_Hid01.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_2"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM2/HIDAN_2_info.o"
    include "zelda_scene/Hidan/ROOM2/HIDAN_2_shape.o"
    include "zelda_scene/Hidan/ROOM2/HIDAN_grp_Hid02.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_3"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM3/HIDAN_3_info.o"
    include "zelda_scene/Hidan/ROOM3/HIDAN_3_shape.o"
    include "zelda_scene/Hidan/ROOM3/HIDAN_grp_Hid03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_4"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM4/HIDAN_4_info.o"
    include "zelda_scene/Hidan/ROOM4/HIDAN_4_shape.o"
    include "zelda_scene/Hidan/ROOM4/HIDAN_grp_Hid04.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_5"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM5/HIDAN_5_info.o"
    include "zelda_scene/Hidan/ROOM5/HIDAN_5_shape.o"
    include "zelda_scene/Hidan/ROOM5/HIDAN_grp_Hid05.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_6"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM6/HIDAN_6_info.o"
    include "zelda_scene/Hidan/ROOM6/HIDAN_6_shape.o"
    include "zelda_scene/Hidan/ROOM6/HIDAN_grp_Hid06.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_7"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM7/HIDAN_7_info.o"
    include "zelda_scene/Hidan/ROOM7/HIDAN_7_shape.o"
    include "zelda_scene/Hidan/ROOM7/HIDAN_grp_Hid07.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_8"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM8/HIDAN_8_info.o"
    include "zelda_scene/Hidan/ROOM8/HIDAN_8_shape.o"
    include "zelda_scene/Hidan/ROOM8/HIDAN_grp_Hid08.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_9"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM9/HIDAN_9_info.o"
    include "zelda_scene/Hidan/ROOM9/HIDAN_9_shape.o"
    include "zelda_scene/Hidan/ROOM9/HIDAN_grp_Hid09.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_10"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM10/HIDAN_10_info.o"
    include "zelda_scene/Hidan/ROOM10/HIDAN_10_shape.o"
    include "zelda_scene/Hidan/ROOM10/HIDAN_grp_Hid0a.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_11"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM11/HIDAN_11_info.o"
    include "zelda_scene/Hidan/ROOM11/HIDAN_11_shape.o"
    include "zelda_scene/Hidan/ROOM11/HIDAN_grp_Hid0b.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_12"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM12/HIDAN_12_info.o"
    include "zelda_scene/Hidan/ROOM12/HIDAN_12_shape.o"
    include "zelda_scene/Hidan/ROOM12/HIDAN_grp_Hid0c.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_13"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM13/HIDAN_13_info.o"
    include "zelda_scene/Hidan/ROOM13/HIDAN_13_shape.o"
    include "zelda_scene/Hidan/ROOM13/HIDAN_grp_Hid0d.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_14"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM14/HIDAN_14_info.o"
    include "zelda_scene/Hidan/ROOM14/HIDAN_14_shape.o"
    include "zelda_scene/Hidan/ROOM14/HIDAN_grp_Hid0e.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_15"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM15/HIDAN_15_info.o"
    include "zelda_scene/Hidan/ROOM15/HIDAN_15_shape.o"
    include "zelda_scene/Hidan/ROOM15/HIDAN_grp_Hid0f.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_16"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM16/HIDAN_16_info.o"
    include "zelda_scene/Hidan/ROOM16/HIDAN_16_shape.o"
    include "zelda_scene/Hidan/ROOM16/HIDAN_grp_Hid10.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_17"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM17/HIDAN_17_info.o"
    include "zelda_scene/Hidan/ROOM17/HIDAN_17_shape.o"
    include "zelda_scene/Hidan/ROOM17/HIDAN_grp_Hid11.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_18"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM18/HIDAN_18_info.o"
    include "zelda_scene/Hidan/ROOM18/HIDAN_18_shape.o"
    include "zelda_scene/Hidan/ROOM18/HIDAN_grp_Hid12.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_19"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM19/HIDAN_19_info.o"
    include "zelda_scene/Hidan/ROOM19/HIDAN_19_shape.o"
    include "zelda_scene/Hidan/ROOM19/HIDAN_grp_Hid13.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_20"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM20/HIDAN_20_info.o"
    include "zelda_scene/Hidan/ROOM20/HIDAN_20_shape.o"
    include "zelda_scene/Hidan/ROOM20/HIDAN_grp_Hid14.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_21"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM21/HIDAN_21_info.o"
    include "zelda_scene/Hidan/ROOM21/HIDAN_21_shape.o"
    include "zelda_scene/Hidan/ROOM21/HIDAN_grp_Hid15.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_22"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM22/HIDAN_22_info.o"
    include "zelda_scene/Hidan/ROOM22/HIDAN_22_shape.o"
    include "zelda_scene/Hidan/ROOM22/HIDAN_grp_Hid16.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_23"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM23/HIDAN_23_info.o"
    include "zelda_scene/Hidan/ROOM23/HIDAN_23_shape.o"
    include "zelda_scene/Hidan/ROOM23/HIDAN_grp_Hid17.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_24"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM24/HIDAN_24_info.o"
    include "zelda_scene/Hidan/ROOM24/HIDAN_24_shape.o"
    include "zelda_scene/Hidan/ROOM24/HIDAN_grp_Hid18.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_25"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM25/HIDAN_25_info.o"
    include "zelda_scene/Hidan/ROOM25/HIDAN_25_shape.o"
    include "zelda_scene/Hidan/ROOM25/HIDAN_grp_Hid19.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HIDAN_room_26"
    romalign 0x1000
    include "zelda_scene/Hidan/ROOM26/HIDAN_26_info.o"
    include "zelda_scene/Hidan/ROOM26/HIDAN_26_shape.o"
    include "zelda_scene/Hidan/ROOM26/HIDAN_grp_Hidff.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_scene"
    romalign 0x1000
    include "zelda_scene/Mizusin/MIZUsin_main_info.o"
    include "zelda_scene/Mizusin/MIZUsin_bgc.o"
    include "zelda_scene/Mizusin/MIZUsin_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "MIZUsin_room_0"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM0/MIZUsin_0_info.o"
    include "zelda_scene/Mizusin/ROOM0/MIZUsin_0_shape.o"
    include "zelda_scene/Mizusin/ROOM0/MIZUsin_grp_Wat00.o"
    include "zelda_scene/Mizusin/ROOM0/MIZUsin_grp_Wat00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_1"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM1/MIZUsin_1_info.o"
    include "zelda_scene/Mizusin/ROOM1/MIZUsin_1_shape.o"
    include "zelda_scene/Mizusin/ROOM1/MIZUsin_grp_Wat01.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_2"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM2/MIZUsin_2_info.o"
    include "zelda_scene/Mizusin/ROOM2/MIZUsin_2_shape.o"
    include "zelda_scene/Mizusin/ROOM2/MIZUsin_grp_Wat02.o"
    include "zelda_scene/Mizusin/ROOM2/MIZUsin_grp_Wat02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_3"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM3/MIZUsin_3_info.o"
    include "zelda_scene/Mizusin/ROOM3/MIZUsin_3_shape.o"
    include "zelda_scene/Mizusin/ROOM3/MIZUsin_grp_Wat03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_4"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM4/MIZUsin_4_info.o"
    include "zelda_scene/Mizusin/ROOM4/MIZUsin_4_shape.o"
    include "zelda_scene/Mizusin/ROOM4/MIZUsin_grp_Wat04.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_5"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM5/MIZUsin_5_info.o"
    include "zelda_scene/Mizusin/ROOM5/MIZUsin_5_shape.o"
    include "zelda_scene/Mizusin/ROOM5/MIZUsin_grp_Wat05.o"
    include "zelda_scene/Mizusin/ROOM5/MIZUsin_grp_Wat05T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_6"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM6/MIZUsin_6_info.o"
    include "zelda_scene/Mizusin/ROOM6/MIZUsin_6_shape.o"
    include "zelda_scene/Mizusin/ROOM6/MIZUsin_grp_Wat06.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_7"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM7/MIZUsin_7_info.o"
    include "zelda_scene/Mizusin/ROOM7/MIZUsin_7_shape.o"
    include "zelda_scene/Mizusin/ROOM7/MIZUsin_grp_Wat07.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_8"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM8/MIZUsin_8_info.o"
    include "zelda_scene/Mizusin/ROOM8/MIZUsin_8_shape.o"
    include "zelda_scene/Mizusin/ROOM8/MIZUsin_grp_Wat08.o"
    include "zelda_scene/Mizusin/ROOM8/MIZUsin_grp_Wat08T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_9"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM9/MIZUsin_9_info.o"
    include "zelda_scene/Mizusin/ROOM9/MIZUsin_9_shape.o"
    include "zelda_scene/Mizusin/ROOM9/MIZUsin_grp_Wat09.o"
    include "zelda_scene/Mizusin/ROOM9/MIZUsin_grp_Wat09T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_10"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM10/MIZUsin_10_info.o"
    include "zelda_scene/Mizusin/ROOM10/MIZUsin_10_shape.o"
    include "zelda_scene/Mizusin/ROOM10/MIZUsin_grp_Wat0a.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_11"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM11/MIZUsin_11_info.o"
    include "zelda_scene/Mizusin/ROOM11/MIZUsin_11_shape.o"
    include "zelda_scene/Mizusin/ROOM11/MIZUsin_grp_Wat0b.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_12"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM12/MIZUsin_12_info.o"
    include "zelda_scene/Mizusin/ROOM12/MIZUsin_12_shape.o"
    include "zelda_scene/Mizusin/ROOM12/MIZUsin_grp_Wat0c.o"
    include "zelda_scene/Mizusin/ROOM12/MIZUsin_grp_Wat0cT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_13"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM13/MIZUsin_13_info.o"
    include "zelda_scene/Mizusin/ROOM13/MIZUsin_13_shape.o"
    include "zelda_scene/Mizusin/ROOM13/MIZUsin_grp_Wat0dT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_14"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM14/MIZUsin_14_info.o"
    include "zelda_scene/Mizusin/ROOM14/MIZUsin_14_shape.o"
    include "zelda_scene/Mizusin/ROOM14/MIZUsin_grp_Wat0e.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_15"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM15/MIZUsin_15_info.o"
    include "zelda_scene/Mizusin/ROOM15/MIZUsin_15_shape.o"
    include "zelda_scene/Mizusin/ROOM15/MIZUsin_grp_Wat0f.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_16"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM16/MIZUsin_16_info.o"
    include "zelda_scene/Mizusin/ROOM16/MIZUsin_16_shape.o"
    include "zelda_scene/Mizusin/ROOM16/MIZUsin_grp_Wat10.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_17"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM17/MIZUsin_17_info.o"
    include "zelda_scene/Mizusin/ROOM17/MIZUsin_17_shape.o"
    include "zelda_scene/Mizusin/ROOM17/MIZUsin_grp_Wat11.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_18"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM18/MIZUsin_18_info.o"
    include "zelda_scene/Mizusin/ROOM18/MIZUsin_18_shape.o"
    include "zelda_scene/Mizusin/ROOM18/MIZUsin_grp_Wat12.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_19"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM19/MIZUsin_19_info.o"
    include "zelda_scene/Mizusin/ROOM19/MIZUsin_19_shape.o"
    include "zelda_scene/Mizusin/ROOM19/MIZUsin_grp_Wat13.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_20"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM20/MIZUsin_20_info.o"
    include "zelda_scene/Mizusin/ROOM20/MIZUsin_20_shape.o"
    include "zelda_scene/Mizusin/ROOM20/MIZUsin_grp_Wat14.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_21"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM21/MIZUsin_21_info.o"
    include "zelda_scene/Mizusin/ROOM21/MIZUsin_21_shape.o"
    include "zelda_scene/Mizusin/ROOM21/MIZUsin_grp_Wat15.o"
    include "zelda_scene/Mizusin/ROOM21/MIZUsin_grp_Wat15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_room_22"
    romalign 0x1000
    include "zelda_scene/Mizusin/ROOM22/MIZUsin_22_info.o"
    include "zelda_scene/Mizusin/ROOM22/MIZUsin_22_shape.o"
    include "zelda_scene/Mizusin/ROOM22/MIZUsin_grp_Watff.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_scene"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/jyasinzou_main_info.o"
    include "zelda_tool_scene/Jyasinzou/jyasinzou_demo00.o"
    include "zelda_tool_scene/Jyasinzou/jyasinzou_demo01.o"
    include "zelda_scene/Jyasinzou/jyasinzou_bgc.o"
    include "zelda_scene/Jyasinzou/jyasinzou_main_info_d0.o"
    include "zelda_scene/Jyasinzou/jyasinzou_main_info_d1.o"
    include "zelda_scene/Jyasinzou/jyasinzou_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "jyasinzou_room_0"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_0_info.o"
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_0_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_0_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_0_shape.o"
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_grp_jya00_00.o"
    include "zelda_scene/Jyasinzou/ROOM0/jyasinzou_grp_jya00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_1"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM1/jyasinzou_1_info.o"
    include "zelda_scene/Jyasinzou/ROOM1/jyasinzou_1_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM1/jyasinzou_1_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM1/jyasinzou_1_shape.o"
    include "zelda_scene/Jyasinzou/ROOM1/jyasinzou_grp_jya01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_2"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM2/jyasinzou_2_info.o"
    include "zelda_scene/Jyasinzou/ROOM2/jyasinzou_2_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM2/jyasinzou_2_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM2/jyasinzou_2_shape.o"
    include "zelda_scene/Jyasinzou/ROOM2/jyasinzou_grp_jya02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_3"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM3/jyasinzou_3_info.o"
    include "zelda_scene/Jyasinzou/ROOM3/jyasinzou_3_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM3/jyasinzou_3_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM3/jyasinzou_3_shape.o"
    include "zelda_scene/Jyasinzou/ROOM3/jyasinzou_grp_jya03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_4"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_4_info.o"
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_4_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_4_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_4_shape.o"
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_grp_jya04_00.o"
    include "zelda_scene/Jyasinzou/ROOM4/jyasinzou_grp_jya04_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_5"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_5_info.o"
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_5_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_5_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_5_shape.o"
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_grp_jya05_00.o"
    include "zelda_scene/Jyasinzou/ROOM5/jyasinzou_grp_jya05_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_6"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM6/jyasinzou_6_info.o"
    include "zelda_scene/Jyasinzou/ROOM6/jyasinzou_6_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM6/jyasinzou_6_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM6/jyasinzou_6_shape.o"
    include "zelda_scene/Jyasinzou/ROOM6/jyasinzou_grp_jya06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_7"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM7/jyasinzou_7_info.o"
    include "zelda_scene/Jyasinzou/ROOM7/jyasinzou_7_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM7/jyasinzou_7_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM7/jyasinzou_7_shape.o"
    include "zelda_scene/Jyasinzou/ROOM7/jyasinzou_grp_jya07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_8"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_8_info.o"
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_8_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_8_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_8_shape.o"
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_grp_jya08_00.o"
    include "zelda_scene/Jyasinzou/ROOM8/jyasinzou_grp_jya08_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_9"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM9/jyasinzou_9_info.o"
    include "zelda_scene/Jyasinzou/ROOM9/jyasinzou_9_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM9/jyasinzou_9_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM9/jyasinzou_9_shape.o"
    include "zelda_scene/Jyasinzou/ROOM9/jyasinzou_grp_jya09_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_10"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM10/jyasinzou_10_info.o"
    include "zelda_scene/Jyasinzou/ROOM10/jyasinzou_10_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM10/jyasinzou_10_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM10/jyasinzou_10_shape.o"
    include "zelda_scene/Jyasinzou/ROOM10/jyasinzou_grp_jya0a_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_11"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_11_info.o"
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_11_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_11_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_11_shape.o"
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_grp_jya0b_00.o"
    include "zelda_scene/Jyasinzou/ROOM11/jyasinzou_grp_jya0b_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_12"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM12/jyasinzou_12_info.o"
    include "zelda_scene/Jyasinzou/ROOM12/jyasinzou_12_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM12/jyasinzou_12_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM12/jyasinzou_12_shape.o"
    include "zelda_scene/Jyasinzou/ROOM12/jyasinzou_grp_jya0c_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_13"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM13/jyasinzou_13_info.o"
    include "zelda_scene/Jyasinzou/ROOM13/jyasinzou_13_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM13/jyasinzou_13_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM13/jyasinzou_13_shape.o"
    include "zelda_scene/Jyasinzou/ROOM13/jyasinzou_grp_jya0d_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_14"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM14/jyasinzou_14_info.o"
    include "zelda_scene/Jyasinzou/ROOM14/jyasinzou_14_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM14/jyasinzou_14_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM14/jyasinzou_14_shape.o"
    include "zelda_scene/Jyasinzou/ROOM14/jyasinzou_grp_jya0e_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_15"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_15_info.o"
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_15_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_15_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_15_shape.o"
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_grp_jya0f_00.o"
    include "zelda_scene/Jyasinzou/ROOM15/jyasinzou_grp_jya0f_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_16"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM16/jyasinzou_16_info.o"
    include "zelda_scene/Jyasinzou/ROOM16/jyasinzou_16_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM16/jyasinzou_16_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM16/jyasinzou_16_shape.o"
    include "zelda_scene/Jyasinzou/ROOM16/jyasinzou_grp_jya10_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_17"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM17/jyasinzou_17_info.o"
    include "zelda_scene/Jyasinzou/ROOM17/jyasinzou_17_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM17/jyasinzou_17_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM17/jyasinzou_17_shape.o"
    include "zelda_scene/Jyasinzou/ROOM17/jyasinzou_grp_jya11_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_18"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_18_info.o"
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_18_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_18_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_18_shape.o"
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_grp_jya12_00.o"
    include "zelda_scene/Jyasinzou/ROOM18/jyasinzou_grp_jya12_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_19"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM19/jyasinzou_19_info.o"
    include "zelda_scene/Jyasinzou/ROOM19/jyasinzou_19_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM19/jyasinzou_19_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM19/jyasinzou_19_shape.o"
    include "zelda_scene/Jyasinzou/ROOM19/jyasinzou_grp_jya13_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_20"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM20/jyasinzou_20_info.o"
    include "zelda_scene/Jyasinzou/ROOM20/jyasinzou_20_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM20/jyasinzou_20_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM20/jyasinzou_20_shape.o"
    include "zelda_scene/Jyasinzou/ROOM20/jyasinzou_grp_jya14_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_21"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM21/jyasinzou_21_info.o"
    include "zelda_scene/Jyasinzou/ROOM21/jyasinzou_21_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM21/jyasinzou_21_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM21/jyasinzou_21_shape.o"
    include "zelda_scene/Jyasinzou/ROOM21/jyasinzou_grp_jya15_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_22"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM22/jyasinzou_22_info.o"
    include "zelda_scene/Jyasinzou/ROOM22/jyasinzou_22_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM22/jyasinzou_22_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM22/jyasinzou_22_shape.o"
    include "zelda_scene/Jyasinzou/ROOM22/jyasinzou_grp_jya16_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_23"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_23_info.o"
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_23_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_23_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_23_shape.o"
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_grp_jya17_00.o"
    include "zelda_scene/Jyasinzou/ROOM23/jyasinzou_grp_jya17_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_24"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_24_info.o"
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_24_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_24_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_24_shape.o"
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_grp_jya18_00.o"
    include "zelda_scene/Jyasinzou/ROOM24/jyasinzou_grp_jya18_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_25"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM25/jyasinzou_25_info.o"
    include "zelda_scene/Jyasinzou/ROOM25/jyasinzou_25_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM25/jyasinzou_25_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM25/jyasinzou_25_shape.o"
    include "zelda_scene/Jyasinzou/ROOM25/jyasinzou_grp_jya19_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_26"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_26_info.o"
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_26_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_26_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_26_shape.o"
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_grp_jya1a_00.o"
    include "zelda_scene/Jyasinzou/ROOM26/jyasinzou_grp_jya1a_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_27"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM27/jyasinzou_27_info.o"
    include "zelda_scene/Jyasinzou/ROOM27/jyasinzou_27_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM27/jyasinzou_27_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM27/jyasinzou_27_shape.o"
    include "zelda_scene/Jyasinzou/ROOM27/jyasinzou_grp_jya1b_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinzou_room_28"
    romalign 0x1000
    include "zelda_scene/Jyasinzou/ROOM28/jyasinzou_28_info.o"
    include "zelda_scene/Jyasinzou/ROOM28/jyasinzou_28_info_d0.o"
    include "zelda_scene/Jyasinzou/ROOM28/jyasinzou_28_info_d1.o"
    include "zelda_scene/Jyasinzou/ROOM28/jyasinzou_28_shape.o"
    include "zelda_scene/Jyasinzou/ROOM28/jyasinzou_grp_jya1c_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_scene"
    romalign 0x1000
    include "zelda_scene/Hakadan/HAKAdan_main_info.o"
    include "zelda_scene/Hakadan/HAKAdan_bgc.o"
    include "zelda_scene/Hakadan/HAKAdan_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "HAKAdan_room_0"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM0/HAKAdan_0_info.o"
    include "zelda_scene/Hakadan/ROOM0/HAKAdan_0_shape.o"
    include "zelda_scene/Hakadan/ROOM0/HAKAdan_grp_Haka00.o"
    include "zelda_scene/Hakadan/ROOM0/HAKAdan_grp_Haka00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_1"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM1/HAKAdan_1_info.o"
    include "zelda_scene/Hakadan/ROOM1/HAKAdan_1_shape.o"
    include "zelda_scene/Hakadan/ROOM1/HAKAdan_grp_Haka01.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_2"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM2/HAKAdan_2_info.o"
    include "zelda_scene/Hakadan/ROOM2/HAKAdan_2_shape.o"
    include "zelda_scene/Hakadan/ROOM2/HAKAdan_grp_Haka02.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_3"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM3/HAKAdan_3_info.o"
    include "zelda_scene/Hakadan/ROOM3/HAKAdan_3_shape.o"
    include "zelda_scene/Hakadan/ROOM3/HAKAdan_grp_Haka03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_4"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM4/HAKAdan_4_info.o"
    include "zelda_scene/Hakadan/ROOM4/HAKAdan_4_shape.o"
    include "zelda_scene/Hakadan/ROOM4/HAKAdan_grp_Haka04.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_5"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM5/HAKAdan_5_info.o"
    include "zelda_scene/Hakadan/ROOM5/HAKAdan_5_shape.o"
    include "zelda_scene/Hakadan/ROOM5/HAKAdan_grp_Haka05.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_6"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM6/HAKAdan_6_info.o"
    include "zelda_scene/Hakadan/ROOM6/HAKAdan_6_shape.o"
    include "zelda_scene/Hakadan/ROOM6/HAKAdan_grp_Haka06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_7"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM7/HAKAdan_7_info.o"
    include "zelda_scene/Hakadan/ROOM7/HAKAdan_7_shape.o"
    include "zelda_scene/Hakadan/ROOM7/HAKAdan_grp_Haka07.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_8"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM8/HAKAdan_8_info.o"
    include "zelda_scene/Hakadan/ROOM8/HAKAdan_8_shape.o"
    include "zelda_scene/Hakadan/ROOM8/HAKAdan_grp_Haka08.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_9"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM9/HAKAdan_9_info.o"
    include "zelda_scene/Hakadan/ROOM9/HAKAdan_9_shape.o"
    include "zelda_scene/Hakadan/ROOM9/HAKAdan_grp_Haka09.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_10"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM10/HAKAdan_10_info.o"
    include "zelda_scene/Hakadan/ROOM10/HAKAdan_10_shape.o"
    include "zelda_scene/Hakadan/ROOM10/HAKAdan_grp_Haka0a.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_11"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM11/HAKAdan_11_info.o"
    include "zelda_scene/Hakadan/ROOM11/HAKAdan_11_shape.o"
    include "zelda_scene/Hakadan/ROOM11/HAKAdan_grp_Haka0b.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_12"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM12/HAKAdan_12_info.o"
    include "zelda_scene/Hakadan/ROOM12/HAKAdan_12_shape.o"
    include "zelda_scene/Hakadan/ROOM12/HAKAdan_grp_Haka0c.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_13"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM13/HAKAdan_13_info.o"
    include "zelda_scene/Hakadan/ROOM13/HAKAdan_13_shape.o"
    include "zelda_scene/Hakadan/ROOM13/HAKAdan_grp_Haka0d.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_14"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM14/HAKAdan_14_info.o"
    include "zelda_scene/Hakadan/ROOM14/HAKAdan_14_shape.o"
    include "zelda_scene/Hakadan/ROOM14/HAKAdan_grp_Haka0e.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_15"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM15/HAKAdan_15_info.o"
    include "zelda_scene/Hakadan/ROOM15/HAKAdan_15_shape.o"
    include "zelda_scene/Hakadan/ROOM15/HAKAdan_grp_Haka0f.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_16"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM16/HAKAdan_16_info.o"
    include "zelda_scene/Hakadan/ROOM16/HAKAdan_16_shape.o"
    include "zelda_scene/Hakadan/ROOM16/HAKAdan_grp_Haka10.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_17"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM17/HAKAdan_17_info.o"
    include "zelda_scene/Hakadan/ROOM17/HAKAdan_17_shape.o"
    include "zelda_scene/Hakadan/ROOM17/HAKAdan_grp_Haka11.o"
    include "zelda_scene/Hakadan/ROOM17/HAKAdan_grp_Haka11T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_18"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM18/HAKAdan_18_info.o"
    include "zelda_scene/Hakadan/ROOM18/HAKAdan_18_shape.o"
    include "zelda_scene/Hakadan/ROOM18/HAKAdan_grp_Haka12.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_19"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM19/HAKAdan_19_info.o"
    include "zelda_scene/Hakadan/ROOM19/HAKAdan_19_shape.o"
    include "zelda_scene/Hakadan/ROOM19/HAKAdan_grp_Haka13.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_20"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM20/HAKAdan_20_info.o"
    include "zelda_scene/Hakadan/ROOM20/HAKAdan_20_shape.o"
    include "zelda_scene/Hakadan/ROOM20/HAKAdan_grp_Haka14.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_21"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM21/HAKAdan_21_info.o"
    include "zelda_scene/Hakadan/ROOM21/HAKAdan_21_shape.o"
    include "zelda_scene/Hakadan/ROOM21/HAKAdan_grp_Haka15.o"
    include "zelda_scene/Hakadan/ROOM21/HAKAdan_grp_Haka15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_room_22"
    romalign 0x1000
    include "zelda_scene/Hakadan/ROOM22/HAKAdan_22_info.o"
    include "zelda_scene/Hakadan/ROOM22/HAKAdan_22_shape.o"
    include "zelda_scene/Hakadan/ROOM22/HAKAdan_grp_Hakaff.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_scene"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/HAKAdanCH_main_info.o"
    include "zelda_scene/Hakadan_ch/HAKAdanCH_bgc.o"
    include "zelda_scene/Hakadan_ch/HAKAdanCH_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "HAKAdanCH_room_0"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM0/HAKAdanCH_0_info.o"
    include "zelda_scene/Hakadan_ch/ROOM0/HAKAdanCH_0_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM0/HAKAdanCH_grp_Hch00.o"
    include "zelda_scene/Hakadan_ch/ROOM0/HAKAdanCH_grp_Hch00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_1"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM1/HAKAdanCH_1_info.o"
    include "zelda_scene/Hakadan_ch/ROOM1/HAKAdanCH_1_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM1/HAKAdanCH_grp_Hch01.o"
    include "zelda_scene/Hakadan_ch/ROOM1/HAKAdanCH_grp_Hch01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_2"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM2/HAKAdanCH_2_info.o"
    include "zelda_scene/Hakadan_ch/ROOM2/HAKAdanCH_2_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM2/HAKAdanCH_grp_Hch02.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_3"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM3/HAKAdanCH_3_info.o"
    include "zelda_scene/Hakadan_ch/ROOM3/HAKAdanCH_3_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM3/HAKAdanCH_grp_Hch03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_4"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM4/HAKAdanCH_4_info.o"
    include "zelda_scene/Hakadan_ch/ROOM4/HAKAdanCH_4_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM4/HAKAdanCH_grp_Hch04.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_5"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM5/HAKAdanCH_5_info.o"
    include "zelda_scene/Hakadan_ch/ROOM5/HAKAdanCH_5_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM5/HAKAdanCH_grp_Hch05.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdanCH_room_6"
    romalign 0x1000
    include "zelda_scene/Hakadan_ch/ROOM6/HAKAdanCH_6_info.o"
    include "zelda_scene/Hakadan_ch/ROOM6/HAKAdanCH_6_shape.o"
    include "zelda_scene/Hakadan_ch/ROOM6/HAKAdanCH_grp_Hch06.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_scene"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ice_doukutu_main_info.o"
    include "zelda_tool_scene/Ice_doukutu/ice_doukuto_demo00.o"
    include "zelda_scene/Ice_doukutu/ice_doukutu_bgc.o"
    include "zelda_scene/Ice_doukutu/ice_doukutu_main_info_d0.o"
    include "zelda_scene/Ice_doukutu/ice_doukutu_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ice_doukutu_room_0"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM0/ice_doukutu_0_info.o"
    include "zelda_scene/Ice_doukutu/ROOM0/ice_doukutu_0_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM0/ice_doukutu_0_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM0/ice_doukutu_grp_ice01.o"
    include "zelda_scene/Ice_doukutu/ROOM0/ice_doukutu_grp_ice01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_1"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM1/ice_doukutu_1_info.o"
    include "zelda_scene/Ice_doukutu/ROOM1/ice_doukutu_1_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM1/ice_doukutu_1_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM1/ice_doukutu_grp_ice02.o"
    include "zelda_scene/Ice_doukutu/ROOM1/ice_doukutu_grp_ice02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_2"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM2/ice_doukutu_2_info.o"
    include "zelda_scene/Ice_doukutu/ROOM2/ice_doukutu_2_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM2/ice_doukutu_2_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM2/ice_doukutu_grp_ice03.o"
    include "zelda_scene/Ice_doukutu/ROOM2/ice_doukutu_grp_ice03T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_3"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM3/ice_doukutu_3_info.o"
    include "zelda_scene/Ice_doukutu/ROOM3/ice_doukutu_3_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM3/ice_doukutu_3_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM3/ice_doukutu_grp_ice04.o"
    include "zelda_scene/Ice_doukutu/ROOM3/ice_doukutu_grp_ice04T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_4"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM4/ice_doukutu_4_info.o"
    include "zelda_scene/Ice_doukutu/ROOM4/ice_doukutu_4_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM4/ice_doukutu_4_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM4/ice_doukutu_grp_ice05.o"
    include "zelda_scene/Ice_doukutu/ROOM4/ice_doukutu_grp_ice05T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_5"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM5/ice_doukutu_5_info.o"
    include "zelda_scene/Ice_doukutu/ROOM5/ice_doukutu_5_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM5/ice_doukutu_5_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM5/ice_doukutu_grp_ice06.o"
    include "zelda_scene/Ice_doukutu/ROOM5/ice_doukutu_grp_ice06T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_6"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM6/ice_doukutu_6_info.o"
    include "zelda_scene/Ice_doukutu/ROOM6/ice_doukutu_6_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM6/ice_doukutu_6_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM6/ice_doukutu_grp_ice07.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_7"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM7/ice_doukutu_7_info.o"
    include "zelda_scene/Ice_doukutu/ROOM7/ice_doukutu_7_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM7/ice_doukutu_7_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM7/ice_doukutu_grp_ice08.o"
    include "zelda_scene/Ice_doukutu/ROOM7/ice_doukutu_grp_ice08T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_8"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM8/ice_doukutu_8_info.o"
    include "zelda_scene/Ice_doukutu/ROOM8/ice_doukutu_8_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM8/ice_doukutu_8_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM8/ice_doukutu_grp_ice09.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_9"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM9/ice_doukutu_9_info.o"
    include "zelda_scene/Ice_doukutu/ROOM9/ice_doukutu_9_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM9/ice_doukutu_9_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM9/ice_doukutu_grp_ice10.o"
    include "zelda_scene/Ice_doukutu/ROOM9/ice_doukutu_grp_ice10T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_10"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM10/ice_doukutu_10_info.o"
    include "zelda_scene/Ice_doukutu/ROOM10/ice_doukutu_10_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM10/ice_doukutu_10_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM10/ice_doukutu_grp_ice11.o"
    include "zelda_scene/Ice_doukutu/ROOM10/ice_doukutu_grp_ice11T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ice_doukutu_room_11"
    romalign 0x1000
    include "zelda_scene/Ice_doukutu/ROOM11/ice_doukutu_11_info.o"
    include "zelda_scene/Ice_doukutu/ROOM11/ice_doukutu_11_info_d0.o"
    include "zelda_scene/Ice_doukutu/ROOM11/ice_doukutu_11_shape.o"
    include "zelda_scene/Ice_doukutu/ROOM11/ice_doukutu_grp_ice12.o"
    include "zelda_scene/Ice_doukutu/ROOM11/ice_doukutu_grp_ice12T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_scene"
    romalign 0x1000
    include "zelda_scene/Men/men_main_info.o"
    include "zelda_scene/Men/men_bgc.o"
    include "zelda_scene/Men/men_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "men_room_0"
    romalign 0x1000
    include "zelda_scene/Men/ROOM0/men_0_info.o"
    include "zelda_scene/Men/ROOM0/men_0_shape.o"
    include "zelda_scene/Men/ROOM0/men_grp_men00_00.o"
    include "zelda_scene/Men/ROOM0/men_grp_men00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_1"
    romalign 0x1000
    include "zelda_scene/Men/ROOM1/men_1_info.o"
    include "zelda_scene/Men/ROOM1/men_1_shape.o"
    include "zelda_scene/Men/ROOM1/men_grp_men01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_2"
    romalign 0x1000
    include "zelda_scene/Men/ROOM2/men_2_info.o"
    include "zelda_scene/Men/ROOM2/men_2_shape.o"
    include "zelda_scene/Men/ROOM2/men_grp_men02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_3"
    romalign 0x1000
    include "zelda_scene/Men/ROOM3/men_3_info.o"
    include "zelda_scene/Men/ROOM3/men_3_shape.o"
    include "zelda_scene/Men/ROOM3/men_grp_men03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_4"
    romalign 0x1000
    include "zelda_scene/Men/ROOM4/men_4_info.o"
    include "zelda_scene/Men/ROOM4/men_4_shape.o"
    include "zelda_scene/Men/ROOM4/men_grp_men04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_5"
    romalign 0x1000
    include "zelda_scene/Men/ROOM5/men_5_info.o"
    include "zelda_scene/Men/ROOM5/men_5_shape.o"
    include "zelda_scene/Men/ROOM5/men_grp_men05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_6"
    romalign 0x1000
    include "zelda_scene/Men/ROOM6/men_6_info.o"
    include "zelda_scene/Men/ROOM6/men_6_shape.o"
    include "zelda_scene/Men/ROOM6/men_grp_men06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_7"
    romalign 0x1000
    include "zelda_scene/Men/ROOM7/men_7_info.o"
    include "zelda_scene/Men/ROOM7/men_7_shape.o"
    include "zelda_scene/Men/ROOM7/men_grp_men07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_8"
    romalign 0x1000
    include "zelda_scene/Men/ROOM8/men_8_info.o"
    include "zelda_scene/Men/ROOM8/men_8_shape.o"
    include "zelda_scene/Men/ROOM8/men_grp_men08_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_9"
    romalign 0x1000
    include "zelda_scene/Men/ROOM9/men_9_info.o"
    include "zelda_scene/Men/ROOM9/men_9_shape.o"
    include "zelda_scene/Men/ROOM9/men_grp_men09_00.o"
    include "zelda_scene/Men/ROOM9/men_grp_men09_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "men_room_10"
    romalign 0x1000
    include "zelda_scene/Men/ROOM10/men_10_info.o"
    include "zelda_scene/Men/ROOM10/men_10_shape.o"
    include "zelda_scene/Men/ROOM10/men_grp_men0a_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_scene"
    romalign 0x1000
    include "zelda_scene/Ganontika/ganontika_main_info.o"
    include "zelda_scene/Ganontika/ganontika_bgc.o"
    include "zelda_scene/Ganontika/ganontika_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganontika_room_0"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM0/ganontika_0_info.o"
    include "zelda_scene/Ganontika/ROOM0/ganontika_0_shape.o"
    include "zelda_scene/Ganontika/ROOM0/ganontika_grp_gat00_00.o"
    include "zelda_scene/Ganontika/ROOM0/ganontika_grp_gat00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_1"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM1/ganontika_1_info.o"
    include "zelda_scene/Ganontika/ROOM1/ganontika_1_shape.o"
    include "zelda_scene/Ganontika/ROOM1/ganontika_grp_gat01_00.o"
    include "zelda_scene/Ganontika/ROOM1/ganontika_grp_gat01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_2"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM2/ganontika_2_info.o"
    include "zelda_scene/Ganontika/ROOM2/ganontika_2_shape.o"
    include "zelda_scene/Ganontika/ROOM2/ganontika_grp_gat02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_3"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM3/ganontika_3_info.o"
    include "zelda_scene/Ganontika/ROOM3/ganontika_3_shape.o"
    include "zelda_scene/Ganontika/ROOM3/ganontika_grp_gat03_00.o"
    include "zelda_scene/Ganontika/ROOM3/ganontika_grp_gat03_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_4"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM4/ganontika_4_info.o"
    include "zelda_scene/Ganontika/ROOM4/ganontika_4_shape.o"
    include "zelda_scene/Ganontika/ROOM4/ganontika_grp_gat04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_5"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM5/ganontika_5_info.o"
    include "zelda_scene/Ganontika/ROOM5/ganontika_5_shape.o"
    include "zelda_scene/Ganontika/ROOM5/ganontika_grp_gat05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_6"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM6/ganontika_6_info.o"
    include "zelda_scene/Ganontika/ROOM6/ganontika_6_shape.o"
    include "zelda_scene/Ganontika/ROOM6/ganontika_grp_gat06_00.o"
    include "zelda_scene/Ganontika/ROOM6/ganontika_grp_gat06_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_7"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM7/ganontika_7_info.o"
    include "zelda_scene/Ganontika/ROOM7/ganontika_7_shape.o"
    include "zelda_scene/Ganontika/ROOM7/ganontika_grp_gat07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_8"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM8/ganontika_8_info.o"
    include "zelda_scene/Ganontika/ROOM8/ganontika_8_shape.o"
    include "zelda_scene/Ganontika/ROOM8/ganontika_grp_gat08_00.o"
    include "zelda_scene/Ganontika/ROOM8/ganontika_grp_gat08_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_9"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM9/ganontika_9_info.o"
    include "zelda_scene/Ganontika/ROOM9/ganontika_9_shape.o"
    include "zelda_scene/Ganontika/ROOM9/ganontika_grp_gat09_00.o"
    include "zelda_scene/Ganontika/ROOM9/ganontika_grp_gat09_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_10"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM10/ganontika_10_info.o"
    include "zelda_scene/Ganontika/ROOM10/ganontika_10_shape.o"
    include "zelda_scene/Ganontika/ROOM10/ganontika_grp_gat0a_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_11"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM11/ganontika_11_info.o"
    include "zelda_scene/Ganontika/ROOM11/ganontika_11_shape.o"
    include "zelda_scene/Ganontika/ROOM11/ganontika_grp_gat0b_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_12"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM12/ganontika_12_info.o"
    include "zelda_scene/Ganontika/ROOM12/ganontika_12_shape.o"
    include "zelda_scene/Ganontika/ROOM12/ganontika_grp_gat0c_00.o"
    include "zelda_scene/Ganontika/ROOM12/ganontika_grp_gat0c_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_13"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM13/ganontika_13_info.o"
    include "zelda_scene/Ganontika/ROOM13/ganontika_13_shape.o"
    include "zelda_scene/Ganontika/ROOM13/ganontika_grp_gat0d_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_14"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM14/ganontika_14_info.o"
    include "zelda_scene/Ganontika/ROOM14/ganontika_14_shape.o"
    include "zelda_scene/Ganontika/ROOM14/ganontika_grp_gat0e_00.o"
    include "zelda_scene/Ganontika/ROOM14/ganontika_grp_gat0e_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_15"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM15/ganontika_15_info.o"
    include "zelda_scene/Ganontika/ROOM15/ganontika_15_shape.o"
    include "zelda_scene/Ganontika/ROOM15/ganontika_grp_gat0f_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_16"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM16/ganontika_16_info.o"
    include "zelda_scene/Ganontika/ROOM16/ganontika_16_shape.o"
    include "zelda_scene/Ganontika/ROOM16/ganontika_grp_gat10_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_17"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM17/ganontika_17_info.o"
    include "zelda_scene/Ganontika/ROOM17/ganontika_17_shape.o"
    include "zelda_scene/Ganontika/ROOM17/ganontika_grp_gat11_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_18"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM18/ganontika_18_info.o"
    include "zelda_scene/Ganontika/ROOM18/ganontika_18_shape.o"
    include "zelda_scene/Ganontika/ROOM18/ganontika_grp_gat12_00.o"
    include "zelda_scene/Ganontika/ROOM18/ganontika_grp_gat12_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontika_room_19"
    romalign 0x1000
    include "zelda_scene/Ganontika/ROOM19/ganontika_19_info.o"
    include "zelda_scene/Ganontika/ROOM19/ganontika_19_shape.o"
    include "zelda_scene/Ganontika/ROOM19/ganontika_grp_gat13_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "syotes_scene"
    romalign 0x1000
    include "zelda_scene/Syotes/syotes_main_info.o"
    include "zelda_scene/Syotes/syotes_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "syotes_room_0"
    romalign 0x1000
    include "zelda_scene/Syotes/ROOM0/syotes_0_shape.o"
    include "zelda_scene/Syotes/ROOM0/syotes_grp_syotes.o"
    include "zelda_scene/Syotes/ROOM0/syotes_grp_syotesT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "syotes2_scene"
    romalign 0x1000
    include "zelda_scene/Syotes2/syotes2_main_info.o"
    include "zelda_scene/Syotes2/syotes2_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "syotes2_room_0"
    romalign 0x1000
    include "zelda_scene/Syotes2/ROOM0/syotes2_0_info.o"
    include "zelda_scene/Syotes2/ROOM0/syotes2_0_shape.o"
    include "zelda_scene/Syotes2/ROOM0/syotes2_grp_syoutes2.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "depth_test_scene"
    romalign 0x1000
    include "zelda_scene/P_depth_test/depth_test_main_info.o"
    include "zelda_scene/P_depth_test/depth_test_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "depth_test_room_0"
    romalign 0x1000
    include "zelda_scene/P_depth_test/ROOM0/depth_test_0_info.o"
    include "zelda_scene/P_depth_test/ROOM0/depth_test.o"
    include "zelda_scene/P_depth_test/ROOM0/depth_test_0_shape.o"
    include "zelda_scene/P_depth_test/ROOM0/depth_test_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot00_scene"
    romalign 0x1000
    include "zelda_scene/Spot00/spot00_main_info.o"
    include "zelda_tool_scene/Spot00/spot00_demo01.o"
    include "zelda_tool_scene/Spot00/spot00_demo02.o"
    include "zelda_tool_scene/Spot00/spot00_demo03.o"
    include "zelda_tool_scene/Spot00/spot00_demo04.o"
    include "zelda_tool_scene/Spot00/spot00_demo05.o"
    include "zelda_tool_scene/Spot00/spot00_demo06.o"
    include "zelda_tool_scene/Spot00/spot00_demo07.o"
    include "zelda_tool_scene/Spot00/spot00_demo08.o"
    include "zelda_tool_scene/Spot00/spot00_demo09.o"
    include "zelda_tool_scene/Spot00/spot00_demouma00.o"
    include "zelda_tool_scene/Spot00/spot00_demouma01.o"
    include "zelda_tool_scene/Spot00/spot00_opening.o"
    include "zelda_tool_scene/Spot00/spot00_show.o"
    include "zelda_scene/Spot00/spot00_bgc.o"
    include "zelda_scene/Spot00/spot00_main_info_ad.o"
    include "zelda_scene/Spot00/spot00_main_info_cn.o"
    include "zelda_scene/Spot00/spot00_main_info_d0.o"
    include "zelda_scene/Spot00/spot00_main_info_d1.o"
    include "zelda_scene/Spot00/spot00_main_info_d2.o"
    include "zelda_scene/Spot00/spot00_main_info_d3.o"
    include "zelda_scene/Spot00/spot00_main_info_d4.o"
    include "zelda_scene/Spot00/spot00_main_info_d5.o"
    include "zelda_scene/Spot00/spot00_main_info_d6.o"
    include "zelda_scene/Spot00/spot00_main_info_d7.o"
    include "zelda_scene/Spot00/spot00_main_info_d8.o"
    include "zelda_scene/Spot00/spot00_main_info_d9.o"
    include "zelda_scene/Spot00/spot00_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot00_room_0"
    romalign 0x1000
    include "zelda_scene/Spot00/ROOM0/spot00_0_info.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_ad.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_cn.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d0.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d1.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d2.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d3.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d4.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d5.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d6.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d7.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d8.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_info_d9.o"
    include "zelda_scene/Spot00/ROOM0/spot00_0_shape.o"
    include "zelda_scene/Spot00/ROOM0/spot00_grp_spot00.o"
    include "zelda_scene/Spot00/ROOM0/spot00_grp_spot00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot01_scene"
    romalign 0x1000
    include "zelda_scene/Spot01/spot01_main_info.o"
    include "zelda_tool_scene/Spot01/spot01_demo00.o"
    include "zelda_tool_scene/Spot01/spot01_demo01.o"
    include "zelda_tool_scene/Spot01/spot01_demo02.o"
    include "zelda_tool_scene/Spot01/spot01_show.o"
    include "zelda_scene/Spot01/spot01_bgc.o"
    include "zelda_scene/Spot01/spot01_demo03.o"
    include "zelda_scene/Spot01/spot01_demo04.o"
    include "zelda_scene/Spot01/spot01_main_info_ad.o"
    include "zelda_scene/Spot01/spot01_main_info_an.o"
    include "zelda_scene/Spot01/spot01_main_info_cn.o"
    include "zelda_scene/Spot01/spot01_main_info_d0.o"
    include "zelda_scene/Spot01/spot01_main_info_d1.o"
    include "zelda_scene/Spot01/spot01_main_info_d2.o"
    include "zelda_scene/Spot01/spot01_main_info_d3.o"
    include "zelda_scene/Spot01/spot01_main_info_d4.o"
    include "zelda_scene/Spot01/spot01_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot01_room_0"
    romalign 0x1000
    include "zelda_scene/Spot01/ROOM0/spot01_0_info.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_ad.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_an.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_cn.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_d0.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_d1.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_d2.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_d3.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_info_d4.o"
    include "zelda_scene/Spot01/ROOM0/spot01_0_shape.o"
    include "zelda_scene/Spot01/ROOM0/spot01_grp_spot01.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot02_scene"
    romalign 0x1000
    include "zelda_scene/Spot02/spot02_main_info.o"
    include "zelda_scene/Spot02/spot02_bgc.o"
    include "zelda_scene/Spot02/spot02_demo00.o"
    include "zelda_scene/Spot02/spot02_demo01.o"
    include "zelda_scene/Spot02/spot02_main_info_ad.o"
    include "zelda_scene/Spot02/spot02_main_info_an.o"
    include "zelda_scene/Spot02/spot02_main_info_cn.o"
    include "zelda_scene/Spot02/spot02_main_info_d0.o"
    include "zelda_scene/Spot02/spot02_main_info_d1.o"
    include "zelda_scene/Spot02/spot02_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot02_room_0"
    romalign 0x1000
    include "zelda_scene/Spot02/ROOM0/spot02_0_info.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_info_ad.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_info_an.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_info_cn.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_info_d0.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_info_d1.o"
    include "zelda_scene/Spot02/ROOM0/spot02_0_shape.o"
    include "zelda_scene/Spot02/ROOM0/spot02_grp_iriguti.o"
    include "zelda_scene/Spot02/ROOM0/spot02_grp_irigutiT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot02_room_1"
    romalign 0x1000
    include "zelda_scene/Spot02/ROOM1/spot02_1_info.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_info_ad.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_info_an.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_info_cn.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_info_d0.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_info_d1.o"
    include "zelda_scene/Spot02/ROOM1/spot02_1_shape.o"
    include "zelda_scene/Spot02/ROOM1/spot02_grp_spot02.o"
    include "zelda_scene/Spot02/ROOM1/spot02_grp_spot02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot03_scene"
    romalign 0x1000
    include "zelda_scene/Spot03/spot03_main_info.o"
    include "zelda_scene/Spot03/spot03_bgc.o"
    include "zelda_scene/Spot03/spot03_main_info_ad.o"
    include "zelda_scene/Spot03/spot03_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot03_room_0"
    romalign 0x1000
    include "zelda_scene/Spot03/ROOM0/spot03_0_info.o"
    include "zelda_scene/Spot03/ROOM0/spot03_0_info_ad.o"
    include "zelda_scene/Spot03/ROOM0/spot03_0_shape.o"
    include "zelda_scene/Spot03/ROOM0/spot03_grp_spot03_1.o"
    include "zelda_scene/Spot03/ROOM0/spot03_grp_spot03_1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot03_room_1"
    romalign 0x1000
    include "zelda_scene/Spot03/ROOM1/spot03_1_info.o"
    include "zelda_scene/Spot03/ROOM1/spot03_1_info_ad.o"
    include "zelda_scene/Spot03/ROOM1/spot03_1_shape.o"
    include "zelda_scene/Spot03/ROOM1/spot03_grp_spot03_2.o"
    include "zelda_scene/Spot03/ROOM1/spot03_grp_spot03_2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot04_scene"
    romalign 0x1000
    include "zelda_scene/Spot04/spot04_main_info.o"
    include "zelda_tool_scene/Spot04/spot04_demo00.o"
    include "zelda_tool_scene/Spot04/spot04_demo01.o"
    include "zelda_tool_scene/Spot04/spot04_demo02.o"
    include "zelda_tool_scene/Spot04/spot04_demo03.o"
    include "zelda_tool_scene/Spot04/spot04_demo04.o"
    include "zelda_tool_scene/Spot04/spot04_yahoo.o"
    include "zelda_scene/Spot04/spot04_bgc.o"
    include "zelda_scene/Spot04/spot04_demo05.o"
    include "zelda_scene/Spot04/spot04_demo06.o"
    include "zelda_scene/Spot04/spot04_demo07.o"
    include "zelda_scene/Spot04/spot04_jr.o"
    include "zelda_scene/Spot04/spot04_jr2.o"
    include "zelda_scene/Spot04/spot04_main_info_ad.o"
    include "zelda_scene/Spot04/spot04_main_info_an.o"
    include "zelda_scene/Spot04/spot04_main_info_d0.o"
    include "zelda_scene/Spot04/spot04_main_info_d1.o"
    include "zelda_scene/Spot04/spot04_main_info_d2.o"
    include "zelda_scene/Spot04/spot04_main_info_d3.o"
    include "zelda_scene/Spot04/spot04_main_info_d4.o"
    include "zelda_scene/Spot04/spot04_main_info_d5.o"
    include "zelda_scene/Spot04/spot04_main_info_d6.o"
    include "zelda_scene/Spot04/spot04_main_info_d7.o"
    include "zelda_scene/Spot04/spot04_main_info_d8.o"
    include "zelda_scene/Spot04/spot04_main_info_d9.o"
    include "zelda_scene/Spot04/spot04_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot04_room_0"
    romalign 0x1000
    include "zelda_scene/Spot04/ROOM0/spot04_0_info.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_ad.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_an.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d0.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d1.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d2.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d3.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d4.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d5.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d6.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d7.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d8.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_info_d9.o"
    include "zelda_scene/Spot04/ROOM0/spot04_0_shape.o"
    include "zelda_scene/Spot04/ROOM0/spot04_grp_spot04_1.o"
    include "zelda_scene/Spot04/ROOM0/spot04_grp_spot04_1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot04_room_1"
    romalign 0x1000
    include "zelda_scene/Spot04/ROOM1/spot04_1_info.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_ad.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_an.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d0.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d1.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d2.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d3.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d4.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d5.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d6.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d7.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d8.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_info_d9.o"
    include "zelda_scene/Spot04/ROOM1/spot04_1_shape.o"
    include "zelda_scene/Spot04/ROOM1/spot04_grp_spot04_2.o"
    include "zelda_scene/Spot04/ROOM1/spot04_grp_spot04_2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot04_room_2"
    romalign 0x1000
    include "zelda_scene/Spot04/ROOM2/spot04_2_info.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_ad.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_an.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d0.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d1.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d2.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d3.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d4.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d5.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d6.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d7.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d8.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_info_d9.o"
    include "zelda_scene/Spot04/ROOM2/spot04_2_shape.o"
    include "zelda_scene/Spot04/ROOM2/spot04_grp_spot04_3.o"
    include "zelda_scene/Spot04/ROOM2/spot04_grp_spot04_3T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot05_scene"
    romalign 0x1000
    include "zelda_scene/Spot05/spot05_main_info.o"
    include "zelda_tool_scene/Spot05/spot05_demo00.o"
    include "zelda_tool_scene/Spot05/spot05_demo01.o"
    include "zelda_scene/Spot05/spot05_bgc.o"
    include "zelda_scene/Spot05/spot05_main_info_ad.o"
    include "zelda_scene/Spot05/spot05_main_info_d0.o"
    include "zelda_scene/Spot05/spot05_main_info_d1.o"
    include "zelda_scene/Spot05/spot05_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot05_room_0"
    romalign 0x1000
    include "zelda_scene/Spot05/ROOM0/spot05_0_info.o"
    include "zelda_scene/Spot05/ROOM0/spot05_0_info_ad.o"
    include "zelda_scene/Spot05/ROOM0/spot05_0_info_d0.o"
    include "zelda_scene/Spot05/ROOM0/spot05_0_info_d1.o"
    include "zelda_scene/Spot05/ROOM0/spot05_0_shape.o"
    include "zelda_scene/Spot05/ROOM0/spot05_grp_spot05.o"
    include "zelda_scene/Spot05/ROOM0/spot05_grp_spot05T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot06_scene"
    romalign 0x1000
    include "zelda_scene/Spot06/spot06_main_info.o"
    include "zelda_tool_scene/Spot06/spot06_demo00.o"
    include "zelda_scene/Spot06/spot06_bgc.o"
    include "zelda_scene/Spot06/spot06_demo01.o"
    include "zelda_scene/Spot06/spot06_main_info_ad.o"
    include "zelda_scene/Spot06/spot06_main_info_d0.o"
    include "zelda_scene/Spot06/spot06_main_info_d1.o"
    include "zelda_scene/Spot06/spot06_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot06_room_0"
    romalign 0x1000
    include "zelda_scene/Spot06/ROOM0/spot06_0_info.o"
    include "zelda_scene/Spot06/ROOM0/spot06_0_info_ad.o"
    include "zelda_scene/Spot06/ROOM0/spot06_0_info_d0.o"
    include "zelda_scene/Spot06/ROOM0/spot06_0_info_d1.o"
    include "zelda_scene/Spot06/ROOM0/spot06_0_shape.o"
    include "zelda_scene/Spot06/ROOM0/spot06_grp_spot06.o"
    include "zelda_scene/Spot06/ROOM0/spot06_grp_spot06T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot07_scene"
    romalign 0x1000
    include "zelda_scene/Spot07/spot07_main_info.o"
    include "zelda_tool_scene/Spot07/spot07_demo00.o"
    include "zelda_tool_scene/Spot07/spot07_show.o"
    include "zelda_scene/Spot07/spot07_bgc.o"
    include "zelda_scene/Spot07/spot07_main_info_ad.o"
    include "zelda_scene/Spot07/spot07_main_info_d0.o"
    include "zelda_scene/Spot07/spot07_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot07_room_0"
    romalign 0x1000
    include "zelda_scene/Spot07/ROOM0/spot07_0_info.o"
    include "zelda_scene/Spot07/ROOM0/spot07_0_info_ad.o"
    include "zelda_scene/Spot07/ROOM0/spot07_0_info_d0.o"
    include "zelda_scene/Spot07/ROOM0/spot07_0_shape.o"
    include "zelda_scene/Spot07/ROOM0/spot07_grp_kingroom.o"
    include "zelda_scene/Spot07/ROOM0/spot07_grp_kingroomT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot07_room_1"
    romalign 0x1000
    include "zelda_scene/Spot07/ROOM1/spot07_1_info.o"
    include "zelda_scene/Spot07/ROOM1/spot07_1_info_ad.o"
    include "zelda_scene/Spot07/ROOM1/spot07_1_info_d0.o"
    include "zelda_scene/Spot07/ROOM1/spot07_1_shape.o"
    include "zelda_scene/Spot07/ROOM1/spot07_grp_spot07.o"
    include "zelda_scene/Spot07/ROOM1/spot07_grp_spot07T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot08_scene"
    romalign 0x1000
    include "zelda_scene/Spot08/spot08_main_info.o"
    include "zelda_tool_scene/Spot08/spot08_demo00.o"
    include "zelda_tool_scene/Spot08/spot08_demo01.o"
    include "zelda_scene/Spot08/spot08_bgc.o"
    include "zelda_scene/Spot08/spot08_demo02.o"
    include "zelda_scene/Spot08/spot08_main_info_ad.o"
    include "zelda_scene/Spot08/spot08_main_info_cn.o"
    include "zelda_scene/Spot08/spot08_main_info_d0.o"
    include "zelda_scene/Spot08/spot08_main_info_d1.o"
    include "zelda_scene/Spot08/spot08_main_info_d2.o"
    include "zelda_scene/Spot08/spot08_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot08_room_0"
    romalign 0x1000
    include "zelda_scene/Spot08/ROOM0/spot08_0_info.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_info_ad.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_info_cn.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_info_d0.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_info_d1.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_info_d2.o"
    include "zelda_scene/Spot08/ROOM0/spot08_0_shape.o"
    include "zelda_scene/Spot08/ROOM0/spot08_grp_spot08.o"
    include "zelda_scene/Spot08/ROOM0/spot08_grp_spot08T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot09_scene"
    romalign 0x1000
    include "zelda_scene/Spot09/spot09_main_info.o"
    include "zelda_tool_scene/Spot09/spot09_demo00.o"
    include "zelda_tool_scene/Spot09/spot09_demo01.o"
    include "zelda_scene/Spot09/spot09_bgc.o"
    include "zelda_scene/Spot09/spot09_demo02.o"
    include "zelda_scene/Spot09/spot09_main_info_ad.o"
    include "zelda_scene/Spot09/spot09_main_info_d0.o"
    include "zelda_scene/Spot09/spot09_main_info_d1.o"
    include "zelda_scene/Spot09/spot09_main_info_d2.o"
    include "zelda_scene/Spot09/spot09_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot09_room_0"
    romalign 0x1000
    include "zelda_scene/Spot09/ROOM0/spot09_0_info.o"
    include "zelda_scene/Spot09/ROOM0/spot09_0_info_ad.o"
    include "zelda_scene/Spot09/ROOM0/spot09_0_info_d0.o"
    include "zelda_scene/Spot09/ROOM0/spot09_0_info_d1.o"
    include "zelda_scene/Spot09/ROOM0/spot09_0_info_d2.o"
    include "zelda_scene/Spot09/ROOM0/spot09_0_shape.o"
    include "zelda_scene/Spot09/ROOM0/spot09_grp_spot09.o"
    include "zelda_scene/Spot09/ROOM0/spot09_grp_spot09T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_scene"
    romalign 0x1000
    include "zelda_scene/Spot10/spot10_main_info.o"
    include "zelda_scene/Spot10/spot10_bgc.o"
    include "zelda_scene/Spot10/spot10_demo00.o"
    include "zelda_scene/Spot10/spot10_main_info_ad.o"
    include "zelda_scene/Spot10/spot10_main_info_d0.o"
    include "zelda_scene/Spot10/spot10_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot10_room_0"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM0/spot10_0_info.o"
    include "zelda_scene/Spot10/ROOM0/spot10_0_info_ad.o"
    include "zelda_scene/Spot10/ROOM0/spot10_0_info_d0.o"
    include "zelda_scene/Spot10/ROOM0/spot10_0_shape.o"
    include "zelda_scene/Spot10/ROOM0/spot10_grp_spot10_1.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_1"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM1/spot10_1_info.o"
    include "zelda_scene/Spot10/ROOM1/spot10_1_info_ad.o"
    include "zelda_scene/Spot10/ROOM1/spot10_1_info_d0.o"
    include "zelda_scene/Spot10/ROOM1/spot10_1_shape.o"
    include "zelda_scene/Spot10/ROOM1/spot10_grp_spot10_2.o"
    include "zelda_scene/Spot10/ROOM1/spot10_grp_spot10_2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_2"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM2/spot10_2_info.o"
    include "zelda_scene/Spot10/ROOM2/spot10_2_info_ad.o"
    include "zelda_scene/Spot10/ROOM2/spot10_2_info_d0.o"
    include "zelda_scene/Spot10/ROOM2/spot10_2_shape.o"
    include "zelda_scene/Spot10/ROOM2/spot10_grp_spot10_3.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_3"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM3/spot10_3_info.o"
    include "zelda_scene/Spot10/ROOM3/spot10_3_info_ad.o"
    include "zelda_scene/Spot10/ROOM3/spot10_3_info_d0.o"
    include "zelda_scene/Spot10/ROOM3/spot10_3_shape.o"
    include "zelda_scene/Spot10/ROOM3/spot10_grp_spot10_4.o"
    include "zelda_scene/Spot10/ROOM3/spot10_grp_spot10_4T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_4"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM4/spot10_4_info.o"
    include "zelda_scene/Spot10/ROOM4/spot10_4_info_ad.o"
    include "zelda_scene/Spot10/ROOM4/spot10_4_info_d0.o"
    include "zelda_scene/Spot10/ROOM4/spot10_4_shape.o"
    include "zelda_scene/Spot10/ROOM4/spot10_grp_spot10_5.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_5"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM5/spot10_5_info.o"
    include "zelda_scene/Spot10/ROOM5/spot10_5_info_ad.o"
    include "zelda_scene/Spot10/ROOM5/spot10_5_info_d0.o"
    include "zelda_scene/Spot10/ROOM5/spot10_5_shape.o"
    include "zelda_scene/Spot10/ROOM5/spot10_grp_spot10_6.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_6"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM6/spot10_6_info.o"
    include "zelda_scene/Spot10/ROOM6/spot10_6_info_ad.o"
    include "zelda_scene/Spot10/ROOM6/spot10_6_info_d0.o"
    include "zelda_scene/Spot10/ROOM6/spot10_6_shape.o"
    include "zelda_scene/Spot10/ROOM6/spot10_grp_spot10_7.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_7"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM7/spot10_7_info.o"
    include "zelda_scene/Spot10/ROOM7/spot10_7_info_ad.o"
    include "zelda_scene/Spot10/ROOM7/spot10_7_info_d0.o"
    include "zelda_scene/Spot10/ROOM7/spot10_7_shape.o"
    include "zelda_scene/Spot10/ROOM7/spot10_grp_spot10_8.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_8"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM8/spot10_8_info.o"
    include "zelda_scene/Spot10/ROOM8/spot10_8_info_ad.o"
    include "zelda_scene/Spot10/ROOM8/spot10_8_info_d0.o"
    include "zelda_scene/Spot10/ROOM8/spot10_8_shape.o"
    include "zelda_scene/Spot10/ROOM8/spot10_grp_spot10_9.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot10_room_9"
    romalign 0x1000
    include "zelda_scene/Spot10/ROOM9/spot10_9_info.o"
    include "zelda_scene/Spot10/ROOM9/spot10_9_info_ad.o"
    include "zelda_scene/Spot10/ROOM9/spot10_9_info_d0.o"
    include "zelda_scene/Spot10/ROOM9/spot10_9_shape.o"
    include "zelda_scene/Spot10/ROOM9/spot10_grp_spot10_hiroba.o"
    include "zelda_scene/Spot10/ROOM9/spot10_grp_spot10_hirobaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot11_scene"
    romalign 0x1000
    include "zelda_scene/Spot11/spot11_main_info.o"
    include "zelda_tool_scene/Spot11/spot11_demo00.o"
    include "zelda_tool_scene/Spot11/spot11_demo01.o"
    include "zelda_scene/Spot11/spot11_bgc.o"
    include "zelda_scene/Spot11/spot11_main_info_ad.o"
    include "zelda_scene/Spot11/spot11_main_info_d0.o"
    include "zelda_scene/Spot11/spot11_main_info_d1.o"
    include "zelda_scene/Spot11/spot11_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot11_room_0"
    romalign 0x1000
    include "zelda_scene/Spot11/ROOM0/spot11_0_info.o"
    include "zelda_scene/Spot11/ROOM0/spot11_0_info_ad.o"
    include "zelda_scene/Spot11/ROOM0/spot11_0_info_d0.o"
    include "zelda_scene/Spot11/ROOM0/spot11_0_info_d1.o"
    include "zelda_scene/Spot11/ROOM0/spot11_0_shape.o"
    include "zelda_scene/Spot11/ROOM0/spot11_grp_spot11.o"
    include "zelda_scene/Spot11/ROOM0/spot11_grp_spot11T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot12_scene"
    romalign 0x1000
    include "zelda_scene/Spot12/spot12_main_info.o"
    include "zelda_tool_scene/Spot12/spot12_demo00.o"
    include "zelda_scene/Spot12/spot12_bgc.o"
    include "zelda_scene/Spot12/spot12_demo01.o"
    include "zelda_scene/Spot12/spot12_demo02.o"
    include "zelda_scene/Spot12/spot12_main_info_ad.o"
    include "zelda_scene/Spot12/spot12_main_info_an.o"
    include "zelda_scene/Spot12/spot12_main_info_d0.o"
    include "zelda_scene/Spot12/spot12_main_info_d1.o"
    include "zelda_scene/Spot12/spot12_main_info_d2.o"
    include "zelda_scene/Spot12/spot12_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot12_room_0"
    romalign 0x1000
    include "zelda_scene/Spot12/ROOM0/spot12_0_info.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_info_ad.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_info_an.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_info_d0.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_info_d1.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_info_d2.o"
    include "zelda_scene/Spot12/ROOM0/spot12_0_shape.o"
    include "zelda_scene/Spot12/ROOM0/spot12_grp_spot12.o"
    include "zelda_scene/Spot12/ROOM0/spot12_grp_spot12T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot12_room_1"
    romalign 0x1000
    include "zelda_scene/Spot12/ROOM1/spot12_1_info.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_info_ad.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_info_an.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_info_d0.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_info_d1.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_info_d2.o"
    include "zelda_scene/Spot12/ROOM1/spot12_1_shape.o"
    include "zelda_scene/Spot12/ROOM1/spot12_grp_yabusame.o"
    include "zelda_scene/Spot12/ROOM1/spot12_grp_yabusameT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot13_scene"
    romalign 0x1000
    include "zelda_scene/Spot13/spot13_main_info.o"
    include "zelda_scene/Spot13/spot13_bgc.o"
    include "zelda_scene/Spot13/spot13_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot13_room_0"
    romalign 0x1000
    include "zelda_scene/Spot13/ROOM0/spot13_0_info.o"
    include "zelda_scene/Spot13/ROOM0/spot13_0_shape.o"
    include "zelda_scene/Spot13/ROOM0/spot13_grp_hokora.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot13_room_1"
    romalign 0x1000
    include "zelda_scene/Spot13/ROOM1/spot13_1_info.o"
    include "zelda_scene/Spot13/ROOM1/spot13_1_shape.o"
    include "zelda_scene/Spot13/ROOM1/spot13_grp_spot13.o"
    include "zelda_scene/Spot13/ROOM1/spot13_grp_spot13T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot15_scene"
    romalign 0x1000
    include "zelda_scene/Spot15/spot15_main_info.o"
    include "zelda_tool_scene/Spot15/spot15_show.o"
    include "zelda_scene/Spot15/spot15_bgc.o"
    include "zelda_scene/Spot15/spot15_main_info_cn.o"
    include "zelda_scene/Spot15/spot15_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot15_room_0"
    romalign 0x1000
    include "zelda_scene/Spot15/ROOM0/spot15_0_info.o"
    include "zelda_scene/Spot15/ROOM0/spot15_0_shape.o"
    include "zelda_scene/Spot15/ROOM0/spot15_grp_spot15.o"
    include "zelda_scene/Spot15/ROOM0/spot15_grp_spot15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot16_scene"
    romalign 0x1000
    include "zelda_scene/Spot16/spot16_main_info.o"
    include "zelda_tool_scene/Spot16/spot16_demo00.o"
    include "zelda_tool_scene/Spot16/spot16_demo01.o"
    include "zelda_tool_scene/Spot16/spot16_demo02.o"
    include "zelda_tool_scene/Spot16/spot16_show.o"
    include "zelda_scene/Spot16/spot16_bgc.o"
    include "zelda_scene/Spot16/spot16_demo03.o"
    include "zelda_scene/Spot16/spot16_demo04.o"
    include "zelda_scene/Spot16/spot16_main_info_ad.o"
    include "zelda_scene/Spot16/spot16_main_info_cn.o"
    include "zelda_scene/Spot16/spot16_main_info_d0.o"
    include "zelda_scene/Spot16/spot16_main_info_d1.o"
    include "zelda_scene/Spot16/spot16_main_info_d2.o"
    include "zelda_scene/Spot16/spot16_main_info_d3.o"
    include "zelda_scene/Spot16/spot16_main_info_d4.o"
    include "zelda_scene/Spot16/spot16_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot16_room_0"
    romalign 0x1000
    include "zelda_scene/Spot16/ROOM0/spot16_0_info.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_ad.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_cn.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_d0.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_d1.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_d2.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_d3.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_info_d4.o"
    include "zelda_scene/Spot16/ROOM0/spot16_0_shape.o"
    include "zelda_scene/Spot16/ROOM0/spot16_grp_spot18.o"
    include "zelda_scene/Spot16/ROOM0/spot16_grp_spot18T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot17_scene"
    romalign 0x1000
    include "zelda_scene/Spot17/spot17_main_info.o"
    include "zelda_tool_scene/Spot17/spot17_demo00.o"
    include "zelda_scene/Spot17/spot17_bgc.o"
    include "zelda_scene/Spot17/spot17_demo01.o"
    include "zelda_scene/Spot17/spot17_main_info_ad.o"
    include "zelda_scene/Spot17/spot17_main_info_d0.o"
    include "zelda_scene/Spot17/spot17_main_info_d1.o"
    include "zelda_scene/Spot17/spot17_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot17_room_0"
    romalign 0x1000
    include "zelda_scene/Spot17/ROOM0/spot17_0_info.o"
    include "zelda_scene/Spot17/ROOM0/spot17_0_info_ad.o"
    include "zelda_scene/Spot17/ROOM0/spot17_0_info_d0.o"
    include "zelda_scene/Spot17/ROOM0/spot17_0_info_d1.o"
    include "zelda_scene/Spot17/ROOM0/spot17_0_shape.o"
    include "zelda_scene/Spot17/ROOM0/spot17_grp_fdan.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot17_room_1"
    romalign 0x1000
    include "zelda_scene/Spot17/ROOM1/spot17_1_info.o"
    include "zelda_scene/Spot17/ROOM1/spot17_1_info_ad.o"
    include "zelda_scene/Spot17/ROOM1/spot17_1_info_d0.o"
    include "zelda_scene/Spot17/ROOM1/spot17_1_info_d1.o"
    include "zelda_scene/Spot17/ROOM1/spot17_1_shape.o"
    include "zelda_scene/Spot17/ROOM1/spot17_grp_spot17.o"
    include "zelda_scene/Spot17/ROOM1/spot17_grp_spot17T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot18_scene"
    romalign 0x1000
    include "zelda_scene/Spot18/spot18_main_info.o"
    include "zelda_tool_scene/Spot18/spot18_show.o"
    include "zelda_scene/Spot18/spot18_bgc.o"
    include "zelda_scene/Spot18/spot18_demo00.o"
    include "zelda_scene/Spot18/spot18_demo01.o"
    include "zelda_scene/Spot18/spot18_main_info_ad.o"
    include "zelda_scene/Spot18/spot18_main_info_d0.o"
    include "zelda_scene/Spot18/spot18_main_info_d1.o"
    include "zelda_scene/Spot18/spot18_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot18_room_0"
    romalign 0x1000
    include "zelda_scene/Spot18/ROOM0/spot18_0_info.o"
    include "zelda_scene/Spot18/ROOM0/spot18_0_info_ad.o"
    include "zelda_scene/Spot18/ROOM0/spot18_0_info_d0.o"
    include "zelda_scene/Spot18/ROOM0/spot18_0_info_d1.o"
    include "zelda_scene/Spot18/ROOM0/spot18_0_shape.o"
    include "zelda_scene/Spot18/ROOM0/spot18_grp_clroom.o"
    include "zelda_scene/Spot18/ROOM0/spot18_grp_clroomT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot18_room_1"
    romalign 0x1000
    include "zelda_scene/Spot18/ROOM1/spot18_1_info.o"
    include "zelda_scene/Spot18/ROOM1/spot18_1_info_ad.o"
    include "zelda_scene/Spot18/ROOM1/spot18_1_info_d0.o"
    include "zelda_scene/Spot18/ROOM1/spot18_1_info_d1.o"
    include "zelda_scene/Spot18/ROOM1/spot18_1_shape.o"
    include "zelda_scene/Spot18/ROOM1/spot18_grp_darunia.o"
    include "zelda_scene/Spot18/ROOM1/spot18_grp_daruniaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot18_room_2"
    romalign 0x1000
    include "zelda_scene/Spot18/ROOM2/spot18_2_info.o"
    include "zelda_scene/Spot18/ROOM2/spot18_2_info_ad.o"
    include "zelda_scene/Spot18/ROOM2/spot18_2_info_d0.o"
    include "zelda_scene/Spot18/ROOM2/spot18_2_info_d1.o"
    include "zelda_scene/Spot18/ROOM2/spot18_2_shape.o"
    include "zelda_scene/Spot18/ROOM2/spot18_grp_mdroom.o"
    include "zelda_scene/Spot18/ROOM2/spot18_grp_mdroomT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot18_room_3"
    romalign 0x1000
    include "zelda_scene/Spot18/ROOM3/spot18_3_info.o"
    include "zelda_scene/Spot18/ROOM3/spot18_3_info_ad.o"
    include "zelda_scene/Spot18/ROOM3/spot18_3_info_d0.o"
    include "zelda_scene/Spot18/ROOM3/spot18_3_info_d1.o"
    include "zelda_scene/Spot18/ROOM3/spot18_3_shape.o"
    include "zelda_scene/Spot18/ROOM3/spot18_grp_spot18.o"
    include "zelda_scene/Spot18/ROOM3/spot18_grp_spot18T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "market_day_scene"
    romalign 0x1000
    include "zelda_scene/Market_day/market_day_main_info.o"
    include "zelda_scene/Market_day/market_day_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "market_day_room_0"
    romalign 0x1000
    include "zelda_scene/Market_day/ROOM0/market_day_0_info.o"
    include "zelda_scene/Market_day/ROOM0/market_day_0_shape.o"
    include "zelda_scene/Market_day/ROOM0/market_day_grp_dis_d.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "market_night_scene"
    romalign 0x1000
    include "zelda_scene/Market_night/market_night_main_info.o"
    include "zelda_scene/Market_night/market_night_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "market_night_room_0"
    romalign 0x1000
    include "zelda_scene/Market_night/ROOM0/market_night_0_info.o"
    include "zelda_scene/Market_night/ROOM0/market_night_0_shape.o"
    include "zelda_scene/Market_night/ROOM0/market_night_grp_dis_d.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "testroom_scene"
    romalign 0x1000
    include "zelda_scene/Testroom/testroom_main_info.o"
    include "zelda_scene/Testroom/testroom_bgc.o"
    include "zelda_scene/Testroom/testroom_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "testroom_room_0"
    romalign 0x1000
    include "zelda_scene/Testroom/ROOM0/testroom_0_info.o"
    include "zelda_scene/Testroom/ROOM0/testroom_0_shape.o"
    include "zelda_scene/Testroom/ROOM0/testroom_grp_tsr00_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "testroom_room_1"
    romalign 0x1000
    include "zelda_scene/Testroom/ROOM1/testroom_1_info.o"
    include "zelda_scene/Testroom/ROOM1/testroom_1_shape.o"
    include "zelda_scene/Testroom/ROOM1/testroom_grp_tsr01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "testroom_room_2"
    romalign 0x1000
    include "zelda_scene/Testroom/ROOM2/testroom_2_info.o"
    include "zelda_scene/Testroom/ROOM2/testroom_2_shape.o"
    include "zelda_scene/Testroom/ROOM2/testroom_grp_tsr02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "testroom_room_3"
    romalign 0x1000
    include "zelda_scene/Testroom/ROOM3/testroom_3_info.o"
    include "zelda_scene/Testroom/ROOM3/testroom_3_shape.o"
    include "zelda_scene/Testroom/ROOM3/testroom_grp_tsr03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "testroom_room_4"
    romalign 0x1000
    include "zelda_scene/Testroom/ROOM4/testroom_4_info.o"
    include "zelda_scene/Testroom/ROOM4/testroom_4_shape.o"
    include "zelda_scene/Testroom/ROOM4/testroom_grp_tsr04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kenjyanoma_scene"
    romalign 0x1000
    include "zelda_scene/Kenjyanoma/kenjyanoma_main_info.o"
    include "zelda_tool_scene/Kenjyanoma/Kenjyanoma_demo00.o"
    include "zelda_tool_scene/Kenjyanoma/Kenjyanoma_demo01.o"
    include "zelda_tool_scene/Kenjyanoma/Kenjyanoma_demo02.o"
    include "zelda_scene/Kenjyanoma/kenjyanoma_bgc.o"
    include "zelda_scene/Kenjyanoma/kenjyanoma_main_info_d0.o"
    include "zelda_scene/Kenjyanoma/kenjyanoma_main_info_d1.o"
    include "zelda_scene/Kenjyanoma/kenjyanoma_main_info_d2.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kenjyanoma_room_0"
    romalign 0x1000
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_0_info.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_0_info_d0.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_0_info_d1.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_0_info_d2.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_0_shape.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_grp_g55.o"
    include "zelda_scene/Kenjyanoma/ROOM0/kenjyanoma_grp_g55T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "tokinoma_scene"
    romalign 0x1000
    include "zelda_scene/Tokinoma/tokinoma_main_info.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_door.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_grupy.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_khisui.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_seiti.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_siku.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_tama.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_zd.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_zd1.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_zd2.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_zd3.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_dm_zsfa.o"
    include "zelda_tool_scene/Tokinoma/tokinoma_show.o"
    include "zelda_scene/Tokinoma/tokinoma_bgc.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d0.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d1.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d10.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d2.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d3.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d4.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d5.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d6.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d7.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d8.o"
    include "zelda_scene/Tokinoma/tokinoma_main_info_d9.o"
    include "zelda_scene/Tokinoma/tokinoma_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "tokinoma_room_0"
    romalign 0x1000
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d0.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d1.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d10.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d2.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d3.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d4.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d5.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d6.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d7.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d8.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_info_d9.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_0_shape.o"
    include "zelda_scene/Tokinoma/ROOM0/tokinoma_grp_kyr00_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "tokinoma_room_1"
    romalign 0x1000
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d0.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d1.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d10.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d2.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d3.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d4.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d5.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d6.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d7.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d8.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_info_d9.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_1_shape.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_grp_kyr01_00.o"
    include "zelda_scene/Tokinoma/ROOM1/tokinoma_grp_kyr01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "sutaru_scene"
    romalign 0x1000
    include "zelda_scene/Sutaru/sutaru_main_info.o"
    include "zelda_scene/Sutaru/sutaru_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "sutaru_room_0"
    romalign 0x1000
    include "zelda_scene/Sutaru/ROOM0/sutaru_0_info.o"
    include "zelda_scene/Sutaru/ROOM0/sutaru2_grp_sta1_00.o"
    include "zelda_scene/Sutaru/ROOM0/sutaru_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "link_home_scene"
    romalign 0x1000
    include "zelda_scene/Link/link_home_main_info.o"
    include "zelda_tool_scene/Link/rinkie_demo.o"
    include "zelda_tool_scene/Link/rinkie_demo01.o"
    include "zelda_scene/Link/link_home_bgc.o"
    include "zelda_scene/Link/link_home_main_info_d0.o"
    include "zelda_scene/Link/link_home_main_info_d1.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "link_home_room_0"
    romalign 0x1000
    include "zelda_scene/Link/ROOM0/link_home_0_info.o"
    include "zelda_scene/Link/ROOM0/link_home.o"
    include "zelda_scene/Link/ROOM0/link_home_0_info_d0.o"
    include "zelda_scene/Link/ROOM0/link_home_0_info_d1.o"
    include "zelda_scene/Link/ROOM0/link_home_0_shape.o"
    include "zelda_scene/Link/ROOM0/link_home_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kokiri_shop_scene"
    romalign 0x1000
    include "zelda_scene/Kokiri/kokiri_shop_main_info.o"
    include "zelda_scene/Kokiri/kokiri_shop_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kokiri_shop_room_0"
    romalign 0x1000
    include "zelda_scene/Kokiri/ROOM0/kokiri_shop_0_info.o"
    include "zelda_scene/Kokiri/ROOM0/kokiri_shop.o"
    include "zelda_scene/Kokiri/ROOM0/kokiri_shop_0_shape.o"
    include "zelda_scene/Kokiri/ROOM0/kokiri_sp1_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kokiri_home_scene"
    romalign 0x1000
    include "zelda_scene/K_Home/kokiri_home_main_info.o"
    include "zelda_scene/K_Home/kokiri_home_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kokiri_home_room_0"
    romalign 0x1000
    include "zelda_scene/K_Home/ROOM0/kokiri_home_0_info.o"
    include "zelda_scene/K_Home/ROOM0/kokiri_home.o"
    include "zelda_scene/K_Home/ROOM0/kokiri_home_0_shape.o"
    include "zelda_scene/K_Home/ROOM0/kokiri_home_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_scene"
    romalign 0x1000
    include "zelda_scene/Kakusiana/kakusiana_main_info.o"
    include "zelda_scene/Kakusiana/kakusiana_bgc.o"
    include "zelda_scene/Kakusiana/kakusiana_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kakusiana_room_0"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM0/kakusiana_0_info.o"
    include "zelda_scene/Kakusiana/ROOM0/kakusiana_0_shape.o"
    include "zelda_scene/Kakusiana/ROOM0/kakusiana_grp_ana00.o"
    include "zelda_scene/Kakusiana/ROOM0/kakusiana_grp_ana00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_1"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM1/kakusiana_1_info.o"
    include "zelda_scene/Kakusiana/ROOM1/kakusiana_1_shape.o"
    include "zelda_scene/Kakusiana/ROOM1/kakusiana_grp_ana02.o"
    include "zelda_scene/Kakusiana/ROOM1/kakusiana_grp_ana02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_2"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM2/kakusiana_2_info.o"
    include "zelda_scene/Kakusiana/ROOM2/kakusiana_2_shape.o"
    include "zelda_scene/Kakusiana/ROOM2/kakusiana_grp_ana03.o"
    include "zelda_scene/Kakusiana/ROOM2/kakusiana_grp_ana03T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_3"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM3/kakusiana_3_info.o"
    include "zelda_scene/Kakusiana/ROOM3/kakusiana_3_shape.o"
    include "zelda_scene/Kakusiana/ROOM3/kakusiana_grp_ana04.o"
    include "zelda_scene/Kakusiana/ROOM3/kakusiana_grp_ana04T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_4"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM4/kakusiana_4_info.o"
    include "zelda_scene/Kakusiana/ROOM4/kakusiana_4_shape.o"
    include "zelda_scene/Kakusiana/ROOM4/kakusiana_grp_ana05.o"
    include "zelda_scene/Kakusiana/ROOM4/kakusiana_grp_ana05T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_5"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM5/kakusiana_5_info.o"
    include "zelda_scene/Kakusiana/ROOM5/kakusiana_5_shape.o"
    include "zelda_scene/Kakusiana/ROOM5/kakusiana_grp_ana06.o"
    include "zelda_scene/Kakusiana/ROOM5/kakusiana_grp_ana06T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_6"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM6/kakusiana_6_info.o"
    include "zelda_scene/Kakusiana/ROOM6/kakusiana_6_shape.o"
    include "zelda_scene/Kakusiana/ROOM6/kakusiana_grp_ana07.o"
    include "zelda_scene/Kakusiana/ROOM6/kakusiana_grp_ana07T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_7"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM7/kakusiana_7_info.o"
    include "zelda_scene/Kakusiana/ROOM7/kakusiana_7_shape.o"
    include "zelda_scene/Kakusiana/ROOM7/kakusiana_grp_ana08.o"
    include "zelda_scene/Kakusiana/ROOM7/kakusiana_grp_ana08T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_8"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM8/kakusiana_8_info.o"
    include "zelda_scene/Kakusiana/ROOM8/kakusiana_8_shape.o"
    include "zelda_scene/Kakusiana/ROOM8/kakusiana_grp_ana09.o"
    include "zelda_scene/Kakusiana/ROOM8/kakusiana_grp_ana09T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_9"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM9/kakusiana_9_info.o"
    include "zelda_scene/Kakusiana/ROOM9/kakusiana_9_shape.o"
    include "zelda_scene/Kakusiana/ROOM9/kakusiana_grp_ana10.o"
    include "zelda_scene/Kakusiana/ROOM9/kakusiana_grp_ana10T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_10"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM10/kakusiana_10_info.o"
    include "zelda_scene/Kakusiana/ROOM10/kakusiana_10_shape.o"
    include "zelda_scene/Kakusiana/ROOM10/kakusiana_grp_ana11.o"
    include "zelda_scene/Kakusiana/ROOM10/kakusiana_grp_ana11T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_11"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM11/kakusiana_11_info.o"
    include "zelda_scene/Kakusiana/ROOM11/kakusiana_11_shape.o"
    include "zelda_scene/Kakusiana/ROOM11/kakusiana_grp_ana12.o"
    include "zelda_scene/Kakusiana/ROOM11/kakusiana_grp_ana12T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_12"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM12/kakusiana_12_info.o"
    include "zelda_scene/Kakusiana/ROOM12/kakusiana_12_shape.o"
    include "zelda_scene/Kakusiana/ROOM12/kakusiana_grp_ana13.o"
    include "zelda_scene/Kakusiana/ROOM12/kakusiana_grp_ana13T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakusiana_room_13"
    romalign 0x1000
    include "zelda_scene/Kakusiana/ROOM13/kakusiana_13_info.o"
    include "zelda_scene/Kakusiana/ROOM13/kakusiana_13_shape.o"
    include "zelda_scene/Kakusiana/ROOM13/kakusiana_grp_ana_01.o"
    include "zelda_scene/Kakusiana/ROOM13/kakusiana_grp_ana_01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "entra_scene"
    romalign 0x1000
    include "zelda_scene/Entra_day/entra_main_info.o"
    include "zelda_scene/Entra_day/entra_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "entra_room_0"
    romalign 0x1000
    include "zelda_scene/Entra_day/ROOM0/entra_0_info.o"
    include "zelda_scene/Entra_day/ROOM0/entra.o"
    include "zelda_scene/Entra_day/ROOM0/entra_0_shape.o"
    include "zelda_scene/Entra_day/ROOM0/entra_d_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "moribossroom_scene"
    romalign 0x1000
    include "zelda_scene/Moriboss/moribossroom_main_info.o"
    include "zelda_scene/Moriboss/moribossroom_bgc.o"
    include "zelda_scene/Moriboss/moribossroom_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "moribossroom_room_0"
    romalign 0x1000
    include "zelda_scene/Moriboss/ROOM0/moribossroom_0_info.o"
    include "zelda_scene/Moriboss/ROOM0/moribossroom_0_shape.o"
    include "zelda_scene/Moriboss/ROOM0/moribossroom_grp_mor11_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "moribossroom_room_1"
    romalign 0x1000
    include "zelda_scene/Moriboss/ROOM1/moribossroom_1_info.o"
    include "zelda_scene/Moriboss/ROOM1/moribossroom_1_shape.o"
    include "zelda_scene/Moriboss/ROOM1/moribossroom_grp_mor12_00.o"
    include "zelda_scene/Moriboss/ROOM1/moribossroom_grp_mor12_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "syatekijyou_scene"
    romalign 0x1000
    include "zelda_scene/Syatekijyou/syatekijyou_main_info.o"
    include "zelda_tool_scene/Syatekijyou/syatekijyou_demo00.o"
    include "zelda_tool_scene/Syatekijyou/syatekijyou_demo01.o"
    include "zelda_tool_scene/Syatekijyou/syatekijyou_demo02.o"
    include "zelda_scene/Syatekijyou/syatekijyou_bgc.o"
    include "zelda_scene/Syatekijyou/syatekijyou_main_info_d0.o"
    include "zelda_scene/Syatekijyou/syatekijyou_main_info_d1.o"
    include "zelda_scene/Syatekijyou/syatekijyou_main_info_d2.o"
    include "zelda_scene/Syatekijyou/syatekijyou_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "syatekijyou_room_0"
    romalign 0x1000
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_0_info.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_0_info_d0.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_0_info_d1.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_0_info_d2.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_0_shape.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_grp_g1.o"
    include "zelda_scene/Syatekijyou/ROOM0/syatekijyou_grp_g1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "shop1_scene"
    romalign 0x1000
    include "zelda_scene/Shop/shop1_main_info.o"
    include "zelda_scene/Shop/shop1_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "shop1_room_0"
    romalign 0x1000
    include "zelda_scene/Shop/ROOM0/shop1_0_info.o"
    include "zelda_scene/Shop/ROOM0/shop1.o"
    include "zelda_scene/Shop/ROOM0/shop1_0_shape.o"
    include "zelda_scene/Shop/ROOM0/shop1_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hairal_niwa_scene"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa/hairal_niwa_main_info.o"
    include "zelda_scene/Hairal_niwa/hairal_niwa_bgc.o"
    include "zelda_scene/Hairal_niwa/hairal_niwa_main_info_cn.o"
    include "zelda_scene/Hairal_niwa/hairal_niwa_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hairal_niwa_room_0"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa/ROOM0/hairal_niwa_0_info.o"
    include "zelda_scene/Hairal_niwa/ROOM0/hairal_niwa_0_shape.o"
    include "zelda_scene/Hairal_niwa/ROOM0/hairal_niwa_grp_niwa.o"
    include "zelda_scene/Hairal_niwa/ROOM0/hairal_niwa_grp_niwaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_tou_scene"
    romalign 0x1000
    include "zelda_scene/Ganon_tou/ganon_tou_main_info.o"
    include "zelda_tool_scene/Ganon_tou/ganon_tou_demo00.o"
    include "zelda_scene/Ganon_tou/ganon_tou_bgc.o"
    include "zelda_scene/Ganon_tou/ganon_tou_main_info_ad.o"
    include "zelda_scene/Ganon_tou/ganon_tou_main_info_d0.o"
    include "zelda_scene/Ganon_tou/ganon_tou_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_tou_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon_tou/ROOM0/ganon_tou_0_info.o"
    include "zelda_scene/Ganon_tou/ROOM0/ganon_tou_0_info_d0.o"
    include "zelda_scene/Ganon_tou/ROOM0/ganon_tou_0_shape.o"
    include "zelda_scene/Ganon_tou/ROOM0/ganon_tou_grp_spot15.o"
    include "zelda_scene/Ganon_tou/ROOM0/ganon_tou_grp_spot15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "sasatest_scene"
    romalign 0x1000
    include "zelda_scene/Sasatest/sasatest_main_info.o"
    include "zelda_scene/Sasatest/sasatest_bgc.o"
    include "zelda_scene/Sasatest/sasatest_main_info_ad.o"
    include "zelda_scene/Sasatest/sasatest_main_info_an.o"
    include "zelda_scene/Sasatest/sasatest_main_info_cn.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "sasatest_room_0"
    romalign 0x1000
    include "zelda_scene/Sasatest/ROOM0/sasatest_0_info.o"
    include "zelda_scene/Sasatest/ROOM0/sasatest_0_info_ad.o"
    include "zelda_scene/Sasatest/ROOM0/sasatest_0_info_an.o"
    include "zelda_scene/Sasatest/ROOM0/sasatest_0_info_cn.o"
    include "zelda_scene/Sasatest/ROOM0/sasatest_0_shape.o"
    include "zelda_scene/Sasatest/ROOM0/sasatest_grp_g1.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "market_alley_scene"
    romalign 0x1000
    include "zelda_scene/Market_alley/market_alley_main_info.o"
    include "zelda_scene/Market_alley/market_alley_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "market_alley_room_0"
    romalign 0x1000
    include "zelda_scene/Market_alley/ROOM0/market_alley_0_info.o"
    include "zelda_scene/Market_alley/ROOM0/alley1_txt.o"
    include "zelda_scene/Market_alley/ROOM0/alley2_txt.o"
    include "zelda_scene/Market_alley/ROOM0/alley3_txt.o"
    include "zelda_scene/Market_alley/ROOM0/market_alley.o"
    include "zelda_scene/Market_alley/ROOM0/market_alley_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "spot20_scene"
    romalign 0x1000
    include "zelda_scene/Spot20/spot20_main_info.o"
    include "zelda_tool_scene/Spot20/okarina_epona.o"
    include "zelda_tool_scene/Spot20/spot20_demo00.o"
    include "zelda_tool_scene/Spot20/spot20_demo01.o"
    include "zelda_tool_scene/Spot20/spot20_demo02.o"
    include "zelda_scene/Spot20/spot20_bgc.o"
    include "zelda_scene/Spot20/spot20_demo03.o"
    include "zelda_scene/Spot20/spot20_demo04.o"
    include "zelda_scene/Spot20/spot20_demo05.o"
    include "zelda_scene/Spot20/spot20_demo06.o"
    include "zelda_scene/Spot20/spot20_demo07.o"
    include "zelda_scene/Spot20/spot20_main_info_ad.o"
    include "zelda_scene/Spot20/spot20_main_info_an.o"
    include "zelda_scene/Spot20/spot20_main_info_cn.o"
    include "zelda_scene/Spot20/spot20_main_info_d0.o"
    include "zelda_scene/Spot20/spot20_main_info_d1.o"
    include "zelda_scene/Spot20/spot20_main_info_d2.o"
    include "zelda_scene/Spot20/spot20_main_info_d3.o"
    include "zelda_scene/Spot20/spot20_main_info_d4.o"
    include "zelda_scene/Spot20/spot20_main_info_d5.o"
    include "zelda_scene/Spot20/spot20_main_info_d6.o"
    include "zelda_scene/Spot20/spot20_main_info_d7.o"
    include "zelda_scene/Spot20/spot20_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "spot20_room_0"
    romalign 0x1000
    include "zelda_scene/Spot20/ROOM0/spot20_0_info.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_ad.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_an.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_cn.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d0.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d1.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d2.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d3.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d4.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d5.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d6.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_info_d7.o"
    include "zelda_scene/Spot20/ROOM0/spot20_0_shape.o"
    include "zelda_scene/Spot20/ROOM0/spot20_grp_spot20.o"
    include "zelda_scene/Spot20/ROOM0/spot20_grp_spot20T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "market_ruins_scene"
    romalign 0x1000
    include "zelda_scene/Market_ruins/market_ruins_main_info.o"
    include "zelda_scene/Market_ruins/market_ruins_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "market_ruins_room_0"
    romalign 0x1000
    include "zelda_scene/Market_ruins/ROOM0/market_ruins_0_info.o"
    include "zelda_scene/Market_ruins/ROOM0/market_ruins_0_shape.o"
    include "zelda_scene/Market_ruins/ROOM0/market_ruins_grp_dis_d.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "entra_n_scene"
    romalign 0x1000
    include "zelda_scene/Entra_night/entra_n_main_info.o"
    include "zelda_scene/Entra_night/entra_n_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "entra_n_room_0"
    romalign 0x1000
    include "zelda_scene/Entra_night/ROOM0/entra_n_0_info.o"
    include "zelda_scene/Entra_night/ROOM0/entra_n.o"
    include "zelda_scene/Entra_night/ROOM0/entra_n_0_shape.o"
    include "zelda_scene/Entra_night/ROOM0/entra_n_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "enrui_scene"
    romalign 0x1000
    include "zelda_scene/Entra_ruins/enrui_main_info.o"
    include "zelda_scene/Entra_ruins/enrui_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "enrui_room_0"
    romalign 0x1000
    include "zelda_scene/Entra_ruins/ROOM0/enrui_0_info.o"
    include "zelda_scene/Entra_ruins/ROOM0/enrui.o"
    include "zelda_scene/Entra_ruins/ROOM0/enrui_0_shape.o"
    include "zelda_scene/Entra_ruins/ROOM0/entra1_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "market_alley_n_scene"
    romalign 0x1000
    include "zelda_scene/Market_alley_n/market_alley_n_main_info.o"
    include "zelda_scene/Market_alley_n/market_alley_n_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "market_alley_n_room_0"
    romalign 0x1000
    include "zelda_scene/Market_alley_n/ROOM0/market_alley_n_0_info.o"
    include "zelda_scene/Market_alley_n/ROOM0/alley_n1_txt.o"
    include "zelda_scene/Market_alley_n/ROOM0/alley_n2_txt.o"
    include "zelda_scene/Market_alley_n/ROOM0/alley_n3_txt.o"
    include "zelda_scene/Market_alley_n/ROOM0/market_alley_n.o"
    include "zelda_scene/Market_alley_n/ROOM0/market_alley_n_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hiral_demo_scene"
    romalign 0x1000
    include "zelda_scene/Hiral_demo/hiral_demo_main_info.o"
    include "zelda_tool_scene/Hiral_demo/hiral_demo00.o"
    include "zelda_tool_scene/Hiral_demo/hiral_demo01.o"
    include "zelda_tool_scene/Hiral_demo/hiral_demo02.o"
    include "zelda_tool_scene/Hiral_demo/hiral_demo03.o"
    include "zelda_tool_scene/Hiral_demo/hiral_demo04.o"
    include "zelda_scene/Hiral_demo/hiral_demo05.o"
    include "zelda_scene/Hiral_demo/hiral_demo06.o"
    include "zelda_scene/Hiral_demo/hiral_demo07.o"
    include "zelda_scene/Hiral_demo/hiral_demo08.o"
    include "zelda_scene/Hiral_demo/hiral_demo_bgc.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_ad.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_an.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_cn.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d0.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d1.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d2.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d3.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d4.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d5.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d6.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d7.o"
    include "zelda_scene/Hiral_demo/hiral_demo_main_info_d8.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hiral_demo_room_0"
    romalign 0x1000
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_ad.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_an.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_cn.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d0.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d1.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d2.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d3.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d4.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d5.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d6.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d7.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_info_d8.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_0_shape.o"
    include "zelda_scene/Hiral_demo/ROOM0/hiral_demo_grp_g1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kokiri_home3_scene"
    romalign 0x1000
    include "zelda_scene/K_Home3/kokiri_home3_main_info.o"
    include "zelda_scene/K_Home3/kokiri_home3_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kokiri_home3_room_0"
    romalign 0x1000
    include "zelda_scene/K_Home3/ROOM0/kokiri_home3_0_info.o"
    include "zelda_scene/K_Home3/ROOM0/kokiri3_txt.o"
    include "zelda_scene/K_Home3/ROOM0/kokiri_home3.o"
    include "zelda_scene/K_Home3/ROOM0/kokiri_home3_0_shape.o"
    include "zelda_scene/K_Home3/ROOM0/room_info.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "malon_stable_scene"
    romalign 0x1000
    include "zelda_scene/Stable/malon_stable_main_info.o"
    include "zelda_scene/Stable/malon_stable_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "malon_stable_room_0"
    romalign 0x1000
    include "zelda_scene/Stable/ROOM0/malon_stable_0_info.o"
    include "zelda_scene/Stable/ROOM0/malon_stable.o"
    include "zelda_scene/Stable/ROOM0/malon_stable1_txt.o"
    include "zelda_scene/Stable/ROOM0/malon_stable_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakariko_scene"
    romalign 0x1000
    include "zelda_scene/Kakariko/kakariko_main_info.o"
    include "zelda_scene/Kakariko/kakariko_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kakariko_room_0"
    romalign 0x1000
    include "zelda_scene/Kakariko/ROOM0/kakariko_0_info.o"
    include "zelda_scene/Kakariko/ROOM0/kakariko.o"
    include "zelda_scene/Kakariko/ROOM0/kakariko1_txt.o"
    include "zelda_scene/Kakariko/ROOM0/kakariko_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_boss_scene"
    romalign 0x1000
    include "zelda_scene/Bdan_boss/bdan_boss_main_info.o"
    include "zelda_scene/Bdan_boss/bdan_boss_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "bdan_boss_room_0"
    romalign 0x1000
    include "zelda_scene/Bdan_boss/ROOM0/bdan_boss_0_info.o"
    include "zelda_scene/Bdan_boss/ROOM0/bdan_boss_0_shape.o"
    include "zelda_scene/Bdan_boss/ROOM0/bdan_boss_grp_bda05_00.o"
    include "zelda_scene/Bdan_boss/ROOM0/bdan_boss_grp_bda05_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bdan_boss_room_1"
    romalign 0x1000
    include "zelda_scene/Bdan_boss/ROOM1/bdan_boss_1_info.o"
    include "zelda_scene/Bdan_boss/ROOM1/bdan_boss_1_shape.o"
    include "zelda_scene/Bdan_boss/ROOM1/bdan_boss_grp_bdaff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "FIRE_bs_scene"
    romalign 0x1000
    include "zelda_scene/Fire_bs/FIRE_bs_main_info.o"
    include "zelda_scene/Fire_bs/FIRE_bs_bgc.o"
    include "zelda_scene/Fire_bs/FIRE_bs_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "FIRE_bs_room_0"
    romalign 0x1000
    include "zelda_scene/Fire_bs/ROOM0/FIRE_bs_0_info.o"
    include "zelda_scene/Fire_bs/ROOM0/FIRE_bs_0_shape.o"
    include "zelda_scene/Fire_bs/ROOM0/FIRE_bs_grp_Hid02.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "FIRE_bs_room_1"
    romalign 0x1000
    include "zelda_scene/Fire_bs/ROOM1/FIRE_bs_1_info.o"
    include "zelda_scene/Fire_bs/ROOM1/FIRE_bs_1_shape.o"
    include "zelda_scene/Fire_bs/ROOM1/FIRE_bs_grp_Hid03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hut_scene"
    romalign 0x1000
    include "zelda_scene/HUT/hut_main_info.o"
    include "zelda_scene/HUT/hut_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hut_room_0"
    romalign 0x1000
    include "zelda_scene/HUT/ROOM0/hut_0_info.o"
    include "zelda_scene/HUT/ROOM0/hut.o"
    include "zelda_scene/HUT/ROOM0/hut1_txt.o"
    include "zelda_scene/HUT/ROOM0/hut_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "daiyousei_izumi_scene"
    romalign 0x1000
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_main_info.o"
    include "zelda_scene/Daiyousei_izumi/Daiyousei_demo00.o"
    include "zelda_scene/Daiyousei_izumi/Daiyousei_demo01.o"
    include "zelda_scene/Daiyousei_izumi/Daiyousei_demo02.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_bgc.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_main_info_cn.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_main_info_d0.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_main_info_d1.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_main_info_d2.o"
    include "zelda_scene/Daiyousei_izumi/daiyousei_izumi_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "daiyousei_izumi_room_0"
    romalign 0x1000
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_info.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_info_cn.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_info_d0.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_info_d1.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_info_d2.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_0_shape.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_grp_g2.o"
    include "zelda_scene/Daiyousei_izumi/ROOM0/daiyousei_izumi_grp_g2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakaana_scene"
    romalign 0x1000
    include "zelda_scene/Hakaana/hakaana_main_info.o"
    include "zelda_scene/Hakaana/hakaana_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hakaana_room_0"
    romalign 0x1000
    include "zelda_scene/Hakaana/ROOM0/hakaana_0_info.o"
    include "zelda_scene/Hakaana/ROOM0/hakaana_0_shape.o"
    include "zelda_scene/Hakaana/ROOM0/hakaana_grp_g1.o"
    include "zelda_scene/Hakaana/ROOM0/hakaana_grp_g1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "yousei_izumi_tate_scene"
    romalign 0x1000
    include "zelda_scene/Yousei_izumi_tate/yousei_izumi_tate_main_info.o"
    include "zelda_scene/Yousei_izumi_tate/yousei_izumi_tate_bgc.o"
    include "zelda_scene/Yousei_izumi_tate/yousei_izumi_tate_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "yousei_izumi_tate_room_0"
    romalign 0x1000
    include "zelda_scene/Yousei_izumi_tate/ROOM0/yousei_izumi_tate_0_info.o"
    include "zelda_scene/Yousei_izumi_tate/ROOM0/yousei_izumi_tate_0_shape.o"
    include "zelda_scene/Yousei_izumi_tate/ROOM0/yousei_izumi_tate_grp_g2.o"
    include "zelda_scene/Yousei_izumi_tate/ROOM0/yousei_izumi_tate_grp_g2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "yousei_izumi_yoko_scene"
    romalign 0x1000
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_main_info.o"
    include "zelda_scene/Yousei_izumi_yoko/Yousei_izumi_yoko_demo00.o"
    include "zelda_scene/Yousei_izumi_yoko/Yousei_izumi_yoko_demo01.o"
    include "zelda_scene/Yousei_izumi_yoko/Yousei_izumi_yoko_demo02.o"
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_bgc.o"
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_main_info_d0.o"
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_main_info_d1.o"
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_main_info_d2.o"
    include "zelda_scene/Yousei_izumi_yoko/yousei_izumi_yoko_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "yousei_izumi_yoko_room_0"
    romalign 0x1000
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_0_info.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_0_info_d0.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_0_info_d1.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_0_info_d2.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_0_shape.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_grp_g2.o"
    include "zelda_scene/Yousei_izumi_yoko/ROOM0/yousei_izumi_yoko_grp_g2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "golon_scene"
    romalign 0x1000
    include "zelda_scene/Shop_golon/golon_main_info.o"
    include "zelda_scene/Shop_golon/golon_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "golon_room_0"
    romalign 0x1000
    include "zelda_scene/Shop_golon/ROOM0/golon_0_info.o"
    include "zelda_scene/Shop_golon/ROOM0/golon.o"
    include "zelda_scene/Shop_golon/ROOM0/golon_0_shape.o"
    include "zelda_scene/Shop_golon/ROOM0/golon_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "zoora_scene"
    romalign 0x1000
    include "zelda_scene/Zoora/zoora_main_info.o"
    include "zelda_scene/Zoora/zoora_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "zoora_room_0"
    romalign 0x1000
    include "zelda_scene/Zoora/ROOM0/zoora_0_info.o"
    include "zelda_scene/Zoora/ROOM0/zoora.o"
    include "zelda_scene/Zoora/ROOM0/zoora_0_shape.o"
    include "zelda_scene/Zoora/ROOM0/zoora_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "drag_scene"
    romalign 0x1000
    include "zelda_scene/Shop_drag/drag_main_info.o"
    include "zelda_scene/Shop_drag/drag_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "drag_room_0"
    romalign 0x1000
    include "zelda_scene/Shop_drag/ROOM0/drag_0_info.o"
    include "zelda_scene/Shop_drag/ROOM0/drag.o"
    include "zelda_scene/Shop_drag/ROOM0/drag1_txt.o"
    include "zelda_scene/Shop_drag/ROOM0/drag_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "alley_shop_scene"
    romalign 0x1000
    include "zelda_scene/Shop_alley/alley_shop_main_info.o"
    include "zelda_scene/Shop_alley/alley_shop_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "alley_shop_room_0"
    romalign 0x1000
    include "zelda_scene/Shop_alley/ROOM0/alley_shop_0_info.o"
    include "zelda_scene/Shop_alley/ROOM0/alley_shop.o"
    include "zelda_scene/Shop_alley/ROOM0/alley_shop1_txt.o"
    include "zelda_scene/Shop_alley/ROOM0/alley_shop_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "night_shop_scene"
    romalign 0x1000
    include "zelda_scene/Shop_night/night_shop_main_info.o"
    include "zelda_scene/Shop_night/night_shop_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "night_shop_room_0"
    romalign 0x1000
    include "zelda_scene/Shop_night/ROOM0/night_shop_0_info.o"
    include "zelda_scene/Shop_night/ROOM0/night_shop.o"
    include "zelda_scene/Shop_night/ROOM0/night_shop1_txt.o"
    include "zelda_scene/Shop_night/ROOM0/night_shop_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "impa_scene"
    romalign 0x1000
    include "zelda_scene/Kakariko_impa/impa_main_info.o"
    include "zelda_scene/Kakariko_impa/impa_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "impa_room_0"
    romalign 0x1000
    include "zelda_scene/Kakariko_impa/ROOM0/impa_0_info.o"
    include "zelda_scene/Kakariko_impa/ROOM0/impa.o"
    include "zelda_scene/Kakariko_impa/ROOM0/impa_0_shape.o"
    include "zelda_scene/Kakariko_impa/ROOM0/impa_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "labo_scene"
    romalign 0x1000
    include "zelda_scene/Labo/labo_main_info.o"
    include "zelda_scene/Labo/labo_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "labo_room_0"
    romalign 0x1000
    include "zelda_scene/Labo/ROOM0/labo_0_info.o"
    include "zelda_scene/Labo/ROOM0/labo.o"
    include "zelda_scene/Labo/ROOM0/labo_0_shape.o"
    include "zelda_scene/Labo/ROOM0/labo_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "tent_scene"
    romalign 0x1000
    include "zelda_scene/Tent/tent_main_info.o"
    include "zelda_scene/Tent/tent_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "tent_room_0"
    romalign 0x1000
    include "zelda_scene/Tent/ROOM0/tent_0_info.o"
    include "zelda_scene/Tent/ROOM0/tent.o"
    include "zelda_scene/Tent/ROOM0/tent_0_shape.o"
    include "zelda_scene/Tent/ROOM0/tent_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "nakaniwa_scene"
    romalign 0x1000
    include "zelda_scene/Nakaniwa/nakaniwa_main_info.o"
    include "zelda_tool_scene/Nakaniwa/nakaniwa_demo00.o"
    include "zelda_tool_scene/Nakaniwa/nakaniwa_demo01.o"
    include "zelda_tool_scene/Nakaniwa/nakaniwa_demo02.o"
    include "zelda_scene/Nakaniwa/nakaniwa_bgc.o"
    include "zelda_scene/Nakaniwa/nakaniwa_main_info_d0.o"
    include "zelda_scene/Nakaniwa/nakaniwa_main_info_d1.o"
    include "zelda_scene/Nakaniwa/nakaniwa_main_info_d2.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "nakaniwa_room_0"
    romalign 0x1000
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_0_info.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_0_info_d0.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_0_info_d1.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_0_info_d2.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_0_shape.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_grp_nakaniwa.o"
    include "zelda_scene/Nakaniwa/ROOM0/nakaniwa_grp_nakaniwaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_boss_scene"
    romalign 0x1000
    include "zelda_scene/Ddan_boss/ddan_boss_main_info.o"
    include "zelda_scene/Ddan_boss/ddan_boss_bgc.o"
    include "zelda_scene/Ddan_boss/ddan_boss_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ddan_boss_room_0"
    romalign 0x1000
    include "zelda_scene/Ddan_boss/ROOM0/ddan_boss_0_info.o"
    include "zelda_scene/Ddan_boss/ROOM0/ddan_boss_0_shape.o"
    include "zelda_scene/Ddan_boss/ROOM0/ddan_boss_grp_dda07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ddan_boss_room_1"
    romalign 0x1000
    include "zelda_scene/Ddan_boss/ROOM1/ddan_boss_1_info.o"
    include "zelda_scene/Ddan_boss/ROOM1/ddan_boss_1_shape.o"
    include "zelda_scene/Ddan_boss/ROOM1/ddan_boss_grp_ddaff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_boss_scene"
    romalign 0x1000
    include "zelda_scene/Ydan_boss/ydan_boss_main_info.o"
    include "zelda_scene/Ydan_boss/ydan_boss_bgc.o"
    include "zelda_scene/Ydan_boss/ydan_boss_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ydan_boss_room_0"
    romalign 0x1000
    include "zelda_scene/Ydan_boss/ROOM0/ydan_boss_0_info.o"
    include "zelda_scene/Ydan_boss/ROOM0/ydan_boss_0_shape.o"
    include "zelda_scene/Ydan_boss/ROOM0/ydan_boss_grp_yda09_00.o"
    include "zelda_scene/Ydan_boss/ROOM0/ydan_boss_grp_yda09_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ydan_boss_room_1"
    romalign 0x1000
    include "zelda_scene/Ydan_boss/ROOM1/ydan_boss_1_info.o"
    include "zelda_scene/Ydan_boss/ROOM1/ydan_boss_1_shape.o"
    include "zelda_scene/Ydan_boss/ROOM1/ydan_boss_grp_ydaff_00.o"
    include "zelda_scene/Ydan_boss/ROOM1/ydan_boss_grp_ydaff_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_bs_scene"
    romalign 0x1000
    include "zelda_scene/Hakadan_boss/HAKAdan_bs_main_info.o"
    include "zelda_scene/Hakadan_boss/HAKAdan_bs_bgc.o"
    include "zelda_scene/Hakadan_boss/HAKAdan_bs_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "HAKAdan_bs_room_0"
    romalign 0x1000
    include "zelda_scene/Hakadan_boss/ROOM0/HAKAdan_bs_0_info.o"
    include "zelda_scene/Hakadan_boss/ROOM0/HAKAdan_bs_0_shape.o"
    include "zelda_scene/Hakadan_boss/ROOM0/HAKAdan_bs_grp_Haka19.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "HAKAdan_bs_room_1"
    romalign 0x1000
    include "zelda_scene/Hakadan_boss/ROOM1/HAKAdan_bs_1_info.o"
    include "zelda_scene/Hakadan_boss/ROOM1/HAKAdan_bs_1_shape.o"
    include "zelda_scene/Hakadan_boss/ROOM1/HAKAdan_bs_grp_Hakaff.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_bs_scene"
    romalign 0x1000
    include "zelda_scene/Mizusin_boss/MIZUsin_bs_main_info.o"
    include "zelda_scene/Mizusin_boss/MIZUsin_bs_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "MIZUsin_bs_room_0"
    romalign 0x1000
    include "zelda_scene/Mizusin_boss/ROOM0/MIZUsin_bs_0_info.o"
    include "zelda_scene/Mizusin_boss/ROOM0/MIZUsin_bs_0_shape.o"
    include "zelda_scene/Mizusin_boss/ROOM0/MIZUsin_bs_grp_Wa0b.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "MIZUsin_bs_room_1"
    romalign 0x1000
    include "zelda_scene/Mizusin_boss/ROOM1/MIZUsin_bs_1_info.o"
    include "zelda_scene/Mizusin_boss/ROOM1/MIZUsin_bs_1_shape.o"
    include "zelda_scene/Mizusin_boss/ROOM1/MIZUsin_bs_grp_Wa0c.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_scene"
    romalign 0x1000
    include "zelda_scene/Ganon/ganon_main_info.o"
    include "zelda_scene/Ganon/ganon_bgc.o"
    include "zelda_scene/Ganon/ganon_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM0/ganon_0_info.o"
    include "zelda_scene/Ganon/ROOM0/ganon_0_shape.o"
    include "zelda_scene/Ganon/ROOM0/ganon_grp_gan00_00.o"
    include "zelda_scene/Ganon/ROOM0/ganon_grp_gan00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_1"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM1/ganon_1_info.o"
    include "zelda_scene/Ganon/ROOM1/ganon_1_shape.o"
    include "zelda_scene/Ganon/ROOM1/ganon_grp_gan01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_2"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM2/ganon_2_info.o"
    include "zelda_scene/Ganon/ROOM2/ganon_2_shape.o"
    include "zelda_scene/Ganon/ROOM2/ganon_grp_gan02_00.o"
    include "zelda_scene/Ganon/ROOM2/ganon_grp_gan02_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_3"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM3/ganon_3_info.o"
    include "zelda_scene/Ganon/ROOM3/ganon_3_shape.o"
    include "zelda_scene/Ganon/ROOM3/ganon_grp_gan03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_4"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM4/ganon_4_info.o"
    include "zelda_scene/Ganon/ROOM4/ganon_4_shape.o"
    include "zelda_scene/Ganon/ROOM4/ganon_grp_gan04_00.o"
    include "zelda_scene/Ganon/ROOM4/ganon_grp_gan04_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_5"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM5/ganon_5_info.o"
    include "zelda_scene/Ganon/ROOM5/ganon_5_shape.o"
    include "zelda_scene/Ganon/ROOM5/ganon_grp_gan05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_6"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM6/ganon_6_info.o"
    include "zelda_scene/Ganon/ROOM6/ganon_6_shape.o"
    include "zelda_scene/Ganon/ROOM6/ganon_grp_gan07_00.o"
    include "zelda_scene/Ganon/ROOM6/ganon_grp_gan07_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_7"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM7/ganon_7_info.o"
    include "zelda_scene/Ganon/ROOM7/ganon_7_shape.o"
    include "zelda_scene/Ganon/ROOM7/ganon_grp_gan08_00.o"
    include "zelda_scene/Ganon/ROOM7/ganon_grp_gan08_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_8"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM8/ganon_8_info.o"
    include "zelda_scene/Ganon/ROOM8/ganon_8_shape.o"
    include "zelda_scene/Ganon/ROOM8/ganon_grp_ganfe_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_room_9"
    romalign 0x1000
    include "zelda_scene/Ganon/ROOM9/ganon_9_info.o"
    include "zelda_scene/Ganon/ROOM9/ganon_9_shape.o"
    include "zelda_scene/Ganon/ROOM9/ganon_grp_ganff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_boss_scene"
    romalign 0x1000
    include "zelda_scene/Ganon_boss/ganon_boss_main_info.o"
    include "zelda_scene/Ganon_boss/ganon_boss_bgc.o"
    include "zelda_scene/Ganon_boss/ganon_boss_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_boss_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon_boss/ROOM0/ganon_boss_0_info.o"
    include "zelda_scene/Ganon_boss/ROOM0/ganon_boss_0_shape.o"
    include "zelda_scene/Ganon_boss/ROOM0/ganon_boss_grp_ganff_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinboss_scene"
    romalign 0x1000
    include "zelda_scene/Jyasinzou_boss/jyasinboss_main_info.o"
    include "zelda_tool_scene/Jyasinzou_boss/jyasinboss_demo00.o"
    include "zelda_tool_scene/Jyasinzou_boss/jyasinboss_demo01.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_bgc.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_demo02.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_main_info_d0.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_main_info_d1.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_main_info_d2.o"
    include "zelda_scene/Jyasinzou_boss/jyasinboss_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "jyasinboss_room_0"
    romalign 0x1000
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_0_info.o"
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_0_info_d0.o"
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_0_info_d1.o"
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_0_info_d2.o"
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_0_shape.o"
    include "zelda_scene/Jyasinzou_boss/ROOM0/jyasinboss_grp_jyb00_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinboss_room_1"
    romalign 0x1000
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_1_info.o"
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_1_info_d0.o"
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_1_info_d1.o"
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_1_info_d2.o"
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_1_shape.o"
    include "zelda_scene/Jyasinzou_boss/ROOM1/jyasinboss_grp_jyb01_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinboss_room_2"
    romalign 0x1000
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_2_info.o"
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_2_info_d0.o"
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_2_info_d1.o"
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_2_info_d2.o"
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_2_shape.o"
    include "zelda_scene/Jyasinzou_boss/ROOM2/jyasinboss_grp_jyb02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "jyasinboss_room_3"
    romalign 0x1000
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_3_info.o"
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_3_info_d0.o"
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_3_info_d1.o"
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_3_info_d2.o"
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_3_shape.o"
    include "zelda_scene/Jyasinzou_boss/ROOM3/jyasinboss_grp_jyb03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kokiri_home4_scene"
    romalign 0x1000
    include "build/scenes/indoors/kokiri_home4/kokiri_home4_scene.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kokiri_home4_room_0"
    romalign 0x1000
    include "build/scenes/indoors/kokiri_home4/kokiri_home4_room_0.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kokiri_home5_scene"
    romalign 0x1000
    include "zelda_scene/K_Home5/kokiri_home5_main_info.o"
    include "zelda_scene/K_Home5/kokiri_home5_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kokiri_home5_room_0"
    romalign 0x1000
    include "zelda_scene/K_Home5/ROOM0/kokiri_home5_0_info.o"
    include "zelda_scene/K_Home5/ROOM0/kokiri5_txt.o"
    include "zelda_scene/K_Home5/ROOM0/kokiri_home5.o"
    include "zelda_scene/K_Home5/ROOM0/kokiri_home5_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_final_scene"
    romalign 0x1000
    include "zelda_scene/Ganon_final/ganon_final_main_info.o"
    include "zelda_scene/Ganon_final/ganon_final_bgc.o"
    include "zelda_scene/Ganon_final/ganon_final_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_final_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon_final/ROOM0/ganon_final_0_info.o"
    include "zelda_scene/Ganon_final/ROOM0/ganon_final_0_shape.o"
    include "zelda_scene/Ganon_final/ROOM0/ganon_final_grp_spot15.o"
    include "zelda_scene/Ganon_final/ROOM0/ganon_final_grp_spot15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kakariko3_scene"
    romalign 0x1000
    include "zelda_scene/Kakariko_home3/kakariko3_main_info.o"
    include "zelda_scene/Kakariko_home3/kakariko3_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kakariko3_room_0"
    romalign 0x1000
    include "zelda_scene/Kakariko_home3/ROOM0/kakariko3_0_info.o"
    include "zelda_scene/Kakariko_home3/ROOM0/kakariko3.o"
    include "zelda_scene/Kakariko_home3/ROOM0/kakariko3_0_shape.o"
    include "zelda_scene/Kakariko_home3/ROOM0/kakariko3_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hairal_niwa2_scene"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa2/hairal_niwa2_main_info.o"
    include "zelda_scene/Hairal_niwa2/hairal_niwa2_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hairal_niwa2_room_0"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa2/ROOM0/hairal_niwa2_0_info.o"
    include "zelda_scene/Hairal_niwa2/ROOM0/hairal_niwa2_0_shape.o"
    include "zelda_scene/Hairal_niwa2/ROOM0/hairal_niwa2_grp_niwa.o"
    include "zelda_scene/Hairal_niwa2/ROOM0/hairal_niwa2_grp_niwaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_scene"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/hakasitarelay_main_info.o"
    include "zelda_tool_scene/Hakasitarelay/okarina_storm.o"
    include "zelda_scene/Hakasitarelay/hakasitarelay_bgc.o"
    include "zelda_scene/Hakasitarelay/hakasitarelay_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hakasitarelay_room_0"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM0/hakasitarelay_0_info.o"
    include "zelda_scene/Hakasitarelay/ROOM0/hakasitarelay_0_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM0/hakasitarelay_grp_rel01_00.o"
    include "zelda_scene/Hakasitarelay/ROOM0/hakasitarelay_grp_rel01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_1"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM1/hakasitarelay_1_info.o"
    include "zelda_scene/Hakasitarelay/ROOM1/hakasitarelay_1_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM1/hakasitarelay_grp_rel02_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_2"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM2/hakasitarelay_2_info.o"
    include "zelda_scene/Hakasitarelay/ROOM2/hakasitarelay_2_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM2/hakasitarelay_grp_rel03_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_3"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM3/hakasitarelay_3_info.o"
    include "zelda_scene/Hakasitarelay/ROOM3/hakasitarelay_3_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM3/hakasitarelay_grp_rel04_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_4"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM4/hakasitarelay_4_info.o"
    include "zelda_scene/Hakasitarelay/ROOM4/hakasitarelay_4_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM4/hakasitarelay_grp_rel05_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_5"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM5/hakasitarelay_5_info.o"
    include "zelda_scene/Hakasitarelay/ROOM5/hakasitarelay_5_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM5/hakasitarelay_grp_rel06_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakasitarelay_room_6"
    romalign 0x1000
    include "zelda_scene/Hakasitarelay/ROOM6/hakasitarelay_6_info.o"
    include "zelda_scene/Hakasitarelay/ROOM6/hakasitarelay_6_shape.o"
    include "zelda_scene/Hakasitarelay/ROOM6/hakasitarelay_grp_rel07_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "shrine_scene"
    romalign 0x1000
    include "zelda_scene/Shrine/shrine_main_info.o"
    include "zelda_scene/Shrine/shrine_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "shrine_room_0"
    romalign 0x1000
    include "zelda_scene/Shrine/ROOM0/shrine_0_info.o"
    include "zelda_scene/Shrine/ROOM0/shrine.o"
    include "zelda_scene/Shrine/ROOM0/shrine1_txt.o"
    include "zelda_scene/Shrine/ROOM0/shrine2_txt.o"
    include "zelda_scene/Shrine/ROOM0/shrine_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "turibori_scene"
    romalign 0x1000
    include "zelda_scene/Turibori/turibori_main_info.o"
    include "zelda_scene/Turibori/turibori_bgc.o"
    include "zelda_scene/Turibori/turibori_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "turibori_room_0"
    romalign 0x1000
    include "zelda_scene/Turibori/ROOM0/turibori_0_info.o"
    include "zelda_scene/Turibori/ROOM0/turibori_0_shape.o"
    include "zelda_scene/Turibori/ROOM0/turibori_grp_g76.o"
    include "zelda_scene/Turibori/ROOM0/turibori_grp_g76T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "shrine_n_scene"
    romalign 0x1000
    include "zelda_scene/Shrine_n/shrine_n_main_info.o"
    include "zelda_scene/Shrine_n/shrine_n_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "shrine_n_room_0"
    romalign 0x1000
    include "zelda_scene/Shrine_n/ROOM0/shrine_n_0_info.o"
    include "zelda_scene/Shrine_n/ROOM0/shrine_n.o"
    include "zelda_scene/Shrine_n/ROOM0/shrine_n1_txt.o"
    include "zelda_scene/Shrine_n/ROOM0/shrine_n2_txt.o"
    include "zelda_scene/Shrine_n/ROOM0/shrine_n_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "shrine_r_scene"
    romalign 0x1000
    include "zelda_scene/Shrine_r/shrine_r_main_info.o"
    include "zelda_scene/Shrine_r/shrine_r_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "shrine_r_room_0"
    romalign 0x1000
    include "zelda_scene/Shrine_r/ROOM0/shrine_r_0_info.o"
    include "zelda_scene/Shrine_r/ROOM0/shrine_r.o"
    include "zelda_scene/Shrine_r/ROOM0/shrine_r1_txt.o"
    include "zelda_scene/Shrine_r/ROOM0/shrine_r2_txt.o"
    include "zelda_scene/Shrine_r/ROOM0/shrine_r_0_shape.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakaana2_scene"
    romalign 0x1000
    include "zelda_scene/Hakaana2/hakaana2_main_info.o"
    include "zelda_scene/Hakaana2/hakaana2_bgc.o"
    include "zelda_scene/Hakaana2/hakaana2_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hakaana2_room_0"
    romalign 0x1000
    include "zelda_scene/Hakaana2/ROOM0/hakaana2_0_info.o"
    include "zelda_scene/Hakaana2/ROOM0/hakaana2_0_shape.o"
    include "zelda_scene/Hakaana2/ROOM0/hakaana2_grp_g2.o"
    include "zelda_scene/Hakaana2/ROOM0/hakaana2_grp_g2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_scene"
    romalign 0x1000
    include "zelda_scene/Gerudoway/gerudoway_main_info.o"
    include "zelda_scene/Gerudoway/gerudoway_bgc.o"
    include "zelda_scene/Gerudoway/gerudoway_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "gerudoway_room_0"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM0/gerudoway_0_info.o"
    include "zelda_scene/Gerudoway/ROOM0/gerudoway_0_shape.o"
    include "zelda_scene/Gerudoway/ROOM0/gerudoway_grp_way_11_12.o"
    include "zelda_scene/Gerudoway/ROOM0/gerudoway_grp_way_11_12T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_room_1"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM1/gerudoway_1_info.o"
    include "zelda_scene/Gerudoway/ROOM1/gerudoway_1_shape.o"
    include "zelda_scene/Gerudoway/ROOM1/gerudoway_grp_way_13.o"
    include "zelda_scene/Gerudoway/ROOM1/gerudoway_grp_way_13T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_room_2"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM2/gerudoway_2_info.o"
    include "zelda_scene/Gerudoway/ROOM2/gerudoway_2_shape.o"
    include "zelda_scene/Gerudoway/ROOM2/gerudoway_grp_way_1_2.o"
    include "zelda_scene/Gerudoway/ROOM2/gerudoway_grp_way_1_2T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_room_3"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM3/gerudoway_3_info.o"
    include "zelda_scene/Gerudoway/ROOM3/gerudoway_3_shape.o"
    include "zelda_scene/Gerudoway/ROOM3/gerudoway_grp_way_34910.o"
    include "zelda_scene/Gerudoway/ROOM3/gerudoway_grp_way_34910T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_room_4"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM4/gerudoway_4_info.o"
    include "zelda_scene/Gerudoway/ROOM4/gerudoway_4_shape.o"
    include "zelda_scene/Gerudoway/ROOM4/gerudoway_grp_way_5_6.o"
    include "zelda_scene/Gerudoway/ROOM4/gerudoway_grp_way_5_6T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "gerudoway_room_5"
    romalign 0x1000
    include "zelda_scene/Gerudoway/ROOM5/gerudoway_5_info.o"
    include "zelda_scene/Gerudoway/ROOM5/gerudoway_5_shape.o"
    include "zelda_scene/Gerudoway/ROOM5/gerudoway_grp_way_7_8.o"
    include "zelda_scene/Gerudoway/ROOM5/gerudoway_grp_way_7_8T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hairal_niwa_n_scene"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa_n/hairal_niwa_n_main_info.o"
    include "zelda_scene/Hairal_niwa_n/hairal_niwa_n_bgc.o"
    include "zelda_scene/Hairal_niwa_n/hairal_niwa_n_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hairal_niwa_n_room_0"
    romalign 0x1000
    include "zelda_scene/Hairal_niwa_n/ROOM0/hairal_niwa_n_0_info.o"
    include "zelda_scene/Hairal_niwa_n/ROOM0/hairal_niwa_n_0_shape.o"
    include "zelda_scene/Hairal_niwa_n/ROOM0/hairal_niwa_n_grp_niwa.o"
    include "zelda_scene/Hairal_niwa_n/ROOM0/hairal_niwa_n_grp_niwaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bowling_scene"
    romalign 0x1000
    include "zelda_scene/Bowling/bowling_main_info.o"
    include "zelda_scene/Bowling/bowling_bgc.o"
    include "zelda_scene/Bowling/bowling_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "bowling_room_0"
    romalign 0x1000
    include "zelda_scene/Bowling/ROOM0/bowling_0_info.o"
    include "zelda_scene/Bowling/ROOM0/bowling_0_shape.o"
    include "zelda_scene/Bowling/ROOM0/bowling_grp_g1.o"
    include "zelda_scene/Bowling/ROOM0/bowling_grp_g1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakaana_ouke_scene"
    romalign 0x1000
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_main_info.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_bgc.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_main_info_d0.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_main_info_d1.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_sunA.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_sunB.o"
    include "zelda_scene/Hakaana_ouke/hakaana_ouke_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hakaana_ouke_room_0"
    romalign 0x1000
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_0_info.o"
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_0_info_d0.o"
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_0_info_d1.o"
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_0_shape.o"
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_grp_ouk00.o"
    include "zelda_scene/Hakaana_ouke/ROOM0/hakaana_ouke_grp_ouk00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakaana_ouke_room_1"
    romalign 0x1000
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_1_info.o"
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_1_info_d0.o"
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_1_info_d1.o"
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_1_shape.o"
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_grp_ouk01.o"
    include "zelda_scene/Hakaana_ouke/ROOM1/hakaana_ouke_grp_ouk01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hakaana_ouke_room_2"
    romalign 0x1000
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_2_info.o"
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_2_info_d0.o"
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_2_info_d1.o"
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_2_shape.o"
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_grp_ouk02.o"
    include "zelda_scene/Hakaana_ouke/ROOM2/hakaana_ouke_grp_ouk02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "hylia_labo_scene"
    romalign 0x1000
    include "zelda_scene/Hylia_labo/hylia_labo_main_info.o"
    include "zelda_scene/Hylia_labo/hylia_labo_bgc.o"
    include "zelda_scene/Hylia_labo/hylia_labo_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "hylia_labo_room_0"
    romalign 0x1000
    include "zelda_scene/Hylia_labo/ROOM0/hylia_labo_0_info.o"
    include "zelda_scene/Hylia_labo/ROOM0/hylia_labo_0_shape.o"
    include "zelda_scene/Hylia_labo/ROOM0/hylia_labo_grp_g76.o"
    include "zelda_scene/Hylia_labo/ROOM0/hylia_labo_grp_g76T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "souko_scene"
    romalign 0x1000
    include "zelda_scene/Souko/souko_main_info.o"
    include "zelda_scene/Souko/souko_bgc.o"
    include "zelda_scene/Souko/souko_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "souko_room_0"
    romalign 0x1000
    include "zelda_scene/Souko/ROOM0/souko_0_info.o"
    include "zelda_scene/Souko/ROOM0/souko_0_shape.o"
    include "zelda_scene/Souko/ROOM0/souko_grp_sou_00.o"
    include "zelda_scene/Souko/ROOM0/souko_grp_sou_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "souko_room_1"
    romalign 0x1000
    include "zelda_scene/Souko/ROOM1/souko_1_info.o"
    include "zelda_scene/Souko/ROOM1/souko_1_shape.o"
    include "zelda_scene/Souko/ROOM1/souko_grp_sou_01.o"
    include "zelda_scene/Souko/ROOM1/souko_grp_sou_01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "souko_room_2"
    romalign 0x1000
    include "zelda_scene/Souko/ROOM2/souko_2_info.o"
    include "zelda_scene/Souko/ROOM2/souko_2_shape.o"
    include "zelda_scene/Souko/ROOM2/souko_grp_sou_02.o"
    include "zelda_scene/Souko/ROOM2/souko_grp_sou_02T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "miharigoya_scene"
    romalign 0x1000
    include "zelda_scene/Miharigoya/miharigoya_main_info.o"
    include "zelda_scene/Miharigoya/miharigoya_bgc.o"
    include "zelda_scene/Miharigoya/miharigoya_main_info_ad.o"
    include "zelda_scene/Miharigoya/miharigoya_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "miharigoya_room_0"
    romalign 0x1000
    include "zelda_scene/Miharigoya/ROOM0/miharigoya_0_info.o"
    include "zelda_scene/Miharigoya/ROOM0/miharigoya_0_info_ad.o"
    include "zelda_scene/Miharigoya/ROOM0/miharigoya_0_shape.o"
    include "zelda_scene/Miharigoya/ROOM0/miharigoya_grp_g76.o"
    include "zelda_scene/Miharigoya/ROOM0/miharigoya_grp_g76T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "mahouya_scene"
    romalign 0x1000
    include "zelda_scene/Mahouya/mahouya_main_info.o"
    include "zelda_scene/Mahouya/mahouya_bgc.o"
    include "zelda_scene/Mahouya/mahouya_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "mahouya_room_0"
    romalign 0x1000
    include "zelda_scene/Mahouya/ROOM0/mahouya_0_info.o"
    include "zelda_scene/Mahouya/ROOM0/mahouya_0_shape.o"
    include "zelda_scene/Mahouya/ROOM0/mahouya_grp_g1.o"
    include "zelda_scene/Mahouya/ROOM0/mahouya_grp_g1T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_scene"
    romalign 0x1000
    include "zelda_scene/Takaraya/takaraya_main_info.o"
    include "zelda_scene/Takaraya/takaraya_bgc.o"
    include "zelda_scene/Takaraya/takaraya_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "takaraya_room_0"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM0/takaraya_0_info.o"
    include "zelda_scene/Takaraya/ROOM0/takaraya_0_shape.o"
    include "zelda_scene/Takaraya/ROOM0/takaraya_grp_tak00.o"
    include "zelda_scene/Takaraya/ROOM0/takaraya_grp_tak00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_1"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM1/takaraya_1_info.o"
    include "zelda_scene/Takaraya/ROOM1/takaraya_1_shape.o"
    include "zelda_scene/Takaraya/ROOM1/takaraya_grp_tak01.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_2"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM2/takaraya_2_info.o"
    include "zelda_scene/Takaraya/ROOM2/takaraya_2_shape.o"
    include "zelda_scene/Takaraya/ROOM2/takaraya_grp_tak02.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_3"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM3/takaraya_3_info.o"
    include "zelda_scene/Takaraya/ROOM3/takaraya_3_shape.o"
    include "zelda_scene/Takaraya/ROOM3/takaraya_grp_tak03.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_4"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM4/takaraya_4_info.o"
    include "zelda_scene/Takaraya/ROOM4/takaraya_4_shape.o"
    include "zelda_scene/Takaraya/ROOM4/takaraya_grp_tak04.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_5"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM5/takaraya_5_info.o"
    include "zelda_scene/Takaraya/ROOM5/takaraya_5_shape.o"
    include "zelda_scene/Takaraya/ROOM5/takaraya_grp_tak05.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "takaraya_room_6"
    romalign 0x1000
    include "zelda_scene/Takaraya/ROOM6/takaraya_6_info.o"
    include "zelda_scene/Takaraya/ROOM6/takaraya_6_shape.o"
    include "zelda_scene/Takaraya/ROOM6/takaraya_grp_tak06.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_sonogo_scene"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ganon_sonogo_main_info.o"
    include "zelda_scene/Ganon_sonogo/ganon_sonogo_bgc.o"
    include "zelda_scene/Ganon_sonogo/ganon_sonogo_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_sonogo_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ROOM0/ganon_sonogo_0_info.o"
    include "zelda_scene/Ganon_sonogo/ROOM0/ganon_sonogo_0_shape.o"
    include "zelda_scene/Ganon_sonogo/ROOM0/ganon_sonogo_grp_gas00_00.o"
    include "zelda_scene/Ganon_sonogo/ROOM0/ganon_sonogo_grp_gas00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_sonogo_room_1"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ROOM1/ganon_sonogo_1_info.o"
    include "zelda_scene/Ganon_sonogo/ROOM1/ganon_sonogo_1_shape.o"
    include "zelda_scene/Ganon_sonogo/ROOM1/ganon_sonogo_grp_gas02_00.o"
    include "zelda_scene/Ganon_sonogo/ROOM1/ganon_sonogo_grp_gas02_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_sonogo_room_2"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ROOM2/ganon_sonogo_2_info.o"
    include "zelda_scene/Ganon_sonogo/ROOM2/ganon_sonogo_2_shape.o"
    include "zelda_scene/Ganon_sonogo/ROOM2/ganon_sonogo_grp_gas04_00.o"
    include "zelda_scene/Ganon_sonogo/ROOM2/ganon_sonogo_grp_gas04_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_sonogo_room_3"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ROOM3/ganon_sonogo_3_info.o"
    include "zelda_scene/Ganon_sonogo/ROOM3/ganon_sonogo_3_shape.o"
    include "zelda_scene/Ganon_sonogo/ROOM3/ganon_sonogo_grp_gas0f_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_sonogo_room_4"
    romalign 0x1000
    include "zelda_scene/Ganon_sonogo/ROOM4/ganon_sonogo_4_info.o"
    include "zelda_scene/Ganon_sonogo/ROOM4/ganon_sonogo_4_shape.o"
    include "zelda_scene/Ganon_sonogo/ROOM4/ganon_sonogo_grp_gasfe_00.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganon_demo_scene"
    romalign 0x1000
    include "zelda_scene/Ganon_demo/ganon_demo_main_info.o"
    include "zelda_scene/Ganon_demo/ganon_demo00.o"
    include "zelda_scene/Ganon_demo/ganon_demo_bgc.o"
    include "zelda_scene/Ganon_demo/ganon_demo_main_info_d0.o"
    include "zelda_scene/Ganon_demo/ganon_demo_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganon_demo_room_0"
    romalign 0x1000
    include "zelda_scene/Ganon_demo/ROOM0/ganon_demo_0_info.o"
    include "zelda_scene/Ganon_demo/ROOM0/ganon_demo_0_info_d0.o"
    include "zelda_scene/Ganon_demo/ROOM0/ganon_demo_0_shape.o"
    include "zelda_scene/Ganon_demo/ROOM0/ganon_demo_grp_spot15.o"
    include "zelda_scene/Ganon_demo/ROOM0/ganon_demo_grp_spot15T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "besitu_scene"
    romalign 0x1000
    include "zelda_scene/Besitu/besitu_main_info.o"
    include "zelda_scene/Besitu/besitu_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "besitu_room_0"
    romalign 0x1000
    include "zelda_scene/Besitu/ROOM0/besitu_0_info.o"
    include "zelda_scene/Besitu/ROOM0/besitu_0_shape.o"
    include "zelda_scene/Besitu/ROOM0/besitu_grp_g1.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "face_shop_scene"
    romalign 0x1000
    include "zelda_scene/Shop_face/face_shop_main_info.o"
    include "zelda_scene/Shop_face/face_shop_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "face_shop_room_0"
    romalign 0x1000
    include "zelda_scene/Shop_face/ROOM0/face_shop_0_info.o"
    include "zelda_scene/Shop_face/ROOM0/face_shop.o"
    include "zelda_scene/Shop_face/ROOM0/face_shop_0_shape.o"
    include "zelda_scene/Shop_face/ROOM0/face_shop_txt.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "kinsuta_scene"
    romalign 0x1000
    include "zelda_scene/Kinsuta/kinsuta_main_info.o"
    include "zelda_scene/Kinsuta/kinsuta_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "kinsuta_room_0"
    romalign 0x1000
    include "zelda_scene/Kinsuta/ROOM0/kinsuta_0_info.o"
    include "zelda_scene/Kinsuta/ROOM0/kinsuta_0_shape.o"
    include "zelda_scene/Kinsuta/ROOM0/kinsuta_grp_kinsuta.o"
    include "zelda_scene/Kinsuta/ROOM0/kinsuta_grp_kinsutaT.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontikasonogo_scene"
    romalign 0x1000
    include "zelda_scene/Ganontikasonogo/ganontikasonogo_main_info.o"
    include "zelda_scene/Ganontikasonogo/ganontikasonogo_bgc.o"
    include "zelda_scene/Ganontikasonogo/ganontikasonogo_texture.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "ganontikasonogo_room_0"
    romalign 0x1000
    include "zelda_scene/Ganontikasonogo/ROOM0/ganontikasonogo_0_info.o"
    include "zelda_scene/Ganontikasonogo/ROOM0/ganontikasonogo_0_shape.o"
    include "zelda_scene/Ganontikasonogo/ROOM0/ganontikasonogo_grp_gas00_00.o"
    include "zelda_scene/Ganontikasonogo/ROOM0/ganontikasonogo_grp_gas00_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "ganontikasonogo_room_1"
    romalign 0x1000
    include "zelda_scene/Ganontikasonogo/ROOM1/ganontikasonogo_1_info.o"
    include "zelda_scene/Ganontikasonogo/ROOM1/ganontikasonogo_1_shape.o"
    include "zelda_scene/Ganontikasonogo/ROOM1/ganontikasonogo_grp_gas01_00.o"
    include "zelda_scene/Ganontikasonogo/ROOM1/ganontikasonogo_grp_gas01_00T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "test01_scene"
    romalign 0x1000
    include "zelda_scene/Test01/test01_main_info.o"
    include "zelda_scene/Test01/test01_bgc.o"
    address SEGMENT_SCENE
endseg

beginseg
    name "test01_room_0"
    romalign 0x1000
    include "zelda_scene/Test01/ROOM0/test01_0_info.o"
    include "zelda_scene/Test01/ROOM0/test01_0_shape.o"
    include "zelda_scene/Test01/ROOM0/test01_grp_test01.o"
    include "zelda_scene/Test01/ROOM0/test01_grp_test01T.o"
    address SEGMENT_ROOM
endseg

beginseg
    name "bump_texture_static"
    romalign 0x1000
    include "build/baserom/bump_texture_static.o"
endseg

beginseg
    name "anime_model_1_static"
    romalign 0x1000
    include "build/baserom/anime_model_1_static.o"
endseg

beginseg
    name "anime_model_2_static"
    romalign 0x1000
    include "build/baserom/anime_model_2_static.o"
endseg

beginseg
    name "anime_model_3_static"
    romalign 0x1000
    include "build/baserom/anime_model_3_static.o"
endseg

beginseg
    name "anime_model_4_static"
    romalign 0x1000
    include "build/baserom/anime_model_4_static.o"
endseg

beginseg
    name "anime_model_5_static"
    romalign 0x1000
    include "build/baserom/anime_model_5_static.o"
endseg

beginseg
    name "anime_model_6_static"
    romalign 0x1000
    include "build/baserom/anime_model_6_static.o"
endseg

beginseg
    name "anime_texture_1_static"
    romalign 0x1000
    include "build/baserom/anime_texture_1_static.o"
endseg

beginseg
    name "anime_texture_2_static"
    romalign 0x1000
    include "build/baserom/anime_texture_2_static.o"
endseg

beginseg
    name "anime_texture_3_static"
    romalign 0x1000
    include "build/baserom/anime_texture_3_static.o"
endseg

beginseg
    name "anime_texture_4_static"
    romalign 0x1000
    include "build/baserom/anime_texture_4_static.o"
endseg

beginseg
    name "anime_texture_5_static"
    romalign 0x1000
    include "build/baserom/anime_texture_5_static.o"
endseg

beginseg
    name "anime_texture_6_static"
    romalign 0x1000
    include "build/baserom/anime_texture_6_static.o"
endseg

beginseg
    name "softsprite_matrix_static"
    romalign 0x1000
    include "build/baserom/softsprite_matrix_static.o"
endseg
