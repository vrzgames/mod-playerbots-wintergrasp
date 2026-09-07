/*
 * mod-playerbots-wintergrasp
 * Created by iCore
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "WintergraspModuleState.h"

#include "BattlefieldWG.h"
#include "Opcodes.h"
#include "Player.h"
#include "WorldPacket.h"

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

namespace
{
    constexpr uint32 WINTERGRASP_ZONE_ID = 4197;
    constexpr uint32 NAVIGATION_WORLD_STATES[] = { 3763, 3757, 3754, 3768, 3773 };

    std::shared_mutex WorldStateMutex;
    std::unordered_map<uint32, uint32> WorldStates;
    std::atomic<uint64> NavigationRevision{ 1 };

    bool IsNavigationWorldState(uint32 variable)
    {
        for (uint32 worldState : NAVIGATION_WORLD_STATES)
            if (variable == worldState)
                return true;
        return false;
    }

    bool NavigationStateChanged(std::unordered_map<uint32, uint32> const& oldStates,
                                std::unordered_map<uint32, uint32> const& newStates)
    {
        for (uint32 worldState : NAVIGATION_WORLD_STATES)
        {
            auto const oldItr = oldStates.find(worldState);
            auto const newItr = newStates.find(worldState);
            if ((oldItr == oldStates.end()) != (newItr == newStates.end()))
                return true;
            if (oldItr != oldStates.end() && oldItr->second != newItr->second)
                return true;
        }
        return false;
    }

    void Store(uint32 variable, uint32 value)
    {
        std::unique_lock<std::shared_mutex> lock(WorldStateMutex);
        auto const itr = WorldStates.find(variable);
        if (itr != WorldStates.end() && itr->second == value)
            return;

        WorldStates[variable] = value;
        if (IsNavigationWorldState(variable))
            NavigationRevision.fetch_add(1, std::memory_order_release);
    }

    bool Read(uint32 variable, uint32& value)
    {
        std::shared_lock<std::shared_mutex> lock(WorldStateMutex);
        auto const itr = WorldStates.find(variable);
        if (itr == WorldStates.end())
            return false;

        value = itr->second;
        return true;
    }
}

void PlayerbotsWintergrasp::ObserveWorldStatePacket(Player* player, WorldPacket const& packet)
{
    try
    {
        WorldPacket copy(packet);
        copy.rpos(0);

        if (packet.GetOpcode() == SMSG_INIT_WORLD_STATES)
        {
            int32 mapId = 0;
            int32 zoneId = 0;
            int32 areaId = 0;
            uint16 count = 0;
            copy >> mapId >> zoneId >> areaId >> count;

            if (zoneId != static_cast<int32>(WINTERGRASP_ZONE_ID))
                return;

            std::unordered_map<uint32, uint32> updatedStates;
            updatedStates.reserve(count);
            for (uint16 i = 0; i < count; ++i)
            {
                int32 variable = 0;
                int32 value = 0;
                copy >> variable >> value;
                updatedStates[static_cast<uint32>(variable)] = static_cast<uint32>(value);
            }

            std::unique_lock<std::shared_mutex> lock(WorldStateMutex);
            bool navigationChanged = NavigationStateChanged(WorldStates, updatedStates);
            WorldStates = std::move(updatedStates);
            if (navigationChanged)
                NavigationRevision.fetch_add(1, std::memory_order_release);
            return;
        }

        if (packet.GetOpcode() == SMSG_UPDATE_WORLD_STATE && player && player->GetZoneId() == WINTERGRASP_ZONE_ID)
        {
            int32 variable = 0;
            int32 value = 0;
            copy >> variable >> value;
            Store(static_cast<uint32>(variable), static_cast<uint32>(value));
        }
    }
    catch (ByteBufferException const&)
    {
        // Ignore malformed/truncated observations. The original packet is
        // untouched and continues through the normal playerbots path.
    }
}

void PlayerbotsWintergrasp::ClearWorldStates()
{
    std::unique_lock<std::shared_mutex> lock(WorldStateMutex);
    if (WorldStates.empty())
        return;

    WorldStates.clear();
    NavigationRevision.fetch_add(1, std::memory_order_release);
}

bool PlayerbotsWintergrasp::IsBuildingDestroyed(uint32 worldState)
{
    uint32 state = BATTLEFIELD_WG_OBJECTSTATE_NONE;
    if (!Read(worldState, state))
        return false;

    return state == BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_DESTROY ||
           state == BATTLEFIELD_WG_OBJECTSTATE_HORDE_DESTROY ||
           state == BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DESTROY;
}

TeamId PlayerbotsWintergrasp::GetWorkshopTeam(uint8 workshopId)
{
    if (workshopId >= WG_MAX_WORKSHOP)
        return TEAM_NEUTRAL;

    uint32 state = BATTLEFIELD_WG_OBJECTSTATE_NONE;
    if (!Read(WorkshopsData[workshopId].worldstate, state))
        return TEAM_NEUTRAL;

    switch (state)
    {
        case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT:
        case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DAMAGE:
        case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DESTROY:
            return TEAM_ALLIANCE;
        case BATTLEFIELD_WG_OBJECTSTATE_HORDE_INTACT:
        case BATTLEFIELD_WG_OBJECTSTATE_HORDE_DAMAGE:
        case BATTLEFIELD_WG_OBJECTSTATE_HORDE_DESTROY:
            return TEAM_HORDE;
        default:
            return TEAM_NEUTRAL;
    }
}

uint64 PlayerbotsWintergrasp::GetNavigationRevision()
{
    return NavigationRevision.load(std::memory_order_acquire);
}
