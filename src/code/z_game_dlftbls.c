#include "global.h"

// Linker symbol declarations (used in the table below)
#define DEFINE_GAMESTATE(typeName, enumName, name) DECLARE_OVERLAY_SEGMENT(name)
#define DEFINE_GAMESTATE_INTERNAL(typeName, enumName, name)

#include "tables/gamestate_table.h"

#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL

// Gamestate Overlay Table definition
#define DEFINE_GAMESTATE_INTERNAL(typeName, enumName, name)                                               \
    {                                                                                                     \
        NULL, ROM_FILE_UNSET,          NULL, NULL, NULL, name##_init, name##_cleanup, NULL, NULL,         \
        0,    sizeof(typeName##State),                                                                    \
    },

#define DEFINE_GAMESTATE(typeName, enumName, name) \
    {                                              \
        NULL,                                      \
        ROM_FILE(ovl_##name),                      \
        _ovl_##name##SegmentStart,                 \
        _ovl_##name##SegmentEnd,                   \
        NULL,                                      \
        name##_init,                               \
        name##_cleanup,                            \
        NULL,                                      \
        NULL,                                      \
        0,                                         \
        sizeof(typeName##State),                   \
    },

GameStateOverlay game_dlftbls[] = {
#include "tables/gamestate_table.h"
};

#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL
