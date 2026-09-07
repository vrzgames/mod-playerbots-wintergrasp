# mod-playerbots-wintergrasp

Created by **iCore**.

License: **GNU GPL v2 or later**.

A standalone AzerothCore module for `mod-playerbots` that automatically accepts both Wintergrasp invitations and can optionally run dedicated Wintergrasp objective, workshop, vehicle, and cannon tactics.

The tactics port is currently an **experimental, local-testing feature** and is disabled by default. The stable packet-driven invitation logic remains independent from it.

## What is it designed for?

Target environment:

* `https://github.com/mod-playerbots/azerothcore-wotlk` — **Playerbot** branch
* `https://github.com/mod-playerbots/mod-playerbots` — master

The module does not modify the AzerothCore or `mod-playerbots` source code. It must be built together with `mod-playerbots` using `-DMODULES=static` because the tactical controller uses the public Playerbot AI classes.

## What does it do?

1. It monitors outgoing packets sent to bots through the Playerbot fork's `OnPlayerbotPacketSent` hook.
2. When the bot receives an `SMSG_BATTLEFIELD_MGR_QUEUE_INVITE` packet:

   * after a small random delay, it calls the core's own `Battlefield::PlayerAcceptInviteToQueue()` function.
3. When the bot receives an `SMSG_BATTLEFIELD_MGR_ENTRY_INVITE` packet:

   * after a small random delay, it calls the core's own `Battlefield::PlayerAcceptInviteToWar()` function.
4. Before accepting the battle-entry invitation, the module checks whether the bot is actually present in the Wintergrasp server-side invited map.
5. The core itself handles invite expiration, adds the bot to the raid group, and sends the entered response.
6. With `PlayerbotsWintergrasp.Tactics.Enable = 1`, bots that are already enrolled in an active Wintergrasp battle use dedicated logic for:

   * A* routes and battlefield objectives
   * Workshop capture and ownership changes
   * Vehicle summoning and boarding
   * Attacker wall/gate pressure and defender tower pressure
   * Fortress cannon boarding and vehicle weapon targeting

The tactics controller never creates or accepts an invitation. It starts only after `Battlefield::IsPlayerInBattlefield()` confirms that the existing invitation path enrolled the bot.

### Why doesn't it accept the invitation directly from the packet hook?

Outgoing Playerbot packet processing can run from multiple threads. Therefore, the module only creates a mutex-protected **"pending accept"** entry in the packet hook.

The actual Battlefield state changes and raid-group modifications are performed from the `WorldScript::OnUpdate` hook on the world thread.

This matches the execution context used by the Core's `PROCESS_THREADUNSAFE` Battlefield packet handlers.

## Installation

Copy the entire module directory here:

```text
azerothcore-wotlk/
└── modules/
    └── mod-playerbots-wintergrasp/
```

Important: the directory name must be exactly:

```text
mod-playerbots-wintergrasp
```

The loader name is based on this directory name:

```cpp
Addmod_playerbots_wintergraspScripts()
```

After that, **CMake must be reconfigured**, and the core must be rebuilt.

Example on Linux (static modules are required for the tactics port):

```bash
cd azerothcore-wotlk/build
cmake .. -DMODULES=static
make -j$(nproc)
make install
```

### Windows / Visual Studio

1. CMake Configure
2. CMake Generate
3. Build `ALL_BUILD`
4. Copy/install the updated worldserver using your usual procedure.

## Configuration

The module provides:

```text
conf/mod_playerbots_wintergrasp.conf.dist
```

After building/installing, create an active `.conf` file in the module's configuration directory:

```text
mod_playerbots_wintergrasp.conf
```

Recommended initial test configuration:

```ini
PlayerbotsWintergrasp.Enable = 1
PlayerbotsWintergrasp.AcceptQueue = 1
PlayerbotsWintergrasp.AcceptBattle = 1
PlayerbotsWintergrasp.Announce = 1
PlayerbotsWintergrasp.AcceptDelayMin = 500
PlayerbotsWintergrasp.AcceptDelayMax = 2500
PlayerbotsWintergrasp.Tactics.Enable = 0
PlayerbotsWintergrasp.Tactics.UpdateInterval = 1000
PlayerbotsWintergrasp.Debug = 1
```

First verify the existing invitation behavior with tactics disabled. Then enable the local tactical test with:

```ini
PlayerbotsWintergrasp.Tactics.Enable = 1
```

Once everything is working:

```ini
PlayerbotsWintergrasp.Debug = 0
```

## Testing

1. Start the worldserver.
2. You should see the following in the console:

```text
>> Loaded mod-playerbots-wintergrasp
```

3. Have a playerbot in Wintergrasp at a time when the WG queue call appears.
4. With `Debug = 1`, you should see messages similar to:

```text
[PlayerbotsWintergrasp] BotName: Wintergrasp queue invite detected, accepting in 1234 ms
[PlayerbotsWintergrasp] BotName: accepted Wintergrasp queue invite
```

When a real player enters the game, if `Announce` is enabled:

```text
mod-playerbots-wintergrasp module created by iCore.
```

When the battle starts:

```text
[PlayerbotsWintergrasp] BotName: Wintergrasp battle-entry invite detected, accepting in 987 ms
[PlayerbotsWintergrasp] BotName: accepted Wintergrasp battle-entry invite
```

The current Battlefield core in the Playerbot fork also provides the `.bf queue` command for checking the queue / invited / in-war state, if that command is available in your build.

## Local tactics test checklist

The current local build provides:

* Queue call acceptance: **yes, unchanged**
* Enter Battle acceptance: **yes, unchanged**
* Joining the WG raid: **yes, through the normal Core path**
* Invite expiration checking: **yes, handled by the Core**
* Dedicated Wintergrasp objective AI: **experimental**
* Workshop / vehicle / wall / tower strategy: **experimental**

During a test battle, check both factions and watch for these milestones:

1. Bots still accept queue and entry invitations with the configured randomized delay.
2. No tactical movement starts before the bot is listed as in-war.
3. Infantry spreads between objectives and contests capturable workshops.
4. Lieutenant-ranked bots summon and board vehicles when slots are available.
5. Attacker vehicles damage the fortress and defender vehicles pressure southern towers.
6. Defenders near the fortress board free cannons and fire at hostile vehicles.
7. Death, logout, battle end, and config reload release the bot's tactical assignment.

## No SQL

The module does not require any database modifications.

## Compatibility Notes

The source was developed against the public Playerbot branch hooks and Battlefield API available as of **2026-09-04**.

The module directly uses the `PlayerbotScript` hook provided by the Playerbot fork, so it is specifically designed for the Playerbot fork and **not for standard upstream AzerothCore**.

## Attribution

The original Wintergrasp bot concept and foundation come from NoxMax's GPL-2.0-or-later work:

* [`NoxMax/mod-playerbots`, branch `The-Winds-of-Wintergrasp`](https://github.com/mod-playerbots/mod-playerbots/compare/master...NoxMax:mod-playerbots:The-Winds-of-Wintergrasp)
* [`NoxMax/azerothcore-wotlk`, branch `Nox-AC-PB-WG`](https://github.com/mod-playerbots/azerothcore-wotlk/compare/Playerbot...NoxMax:azerothcore-wotlk:Nox-AC-PB-WG)

The implementation included in this module has since been revised and improved.

This module replaces the two extra `BattlefieldWG` query methods from that core branch with a module-owned cache of the authoritative world-state packets. This is what keeps the port installable without patching the core.
