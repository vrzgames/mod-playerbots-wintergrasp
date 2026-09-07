/*
 * mod-playerbots-wintergrasp
 * Created by iCore
 * Tactical logic ported from NoxMax's The-Winds-of-Wintergrasp work.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "WintergraspBotController.h"

#include "BattlefieldMgr.h"
#include "WintergraspTactics.h"
#include "BattlefieldWG.h"
#include "Creature.h"
#include "Event.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Playerbots.h"
#include "Vehicle.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
    struct BotTactics
    {
        explicit BotTactics(PlayerbotAI* ai)
            : botAI(ai), checkFlag(ai), summonVehicle(ai), mountCannon(ai, &checkFlag),
              fireCannon(ai), hurlBoulder(ai)
        {
        }

        PlayerbotAI* botAI;
        WgCheckFlagAction checkFlag;
        WgSummonVehicleAction summonVehicle;
        WgMountTowerCannonAction mountCannon;
        WgFireCannonAction fireCannon;
        WgHurlBoulderAction hurlBoulder;
    };

    std::mutex ControllerMutex;
    std::unordered_set<uint32> TrackedBots;
    std::unordered_map<uint32, std::shared_ptr<BotTactics>> TacticsByBot;
    uint32 UpdateTimer = 0;

    BattlefieldWG* GetActiveWintergrasp(Player* bot)
    {
        if (!bot || !bot->InBattlefield())
            return nullptr;

        Battlefield* battlefield = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG);
        if (!battlefield || !battlefield->IsWarTime() || !battlefield->IsPlayerInBattlefield(bot->GetGUID()))
            return nullptr;

        return static_cast<BattlefieldWG*>(battlefield);
    }

    bool TryBoardNearbyWintergraspVehicle(Player* bot)
    {
        if (!bot || bot->GetVehicle() || bot->isDead())
            return false;

        static constexpr uint32 Entries[] =
        {
            NPC_WINTERGRASP_CATAPULT,
            NPC_WINTERGRASP_DEMOLISHER,
            NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE,
            NPC_WINTERGRASP_SIEGE_ENGINE_HORDE
        };

        std::vector<Creature*> candidates;
        for (uint32 entry : Entries)
            if (Creature* found = bot->FindNearestCreature(entry, INTERACTION_DISTANCE, true))
                candidates.push_back(found);

        std::sort(candidates.begin(), candidates.end(), [bot](Creature const* left, Creature const* right)
        {
            return bot->GetDistance(left) < bot->GetDistance(right);
        });

        for (Creature* vehicleBase : candidates)
        {
            if (!vehicleBase || !vehicleBase->IsAlive() || !vehicleBase->IsFriendlyTo(bot) ||
                vehicleBase->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE))
                continue;

            Vehicle* vehicle = vehicleBase->GetVehicleKit();
            if (!vehicle || !vehicle->GetAvailableSeatCount() || vehicle->IsVehicleInUse())
                continue;

            // Vehicle entry can fail while mounted. Dismount first and retry boarding on the next controller tick.
            if (bot->IsMounted())
            {
                if (bot->isMoving())
                    bot->StopMoving();

                WorldPacket emptyPacket;
                bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
                return true;
            }

            vehicleBase->HandleSpellClick(bot);
            if (!bot->IsOnVehicle(vehicleBase))
                continue;

            return true;
        }

        return false;
    }

    bool ExecuteIfReady(Action& action)
    {
        return action.isUseful() && action.isPossible() && action.Execute(Event());
    }

    void UpdateOne(Player* bot, BotTactics& tactics)
    {
        if (!GetActiveWintergrasp(bot) || !tactics.botAI)
        {
            tactics.checkFlag.ResetBattleState();
            return;
        }

        if (bot->isDead())
        {
            tactics.checkFlag.ResetBattleState();
            return;
        }

        // Mirror the original Wintergrasp strategy priorities. Only one
        // successful high-priority action is run per controller tick.
        if (bot->GetVehicle())
        {
            if (ExecuteIfReady(tactics.fireCannon))
                return;
            if (ExecuteIfReady(tactics.hurlBoulder))
                return;
            if (tactics.botAI->DoSpecificAction("ram", Event(), true))
                return;

            tactics.checkFlag.Execute(Event());
            return;
        }

        if (TryBoardNearbyWintergraspVehicle(bot))
            return;
        if (tactics.summonVehicle.Execute(Event()))
            return;
        if (tactics.mountCannon.Execute(Event()))
            return;

        // FollowRoute owns mounting because it knows the route length, objective proximity, vehicle interactions,
        // and whether mounted travel should suppress unengaged hostile targets.
        tactics.checkFlag.Execute(Event());
    }
}

void PlayerbotsWintergrasp::TrackBot(Player* player)
{
    if (!player)
        return;

    std::lock_guard<std::mutex> lock(ControllerMutex);
    TrackedBots.insert(player->GetGUID().GetCounter());
}

void PlayerbotsWintergrasp::ForgetBot(uint32 guidLow)
{
    std::lock_guard<std::mutex> lock(ControllerMutex);
    TrackedBots.erase(guidLow);
    TacticsByBot.erase(guidLow);
}

void PlayerbotsWintergrasp::UpdateBotTactics(uint32 diff, bool enabled, uint32 updateIntervalMs)
{
    if (!enabled)
    {
        ClearBotTactics();
        return;
    }

    UpdateTimer += diff;
    if (UpdateTimer < updateIntervalMs)
        return;
    UpdateTimer = 0;

    std::vector<uint32> tracked;
    {
        std::lock_guard<std::mutex> lock(ControllerMutex);
        tracked.assign(TrackedBots.begin(), TrackedBots.end());
    }

    for (uint32 guidLow : tracked)
    {
        Player* bot = ObjectAccessor::FindPlayerByLowGUID(guidLow);
        if (!bot)
        {
            ForgetBot(guidLow);
            continue;
        }

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI)
            continue;

        BattlefieldWG* wintergrasp = GetActiveWintergrasp(bot);
        if (!wintergrasp)
        {
            std::lock_guard<std::mutex> lock(ControllerMutex);
            TacticsByBot.erase(guidLow);
            continue;
        }

        std::shared_ptr<BotTactics> tactics;
        bool created = false;
        {
            std::lock_guard<std::mutex> lock(ControllerMutex);
            std::shared_ptr<BotTactics>& entry = TacticsByBot[guidLow];
            if (!entry || entry->botAI != botAI)
            {
                entry = std::make_shared<BotTactics>(botAI);
                created = true;
            }
            tactics = entry;
        }

        // Refresh the authoritative workshop/building snapshot once whenever
        // this bot enters a battle (or tactics are enabled mid-battle).
        if (created)
            wintergrasp->SendInitWorldStatesTo(bot);

        UpdateOne(bot, *tactics);
    }
}

void PlayerbotsWintergrasp::ClearBotTactics()
{
    std::lock_guard<std::mutex> lock(ControllerMutex);
    TacticsByBot.clear();
    UpdateTimer = 0;
}
