#ifndef Z64INVENTORY_H
#define Z64INVENTORY_H

#include "ultra64.h"
#include "z64item.h"
#include "z64save.h"

struct PlayState;

void SetEquip_Item(s16 equipment, u16 value);
u8 ClearEquip_Item(struct PlayState* play, s16 equipment);
void Set_Non_Equip_Register(s16 upgrade, s16 value);

extern u32 check_bit[32];
extern u16 bit_check_data[EQUIP_TYPE_MAX];
extern u16 bit_and_data[EQUIP_TYPE_MAX];
extern u32 non_equip_bit[UPG_MAX];
extern u8 bit_shift_data[EQUIP_TYPE_MAX];
extern u8 non_equip_shift[UPG_MAX];
extern u16 item_max_data[UPG_MAX][4];
extern u32 bit_check_kinsta[4];
extern u32 bit_shift_kinsta[4];
extern void* item_data[0x82];
extern u8 number_pt[56];

#endif
