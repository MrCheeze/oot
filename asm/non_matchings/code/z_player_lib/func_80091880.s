glabel func_80091880
/* B08A20 80091880 AFA40000 */  sw    $a0, ($sp)
/* B08A24 80091884 8FA40014 */  lw    $a0, 0x14($sp)
/* B08A28 80091888 AFA60008 */  sw    $a2, 8($sp)
/* B08A2C 8009188C AFA7000C */  sw    $a3, 0xc($sp)
/* B08A30 80091890 908E0000 */  lbu   $t6, ($a0)
/* B08A34 80091894 3C028012 */  lui   $v0, %hi(D_801261F8-1)
/* B08A38 80091898 24080002 */  li    $t0, 2
/* B08A3C 8009189C 004E1021 */  addu  $v0, $v0, $t6
/* B08A40 800918A0 904261F7 */  lbu   $v0, %lo(D_801261F8-1)($v0)
/* B08A44 800918A4 00C03825 */  move  $a3, $a2
/* B08A48 800918A8 00001825 */  move  $v1, $zero
/* B08A4C 800918AC 15020009 */  bne   $t0, $v0, .L800918D4
/* B08A50 800918B0 24010010 */   li    $at, 16
/* B08A54 800918B4 3C0F8016 */  lui   $t7, %hi(gSaveContext+4) # $t7, 0x8016
/* B08A58 800918B8 8DEFE664 */  lw    $t7, %lo(gSaveContext+4)($t7)
/* B08A5C 800918BC 11E00005 */  beqz  $t7, .L800918D4
/* B08A60 800918C0 00000000 */   nop   
/* B08A64 800918C4 90980001 */  lbu   $t8, 1($a0)
/* B08A68 800918C8 15180002 */  bne   $t0, $t8, .L800918D4
/* B08A6C 800918CC 00000000 */   nop   
/* B08A70 800918D0 24020001 */  li    $v0, 1
.L800918D4:
/* B08A74 800918D4 14A1001C */  bne   $a1, $at, .L80091948
/* B08A78 800918D8 3C188012 */   lui   $t8, %hi(D_80125F40)
/* B08A7C 800918DC 0002C880 */  sll   $t9, $v0, 2
/* B08A80 800918E0 0322C821 */  addu  $t9, $t9, $v0
/* B08A84 800918E4 3C068012 */  lui   $a2, %hi(D_80125C98+1)
/* B08A88 800918E8 00D93021 */  addu  $a2, $a2, $t9
/* B08A8C 800918EC 90C65C99 */  lbu   $a2, %lo(D_80125C98+1)($a2)
/* B08A90 800918F0 3C018016 */  lui   $at, %hi(D_80160014) # $at, 0x8016
/* B08A94 800918F4 3C058016 */  lui   $a1, %hi(gSaveContext+4)
/* B08A98 800918F8 AC260014 */  sw    $a2, %lo(D_80160014)($at)
/* B08A9C 800918FC 24010004 */  li    $at, 4
/* B08AA0 80091900 14C1003C */  bne   $a2, $at, .L800919F4
/* B08AA4 80091904 8CA5E664 */   lw    $a1, %lo(gSaveContext+4)($a1)
/* B08AA8 80091908 3C0A8016 */  lui   $t2,  %hi(gSaveContext+0x36)
/* B08AAC 8009190C 954AE696 */  lhu   $t2, %lo(gSaveContext+0x36)($t2)
/* B08AB0 80091910 44802000 */  mtc1  $zero, $f4
/* B08AB4 80091914 3C014F80 */  li    $at, 0x4F800000 # 0.000000
/* B08AB8 80091918 448A3000 */  mtc1  $t2, $f6
/* B08ABC 8009191C 05410004 */  bgez  $t2, .L80091930
/* B08AC0 80091920 46803220 */   cvt.s.w $f8, $f6
/* B08AC4 80091924 44815000 */  mtc1  $at, $f10
/* B08AC8 80091928 00000000 */  nop   
/* B08ACC 8009192C 460A4200 */  add.s $f8, $f8, $f10
.L80091930:
/* B08AD0 80091930 4604403E */  c.le.s $f8, $f4
/* B08AD4 80091934 00000000 */  nop   
/* B08AD8 80091938 4502002F */  bc1fl .L800919F8
/* B08ADC 8009193C 00067880 */   sll   $t7, $a2, 2
/* B08AE0 80091940 1000002C */  b     .L800919F4
/* B08AE4 80091944 24030004 */   li    $v1, 4
.L80091948:
/* B08AE8 80091948 24090013 */  li    $t1, 19
/* B08AEC 8009194C 14A9000F */  bne   $a1, $t1, .L8009198C
/* B08AF0 80091950 24010014 */   li    $at, 20
/* B08AF4 80091954 00025880 */  sll   $t3, $v0, 2
/* B08AF8 80091958 01625821 */  addu  $t3, $t3, $v0
/* B08AFC 8009195C 3C068012 */  lui   $a2, %hi(D_80125C98+2)
/* B08B00 80091960 00CB3021 */  addu  $a2, $a2, $t3
/* B08B04 80091964 90C65C9A */  lbu   $a2, %lo(D_80125C98+2)($a2)
/* B08B08 80091968 3C018016 */  lui   $at, %hi(D_80160018) # $at, 0x8016
/* B08B0C 8009196C 3C058016 */  lui   $a1, %hi(gSaveContext+4) # $a1, 0x8016
/* B08B10 80091970 AC260018 */  sw    $a2, %lo(D_80160018)($at)
/* B08B14 80091974 2401000A */  li    $at, 10
/* B08B18 80091978 14C1001E */  bne   $a2, $at, .L800919F4
/* B08B1C 8009197C 8CA5E664 */   lw    $a1, %lo(gSaveContext+4)($a1)
/* B08B20 80091980 90830001 */  lbu   $v1, 1($a0)
/* B08B24 80091984 1000001B */  b     .L800919F4
/* B08B28 80091988 00031880 */   sll   $v1, $v1, 2
.L8009198C:
/* B08B2C 8009198C 14A1000E */  bne   $a1, $at, .L800919C8
/* B08B30 80091990 00026080 */   sll   $t4, $v0, 2
/* B08B34 80091994 01826021 */  addu  $t4, $t4, $v0
/* B08B38 80091998 3C068012 */  lui   $a2, %hi(D_80125C98+3)
/* B08B3C 8009199C 00CC3021 */  addu  $a2, $a2, $t4
/* B08B40 800919A0 90C65C9B */  lbu   $a2, %lo(D_80125C98+3)($a2)
/* B08B44 800919A4 3C058016 */  lui   $a1, %hi(gSaveContext+4) # $a1, 0x8016
/* B08B48 800919A8 24010012 */  li    $at, 18
/* B08B4C 800919AC 10C10003 */  beq   $a2, $at, .L800919BC
/* B08B50 800919B0 8CA5E664 */   lw    $a1, %lo(gSaveContext+4)($a1)
/* B08B54 800919B4 54C90010 */  bnel  $a2, $t1, .L800919F8
/* B08B58 800919B8 00067880 */   sll   $t7, $a2, 2
.L800919BC:
/* B08B5C 800919BC 90830001 */  lbu   $v1, 1($a0)
/* B08B60 800919C0 1000000C */  b     .L800919F4
/* B08B64 800919C4 00031880 */   sll   $v1, $v1, 2
.L800919C8:
/* B08B68 800919C8 14A80008 */  bne   $a1, $t0, .L800919EC
/* B08B6C 800919CC 00026880 */   sll   $t5, $v0, 2
/* B08B70 800919D0 01A26821 */  addu  $t5, $t5, $v0
/* B08B74 800919D4 3C068012 */  lui   $a2, %hi(D_80125C98+4)
/* B08B78 800919D8 00CD3021 */  addu  $a2, $a2, $t5
/* B08B7C 800919DC 3C058016 */  lui   $a1, %hi(gSaveContext+4) # $a1, 0x8016
/* B08B80 800919E0 8CA5E664 */  lw    $a1, %lo(gSaveContext+4)($a1)
/* B08B84 800919E4 10000003 */  b     .L800919F4
/* B08B88 800919E8 90C65C9C */   lbu   $a2, %lo(D_80125C98+4)($a2)
.L800919EC:
/* B08B8C 800919EC 03E00008 */  jr    $ra
/* B08B90 800919F0 00001025 */   move  $v0, $zero

.L800919F4:
/* B08B94 800919F4 00067880 */  sll   $t7, $a2, 2
.L800919F8:
/* B08B98 800919F8 030FC021 */  addu  $t8, $t8, $t7
/* B08B9C 800919FC 8F185F40 */  lw    $t8, %lo(D_80125F40)($t8)
/* B08BA0 80091A00 00057080 */  sll   $t6, $a1, 2
/* B08BA4 80091A04 0003C880 */  sll   $t9, $v1, 2
/* B08BA8 80091A08 01D82021 */  addu  $a0, $t6, $t8
/* B08BAC 80091A0C 00995021 */  addu  $t2, $a0, $t9
/* B08BB0 80091A10 8D4B0000 */  lw    $t3, ($t2)
/* B08BB4 80091A14 00001025 */  move  $v0, $zero
/* B08BB8 80091A18 ACEB0000 */  sw    $t3, ($a3)
/* B08BBC 80091A1C 03E00008 */  jr    $ra
/* B08BC0 80091A20 00000000 */   nop   

