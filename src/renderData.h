#pragma once

#include <cstdint>

struct RenderData
{
    const unsigned char (*board)[11];
    uint64_t channel_id;
    int turn;        // 0 or 1
    bool axis_swapped; // which player connects which axis
    bool game_over;
    int winner;      // 0 or 1, only valid if game_over
    int last_i = -1; // for highlighting last move
    int last_j = -1;
};