glabel func_800F8FF4
/* B70194 800F8FF4 308EF000 */  andi  $t6, $a0, 0xf000
/* B70198 800F8FF8 000E7B02 */  srl   $t7, $t6, 0xc
/* B7019C 800F8FFC 31F800FF */  andi  $t8, $t7, 0xff
/* B701A0 800F9000 0018C880 */  sll   $t9, $t8, 2
/* B701A4 800F9004 3C038013 */  lui   $v1, %hi(D_801333A8)
/* B701A8 800F9008 00791821 */  addu  $v1, $v1, $t9
/* B701AC 800F900C 8C6333A8 */  lw    $v1, %lo(D_801333A8)($v1)
/* B701B0 800F9010 240600FF */  li    $a2, 255
/* B701B4 800F9014 24070030 */  li    $a3, 48
/* B701B8 800F9018 9062002D */  lbu   $v0, 0x2d($v1)
/* B701BC 800F901C 50C2000D */  beql  $a2, $v0, .L800F9054
/* B701C0 800F9020 00001025 */   move  $v0, $zero
.L800F9024:
/* B701C4 800F9024 00470019 */  multu $v0, $a3
/* B701C8 800F9028 00004012 */  mflo  $t0
/* B701CC 800F902C 00682821 */  addu  $a1, $v1, $t0
/* B701D0 800F9030 94A90028 */  lhu   $t1, 0x28($a1)
/* B701D4 800F9034 54890004 */  bnel  $a0, $t1, .L800F9048
/* B701D8 800F9038 90A2002D */   lbu   $v0, 0x2d($a1)
/* B701DC 800F903C 03E00008 */  jr    $ra
/* B701E0 800F9040 24020001 */   li    $v0, 1

/* B701E4 800F9044 90A2002D */  lbu   $v0, 0x2d($a1)
.L800F9048:
/* B701E8 800F9048 14C2FFF6 */  bne   $a2, $v0, .L800F9024
/* B701EC 800F904C 00000000 */   nop   
/* B701F0 800F9050 00001025 */  move  $v0, $zero
.L800F9054:
/* B701F4 800F9054 03E00008 */  jr    $ra
/* B701F8 800F9058 00000000 */   nop   

