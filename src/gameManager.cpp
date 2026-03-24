#include "gameManager.h"
#include <string>
#include <cstdio>
#include <filesystem>

bool GameManager::has_game(uint64_t channel_id) const
{
    return active_games.find(channel_id) != active_games.end();
}

bool GameManager::create_game(uint64_t channel_id, uint64_t player1, uint64_t player2) 
{
    if(has_game(channel_id)) return false;
    
    active_games.emplace(channel_id, Game(channel_id, player1, player2));
    return true;
}

Game* GameManager::get_game(uint64_t channel_id)
{
    auto it = active_games.find(channel_id);
    if (it != active_games.end()) return &(it->second);
    return nullptr;
}

void GameManager::end_game(uint64_t channel_id)
{
    std::string path = "../temp/game_" + std::to_string(channel_id);

    std::filesystem::remove_all(path);

    active_games.erase(channel_id);
}

int GameManager::get_active_game_count() const
{
    return active_games.size();
}

std::vector<uint64_t> GameManager::get_timed_out_games(int timeout_seconds) const
{
    std::vector<uint64_t> timed_out;
    std::time_t now = std::time(nullptr);

    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& pair : active_games)
        if (now - pair.second.get_last_move_time() > timeout_seconds) 
            timed_out.push_back(pair.first);
    

    return timed_out;
}