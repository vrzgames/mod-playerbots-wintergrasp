/*
 * mod-playerbots-wintergrasp
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef PLAYERBOTS_WINTERGRASP_MODULE_STATE_H
#define PLAYERBOTS_WINTERGRASP_MODULE_STATE_H

#include "Define.h"
#include "SharedDefines.h"

class Player;
class WorldPacket;

namespace PlayerbotsWintergrasp
{
    // Observe the same world-state packets that are sent to a bot. This keeps
    // the tactical port independent from NoxMax's small BattlefieldWG core API
    // additions while still using the authoritative live battlefield state.
    void ObserveWorldStatePacket(Player* player, WorldPacket const& packet);
    void ClearWorldStates();

    bool IsBuildingDestroyed(uint32 worldState);
    TeamId GetWorkshopTeam(uint8 workshopId);
    // Changes only when navigation-relevant fortress structures change state.
    uint64 GetNavigationRevision();
}

#endif
