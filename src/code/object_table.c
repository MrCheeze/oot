#include "global.h"

s16 object_exchange_player_bank[] = { OBJECT_LINK_BOY, OBJECT_LINK_CHILD };

u32 object_exchange_bank_max = ARRAY_COUNT(object_exchange_rom_address);

// Object linker symbol declarations (used in the table below)
#define DEFINE_OBJECT(name, _1) DECLARE_ROM_SEGMENT(name)
#define DEFINE_OBJECT_EMPTY(_0, _1)
#define DEFINE_OBJECT_UNSET(_0)

#include "tables/object_table.h"

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_EMPTY
#undef DEFINE_OBJECT_UNSET

// Object Table definition
#define DEFINE_OBJECT(name, _1) ROM_FILE(name),
#define DEFINE_OBJECT_EMPTY(name, _1) ROM_FILE_EMPTY(name),
#define DEFINE_OBJECT_UNSET(_0) ROM_FILE_UNSET,

RomFile object_exchange_rom_address[] = {
#include "tables/object_table.h"
};

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_EMPTY
#undef DEFINE_OBJECT_UNSET
