#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_DodongosCavern() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_DODONGOS_CAVERN_ENTRYWAY] = Region("Dodongos Cavern Entryway", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BEGINNING,    ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEGINNING, ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ()),
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL,         true),
    });

#pragma region Vanilla

    areaTable[RR_DODONGOS_CAVERN_BEGINNING] = Region("Dodongos Cavern Beginning", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ENTRYWAY, true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,    Here(RR_DODONGOS_CAVERN_BEGINNING, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_LOBBY] = Region("Dodongos Cavern Lobby", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GossipStoneFairy, (Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls();}) || logic->HasItem(RG_GORONS_BRACELET)) && logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MAP_CHEST,              Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,       logic->CanStunDeku() || logic->HasItem(RG_GORONS_BRACELET)),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY,     Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);}) && logic->CallGossipFairy()),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY_BIG, Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);}) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,           Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BEGINNING,     true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY_SWITCH,  logic->IsAdult),
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR,   Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER,  HasAccessTo(RR_DODONGOS_CAVERN_LOBBY_SWITCH)),
        ENTRANCE(RR_DODONGOS_CAVERN_FAR_BRIDGE,    HasAccessTo(RR_DODONGOS_CAVERN_FAR_BRIDGE)),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA,     Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->HasExplosives();})),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, false),
    });

    areaTable[RR_DODONGOS_CAVERN_LOBBY_SWITCH] = Region("Dodongos Cavern Lobby Switch", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,        true),
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_SE_CORRIDOR] = Region("Dodongos Cavern SE Corridor", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_SCARECROW,    logic->CanUse(RG_SCARECROW) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (ctx->GetTrickOption(RT_DC_SCARECROW_GS) && (logic->CanAttack()))),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_4, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_5, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_6, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,               true),
        //Shield seems to be in logic to drop a pot on their head as they hit you to blow up the wall
        ENTRANCE(RR_DODONGOS_CAVERN_SE_ROOM,             Here(RR_DODONGOS_CAVERN_SE_CORRIDOR, []{return logic->CanBreakMudWalls() || logic->CanAttack() || (logic->TakeDamage() && logic->CanShield());})),
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, true),
    });

    areaTable[RR_DODONGOS_CAVERN_SE_ROOM] = Region("Dodongos Cavern SE Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, logic->CanAttack()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR, true),
    });

    areaTable[RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Region("Dodongos Cavern Near Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, true),
    });

    areaTable[RR_DODONGOS_CAVERN_LOWER_LIZALFOS] = Region("Dodongos Cavern Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_3,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_4,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LOWER_LIZALFOS_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM,        Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_DODONGOS_CAVERN_DODONGO_ROOM] = Region("Dodongos Cavern Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY_SWITCH,      logic->HasFireSourceWithTorch()),
        ENTRANCE(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM, Here(RR_DODONGOS_CAVERN_DODONGO_ROOM, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Region("Dodongos Cavern Near Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, logic->CanStunDeku()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_STAIRS_LOWER] = Region("Dodongos Cavern Stairs Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,        true),
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_UPPER, logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW))),
        ENTRANCE(RR_DODONGOS_CAVERN_COMPASS_ROOM, Here(RR_DODONGOS_CAVERN_STAIRS_LOWER, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_STAIRS_UPPER] = Region("Dodongos Cavern Stairs Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->HookshotOrBoomerang();}) || logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  logic->IsAdult || logic->CanAttack() || (HasAccessTo(RR_DODONGOS_CAVERN_STAIRS_LOWER) && logic->CanUse(RG_LONGSHOT) && ctx->GetTrickOption(RT_DC_VINES_GS))),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_2,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_3,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_4,        logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_ARMOS_ROOM,   true),
    });

    areaTable[RR_DODONGOS_CAVERN_COMPASS_ROOM] = Region("Dodongos Cavern Compass Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_COMPASS_CHEST, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER, logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET)),
    });

    areaTable[RR_DODONGOS_CAVERN_ARMOS_ROOM] = Region("Dodongos Cavern Armos Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_UPPER,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Region("Dodongos Cavern Bomb Room Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, true),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_ROOM_HEART,           true),
        LOCATION(RC_DODONGOS_CAVERN_FIRST_BRIDGE_GRASS,         logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_GRASS,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ARMOS_ROOM,           true),
        ENTRANCE(RR_DODONGOS_CAVERN_2F_SIDE_ROOM,         Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return logic->CanBreakMudWalls() || (ctx->GetTrickOption(RT_DC_SCRUB_ROOM) && logic->HasItem(RG_GORONS_BRACELET));})),
        ENTRANCE(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER,      (logic->IsAdult && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->CanJumpslash())),
    });

    areaTable[RR_DODONGOS_CAVERN_2F_SIDE_ROOM] = Region("Dodongos Cavern 2F Side Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  logic->CanStunDeku()),
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, logic->CanStunDeku()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Region("Dodongos Cavern First Slingshot Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_GRASS, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP)),
    });

    areaTable[RR_DODONGOS_CAVERN_UPPER_LIZALFOS] = Region("Dodongos Cavern Upper Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_LOWER_LIZALFOS_HEART,       true),
        LOCATION(RC_DODONGOS_CAVERN_UPPER_LIZALFOS_LEFT_HEART,  true),
        LOCATION(RC_DODONGOS_CAVERN_UPPER_LIZALFOS_RIGHT_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,        true),
        ENTRANCE(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Region("Dodongos Cavern Second Slingshot Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Location
        LOCATION(RC_DODONGOS_CAVERN_DOUBLE_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_DOUBLE_EYE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP)),
    });

    areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Region("Dodongos Cavern Bomb Room Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, true),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_POT_2,    logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,       true),
        ENTRANCE(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, true),
        ENTRANCE(RR_DODONGOS_CAVERN_FAR_BRIDGE,            true),
    });

    areaTable[RR_DODONGOS_CAVERN_FAR_BRIDGE] = Region("Dodongos Cavern Far Bridge", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->CanBreakMudWalls();})),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,           true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_AREA] = Region("Dodongos Cavern Boss Region", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(FairyPot, true),
    }, {
        //Location
        LOCATION(RC_DODONGOS_CAVERN_BEFORE_BOSS_GRASS, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,         true),
        ENTRANCE(RR_DODONGOS_CAVERN_BACK_ROOM,     Here(RR_DODONGOS_CAVERN_BOSS_AREA, []{return logic->CanBreakMudWalls();})),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BACK_ROOM] = Region("Dodongos Cavern Back Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_BACK_ROOM,    logic->CanAttack()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_DODONGOS_CAVERN_MQ_BEGINNING] = Region("Dodongos Cavern MQ Beginning", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ENTRYWAY, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY, Here(RR_DODONGOS_CAVERN_MQ_BEGINNING, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOBBY] = Region("Dodongos Cavern MQ Lobby", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_MAP_CHEST,              logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,  logic->CanStunDeku()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, logic->CanStunDeku()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEGINNING,         true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_GOSSIP_STONE,      Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE, Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,      Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->CanBreakMudWalls();}) || Here(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, []{return logic->HasItem(RG_GORONS_BRACELET) && logic->TakeDamage();})), //strength 1 and bunny speed works too
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM,         logic->IsAdult),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,      Here(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE, []{return logic->HasExplosives() || (logic->ClearMQDCUpperLobbyRocks && logic->HasItem(RG_GORONS_BRACELET) && ((logic->IsAdult && ctx->GetTrickOption(RT_DC_MQ_ADULT_EYES)) || (logic->IsChild && ctx->GetTrickOption(RT_DC_MQ_CHILD_EYES))));})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_GOSSIP_STONE] = Region("Dodongos Cavern MQ Gossip Stone", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GossipStoneFairy, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,              true),
        LOCATION(RC_DODONGOS_CAVERN_MQ_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE] = Region("Dodongos Cavern MQ Mouth Side Bridge", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(ClearMQDCUpperLobbyRocks, logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE)),
    }, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,              true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, logic->ClearMQDCUpperLobbyRocks),
        //Bunny hood jump + jumpslash can also make it directly from the raising platform
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM,          logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DC_MQ_CHILD_BOMBS) && logic->CanJumpslashExceptHammer() && logic->TakeDamage())), //RANDOTODO is this possible with equip swapped hammer?
        //it is possible to use bunny hood speed, hovers and a jumpslash to go between here and the other bridge (included with TORCH_ROOM_LOWER), but this would be a trick
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER] = Region("Dodongos Cavern MQ Stairs Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //EVENT_ACCESS(CanClimbDCStairs, logic->HasExplosives || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW))),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_1,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_2,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_3,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_4,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_LOWER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_LOWER_CRATE_2, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,               true),
        //This is possible with sticks and shield, igniting a first flower by "touch" then very quickly crouch stabbing in a way that cuts the corner to light the 3rd bomb on the other side, but that's a trick
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER,         Here(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, []{return logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW));})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL, Here(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, []{return logic->CanBreakMudWalls();})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL] = Region("Dodongos Cavern MQ Stairs Past Mud Wall", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(DekuBabaSticks, logic->CanGetDekuBabaSticks()),
        //EVENT_ACCESS(CanClimbDCStairs, logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_STICKS))),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_STICKS))),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER] = Region("Dodongos Cavern MQ Stairs Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,    logic->CanStunDeku()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_4, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,               true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS] = Region("Dodongos Cavern MQ Past Big Skulltulas", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, logic->TakeDamage()),
        //If some case comes up where you can directly (void?)warp here without going through Dodongo room or climbing up from below,
        //the commented out logic is to handle going down and reclimbing to get silver rupees. A new eventVar will need decalring to handle this.
        /*(logic->CanPassEnemy(RE_BIG_SKULLTULA) || CanUse(RG_HOVER_BOOTS)) && logic->CanClimbDCStairs;*/
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, true),//if we add BONKO or other crate logic, logic for silver rupees goes here
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM] = Region("Dodongos Cavern MQ Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST,   logic->CanKillEnemy(RE_DODONGO) || logic->HasItem(RG_GORONS_BRACELET)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER,         Here(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, []{return logic->CanKillEnemy(RE_DODONGO) || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER] = Region("Dodongos Cavern MQ Torch Puzzle Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(ClearMQDCUpperLobbyRocks, (((logic->IsAdult /*or bunny hood jump*/) && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_STICKS)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_MIDDLE_POT, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,              logic->TakeDamage()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM,       true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM,        logic->HasFireSourceWithTorch()),//torch checks here need strength 0 with sticks when that is implemented
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM,     Here(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, []{return logic->HasFireSourceWithTorch();})), //Includes an implied CanPass(RE_BIG_SKULLTULA)
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, ((logic->IsAdult /*or bunny hood jump*/) && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     logic->CanUse(RG_STICKS) && logic->HasItem(RG_GORONS_BRACELET)), //Implies access to RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM from here
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM] = Region("Dodongos Cavern MQ Big Block Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_BIG_BLOCK_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BIG_BLOCK_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     (logic->HasFireSource() && logic->HasItem(RG_GORONS_BRACELET)) || logic->CanBreakMudWalls()), //Requires stregnth 0, If you can somehow warp into this room, add logic->CanPassEnemy(RE_BIG_SKULLTULA)
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM] = Region("Dodongos Cavern MQ Larvae Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,   true), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,      logic->CanBreakCrates()), //implied logic->CanKillEnemy(RE_GOLD_SKULTULLA) based on entry reqs. Add crate logic when BONKO is added
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_5, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_6, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, true), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS] = Region("Dodongos Cavern MQ Before Upper Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,     logic->BlastOrSmash()), //Implied CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_4, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_HEART,  logic->BlastOrSmash()),
    }, {
        //Exits
        //Falling down gets you stuck with nothing there, not a useful exit for logic
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM, Here(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, Here(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM] = Region("Dodongos Cavern MQ Two Fires Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_CRATE_1,  logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_CRATE_2,  logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, logic->IsAdult || (Here(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, []{return logic->BlastOrSmash() || (logic->CanAttack() && logic->HasItem(RG_GORONS_BRACELET));}))),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER] = Region("Dodongos Cavern MQ Torch Puzzle Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(ClearMQDCUpperLobbyRocks, logic->CanDetonateUprightBombFlower() || logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, true),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_CORNER_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_MIDDLE_POT, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE,  logic->ClearMQDCUpperLobbyRocks),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM,     true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Region("Dodongos Cavern MQ Lower Right Side", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)) && logic->CanStunDeku()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_1,                         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_2,                         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_3,                         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_4,                         logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,          true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, Here(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);}) && logic->CanHitEyeTargets()),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS] = Region("Dodongos Cavern MQ Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, Here(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM,        Here(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_POES_ROOM] = Region("Dodongos Cavern MQ Poes Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, true), //If you can get to the locked part of POES_ROOM without a way to open it or passing the chest, this will need it's own room
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);}) && //could be a seperate room if it gets busy
                                                                                                        logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG, true))), //Implies you can avoid/kill the enemies with what you use on the skull, if this assumption is broken, add
                                                                                                    //&& (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);}) || (logic->CanAvoidEnemy(RE_FIRE_KEESE) && logic->CanAvoidEnemy(RE_MAD_SCRUB)))
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_1,      logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_2,      logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_3,      logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_4,      logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_1,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_2,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_3,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_4,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_5,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_6,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_7,    logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_8,    logic->CanBreakCrates()),
        }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,          Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM, Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM] = Region("Dodongos Cavern Mad Scrub Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM, (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG, true))), //Implies you can avoid/kill the enemies with what you use on the skull, if this assumption is broken, add
                                                                                                    //&& (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);}) || (logic->CanAvoidEnemy(RE_FIRE_KEESE) && logic->CanAvoidEnemy(RE_MAD_SCRUB)))
        LOCATION(RC_DODONGOS_CAVERN_MQ_SCRUB_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_SCRUB_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM, Here(RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH] = Region("Dodongos Cavern MQ Behind Mouth", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_BEFORE_BOSS_SW_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BEFORE_BOSS_NE_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_SE_POT,  logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_SW_POT,  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,             true),
        //using pots to get past the fire is in default logic. if stregnth 0 gets added, this will need to be:
        //stregnth 0 || explosives, or projectiles if str0 isn't needed to pull graves (it's a narrow shot though, may be trick worthy)
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE,  true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, logic->IsAdult),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE] = Region("Dodongos Cavern MQ Back Behind Fire", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, true), //pulling the grave isn't required, as you can open the chest through it
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACKROOM_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACKROOM_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACK_POE_GRASS,    logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,      logic->CanAttack()),
        //There's a trick N64 rolls into the child eyes trick for using armos blow up the bomb flowers when dieing, which would be killing an armos
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, Here(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, []{return logic->CanDetonateBombFlowers();}) || Here(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, []{return logic->CanAttack();})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE] = Region("Dodongos Cavern MQ BossArea", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(FairyPot, true),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA) || logic->HasItem(RG_GORONS_BRACELET) || //even if you somehow warp to BACK_BEHIND_FIRE, if you can kill the skull at range, you can get to BEHIND_MOUTH
                                                                                                        Here(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, []{return (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) /* || bunny jumps*/);})),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_NW_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_NE_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_GRASS,       logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,       true), //if strength 0 prevents grave pulls, add it here
    });

#pragma endregion

    // Boss Room
    // RANDOTODO make it so entrance randomiser can properly handle more than 1 access to that entrance
    areaTable[RR_DODONGOS_CAVERN_BOSS_ENTRYWAY] = Region("Dodongos Cavern Boss Entryway", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_EXIT] = Region("Dodongos Cavern Boss Exit", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA,       ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ()),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_ROOM] = Region("Dodongos Cavern Boss Room", "Dodongos Cavern", {}, NO_DAY_NIGHT_CYCLE, {
        // Events
        EVENT_ACCESS(DodongosCavernClear, logic->DodongosCavernClear || (Here(RR_DODONGOS_CAVERN_BOSS_ROOM, []{return logic->HasExplosives() || (logic->CanUse(RG_MEGATON_HAMMER) && ctx->GetTrickOption(RT_DC_HAMMER_FLOOR));}) && logic->CanKillEnemy(RE_KING_DODONGO)); /*todo add chu kill to tricks*/),
    }, {
        // Locations
        LOCATION(RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST,    true),
        LOCATION(RC_DODONGOS_CAVERN_KING_DODONGO_HEART, logic->DodongosCavernClear),
        LOCATION(RC_KING_DODONGO,                       logic->DodongosCavernClear),
    }, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_EXIT, true),
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL,      logic->DodongosCavernClear, false),
    });

    // clang-format on
}
