.rdata
glabel D_8013DEC8
    .asciz "Register_Item_Pt(%d)=%d\n"
    .balign 4

glabel D_8013DEE4
    .asciz "Register_Item_Pt(%d)=%d\n"
    .balign 4

.text
glabel Inventory_SwapAgeEquipment
/* AFB880 800846E0 27BDFFD8 */  addiu $sp, $sp, -0x28
/* AFB884 800846E4 AFB20020 */  sw    $s2, 0x20($sp)
/* AFB888 800846E8 3C128016 */  lui   $s2, %hi(gSaveContext) # $s2, 0x8016
/* AFB88C 800846EC 2652E660 */  addiu $s2, %lo(gSaveContext) # addiu $s2, $s2, -0x19a0
/* AFB890 800846F0 8E4E0004 */  lw    $t6, 4($s2)
/* AFB894 800846F4 AFBF0024 */  sw    $ra, 0x24($sp)
/* AFB898 800846F8 AFB1001C */  sw    $s1, 0x1c($sp)
/* AFB89C 800846FC 11C00003 */  beqz  $t6, .L8008470C
/* AFB8A0 80084700 AFB00018 */   sw    $s0, 0x18($sp)
/* AFB8A4 80084704 10000002 */  b     .L80084710
/* AFB8A8 80084708 24020005 */   li    $v0, 5
.L8008470C:
/* AFB8AC 8008470C 24020011 */  li    $v0, 17
.L80084710:
/* AFB8B0 80084710 24010005 */  li    $at, 5
/* AFB8B4 80084714 14410051 */  bne   $v0, $at, .L8008485C
/* AFB8B8 80084718 00008825 */   move  $s1, $zero
/* AFB8BC 8008471C 00008825 */  move  $s1, $zero
/* AFB8C0 80084720 2402003B */  li    $v0, 59
.L80084724:
/* AFB8C4 80084724 12200005 */  beqz  $s1, .L8008473C
/* AFB8C8 80084728 02518021 */   addu  $s0, $s2, $s1
/* AFB8CC 8008472C 02518021 */  addu  $s0, $s2, $s1
/* AFB8D0 80084730 920F0068 */  lbu   $t7, 0x68($s0)
/* AFB8D4 80084734 10000002 */  b     .L80084740
/* AFB8D8 80084738 A20F0040 */   sb    $t7, 0x40($s0)
.L8008473C:
/* AFB8DC 8008473C A2020040 */  sb    $v0, 0x40($s0)
.L80084740:
/* AFB8E0 80084740 52200004 */  beql  $s1, $zero, .L80084754
/* AFB8E4 80084744 26310001 */   addiu $s1, $s1, 1
/* AFB8E8 80084748 9218006B */  lbu   $t8, 0x6b($s0)
/* AFB8EC 8008474C A2180043 */  sb    $t8, 0x43($s0)
/* AFB8F0 80084750 26310001 */  addiu $s1, $s1, 1
.L80084754:
/* AFB8F4 80084754 00118C00 */  sll   $s1, $s1, 0x10
/* AFB8F8 80084758 00118C03 */  sra   $s1, $s1, 0x10
/* AFB8FC 8008475C 2A210004 */  slti  $at, $s1, 4
/* AFB900 80084760 1420FFF0 */  bnez  $at, .L80084724
/* AFB904 80084764 00000000 */   nop   
/* AFB908 80084768 9248004A */  lbu   $t0, 0x4a($s2)
/* AFB90C 8008476C 96590070 */  lhu   $t9, 0x70($s2)
/* AFB910 80084770 240300FF */  li    $v1, 255
/* AFB914 80084774 14680016 */  bne   $v1, $t0, .L800847D0
/* AFB918 80084778 A6590048 */   sh    $t9, 0x48($s2)
/* AFB91C 8008477C 924A0075 */  lbu   $t2, 0x75($s2)
/* AFB920 80084780 2409003C */  li    $t1, 60
/* AFB924 80084784 A2490068 */  sb    $t1, 0x68($s2)
/* AFB928 80084788 106A0005 */  beq   $v1, $t2, .L800847A0
/* AFB92C 8008478C 240E0007 */   li    $t6, 7
/* AFB930 80084790 24020001 */  li    $v0, 1
/* AFB934 80084794 A2420069 */  sb    $v0, 0x69($s2)
/* AFB938 80084798 10000004 */  b     .L800847AC
/* AFB93C 8008479C A242006C */   sb    $v0, 0x6c($s2)
.L800847A0:
/* AFB940 800847A0 240C00FF */  li    $t4, 255
/* AFB944 800847A4 A24C006C */  sb    $t4, 0x6c($s2)
/* AFB948 800847A8 A24C0069 */  sb    $t4, 0x69($s2)
.L800847AC:
/* AFB94C 800847AC 924D007B */  lbu   $t5, 0x7b($s2)
/* AFB950 800847B0 24020002 */  li    $v0, 2
/* AFB954 800847B4 240F1122 */  li    $t7, 4386
/* AFB958 800847B8 A242006A */  sb    $v0, 0x6a($s2)
/* AFB95C 800847BC A242006D */  sb    $v0, 0x6d($s2)
/* AFB960 800847C0 A24E006E */  sb    $t6, 0x6e($s2)
/* AFB964 800847C4 A64F0070 */  sh    $t7, 0x70($s2)
/* AFB968 800847C8 10000022 */  b     .L80084854
/* AFB96C 800847CC A24D006B */   sb    $t5, 0x6b($s2)
.L800847D0:
/* AFB970 800847D0 00008825 */  move  $s1, $zero
/* AFB974 800847D4 02518021 */  addu  $s0, $s2, $s1
.L800847D8:
/* AFB978 800847D8 9218004A */  lbu   $t8, 0x4a($s0)
/* AFB97C 800847DC 12200003 */  beqz  $s1, .L800847EC
/* AFB980 800847E0 A2180068 */   sb    $t8, 0x68($s0)
/* AFB984 800847E4 9219004D */  lbu   $t9, 0x4d($s0)
/* AFB988 800847E8 A219006B */  sb    $t9, 0x6b($s0)
.L800847EC:
/* AFB98C 800847EC 92020068 */  lbu   $v0, 0x68($s0)
/* AFB990 800847F0 28410014 */  slti  $at, $v0, 0x14
/* AFB994 800847F4 14200002 */  bnez  $at, .L80084800
/* AFB998 800847F8 28410021 */   slti  $at, $v0, 0x21
/* AFB99C 800847FC 14200004 */  bnez  $at, .L80084810
.L80084800:
/* AFB9A0 80084800 28410021 */   slti  $at, $v0, 0x21
/* AFB9A4 80084804 1420000B */  bnez  $at, .L80084834
/* AFB9A8 80084808 28410038 */   slti  $at, $v0, 0x38
/* AFB9AC 8008480C 10200009 */  beqz  $at, .L80084834
.L80084810:
/* AFB9B0 80084810 3C048014 */   lui   $a0, %hi(D_8013DEC8) # $a0, 0x8014
/* AFB9B4 80084814 2484DEC8 */  addiu $a0, %lo(D_8013DEC8) # addiu $a0, $a0, -0x2138
/* AFB9B8 80084818 02202825 */  move  $a1, $s1
/* AFB9BC 8008481C 0C00084C */  jal   osSyncPrintf
/* AFB9C0 80084820 9206006B */   lbu   $a2, 0x6b($s0)
/* AFB9C4 80084824 9208006B */  lbu   $t0, 0x6b($s0)
/* AFB9C8 80084828 02484821 */  addu  $t1, $s2, $t0
/* AFB9CC 8008482C 912A0074 */  lbu   $t2, 0x74($t1)
/* AFB9D0 80084830 A20A0068 */  sb    $t2, 0x68($s0)
.L80084834:
/* AFB9D4 80084834 26310001 */  addiu $s1, $s1, 1
/* AFB9D8 80084838 00118C00 */  sll   $s1, $s1, 0x10
/* AFB9DC 8008483C 00118C03 */  sra   $s1, $s1, 0x10
/* AFB9E0 80084840 2A210004 */  slti  $at, $s1, 4
/* AFB9E4 80084844 5420FFE4 */  bnezl $at, .L800847D8
/* AFB9E8 80084848 02518021 */   addu  $s0, $s2, $s1
/* AFB9EC 8008484C 964B0052 */  lhu   $t3, 0x52($s2)
/* AFB9F0 80084850 A64B0070 */  sh    $t3, 0x70($s2)
.L80084854:
/* AFB9F4 80084854 10000038 */  b     .L80084938
/* AFB9F8 80084858 96420070 */   lhu   $v0, 0x70($s2)
.L8008485C:
/* AFB9FC 8008485C 02518021 */  addu  $s0, $s2, $s1
.L80084860:
/* AFBA00 80084860 920C0068 */  lbu   $t4, 0x68($s0)
/* AFBA04 80084864 12200003 */  beqz  $s1, .L80084874
/* AFBA08 80084868 A20C004A */   sb    $t4, 0x4a($s0)
/* AFBA0C 8008486C 920D006B */  lbu   $t5, 0x6b($s0)
/* AFBA10 80084870 A20D004D */  sb    $t5, 0x4d($s0)
.L80084874:
/* AFBA14 80084874 26310001 */  addiu $s1, $s1, 1
/* AFBA18 80084878 00118C00 */  sll   $s1, $s1, 0x10
/* AFBA1C 8008487C 00118C03 */  sra   $s1, $s1, 0x10
/* AFBA20 80084880 2A210004 */  slti  $at, $s1, 4
/* AFBA24 80084884 5420FFF6 */  bnezl $at, .L80084860
/* AFBA28 80084888 02518021 */   addu  $s0, $s2, $s1
/* AFBA2C 8008488C 924E0040 */  lbu   $t6, 0x40($s2)
/* AFBA30 80084890 96420070 */  lhu   $v0, 0x70($s2)
/* AFBA34 80084894 240300FF */  li    $v1, 255
/* AFBA38 80084898 106E0027 */  beq   $v1, $t6, .L80084938
/* AFBA3C 8008489C A6420052 */   sh    $v0, 0x52($s2)
/* AFBA40 800848A0 00008825 */  move  $s1, $zero
/* AFBA44 800848A4 02518021 */  addu  $s0, $s2, $s1
.L800848A8:
/* AFBA48 800848A8 920F0040 */  lbu   $t7, 0x40($s0)
/* AFBA4C 800848AC 12200003 */  beqz  $s1, .L800848BC
/* AFBA50 800848B0 A20F0068 */   sb    $t7, 0x68($s0)
/* AFBA54 800848B4 92180043 */  lbu   $t8, 0x43($s0)
/* AFBA58 800848B8 A218006B */  sb    $t8, 0x6b($s0)
.L800848BC:
/* AFBA5C 800848BC 92020068 */  lbu   $v0, 0x68($s0)
/* AFBA60 800848C0 28410014 */  slti  $at, $v0, 0x14
/* AFBA64 800848C4 14200002 */  bnez  $at, .L800848D0
/* AFBA68 800848C8 28410021 */   slti  $at, $v0, 0x21
/* AFBA6C 800848CC 14200004 */  bnez  $at, .L800848E0
.L800848D0:
/* AFBA70 800848D0 28410021 */   slti  $at, $v0, 0x21
/* AFBA74 800848D4 1420000B */  bnez  $at, .L80084904
/* AFBA78 800848D8 28410038 */   slti  $at, $v0, 0x38
/* AFBA7C 800848DC 10200009 */  beqz  $at, .L80084904
.L800848E0:
/* AFBA80 800848E0 3C048014 */   lui   $a0, %hi(D_8013DEE4) # $a0, 0x8014
/* AFBA84 800848E4 2484DEE4 */  addiu $a0, %lo(D_8013DEE4) # addiu $a0, $a0, -0x211c
/* AFBA88 800848E8 02202825 */  move  $a1, $s1
/* AFBA8C 800848EC 0C00084C */  jal   osSyncPrintf
/* AFBA90 800848F0 9206006B */   lbu   $a2, 0x6b($s0)
/* AFBA94 800848F4 9219006B */  lbu   $t9, 0x6b($s0)
/* AFBA98 800848F8 02594021 */  addu  $t0, $s2, $t9
/* AFBA9C 800848FC 91090074 */  lbu   $t1, 0x74($t0)
/* AFBAA0 80084900 A2090068 */  sb    $t1, 0x68($s0)
.L80084904:
/* AFBAA4 80084904 26310001 */  addiu $s1, $s1, 1
/* AFBAA8 80084908 00118C00 */  sll   $s1, $s1, 0x10
/* AFBAAC 8008490C 00118C03 */  sra   $s1, $s1, 0x10
/* AFBAB0 80084910 2A210004 */  slti  $at, $s1, 4
/* AFBAB4 80084914 5420FFE4 */  bnezl $at, .L800848A8
/* AFBAB8 80084918 02518021 */   addu  $s0, $s2, $s1
/* AFBABC 8008491C 964B0048 */  lhu   $t3, 0x48($s2)
/* AFBAC0 80084920 316DFFF0 */  andi  $t5, $t3, 0xfff0
/* AFBAC4 80084924 35AE0001 */  ori   $t6, $t5, 1
/* AFBAC8 80084928 A64B0070 */  sh    $t3, 0x70($s2)
/* AFBACC 8008492C A64D0070 */  sh    $t5, 0x70($s2)
/* AFBAD0 80084930 A64E0070 */  sh    $t6, 0x70($s2)
/* AFBAD4 80084934 31C2FFFF */  andi  $v0, $t6, 0xffff
.L80084938:
/* AFBAD8 80084938 3C0F8012 */  lui   $t7, %hi(gEquipMasks+2) # $t7, 0x8012
/* AFBADC 8008493C 95EF71A2 */  lhu   $t7, %lo(gEquipMasks+2)($t7)
/* AFBAE0 80084940 3C188012 */  lui   $t8, %hi(gEquipShifts+1) # $t8, 0x8012
/* AFBAE4 80084944 01E21824 */  and   $v1, $t7, $v0
/* AFBAE8 80084948 3063FFFF */  andi  $v1, $v1, 0xffff
/* AFBAEC 8008494C 50600011 */  beql  $v1, $zero, .L80084994
/* AFBAF0 80084950 8FBF0024 */   lw    $ra, 0x24($sp)
/* AFBAF4 80084954 931871F1 */  lbu   $t8, %lo(gEquipShifts+1)($t8)
/* AFBAF8 80084958 3C088012 */  lui   $t0, %hi(gBitFlags+0xc)
/* AFBAFC 8008495C 9649009C */  lhu   $t1, 0x9c($s2)
/* AFBB00 80084960 03031806 */  srlv  $v1, $v1, $t8
/* AFBB04 80084964 3063FFFF */  andi  $v1, $v1, 0xffff
/* AFBB08 80084968 0003C880 */  sll   $t9, $v1, 2
/* AFBB0C 8008496C 01194021 */  addu  $t0, $t0, $t9
/* AFBB10 80084970 8D08712C */  lw    $t0, %lo(gBitFlags+0xc)($t0)
/* AFBB14 80084974 3C0B8012 */  lui   $t3, %hi(gEquipNegMasks+2) # $t3, 0x8012
/* AFBB18 80084978 01095024 */  and   $t2, $t0, $t1
/* AFBB1C 8008497C 55400005 */  bnezl $t2, .L80084994
/* AFBB20 80084980 8FBF0024 */   lw    $ra, 0x24($sp)
/* AFBB24 80084984 956B71AA */  lhu   $t3, %lo(gEquipNegMasks+2)($t3)
/* AFBB28 80084988 004B6024 */  and   $t4, $v0, $t3
/* AFBB2C 8008498C A64C0070 */  sh    $t4, 0x70($s2)
/* AFBB30 80084990 8FBF0024 */  lw    $ra, 0x24($sp)
.L80084994:
/* AFBB34 80084994 8FB00018 */  lw    $s0, 0x18($sp)
/* AFBB38 80084998 8FB1001C */  lw    $s1, 0x1c($sp)
/* AFBB3C 8008499C 8FB20020 */  lw    $s2, 0x20($sp)
/* AFBB40 800849A0 03E00008 */  jr    $ra
/* AFBB44 800849A4 27BD0028 */   addiu $sp, $sp, 0x28

