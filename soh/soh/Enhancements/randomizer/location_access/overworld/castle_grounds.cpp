#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_CastleGrounds() {
    // clang-format off
    //With multi-area support {RA_CASTLE_GROUNDS} is not strictly required anymore, as any interior here could inherit both
    //{RA_HYRULE_CASTLE} and {RA_OUTSIDE_GANONS_CASTLE}, but a setting to merge the latter 2 into the former may be preferred
    areaTable[RR_CASTLE_GROUNDS] = Region("Castle Grounds", "Castle Grounds", {RA_CASTLE_GROUNDS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_THE_MARKET,            true),
        ENTRANCE(RR_HYRULE_CASTLE_GROUNDS, logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS, logic->IsAdult),
    });

    areaTable[RR_HYRULE_CASTLE_GROUNDS] = Region("Hyrule Castle Grounds", "Castle Grounds", {RA_HYRULE_CASTLE}, DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GossipStoneFairy, logic->CallGossipFairy()),
        EVENT_ACCESS(ButterflyFairy,   logic->ButterflyFairy   || logic->CanUse(RG_STICKS)),
        EVENT_ACCESS(BugRock,          true),
    }, {
        //Locations
        LOCATION(RC_HC_MALON_EGG,                        true),
        LOCATION(RC_HC_GS_TREE,                          logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE)),
        LOCATION(RC_HC_MALON_GOSSIP_STONE_FAIRY,         logic->CallGossipFairy()),
        LOCATION(RC_HC_MALON_GOSSIP_STONE_FAIRY_BIG,     logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HC_MALON_GOSSIP_STONE,               true),
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE,           true),
        LOCATION(RC_HC_GRASS_1,                          logic->CanCutShrubs()),
        LOCATION(RC_HC_GRASS_2,                          logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS,          true),
        ENTRANCE(RR_HC_GARDEN,               logic->CanUse(RG_WEIRD_EGG) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->CanJumpslash())),
        ENTRANCE(RR_HC_GREAT_FAIRY_FOUNTAIN, logic->BlastOrSmash()),
        ENTRANCE(RR_HC_STORMS_GROTTO,        logic->CanOpenStormsGrotto()),
    });

    areaTable[RR_HC_GARDEN] = Region("HC Garden", "Castle Grounds", {RA_HYRULE_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_HC_ZELDAS_LETTER, true),
        LOCATION(RC_SONG_FROM_IMPA,   true),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_CASTLE_GROUNDS, true),
    });

    areaTable[RR_HC_GREAT_FAIRY_FOUNTAIN] = Region("HC Great Fairy Fountain", "HC Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_HC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS, true),
    });

    areaTable[RR_HC_STORMS_GROTTO] = Region("HC Storms Grotto", "HC Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_HC_GS_STORMS_GROTTO, logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_HC_STORMS_GS)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS,                true),
        ENTRANCE(RR_HC_STORMS_GROTTO_BEHIND_WALLS, logic->CanBreakMudWalls()),
    });

    areaTable[RR_HC_STORMS_GROTTO_BEHIND_WALLS] = Region("HC Storms Grotto Behind Walls", "HC Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(NutPot,           true),
        EVENT_ACCESS(GossipStoneFairy, logic->CallGossipFairy()),
        EVENT_ACCESS(WanderingBugs,    true),
    }, {
        //Locations
        LOCATION(RC_HC_GS_STORMS_GROTTO,                     logic->HookshotOrBoomerang()),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_HC_STORMS_GROTTO_POT_1,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_2,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_3,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_4,                  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_HC_STORMS_GROTTO, true),
    });

    areaTable[RR_GANONS_CASTLE_GROUNDS] = Region("Ganon's Castle Grounds", "Castle Grounds", {RA_OUTSIDE_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(BuiltRainbowBridge, logic->CanBuildRainbowBridge()),
    }, {
        //Locations
        LOCATION(RC_OGC_GS, logic->CanJumpslashExceptHammer() || logic->CanUseProjectile() || (logic->CanShield() && logic->CanUse(RG_MEGATON_HAMMER)) || logic->CanUse(RG_DINS_FIRE)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS,           logic->AtNight),
        ENTRANCE(RR_OGC_GREAT_FAIRY_FOUNTAIN, logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->AtNight),
        ENTRANCE(RR_GANONS_CASTLE_LEDGE,      logic->BuiltRainbowBridge),
    });

    areaTable[RR_OGC_GREAT_FAIRY_FOUNTAIN] = Region("OGC Great Fairy Fountain", "OGC Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_OGC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS, true),
    });

    areaTable[RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE] = Region("Castle Grounds From Ganon's Castle", "Castle Grounds From Ganon's Castle", {RA_CASTLE_GROUNDS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        // Exits
        ENTRANCE(RR_HYRULE_CASTLE_GROUNDS, logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_LEDGE,   logic->IsAdult),
    });

    areaTable[RR_GANONS_CASTLE_LEDGE] = Region("Ganon's Castle Ledge", "OGC Ganon's Castle Ledge", {RA_OUTSIDE_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        // Exits
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS,  logic->BuiltRainbowBridge),
        ENTRANCE(RR_GANONS_CASTLE_ENTRYWAY, logic->IsAdult),
    });

    // clang-format on
}
