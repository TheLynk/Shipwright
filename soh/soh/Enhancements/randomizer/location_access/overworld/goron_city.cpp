#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_GoronCity() {
    // clang-format off
    areaTable[RR_GORON_CITY] = Region("Goron City", "Goron City", {RA_GORON_CITY}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GossipStoneFairy,          logic->CallGossipFairyExceptSuns()),
        EVENT_ACCESS(StickPot,                  logic->StickPot                  || logic->IsChild),
        EVENT_ACCESS(BugRock,                   logic->BugRock                   || (logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS))),
        EVENT_ACCESS(GoronCityChildFire,        logic->GoronCityChildFire        || (logic->IsChild && logic->CanUse(RG_DINS_FIRE))),
        EVENT_ACCESS(GCWoodsWarpOpen,           logic->GCWoodsWarpOpen           || (logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_FAIRY_BOW) || logic->HasItem(RG_GORONS_BRACELET) || logic->GoronCityChildFire)),
        EVENT_ACCESS(GCDaruniasDoorOpenChild,   logic->GCDaruniasDoorOpenChild   || (logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY))),
        EVENT_ACCESS(StopGCRollingGoronAsAdult, logic->StopGCRollingGoronAsAdult || (logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_GC_LINK_GORON_DINS) && logic->CanUse(RG_DINS_FIRE))))),
    }, {
        //Locations
        LOCATION(RC_GC_MAZE_LEFT_CHEST,             logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_SILVER_GAUNTLETS) || (ctx->GetTrickOption(RT_GC_LEFTMOST) && logic->HasExplosives() && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_GC_MAZE_CENTER_CHEST,           logic->BlastOrSmash()  || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_MAZE_RIGHT_CHEST,            logic->BlastOrSmash()  || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_POT_FREESTANDING_POH,        logic->IsChild && logic->GoronCityChildFire && (logic->CanUse(RG_BOMB_BAG) || (logic->HasItem(RG_GORONS_BRACELET) && ctx->GetTrickOption(RT_GC_POT_STRENGTH)) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_GC_POT)))),
        LOCATION(RC_GC_ROLLING_GORON_AS_CHILD,      logic->IsChild && (logic->HasExplosives() || (logic->HasItem(RG_GORONS_BRACELET) && ctx->GetTrickOption(RT_GC_ROLLING_STRENGTH)))),
        LOCATION(RC_GC_ROLLING_GORON_AS_ADULT,      logic->StopGCRollingGoronAsAdult),
        LOCATION(RC_GC_GS_BOULDER_MAZE,             logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_GC_GS_CENTER_PLATFORM,          logic->IsAdult && logic->CanAttack()),
        LOCATION(RC_GC_MEDIGORON,                   logic->IsAdult && (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE_FAIRY,     (logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)) && logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE_FAIRY_BIG, (logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE,           logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_LOWER_STAIRCASE_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_GC_LOWER_STAIRCASE_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_3,       logic->CanBreakPots()),
        LOCATION(RC_GC_MAZE_CRATE,                  logic->BlastOrSmash()  || (logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanBreakCrates())),

    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL, true),
        ENTRANCE(RR_GC_MEDIGORON,         logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)),
        ENTRANCE(RR_GC_WOODS_WARP,        logic->GCWoodsWarpOpen),
        ENTRANCE(RR_GC_SHOP,              (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && (logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET) || logic->GoronCityChildFire || logic->CanUse(RG_FAIRY_BOW)))),
        ENTRANCE(RR_GC_DARUNIAS_CHAMBER,  (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && logic->GCDaruniasDoorOpenChild)),
        ENTRANCE(RR_GC_GROTTO_PLATFORM,   logic->IsAdult && ((logic->CanUse(RG_SONG_OF_TIME) && ((logic->EffectiveHealth() > 2) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_NAYRUS_LOVE))) || (logic->EffectiveHealth() > 1 && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_HOOKSHOT)) || (logic->CanUse(RG_NAYRUS_LOVE) && logic->CanUse(RG_HOOKSHOT)) || (logic->EffectiveHealth() > 2 && logic->CanUse(RG_HOOKSHOT) && ctx->GetTrickOption(RT_GC_GROTTO)))),
    });

    areaTable[RR_GC_MEDIGORON] = Region("GC Medigoron", "Goron City", {RA_GORON_CITY}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE,           true),
        LOCATION(RC_GC_MEDIGORON_POT_1,                  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GORON_CITY, true),
    });

    areaTable[RR_GC_WOODS_WARP] = Region("GC Woods Warp", "Goron City", {RA_GORON_CITY}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GCWoodsWarpOpen, logic->GCWoodsWarpOpen || (logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE))),
    }, {}, {
        //Exits
        ENTRANCE(RR_GORON_CITY,     logic->CanLeaveForest() && logic->GCWoodsWarpOpen),
        ENTRANCE(RR_THE_LOST_WOODS, true),
    });

    areaTable[RR_GC_DARUNIAS_CHAMBER] = Region("GC Darunias Chamber", "Goron City", {RA_GORON_CITY}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EVENT_ACCESS(GoronCityChildFire, logic->GoronCityChildFire || (logic->IsChild && logic->CanUse(RG_STICKS))),
    }, {
        //Locations
        LOCATION(RC_GC_DARUNIAS_JOY,  logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_GC_DARUNIA_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GC_DARUNIA_POT_2, logic->CanBreakPots()),
        LOCATION(RC_GC_DARUNIA_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GORON_CITY,      true),
        ENTRANCE(RR_DMC_LOWER_LOCAL, logic->IsAdult),
    });

    areaTable[RR_GC_GROTTO_PLATFORM] = Region("GC Grotto Platform", "Goron City", {RA_GORON_CITY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GC_GROTTO,  true),
        ENTRANCE(RR_GORON_CITY, logic->EffectiveHealth() > 2 || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_NAYRUS_LOVE) || ((logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanUse(RG_LONGSHOT))),
    });

    areaTable[RR_GC_SHOP] = Region("GC Shop", "GC Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GC_SHOP_ITEM_1, true),
        LOCATION(RC_GC_SHOP_ITEM_2, true),
        LOCATION(RC_GC_SHOP_ITEM_3, true),
        LOCATION(RC_GC_SHOP_ITEM_4, true),
        LOCATION(RC_GC_SHOP_ITEM_5, true),
        LOCATION(RC_GC_SHOP_ITEM_6, true),
        LOCATION(RC_GC_SHOP_ITEM_7, true),
        LOCATION(RC_GC_SHOP_ITEM_8, true),
    }, {
        //Exits
        ENTRANCE(RR_GORON_CITY, true),
    });

    areaTable[RR_GC_GROTTO] = Region("GC Grotto", "GC Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
        LOCATION(RC_GC_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_GC_GROTTO_PLATFORM, true),
    });

    // clang-format on
}
