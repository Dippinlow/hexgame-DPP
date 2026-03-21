#pragma once

#include <nlohmann/json.hpp>
#include <cstdint>
#include <string>
#include <vector>

class DataHandler
{
public:
    DataHandler(const std::string& filename);

    // Load + save
    bool load();
    bool save() const;

    // Player data access
    int get_elo(uint64_t player_id) const;
    void set_elo(uint64_t player_id, int elo);

    int get_wins(uint64_t player_id) const; 
    void add_win(uint64_t player_id);

    int get_total_games(uint64_t player_id) const;
    void add_game(uint64_t player_id);

    std::string get_colour(uint64_t player_id) const;
    void set_colour(uint64_t player_id, const std::string& colour);

    std::string get_name(uint64_t player_id) const;
    void set_name(uint64_t player_id, const std::string& name);

    std::vector<std::pair<uint64_t, int>> get_all_players() const;

private:
    std::string filename;
    nlohmann::json data;

    // Ensures player exists in JSON
    void ensure_player(uint64_t player_id);
};