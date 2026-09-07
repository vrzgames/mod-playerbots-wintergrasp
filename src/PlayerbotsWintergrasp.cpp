/*
 * mod-playerbots-wintergrasp
 * Created by iCore
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Automatically accepts Wintergrasp queue and battle-entry invitations
 * for AzerothCore mod-playerbots bots.
 *
 * Target:
 *   - https://github.com/mod-playerbots/azerothcore-wotlk (Playerbot branch)
 *   - https://github.com/mod-playerbots/mod-playerbots
 */

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Chat.h"
#include "Config.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Player.h"
#include "Random.h"
#include "ScriptMgr.h"
#include "WorldPacket.h"
#include "WorldScript.h"
#include "WorldSession.h"
#include "WintergraspBotController.h"
#include "WintergraspModuleState.h"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace
{
    enum class InviteKind : uint8
    {
        Queue,
        War
    };

    struct PendingInvite
    {
        bool queuePending = false;
        uint32 queueDelayMs = 0;

        bool warPending = false;
        uint32 warDelayMs = 0;
    };

    struct DueInvite
    {
        bool queue = false;
        bool war = false;
    };

    class WintergraspPendingStore
    {
    public:
        static WintergraspPendingStore& Instance()
        {
            static WintergraspPendingStore instance;
            return instance;
        }

        void Schedule(uint32 guidLow, InviteKind kind, uint32 delayMs)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            PendingInvite& pending = _pending[guidLow];

            if (kind == InviteKind::War)
            {
                // A real battle-entry invite supersedes a still-pending queue click.
                pending.queuePending = false;
                pending.queueDelayMs = 0;

                // Do not keep postponing acceptance if the server sends the same
                // entry invite more than once.
                if (!pending.warPending)
                {
                    pending.warPending = true;
                    pending.warDelayMs = delayMs;
                }

                return;
            }

            // If an entry invite is already pending, never queue afterwards.
            if (pending.warPending)
                return;

            // Do not reset the timer on duplicate queue packets.
            if (!pending.queuePending)
            {
                pending.queuePending = true;
                pending.queueDelayMs = delayMs;
            }
        }

        std::vector<std::pair<uint32, DueInvite>> Update(uint32 diff)
        {
            std::vector<std::pair<uint32, DueInvite>> dueInvites;

            std::lock_guard<std::mutex> lock(_mutex);
            for (auto itr = _pending.begin(); itr != _pending.end();)
            {
                DueInvite due;
                PendingInvite& pending = itr->second;

                // Process WAR first. If both ever become due in the same update,
                // TryAcceptQueue() will still refuse to queue a player already in war.
                if (pending.warPending)
                {
                    if (pending.warDelayMs <= diff)
                    {
                        due.war = true;
                        pending.warPending = false;
                        pending.warDelayMs = 0;
                    }
                    else
                    {
                        pending.warDelayMs -= diff;
                    }
                }

                if (pending.queuePending)
                {
                    if (pending.queueDelayMs <= diff)
                    {
                        due.queue = true;
                        pending.queuePending = false;
                        pending.queueDelayMs = 0;
                    }
                    else
                    {
                        pending.queueDelayMs -= diff;
                    }
                }

                if (due.queue || due.war)
                    dueInvites.emplace_back(itr->first, due);

                if (!pending.queuePending && !pending.warPending)
                    itr = _pending.erase(itr);
                else
                    ++itr;
            }

            return dueInvites;
        }

        void Erase(uint32 guidLow)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _pending.erase(guidLow);
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _pending.clear();
        }

    private:
        WintergraspPendingStore() = default;

        std::mutex _mutex;
        std::unordered_map<uint32, PendingInvite> _pending;
    };

    class WintergraspConfig
    {
    public:
        static WintergraspConfig& Instance()
        {
            static WintergraspConfig instance;
            return instance;
        }

        void Load()
        {
            _enabled.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.Enable", true));
            _acceptQueue.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.AcceptQueue", true));
            _acceptBattle.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.AcceptBattle", true));
            _announce.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.Announce", true));
            _debug.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.Debug", false));
            _tactics.store(sConfigMgr->GetOption<bool>("PlayerbotsWintergrasp.Tactics.Enable", false));

            uint32 tacticsInterval = sConfigMgr->GetOption<uint32>("PlayerbotsWintergrasp.Tactics.UpdateInterval", 1000);
            _tacticsUpdateInterval.store(std::max<uint32>(250, tacticsInterval));

            uint32 minDelay = sConfigMgr->GetOption<uint32>("PlayerbotsWintergrasp.AcceptDelayMin", 500);
            uint32 maxDelay = sConfigMgr->GetOption<uint32>("PlayerbotsWintergrasp.AcceptDelayMax", 2500);

            if (maxDelay < minDelay)
                std::swap(minDelay, maxDelay);

            _acceptDelayMin.store(minDelay);
            _acceptDelayMax.store(maxDelay);
        }

        bool IsEnabled() const { return _enabled.load(); }
        bool AcceptQueueEnabled() const { return _acceptQueue.load(); }
        bool AcceptBattleEnabled() const { return _acceptBattle.load(); }
        bool AnnounceEnabled() const { return _announce.load(); }
        bool DebugEnabled() const { return _debug.load(); }
        bool TacticsEnabled() const { return _tactics.load(); }
        uint32 TacticsUpdateInterval() const { return _tacticsUpdateInterval.load(); }

        uint32 GetAcceptDelay() const
        {
            uint32 minDelay = _acceptDelayMin.load();
            uint32 maxDelay = _acceptDelayMax.load();

            if (maxDelay < minDelay)
                std::swap(minDelay, maxDelay);

            return minDelay == maxDelay ? minDelay : urand(minDelay, maxDelay);
        }

    private:
        WintergraspConfig() = default;

        std::atomic<bool> _enabled{ true };
        std::atomic<bool> _acceptQueue{ true };
        std::atomic<bool> _acceptBattle{ true };
        std::atomic<bool> _announce{ true };
        std::atomic<bool> _debug{ false };
        std::atomic<bool> _tactics{ false };
        std::atomic<uint32> _acceptDelayMin{ 500 };
        std::atomic<uint32> _acceptDelayMax{ 2500 };
        std::atomic<uint32> _tacticsUpdateInterval{ 1000 };
    };

    bool IsEnabled() { return WintergraspConfig::Instance().IsEnabled(); }
    bool AcceptQueueEnabled() { return WintergraspConfig::Instance().AcceptQueueEnabled(); }
    bool AcceptBattleEnabled() { return WintergraspConfig::Instance().AcceptBattleEnabled(); }
    bool AnnounceEnabled() { return WintergraspConfig::Instance().AnnounceEnabled(); }
    bool DebugEnabled() { return WintergraspConfig::Instance().DebugEnabled(); }
    uint32 GetAcceptDelay() { return WintergraspConfig::Instance().GetAcceptDelay(); }
    bool TacticsEnabled() { return WintergraspConfig::Instance().TacticsEnabled(); }
    uint32 TacticsUpdateInterval() { return WintergraspConfig::Instance().TacticsUpdateInterval(); }

    bool IsPlayerbot(Player* player)
    {
        return player &&
               player->GetSession() &&
               player->GetSession()->IsBot();
    }

    Battlefield* GetWintergrasp()
    {
        return sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG);
    }

    void DebugLog(Player* player, char const* text)
    {
        if (!DebugEnabled() || !player)
            return;

        LOG_INFO("playerbots", "[PlayerbotsWintergrasp] {}: {}", player->GetName(), text);
    }

    void TryAcceptQueue(Player* player)
    {
        if (!AcceptQueueEnabled() || !IsPlayerbot(player))
            return;

        Battlefield* wg = GetWintergrasp();
        if (!wg)
        {
            DebugLog(player, "Wintergrasp battlefield is not available");
            return;
        }

        TeamId team = player->GetTeamId();
        ObjectGuid guid = player->GetGUID();

        // Never add a player back to the queue after they have already entered war.
        if (wg->GetPlayersInWarSet(team).count(guid))
            return;

        // Duplicate-safe.
        if (wg->GetPlayersQueueSet(team).count(guid))
            return;

        // If the real battle entry invite has already arrived, let that win.
        if (wg->GetInvitedPlayersMap(team).count(guid))
            return;

        // This method is the same core path used by
        // CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE.
        //
        // It does not itself validate a "pending queue invite", therefore this
        // module calls it ONLY after seeing the actual outgoing queue-invite
        // packet for this bot.
        wg->PlayerAcceptInviteToQueue(player);

        if (wg->GetPlayersQueueSet(team).count(guid))
            DebugLog(player, "accepted Wintergrasp queue invite");
    }

    void TryAcceptWar(Player* player)
    {
        if (!AcceptBattleEnabled() || !IsPlayerbot(player))
            return;

        Battlefield* wg = GetWintergrasp();
        if (!wg)
        {
            DebugLog(player, "Wintergrasp battlefield is not available");
            return;
        }

        ObjectGuid guid = player->GetGUID();

        // Team can be changed by battlefield hooks during join, so use
        // IsPlayerInBattlefield() for the final result check.
        if (wg->IsPlayerInBattlefield(guid))
            return;

        TeamId invitedTeam = player->GetTeamId();

        // Require a real, server-side active invite. The core performs the
        // authoritative expiry check again inside PlayerAcceptInviteToWar().
        if (!wg->GetInvitedPlayersMap(invitedTeam).count(guid))
        {
            DebugLog(player, "battle-entry packet seen, but no active server invite remains");
            return;
        }

        // This is the same core path used by
        // CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE.
        wg->PlayerAcceptInviteToWar(player);

        if (wg->IsPlayerInBattlefield(guid))
            DebugLog(player, "accepted Wintergrasp battle-entry invite");
        else
            DebugLog(player, "Wintergrasp battle-entry accept was rejected by the core");
    }

    class PlayerbotsWintergraspPacketScript : public PlayerbotScript
    {
    public:
        PlayerbotsWintergraspPacketScript()
            : PlayerbotScript("PlayerbotsWintergraspPacketScript")
        {
        }

        void OnPlayerbotPacketSent(Player* player, WorldPacket const* packet) override
        {
            if (!packet || !IsPlayerbot(player) || !IsEnabled())
                return;

            PlayerbotsWintergrasp::TrackBot(player);
            PlayerbotsWintergrasp::ObserveWorldStatePacket(player, *packet);

            switch (packet->GetOpcode())
            {
                case SMSG_BATTLEFIELD_MGR_QUEUE_INVITE:
                {
                    if (!AcceptQueueEnabled())
                        return;

                    uint32 delay = GetAcceptDelay();
                    WintergraspPendingStore::Instance().Schedule(
                        player->GetGUID().GetCounter(), InviteKind::Queue, delay);

                    if (DebugEnabled())
                        LOG_INFO("playerbots",
                            "[PlayerbotsWintergrasp] {}: Wintergrasp queue invite detected, accepting in {} ms",
                            player->GetName(), delay);

                    break;
                }

                case SMSG_BATTLEFIELD_MGR_ENTRY_INVITE:
                {
                    if (!AcceptBattleEnabled())
                        return;

                    uint32 delay = GetAcceptDelay();
                    WintergraspPendingStore::Instance().Schedule(
                        player->GetGUID().GetCounter(), InviteKind::War, delay);

                    if (DebugEnabled())
                        LOG_INFO("playerbots",
                            "[PlayerbotsWintergrasp] {}: Wintergrasp battle-entry invite detected, accepting in {} ms",
                            player->GetName(), delay);

                    break;
                }

                default:
                    break;
            }
        }

        void OnPlayerbotLogout(Player* player) override
        {
            if (player)
            {
                WintergraspPendingStore::Instance().Erase(player->GetGUID().GetCounter());
                PlayerbotsWintergrasp::ForgetBot(player->GetGUID().GetCounter());
            }
        }

        void OnPlayerbotUpdate(uint32 diff) override
        {
            PlayerbotsWintergrasp::UpdateBotTactics(
                diff, IsEnabled() && TacticsEnabled(), TacticsUpdateInterval());
        }
    };

    class PlayerbotsWintergraspAnnounceScript : public PlayerScript
    {
    public:
        PlayerbotsWintergraspAnnounceScript()
            : PlayerScript("PlayerbotsWintergraspAnnounceScript", { PLAYERHOOK_ON_LOGIN })
        {
        }

        void OnPlayerLogin(Player* player) override
        {
            if (!player || IsPlayerbot(player) || !IsEnabled() || !AnnounceEnabled())
                return;

            ChatHandler(player->GetSession()).SendSysMessage(
                "|cff4CFF00mod-playerbots-wintergrasp|r module created by |cff00ccffiCore|r.");
        }
    };

    class PlayerbotsWintergraspWorldScript : public WorldScript
    {
    public:
        PlayerbotsWintergraspWorldScript()
            : WorldScript("PlayerbotsWintergraspWorldScript",
                {
                    WORLDHOOK_ON_AFTER_CONFIG_LOAD,
                    WORLDHOOK_ON_UPDATE
                })
        {
        }

        void OnAfterConfigLoad(bool /*reload*/) override
        {
            WintergraspConfig::Instance().Load();

            if (!IsEnabled())
            {
                WintergraspPendingStore::Instance().Clear();
                PlayerbotsWintergrasp::ClearBotTactics();
                PlayerbotsWintergrasp::ClearWorldStates();
            }
        }

        void OnUpdate(uint32 diff) override
        {
            if (!IsEnabled())
            {
                WintergraspPendingStore::Instance().Clear();
                return;
            }

            for (auto const& [guidLow, due] : WintergraspPendingStore::Instance().Update(diff))
            {
                Player* player = ObjectAccessor::FindPlayerByLowGUID(guidLow);
                if (!IsPlayerbot(player))
                    continue;

                // Accept battle first. This prevents a stale queue event from
                // re-queuing the bot after a successful war join.
                if (due.war)
                    TryAcceptWar(player);

                if (due.queue)
                    TryAcceptQueue(player);
            }
        }
    };
}

void AddPlayerbotsWintergraspScripts()
{
    new PlayerbotsWintergraspPacketScript();
    new PlayerbotsWintergraspAnnounceScript();
    new PlayerbotsWintergraspWorldScript();

    LOG_INFO("server.loading", ">> Loaded mod-playerbots-wintergrasp | Created by iCore");
}
