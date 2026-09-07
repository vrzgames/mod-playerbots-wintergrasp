/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WINTERGRASP_PATHS_H
#define PLAYERBOTS_WINTERGRASP_PATHS_H

#include "BattlefieldWG.h"
#include "Position.h"
#include <vector>

namespace Wg
{

struct Waypoint
{
    float  x, y, z;
    uint32 pathBlock = 0;       // WorldState ID of the building blocking this waypoint (0 = none).
    bool   noSkip    = false;   // True = bot must reach this node before advancing past it.
};

using Path = std::vector<Waypoint>;

// ######################## //
// Wintergrasp Path Network
// ######################## //

// { Xf, Yf, Zf, #, bool}
// XYZ coordinates with # world state value of blocker to destroy, and bool for unskippable waypoints.
// World state values imported from the core at BattlefieldWG.h

// Ring Road North: The upper half of the Wintergrasp ring road
static Path const vPath_Ring_Road_North = {
    { 4784.530f, 3291.680f, 365.614f },             // Western connection to Ring Road South
    { 4879.030f, 3331.760f, 372.128f },
    { 4936.400f, 3333.610f, 376.881f },             // Connection to NW Workshop
    { 4999.050f, 3309.470f, 376.573f },             // Connection to Horde Route Part A from Horde Spawn
    { 5049.280f, 3228.850f, 358.029f },             // Connection to Horde Route Part B to Fortress Wall West
    { 5056.060f, 3138.430f, 358.508f },
    { 5043.360f, 3077.020f, 366.621f },          // Connection to Horde Bypath
    { 5051.850f, 3015.960f, 367.854f },
    { 5041.590f, 2950.240f, 378.512f },
    { 5051.660f, 2847.730f, 393.182f },             // Connections to Outer Fortress Path and northern connection to Central Road
    { 5049.070f, 2772.040f, 381.497f },
    { 5011.900f, 2720.613f, 372.243f },
    { 5009.192f, 2670.899f, 363.185f },
    { 5023.000f, 2608.810f, 356.103f },          // First connection to Alliance Bypath
    { 5019.910f, 2540.720f, 345.466f },             // Double connections to Alliance Bypath
    { 4964.320f, 2455.880f, 322.499f },             // Connection to NE Workshop
    { 4906.290f, 2456.620f, 320.187f },
    { 4874.860f, 2445.190f, 320.399f },
    { 4850.280f, 2414.350f, 321.875f },
    { 4764.320f, 2428.530f, 350.697f },
    { 4687.390f, 2402.290f, 368.945f },             // Eastern connection to Ring Road South
};

// Ring Road South: The lower half of the Wintergrasp ring road
static Path const vPath_Ring_Road_South = {
    { 4607.540f, 2366.920f, 379.028f },             // Eastern connection to Ring Road North
    { 4515.940f, 2327.430f, 369.028f },             // Connections to SE Workshop and SE Tower Road
    { 4468.230f, 2365.330f, 359.855f },
    { 4441.990f, 2434.810f, 358.032f },
    { 4454.170f, 2539.170f, 358.279f },
    { 4472.150f, 2605.220f, 358.307f },
    { 4467.140f, 2690.180f, 373.965f },
    { 4454.320f, 2738.990f, 388.486f },
    { 4504.510f, 2781.980f, 389.473f },
    { 4509.100f, 2824.040f, 391.663f },             // Connection to South Tower and southern connection to Central Road
    { 4466.110f, 2888.600f, 389.463f },
    { 4413.290f, 2922.470f, 384.222f },
    { 4413.110f, 3006.020f, 363.123f },
    { 4399.750f, 3103.200f, 358.638f },
    { 4380.990f, 3202.650f, 363.351f },
    { 4381.770f, 3252.550f, 372.235f },
    { 4382.120f, 3296.330f, 372.429f, 0, true }, // Connections to SW Workshop and SW Tower Road
    { 4444.060f, 3311.240f, 362.362f },
    { 4518.720f, 3352.470f, 359.448f },
    { 4596.720f, 3347.880f, 363.652f },             // Connection to SW Tower Road
    { 4689.620f, 3311.830f, 374.640f },             // Western connection to Ring Road North
};

// Central Road: The road that split the ring road from north to south
static Path const vPath_Central_Road = {
    { 4975.560f, 2870.710f, 385.570f },             // Connection to Ring Road North
    { 4896.410f, 2887.980f, 379.237f },
    { 4801.270f, 2892.340f, 373.895f },
    { 4706.890f, 2867.940f, 387.224f },
    { 4608.260f, 2845.990f, 396.896f },             // Connection to Ring Road South
};

// Alliance Route: From Alliance spawn, to the NE graveyard, to the eastern wall of the fortress
static Path const vPath_Alliance_Route = {

    { 5067.980f, 2203.720f, 356.622f },             // Alliance Spawn
    { 5059.815f, 2261.002f, 356.533f },
    { 5075.046f, 2320.963f, 357.256f },             // First connection to Alliance Bypath
    { 5106.946f, 2416.543f, 357.371f },
    { 5137.308f, 2514.138f, 358.234f },
    { 5165.662f, 2608.387f, 382.992f },           // Second connection to Alliance Bypath
    { 5184.894f, 2665.395f, 397.137f },          // Connection to Fortress Bypath East (1)
    { 5195.485f, 2691.625f, 405.725f },          // Connection to Vehicle Teleporter Exit East and NE Exit Path
    { 5215.000f, 2740.100f, 409.190f, 3757 },      // Fortress Wall East (destructible) and connections to Fortress Bypath East (2) and Inner Fortress Path
};

// Auxiliary path: For navigation form the NE graveyard, towards Ring Road North, then remerge Alliance Route
static Path const vPath_Alliance_Bypath = {
    { 5048.789f, 2350.387f, 360.484f },          // First connection to Alliance Route
    { 5048.169f, 2429.670f, 360.649f },
    { 5046.729f, 2511.246f, 356.988f },          // Connection to Ring Road North
    { 5093.820f, 2573.800f, 366.741f },          // Second connection to Alliance Route and double connections to Ring Road North
};

// Auxiliary path: Providing a second connection point between Ring Road North and Horde Route B
static Path const vPath_Horde_Bypath = {
    { 5087.630f, 3105.240f, 363.656f },          // Connections Ring Road North (6) and Horde Route B (1)
};

// Horde Route A: From Horde spawn spawn, to the NW graveyard, and into Ring Road North near NW Workshop
static Path const vPath_Horde_Route_Part_A = {
    { 5005.520f, 3644.310f, 360.585f },             // Horde Spawn and connection to Far SW Path
    { 5026.290f, 3584.060f, 356.285f },
    { 4997.950f, 3516.910f, 356.520f },
    { 5033.730f, 3435.770f, 359.015f },          // Connection to Far NW Path
    { 5006.879f, 3375.797f, 375.745f },             // Connection to Ring Road North
};

// Horde Route B: From the NW side of Ring Road North near NW Workshop, to the western wall of the fortress
static Path const vPath_Horde_Route_Part_B = {
    { 5097.330f, 3153.350f, 360.052f },             // Connection to Ring Road North
    { 5152.640f, 3074.960f, 380.069f },          // Connections to Horde Bypath and Far NW Path
    { 5198.530f, 3000.700f, 404.440f },          // Connections to Vehicle Teleporter Exit West, Fortress Bypath West (1), and NW Exit Path
    { 5215.000f, 2941.900f, 409.192f, 3754 },     // Fortress Wall West (destructible) and connections to Fortress Bypath West (2) and Inner Fortress Path
};

// Defenders Route: From the fortress graveyard down to Fortress Central Court
static Path const vPath_Defenders_Route = {
    { 5543.799f, 2825.115f, 515.386f },             // Fortress Graveyard
    { 5548.690f, 2745.170f, 509.186f },
    { 5527.080f, 2718.280f, 491.877f },
    { 5499.430f, 2737.160f, 471.548f },
    { 5476.960f, 2711.460f, 451.789f },
    { 5400.870f, 2757.510f, 409.240f },          // Connection to Fortress Workshop East
};

// Inner Fortress Path: From Fortress Front Court to Vault Door
static Path const vPath_Inner_Fortress_Path = {
    { 5215.000f, 2841.200f, 409.192f, 0 },         // Fortress Front Court and connections to Alliance Route, Horde Route Part B,
                                                 //      Fortress SW Exit Path, Fortress SE Exit Path, and Outer Fortress Path
    { 5272.000f, 2841.200f, 409.192f, 3768 },     // Fortress Central Wall (destructible)
    { 5342.800f, 2841.200f, 409.240f, 0, true }, // Fortress Central Court and connections to Fortress Workshop West and East
    { 5393.500f, 2841.200f, 418.675f, 3773 },     // Inner vault door (destructible)
};

// Auxiliary path: For exiting the fortress through its SE tower
static Path const vPath_Fortress_SE_Exit_Path = {
    { 5187.440f, 2759.230f, 413.492f },             // Connection to Inner Fortress Path
    { 5187.590f, 2738.650f, 413.492f },
    { 5168.230f, 2717.280f, 413.492f },             // Fortress SE Exit and connection to Alliance Route
};

// Auxiliary path: For exiting the fortress through its SW tower
static Path const vPath_Fortress_SW_Exit_Path = {
    { 5186.490f, 2921.640f, 413.494f },             // Connection to Inner Fortress Path
    { 5186.100f, 2944.320f, 413.494f },
    { 5167.230f, 2964.020f, 413.494f },             // Fortress SW Exit and connection to Horde Route Part B
};

// NE Workshop
static Path const vPath_NE_Workshop = {
    { 4943.000f, 2388.200f, 324.045f },             // NE Workshop Engineer
    { 4954.340f, 2414.220f, 320.176f },             // Connection to Ring Road North
};

// NW Workshop
static Path const vPath_NW_Workshop = {
    { 4961.000f, 3384.500f, 380.800f },             // NW Workshop Engineer
    { 4946.560f, 3361.500f, 376.877f, 0, true }, // Connection to Ring Road North
};

// SE Workshop
static Path const vPath_SE_Workshop = {
    { 4357.700f, 2353.700f, 379.896f },             // SE Workshop Engineer
    { 4383.600f, 2348.460f, 376.318f },
    { 4458.200f, 2327.260f, 367.101f },             // Connection to Ring Road South

};

// SW Workshop
static Path const vPath_SW_Workshop = {
    { 4353.200f, 3308.600f, 375.937f },             // SW Workshop Engineer and connection to Ring Road South
};

// SE Tower: Straight road from the ring road to the tower
static Path const vPath_SE_Tower_Road = {
    { 4467.910f, 1964.130f, 439.296f },             // SE Tower
    { 4503.460f, 2047.800f, 413.551f },          // Connection to Far SE Path
    { 4541.210f, 2139.570f, 378.752f },
    { 4555.610f, 2236.460f, 358.642f },             // Connections to Ring Road South and Far East Path
};

// South Tower: Single coordinate that immediately connects to the ring road
static Path const vPath_South_Tower = {
    { 4420.040f, 2823.010f, 409.931f },             // South Tower and connection to Ring Road South
};

// SW Tower: Diverges from the ring road on the upper side, heads to the tower, then turns back to merge
// into the ring road at the bottom.
static Path const vPath_SW_Tower_Road = {
    { 4576.070f, 3408.500f, 359.965f },             // Connection to Ring Road South towards NW Workshop
    { 4555.770f, 3476.150f, 363.415f },
    { 4555.940f, 3556.160f, 386.661f },
    { 4533.640f, 3595.070f, 397.198f },             // SW Tower
    { 4486.850f, 3608.960f, 386.464f },
    { 4437.850f, 3573.030f, 367.624f },          // Connection to Far SW Path
    { 4399.420f, 3483.940f, 359.024f },
    { 4384.300f, 3389.680f, 361.563f },             // Connection to Ring Road South towards SW Workshop
};

// Auxiliary path: For navigation from the eastern side of the fortress to the front, while outside it
static Path const vPath_Fortress_Bypath_East = {
    { 5164.035f, 2698.225f, 404.097f },             // Connections to SE Exit Path and double to Alliance Route
    { 5133.060f, 2722.050f, 409.182f },
    { 5113.960f, 2755.300f, 408.008f },          // Connection to Outer Fortress Path
};

// Auxiliary path: For navigation from the western side of the fortress to the front, while outside it
static Path const vPath_Fortress_Bypath_West = {
    { 5163.267f, 2984.374f, 409.141f },             // Connections to SW Exit Path and double to Horde Route Part B
    { 5131.660f, 2961.040f, 409.082f },
    { 5113.960f, 2927.860f, 408.599f },          // Connection to Outer Fortress Path
};

// Outer Fortress Path: From Ring Road North to Fortress Gate
static Path const vPath_Outer_Fortress_Path = {
    { 5158.000f, 2841.200f, 408.799f, 3763 },      // Fortress Gate (destructible) and connections to Inner Fortress Path
    { 5108.114f, 2843.619f, 402.798f },          // Connections to Ring Road North, Fortress Bypath East, and Fortress Bypath West
};

// Auxiliary path: From the far SE edges to the rest of the map
static Path const vPath_Far_SE_Path = {
    { 4277.794f, 1827.926f, 350.595f },
    { 4371.759f, 1810.238f, 354.242f },
    { 4454.418f, 1852.759f, 373.527f },
    { 4538.867f, 1899.108f, 397.904f },
    { 4552.810f, 1970.557f, 410.607f },             // Connections to SE Tower Road and Far East SE Bypath
};

// Auxiliary path: From the far SW edges to the rest of the map
static Path const vPath_Far_SW_Path = {
    { 4918.395f, 3686.224f, 352.428f },             // Connection to Horde Route Part A
    { 4831.536f, 3728.451f, 350.424f },
    { 4741.445f, 3757.678f, 355.753f },
    { 4686.047f, 3822.695f, 352.459f },
    { 4647.447f, 3907.755f, 355.973f },
    { 4613.291f, 3990.165f, 376.090f },
    { 4584.142f, 4077.007f, 410.790f },
    { 4472.855f, 4045.411f, 413.114f },
    { 4428.973f, 3955.076f, 411.535f },
    { 4409.458f, 3869.009f, 395.363f },
    { 4417.413f, 3774.362f, 362.861f },
    { 4439.825f, 3684.148f, 362.966f },
    { 4386.598f, 3610.822f, 357.952f },             // Connection to SW Tower Road
};

// Auxiliary path: From the far NW edges to the rest of the map
static Path const vPath_Far_NW_Path = {
    { 5104.822f, 3398.345f, 356.628f },          // Connection to Horde Route Part A
    { 5184.610f, 3395.535f, 356.526f },
    { 5274.457f, 3345.488f, 356.526f },
    { 5330.025f, 3276.951f, 356.746f },
    { 5292.549f, 3193.753f, 369.141f },
    { 5228.770f, 3129.303f, 386.228f },          // Connection to Horde Route Part B
};

// Auxiliary path: From the far east edges to the rest of the map
static Path const vPath_Far_East_Path = {
    { 4837.589f, 1887.022f, 446.530f },
    { 4745.480f, 1882.260f, 446.206f },
    { 4689.004f, 1951.491f, 427.119f },          // Connection to Far East SE Bypath
    { 4723.038f, 2008.358f, 426.284f },
    { 4771.914f, 2080.041f, 422.000f },
    { 4711.448f, 2123.804f, 398.465f },
    { 4648.592f, 2198.310f, 359.948f },          // Connection to SE Tower Road
};

// Auxiliary path: Connecting the Far SE and Far East paths
static Path const vPath_Far_SE_East_Bypath = {
    { 4619.401f, 1952.173f, 423.072f },          // Connections to Far SE Path and Far East Path
};

// Fortress Workshop East
static Path const vPath_Fortress_Workshop_East = {
    { 5391.800f, 2712.400f, 412.942f },          // Fortress Workshop East Engineer
    { 5342.800f, 2718.600f, 409.167f, 0, true }, // Connection to NE Exit Path
    { 5342.800f, 2762.000f, 409.191f, 0, true }, // Connections to Inner Fortress Path and Defenders Route
};

// Fortress Workshop West
static Path const vPath_Fortress_Workshop_West = {
    { 5392.900f, 2980.000f, 413.113f },          // Fortress Workshop West Engineer
    { 5342.800f, 2984.800f, 409.192f, 0, true }, // Connection to NW Exit Path
    { 5342.800f, 2917.800f, 409.192f, 0, true }, // Connection to Inner Fortress Path
};

// Auxiliary path: Connecting the vehicle teleporter exit point to the rest of the map
static Path const vPath_Vehicle_Tele_Con_East = {
    { 5256.993f, 2704.333f, 409.191f },          // Connection to Alliance Route
};

// Auxiliary path: Connecting the vehicle teleporter exit point to the rest of the map
static Path const vPath_Vehicle_Tele_Con_West = {
    { 5257.326f, 2976.304f, 409.191f },          // Connection to Horde Route B
};

// Auxiliary path: For exiting the fortress through its NE tower
static Path const vPath_Fortress_NE_Exit_Path = {
    { 5293.171f, 2654.581f, 413.403f, 0, true }, // Connection to Fortress Workshop East
    { 5268.100f, 2654.366f, 413.403f },
    { 5249.127f, 2636.415f, 413.403f },          // Fortress NE Exit and connection to Alliance Route
};

// Auxiliary path: For exiting the fortress through its NW tower
static Path const vPath_Fortress_NW_Exit_Path = {
    { 5293.425f, 3023.295f, 412.148f, 0, true }, // Connection to Fortress Workshop West
    { 5268.628f, 3024.847f, 412.148f },
    { 5250.163f, 3044.446f, 412.148f },          // Fortress NW Exit and connection to Horde Route Part B
};

// Fortress Cannons: the fortress has 24 cannons, but only these 12 cover the typical hostile approaches. Each cannon
// is a single-waypoint A* path (paths 33-44) that's connected with a junction to a path inside the fortress. The
// cannon scanner matches creatures against these same coordinates, keeping one source of truth for cannon positions.
static Path const g_CannonPaths[] = {
    { { 5264.887f, 2704.792f, 421.783f } },      // Connected to Fortress Workshop East
    { { 5236.105f, 2732.727f, 421.732f } },      // Connected to Fortress Front Court
    { { 5163.863f, 2721.933f, 439.928f } },      // Connected to SE fortress tower
    { { 5137.889f, 2747.527f, 439.928f } },      // Connected to SE fortress tower
    { { 5148.564f, 2820.538f, 421.704f } },      // Connected to Fortress Front Court
    { { 5147.750f, 2861.868f, 421.713f } },      // Connected to Fortress Front Court
    { { 5136.843f, 2935.265f, 439.930f } },      // Connected to SW fortress tower
    { { 5163.509f, 2960.821f, 439.930f } },      // Connected to SW fortress tower
    { { 5234.786f, 2948.732f, 420.963f } },      // Connected to Fortress Front Court
    { { 5265.910f, 2976.459f, 421.149f } },      // Connected to Fortress Workshop West
    { { 5264.585f, 2819.800f, 421.739f } },      // Connected to Fortress Central Wall
    { { 5264.236f, 2861.381f, 421.669f } },      // Connected to Fortress Central Wall
};
static constexpr uint8 DEFENDER_CANNON_COUNT = 12;

// General objectives positions:
// Infantry objectives:
static Position const OBJ_INI_CONFLICT_EAST  = { 5165.662f, 2608.387f, 382.992f, 0.0f };   // Eastern pre-wall-fall conflict
static Position const OBJ_INI_CONFLICT_WEST  = { 5152.640f, 3074.960f, 380.069f, 0.0f };   // Western pre-wall-fall conflict
static Position const OBJ_CENTRAL_COURT      = { 5342.800f, 2841.200f, 409.240f, 0.0f };   // Attackers goal
static Position const OBJ_FRONT_COURT        = { 5215.000f, 2841.200f, 409.192f, 0.0f };   // Defenders goal
// Vehicle objectives:
static Position const OBJ_CENTRAL_STAGE      = { 5051.660f, 2847.730f, 393.182f, 0.0f };   // Central staging area
static Position const OBJ_DEF_GUARD_EAST     = { 5195.485f, 2691.625f, 405.725f, 0.0f };   // East wall defender vehicles guard point
static Position const OBJ_DEF_GUARD_WEST     = { 5198.530f, 3000.700f, 404.440f, 0.0f };   // West wall defender vehicles guard point
static Position const OBJ_DEF_GUARD_GATE     = { 5108.114f, 2843.619f, 402.798f, 0.0f };   // Fotress Gate defender vehicles guard point
static Position const OBJ_FORTRESS_GATE      = { 5158.000f, 2841.200f, 408.799f, 0.0f };   // Goal A (Gate)
static Position const OBJ_CENTRAL_WALL       = { 5272.000f, 2841.200f, 409.192f, 0.0f };   // Goal B (Central Wall)
static Position const OBJ_VAULT_DOOR         = { 5393.500f, 2841.200f, 418.675f, 0.0f };   // Goal C (Vault Door)
static Position const OBJ_EAST_WALL          = { 5215.000f, 2740.100f, 409.190f, 0.0f };   // Alternative goal A (Eastern Wall)
static Position const OBJ_WEST_WALL          = { 5215.000f, 2941.900f, 409.192f, 0.0f };   // Alternative goal A (Western Wall)
static Position const OBJ_WORKSHOP_TELE_WEST = { 5316.250f, 2977.040f, 408.539f, 0.0f };   // Fortress Vehicle Teleporter (Workshop West)
static Position const OBJ_WORKSHOP_TELE_EAST = { 5314.510f, 2703.690f, 408.550f, 0.0f };   // Fortress Vehicle Teleporter (Workshop East)
static Position const OBJ_SE_TOWER           = { 4467.910f, 1964.130f, 439.296f, 0.0f };   // Attackers Tower (SE)
static Position const OBJ_SOUTH_TOWER        = { 4420.040f, 2823.010f, 409.931f, 0.0f };   // Attackers Tower (South)
static Position const OBJ_SW_TOWER           = { 4533.640f, 3595.070f, 397.198f, 0.0f };   // Attackers Tower (SW)

// Workshop data for navigation. Maps workshop IDs to their Path. workshopId values are from BattlefieldWG.
struct WorkshopData { uint8 workshopId; Path const* path; };
static WorkshopData const WORKSHOPS[] = {
    { BATTLEFIELD_WG_WORKSHOP_NE,        &vPath_NE_Workshop },            // NE - Sunken Ring
    { BATTLEFIELD_WG_WORKSHOP_NW,        &vPath_NW_Workshop },            // NW - Broken Temple
    { BATTLEFIELD_WG_WORKSHOP_SE,        &vPath_SE_Workshop },            // SE - Eastspark
    { BATTLEFIELD_WG_WORKSHOP_SW,        &vPath_SW_Workshop },            // SW - Westspark
    { BATTLEFIELD_WG_WORKSHOP_KEEP_EAST, &vPath_Fortress_Workshop_East},  // East - Fortress
    { BATTLEFIELD_WG_WORKSHOP_KEEP_WEST, &vPath_Fortress_Workshop_West},  // West - Fortress
};

// Values of WORKSHOPS[] indices, whose entries are used to reference a workshop's ID and path.
static constexpr uint8 WORKSHOP_IDX_NE        = 0;
static constexpr uint8 WORKSHOP_IDX_NW        = 1;
static constexpr uint8 WORKSHOP_IDX_SE        = 2;
static constexpr uint8 WORKSHOP_IDX_SW        = 3;
static constexpr uint8 WORKSHOP_IDX_FORT_EAST = 4;
static constexpr uint8 WORKSHOP_IDX_FORT_WEST = 5;

static Path const* const g_AllPaths[] = {
    &vPath_Ring_Road_North,             // Path  0 - Waypoints: 21
    &vPath_Ring_Road_South,             // Path  1 - Waypoints: 21
    &vPath_Central_Road,                // Path  2 - Waypoints: 5
    &vPath_Alliance_Route,              // Path  3 - Waypoints: 9
    &vPath_Alliance_Bypath,             // Path  4 - Waypoints: 4
    &vPath_Horde_Route_Part_A,          // Path  5 - Waypoints: 5
    &vPath_Horde_Route_Part_B,          // Path  6 - Waypoints: 4
    &vPath_Horde_Bypath,                // Path  7 - Waypoints: 1
    &vPath_Defenders_Route,             // Path  8 - Waypoints: 6
    &vPath_Inner_Fortress_Path,         // Path  9 - Waypoints: 4
    &vPath_Fortress_SE_Exit_Path,       // Path 10 - Waypoints: 3
    &vPath_Fortress_SW_Exit_Path,       // Path 11 - Waypoints: 3
    &vPath_NE_Workshop,                 // Path 12 - Waypoints: 2
    &vPath_NW_Workshop,                 // Path 13 - Waypoints: 2
    &vPath_SE_Workshop,                 // Path 14 - Waypoints: 3
    &vPath_SW_Workshop,                 // Path 15 - Waypoints: 1
    &vPath_SE_Tower_Road,               // Path 16 - Waypoints: 4
    &vPath_South_Tower,                 // Path 17 - Waypoints: 1
    &vPath_SW_Tower_Road,               // Path 18 - Waypoints: 8
    &vPath_Outer_Fortress_Path,         // Path 19 - Waypoints: 2
    &vPath_Fortress_Bypath_East,        // Path 20 - Waypoints: 3
    &vPath_Fortress_Bypath_West,        // Path 21 - Waypoints: 3
    &vPath_Far_SE_Path,                 // Path 22 - Waypoints: 5
    &vPath_Far_SW_Path,                 // Path 23 - Waypoints: 13
    &vPath_Far_NW_Path,                 // Path 24 - Waypoints: 6
    &vPath_Far_East_Path,               // Path 25 - Waypoints: 7
    &vPath_Far_SE_East_Bypath,          // Path 26 - Waypoints: 1
    &vPath_Fortress_Workshop_East,      // Path 27 - Waypoints: 3
    &vPath_Fortress_Workshop_West,      // Path 28 - Waypoints: 3
    &vPath_Vehicle_Tele_Con_East,       // Path 29 - Waypoints: 1
    &vPath_Vehicle_Tele_Con_West,       // Path 30 - Waypoints: 1
    &vPath_Fortress_NE_Exit_Path,       // Path 31 - Waypoints: 3
    &vPath_Fortress_NW_Exit_Path,       // Path 32 - Waypoints: 3
    &g_CannonPaths[0],                  // Path 33 - Defender cannon 0
    &g_CannonPaths[1],                  // Path 34 - Defender cannon 1
    &g_CannonPaths[2],                  // Path 35 - Defender cannon 2
    &g_CannonPaths[3],                  // Path 36 - Defender cannon 3
    &g_CannonPaths[4],                  // Path 37 - Defender cannon 4
    &g_CannonPaths[5],                  // Path 38 - Defender cannon 5
    &g_CannonPaths[6],                  // Path 39 - Defender cannon 6
    &g_CannonPaths[7],                  // Path 40 - Defender cannon 7
    &g_CannonPaths[8],                  // Path 41 - Defender cannon 8
    &g_CannonPaths[9],                  // Path 42 - Defender cannon 9
    &g_CannonPaths[10],                 // Path 43 - Defender cannon 10
    &g_CannonPaths[11],                 // Path 44 - Defender cannon 11
};
static constexpr uint8 PATH_COUNT = 45; // Total waypoints: 173

// WorldState IDs for the four passable fortress obstacles.
// These are the IDs broadcast via UpdateWorldState when building state changes,
// allowing state checks from anywhere on the map without range-limited GO scanning.
static constexpr uint32 WS_FORTRESS_GATE  = 3763;  // Entry 190375 - Fortress Gate
static constexpr uint32 WS_EAST_WALL      = 3757;  // Entry 190372 - Eastern Wall
static constexpr uint32 WS_WEST_WALL      = 3754;  // Entry 190371 - Western Wall
static constexpr uint32 WS_CENTRAL_WALL   = 3768;  // Entry 191805 - Central Wall
static constexpr uint32 WS_VAULT_DOOR     = 3773;  // Entry 191810 - Vault Door

// WorldState IDs for the three attacker towers.
static constexpr uint32 WS_TOWER_SE       = 3706;  // Entry 190358 - SE Tower (Flamewatch)
static constexpr uint32 WS_TOWER_SOUTH    = 3705;  // Entry 190357 - South Tower (Winter's Edge)
static constexpr uint32 WS_TOWER_SW       = 3704;  // Entry 190356 - SW Tower (Shadowsight)

// State-dependent blockers on sequential edges inside a single path. The wall waypoint remains reachable from
// the attack side, while the edge beyond it stays closed until the structure is destroyed.
struct PathEdgeBlock { uint8 path, wpA, wpB; uint32 worldState; };
static constexpr PathEdgeBlock PATH_EDGE_BLOCKS[] = {
    { 9, 1, 2, WS_CENTRAL_WALL }, // IFP[1] <-> IFP[2]: Central Wall to Central Court
};

// Cross-path junction edges connecting waypoints across different paths.
// oneWay=true: only the A->B edge is added (pathA is the source direction).
// blockWorldState!=0: the edge is impassable to all until the wall with that WorldState ID is destroyed. Vehicles
// can still approach (without crossing) a standing blocker, as their attack objective is the wall node itself.
// blockAura!=0: the edge is impassable to any bot that currently has the aura with that spell ID.
struct JunctionDef { uint8 pathA, wpA, pathB, wpB; bool oneWay = false; uint32 blockWorldState = 0; uint32 blockAura = 0; };
static JunctionDef const PATH_JUNCTIONS[] = {
    // Bi-directional junctions:
    {  0,  0,   1, 20 },  // RRN[0]      <->    RRS[20]     Western connection of ring roads
    {  0, 20,   1,  0 },  // RRN[20]     <->    RRS[0]      Eastern connection of ring roads
    {  0,  9,   2,  0 },  // RRN[9]      <->    CRoad[0]    Central Road North
    {  0, 13,   4,  3 },  // RRN[13]     <->    A_By[3]     Alliance Bypath
    {  0, 14,   4,  2 },  // RRN[14]     <->    A_By[2]     Alliance Bypath
    {  0, 14,   4,  3 },  // RRN[14]     <->    A_By[3]     Alliance Bypath
    {  0,  3,   5,  4 },  // RRN[3]      <->    HR_A[4]     Horde Part A
    {  0,  4,   6,  0 },  // RRN[4]      <->    HR_B[0]     Horde Part B
    {  0,  6,   7,  0 },  // RRN[6]      <->    HR_By[0]    Horde Bypath
    {  0, 15,  12,  1 },  // RRN[15]     <->    NEWork[1]   To NE Workshop
    {  0,  2,  13,  1 },  // RRN[2]      <->    NWWork[1]   To NW Workshop
    {  0,  9,  19,  1 },  // RRN[9]      <->    OFP[1]      Outer Fortress Path

    {  1,  9,   2,  4 },  // RRS[9]      <->    CRoad[4]    Central Road South
    {  1,  1,  14,  2 },  // RRS[1]      <->    SEWork[2]   To SE Workshop
    {  1, 16,  15,  0 },  // RRS[16]     <->    SWWork[0]   To SW Workshop
    {  1,  1,  16,  3 },  // RRS[1]      <->    SETwr[3]    To SE Tower
    {  1,  9,  17,  0 },  // RRS[9]      <->    STwr[0]     South Tower
    {  1, 19,  18,  0 },  // RRS[19]     <->    SWTwr[0]    To SW Tower (top)
    {  1, 16,  18,  7 },  // RRS[16]     <->    SWTwr[7]    To SW Tower (bottom)

    {  3,  2,   4,  0 },  // AR[2]       <->    A_By[0]     Alliance Bypath
    {  3,  5,   4,  3 },  // AR[5]       <->    A_By[3]     Alliance Bypath
    {  3,  6,  20,  0 },  // AR[6]       <->    F_By_E[0]   Fortress Bypath East (first connection)
    {  3,  7,  29,  0 },  // AR[7]       <->    Tel_Ex_E[0] Vehicle Teleporter Exit East
    {  3,  8,  20,  0 },  // AR[8]       <->    F_By_E[0]   Fortress Bypath East (second connection)
    {  5,  0,  23,  0 },  // HR_A[0]     <->    Far_SW[0]   To the far SW edges of the map
    {  5,  3,  24,  0 },  // HR_A[3]     <->    Far_NW[0]   To the far NW edges of the map
    {  6,  1,  7,   0 },  // HR_B[1]     <->    HR_By[0]    Horde Bypath
    {  6,  1,  24,  5 },  // HR_B[1]     <->    Far_NW[5]   To the far NW edges of the map
    {  6,  2,  21,  0 },  // HR_B[2]     <->    F_By_W[0]   Fortress Bypath West (first connection)
    {  6,  2,  30,  0 },  // HR_B[2]     <->    Tel_Ex_W[0] Vehicle Teleporter Exit West
    {  6,  3,  21,  0 },  // HR_B[3]     <->    F_By_W[0]   Fortress Bypath West (second connection)

    { 16,  1,  22,  4 },  // SETwr[1]    <->    Far_SE[4]   To the far SE edges of the map
    { 16,  3,  25,  6 },  // SETwr[3]    <->    Far_E[6]    To the far east edges of the map
    { 18,  5,  23, 12 },  // SWTwr[5]    <->    Far_SW[12]  To the far SW edges of the map

    { 19,  1,  20,  2 },  // OFP[1]      <->    F_By_E[2]   Fortress Bypath East
    { 19,  1,  21,  2 },  // OFP[1]      <->    F_By_W[2]   Fortress Bypath West

    { 26,  0,  22,  4 },  // Far_E_SE[0] <->    Far_SE[4]   Far East and Far SE bypath connection
    { 26,  0,  25,  2 },  // Far_E_SE[0] <->    Far_E[2]    Far East and Far SE bypath connection

    { 27,  2,   8,  5 },  // F_WS_E[2]   <->    DefR[5]     From Fortress Workshop East to the Defenders Route
    { 27,  2,   9,  2 },  // F_WS_E[2]   <->    IFP[2]      From Fortress Workshop East to Central Fortress Court
    { 28,  2,   9,  2 },  // F_WS_W[2]   <->    IFP[2]      From Fortress Workshop West to Central Fortress Court

    // Cannon junctions:
    { 33,  0,  27,  1 },  // Cannon  0   <->    F_WS_E[1]   Fortress Workshop East
    { 34,  0,   9,  0 },  // Cannon  1   <->    IFP[0]      Inner Fortress Path (Fortress Front Court)
    { 35,  0,  10,  0 },  // Cannon  2   <->    SE_Exit[0]  SE fortress tower
    { 36,  0,  10,  0 },  // Cannon  3   <->    SE_Exit[0]  SE fortress tower
    { 37,  0,   9,  0 },  // Cannon  4   <->    IFP[0]      Inner Fortress Path (Fortress Front Court)
    { 38,  0,   9,  0 },  // Cannon  5   <->    IFP[0]      Inner Fortress Path (Fortress Front Court)
    { 39,  0,  11,  0 },  // Cannon  6   <->    SW_Exit[0]  SW fortress tower
    { 40,  0,  11,  0 },  // Cannon  7   <->    SW_Exit[0]  SW fortress tower
    { 41,  0,   9,  0 },  // Cannon  8   <->    IFP[0]      Inner Fortress Path (Fortress Front Court)
    { 42,  0,  28,  1 },  // Cannon  9   <->    F_WS_W[1]   Fortress Workshop West
    { 43,  0,   9,  1 },  // Cannon 10   <->    IFP[1]      Inner Fortress Path (Central Wall)
    { 44,  0,   9,  1 },  // Cannon 11   <->    IFP[1]      Inner Fortress Path (Central Wall)

    // Mono-directional junctions:
    {  9,  0,  10,  0, true },  // IFP[0]      ->    SE_Exit[0]  Inner Fortress Path to SE Exit
    {  9,  0,  11,  0, true },  // IFP[0]      ->    SW_Exit[0]  Inner Fortress Path to SW Exit
    { 10,  2,   3,  6, true },  // SE_Exit[2]  ->    AR[6]       From SE fortress tower to Alliance Route
    { 10,  2,  20,  0, true },  // SE_Exit[2]  ->    F_By_E[0]   From SE fortress tower to Fortress Bypath East
    { 11,  2,   6,  2, true },  // SW_Exit[2]  ->    HR_B[2]     From SW fortress tower to Horde Route B
    { 11,  2,  21,  0, true },  // SW_Exit[2]  ->    F_By_W[0]   From SW fortress tower to Fortress Bypath West
    { 31,  2,   3,  7, true },  // NE_Exit[2]  ->    AR[7]       From NE fortress tower to Alliance Route
    { 32,  2,   6,  2, true },  // NW_Exit[2]  ->    HR_B[2]     From NW fortress tower to Horde Route B

    // Object-blocked junctions:
    {  9,  0,   3,  8,  false, WS_EAST_WALL },       // IFP[0]   <->   AR[8]     East wall
    {  9,  0,   6,  3,  false, WS_WEST_WALL },       // IFP[0]   <->   HR_B[3]   West wall
    {  9,  0,  19,  0,  false, WS_FORTRESS_GATE },   // IFP[0]   <->   OFP[0]    Front Gate

    // Mono-directional, aura based blocked junctions:
    // These paths are a quick exit shortcut, but Recruit bots should be forced through the Inner Fortress Path to increase
    // their chance of combat encounter and rank-up.
    { 27,  1,  31,  0, true,  0, SPELL_RECRUIT },       // F_WS_E[1]   ->    NE_Exit[0]  From Fortress Workshop East to NE Exit
    { 28,  1,  32,  0, true,  0, SPELL_RECRUIT },       // F_WS_W[1]   ->    NW_Exit[0]  From Fortress Workshop West to NW Exit
};

}  // namespace Wg

#endif  // PLAYERBOTS_WINTERGRASP_PATHS_H
