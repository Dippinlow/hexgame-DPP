#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "game.h"
class GameManager 
{
public:
    GameManager() = default;

    bool has_game(uint64_t channel_id) const;

    bool create_game(uint64_t channel_id, uint64_t player1, uint64_t player2);

    Game* get_game(uint64_t channel_id);

    void end_game(uint64_t channel_id);

    int get_active_game_count() const;

    std::vector<uint64_t> get_timed_out_games(int timeout_seconds) const;

private:
    std::unordered_map<uint64_t, Game> active_games;
    mutable std::mutex mtx;
};