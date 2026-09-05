# mod-playerbots-wintergrasp

Created by **iCore**.

License: **GNU GPL v2 or later**.

A standalone AzerothCore module for `mod-playerbots` that automatically accepts both the Wintergrasp queue invitation and the actual **"Enter Battle"** invitation for bots.

## What is it designed for?

Target environment:

* `https://github.com/mod-playerbots/azerothcore-wotlk` — **Playerbot** branch
* `https://github.com/mod-playerbots/mod-playerbots` — master

The module does not modify the `mod-playerbots` source code.

## What does it do?

1. It monitors outgoing packets sent to bots through the Playerbot fork's `OnPlayerbotPacketSent` hook.
2. When the bot receives an `SMSG_BATTLEFIELD_MGR_QUEUE_INVITE` packet:

   * after a small random delay, it calls the core's own `Battlefield::PlayerAcceptInviteToQueue()` function.
3. When the bot receives an `SMSG_BATTLEFIELD_MGR_ENTRY_INVITE` packet:

   * after a small random delay, it calls the core's own `Battlefield::PlayerAcceptInviteToWar()` function.
4. Before accepting the battle-entry invitation, the module checks whether the bot is actually present in the Wintergrasp server-side invited map.
5. The core itself handles invite expiration, adds the bot to the raid group, and sends the entered response.

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

Example on Linux, if you are also using a static module build:

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
PlayerbotsWintergrasp.Debug = 1
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

## Important: What does it NOT do yet?

This version handles **invitation acceptance**.

Therefore:

* Queue call acceptance: **yes**
* Enter Battle acceptance: **yes**
* Joining the WG raid: **yes, through the normal Core path**
* Invite expiration checking: **yes, handled by the Core**
* Dedicated Wintergrasp objective AI: **not yet**
* Workshop / vehicle / wall / tower strategy: **not yet**

Once the bot is inside Wintergrasp, the existing general movement/combat AI provided by Playerbots may continue to operate, but this module itself does not teach the bots the complete Wintergrasp strategy.

## No SQL

The module does not require any database modifications.

## Compatibility Notes

The source was developed against the public Playerbot branch hooks and Battlefield API available as of **2026-09-04**.

The module directly uses the `PlayerbotScript` hook provided by the Playerbot fork, so it is specifically designed for the Playerbot fork and **not for standard upstream AzerothCore**.
