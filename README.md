# mod-playerbots-wintergrasp

Created by **iCore**.

License: **GNU GPL v2 or later**.

Külön AzerothCore modul a `mod-playerbots` mellé, amely automatikusan elfogadja a
Wintergrasp queue meghívást és a tényleges "Enter Battle" meghívást a botoknál.

## Mire készült?

Célzott környezet:

- `https://github.com/mod-playerbots/azerothcore-wotlk` — **Playerbot** branch
- `https://github.com/mod-playerbots/mod-playerbots` — master

A modul nem módosítja a `mod-playerbots` forrását.

## Mit csinál?

1. A Playerbot fork saját `OnPlayerbotPacketSent` hookján figyeli a botnak kimenő packeteket.
2. Ha a bot `SMSG_BATTLEFIELD_MGR_QUEUE_INVITE` packetet kap:
   - kis véletlen késleltetés után meghívja a core saját
     `Battlefield::PlayerAcceptInviteToQueue()` függvényét.
3. Ha a bot `SMSG_BATTLEFIELD_MGR_ENTRY_INVITE` packetet kap:
   - kis véletlen késleltetés után meghívja a core saját
     `Battlefield::PlayerAcceptInviteToWar()` függvényét.
4. A battle-entry elfogadás előtt a modul megnézi, hogy a bot ténylegesen benne van-e
   a Wintergrasp szerveroldali invited mapjában.
5. A core maga ellenőrzi az invite lejáratát, raidbe teszi a botot és elküldi az
   entered választ.

### Miért nem közvetlenül a packet hookban fogadja el?

A Playerbot outgoing packet feldolgozás több szálról is futhat. A modul ezért a packet
hookban csak egy mutex-szel védett "pending accept" bejegyzést készít. A Battlefield
állapot és a raidcsoport módosítása a `WorldScript::OnUpdate` hookban, a world-szálon
történik. Ez megegyezik a Core `PROCESS_THREADUNSAFE` battlefield packetkezelőinek
végrehajtási helyével.

## Telepítés

Másold a teljes mappát ide:

```text
azerothcore-wotlk/
└── modules/
    ├── mod-playerbots/
    └── mod-playerbots-wintergrasp/
```

Fontos: a mappa neve pontosan ez legyen:

```text
mod-playerbots-wintergrasp
```

A loader neve ehhez igazodik:

```cpp
Addmod_playerbots_wintergraspScripts()
```

Ezután **újra kell konfigurálni a CMake-et**, majd újra kell fordítani a core-t.

Példa Linuxon, ha nálad is static module build van:

```bash
cd azerothcore-wotlk/build
cmake .. -DMODULES=static
make -j$(nproc)
make install
```

Windows / Visual Studio esetén:
1. CMake Configure
2. CMake Generate
3. Build `ALL_BUILD`
4. Másold/telepítsd a friss worldservert a szokásos módon.

## Config

A modulban:

```text
conf/mod_playerbots_wintergrasp.conf.dist
```

Build/install után a modul config mappájában legyen belőle aktív `.conf` fájl:

```text
mod_playerbots_wintergrasp.conf
```

Ajánlott első teszt:

```ini
PlayerbotsWintergrasp.Enable = 1
PlayerbotsWintergrasp.AcceptQueue = 1
PlayerbotsWintergrasp.AcceptBattle = 1
PlayerbotsWintergrasp.Announce = 1
PlayerbotsWintergrasp.AcceptDelayMin = 500
PlayerbotsWintergrasp.AcceptDelayMax = 2500
PlayerbotsWintergrasp.Debug = 1
```

Ha már működik:

```ini
PlayerbotsWintergrasp.Debug = 0
```

## Teszt

1. Indítsd el a worldservert.
2. A konzolban ezt kell látnod:

```text
>> Loaded mod-playerbots-wintergrasp
```

3. Legyen egy playerbot Wintergraspban olyan időpontban, amikor a WG queue call megjelenik.
4. Debug = 1 mellett ilyesmit kell látnod:

```text
[PlayerbotsWintergrasp] BotName: Wintergrasp queue invite detected, accepting in 1234 ms
[PlayerbotsWintergrasp] BotName: accepted Wintergrasp queue invite
```

Valódi játékos belépésekor, ha az `Announce` engedélyezve van:

```text
mod-playerbots-wintergrasp module created by iCore.
```

Battle startkor:

```text
[PlayerbotsWintergrasp] BotName: Wintergrasp battle-entry invite detected, accepting in 987 ms
[PlayerbotsWintergrasp] BotName: accepted Wintergrasp battle-entry invite
```

A Playerbot fork jelenlegi Battlefield core-jában a `.bf queue` paranccsal is
ellenőrizhető a queue / invited / in-war állapot, ha az adott buildedben ez a parancs elérhető.

## Fontos: mit NEM csinál még?

Ez a verzió az **invite elfogadást** oldja meg.

Tehát:
- queue call elfogadás: igen
- Enter Battle elfogadás: igen
- WG raidbe bekerülés: a core normál útvonalán igen
- invite lejárat ellenőrzése: igen, core oldalon
- külön Wintergrasp objective AI: még nem
- workshop/jármű/fal/tower stratégia: még nem

Ha a bot már bent van Wintergraspban, a jelenlegi playerbots általános mozgás/combat AI-ja
tovább működhet, de ez a modul önmagában nem tanítja meg a botokat a teljes WG stratégiára.

## Nincs SQL

A modulhoz nem kell adatbázis-módosítás.

## Kompatibilitási megjegyzés

A forrás a 2026-09-04-én aktuális Playerbot branch publikus hookjai és Battlefield API-ja
alapján készült. A modul direkt a Playerbot fork `PlayerbotScript` hookját használja,
ezért a sima upstream AzerothCore-ra nem ez a célzott build.
