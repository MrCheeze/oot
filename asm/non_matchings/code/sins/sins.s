glabel sins
/* B775F0 80100450 308EFFFF */  andi  $t6, $a0, 0xffff
/* B775F4 80100454 000E7902 */  srl   $t7, $t6, 4
/* B775F8 80100458 31F8FFFF */  andi  $t8, $t7, 0xffff
/* B775FC 8010045C 33190400 */  andi  $t9, $t8, 0x400
/* B77600 80100460 AFA40000 */  sw    $a0, ($sp)
/* B77604 80100464 13200008 */  beqz  $t9, .L80100488
/* B77608 80100468 03001025 */   move  $v0, $t8
/* B7760C 8010046C 330803FF */  andi  $t0, $t8, 0x3ff
/* B77610 80100470 00084840 */  sll   $t1, $t0, 1
/* B77614 80100474 00095023 */  negu  $t2, $t1
/* B77618 80100478 3C038013 */  lui   $v1, %hi(D_80134CCE)
/* B7761C 8010047C 006A1821 */  addu  $v1, $v1, $t2
/* B77620 80100480 10000006 */  b     .L8010049C
/* B77624 80100484 84634CCE */   lh    $v1, %lo(D_80134CCE)($v1)
.L80100488:
/* B77628 80100488 304B03FF */  andi  $t3, $v0, 0x3ff
/* B7762C 8010048C 000B6040 */  sll   $t4, $t3, 1
/* B77630 80100490 3C038013 */  lui   $v1, %hi(D_801344D0)
/* B77634 80100494 006C1821 */  addu  $v1, $v1, $t4
/* B77638 80100498 846344D0 */  lh    $v1, %lo(D_801344D0)($v1)
.L8010049C:
/* B7763C 8010049C 304D0800 */  andi  $t5, $v0, 0x800
/* B77640 801004A0 11A00005 */  beqz  $t5, .L801004B8
/* B77644 801004A4 00601025 */   move  $v0, $v1
/* B77648 801004A8 00031023 */  negu  $v0, $v1
/* B7764C 801004AC 00027400 */  sll   $t6, $v0, 0x10
/* B77650 801004B0 03E00008 */  jr    $ra
/* B77654 801004B4 000E1403 */   sra   $v0, $t6, 0x10

.L801004B8:
/* B77658 801004B8 03E00008 */  jr    $ra
/* B7765C 801004BC 00000000 */   nop   