/*
 * mod-playerbots-wintergrasp
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef PLAYERBOTS_WINTERGRASP_BOT_CONTROLLER_H
#define PLAYERBOTS_WINTERGRASP_BOT_CONTROLLER_H

#include "Define.h"

class Player;

namespace PlayerbotsWintergrasp
{
    void TrackBot(Player* player);
    void ForgetBot(uint32 guidLow);
    void UpdateBotTactics(uint32 diff, bool enabled, uint32 updateIntervalMs);
    void ClearBotTactics();
}

#endif
