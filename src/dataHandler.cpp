#include "dataHandler.h"

#include <fstream>
#include <algorithm>

// Constructor
DataHandler::DataHandler(const std::string& filename)
    : filename(filename)
{
}

// Load JSON from file
bool DataHandler::load()
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        // File doesn't exist yet → start fresh
        data = nlohmann::json::object();
        data["players"] = nlohmann::json::object();
        save();
        return true;
    }

    try {
        in >> data;
    } catch (...) {
        return false;
    }

    // Ensure base structure exists
    if (!data.contains("players")) {
        data["players"] = nlohmann::json::object();
    }

    return true;
}

// Save JSON to file
bool DataHandler::save() const
{
    std::string tmp = filename + ".tmp";
    std::ofstream out(tmp);
    if (!out.is_open()) return false;
    out << data.dump(4);
    out.close();
    return std::rename(tmp.c_str(), filename.c_str()) == 0;
}

// Ensure player exists
void DataHandler::ensure_player(uint64_t player_id)
{
    std::string id = std::to_string(player_id);

    if (!data["players"].contains(id)) {
        data["players"][id] = {
            {"elo", 1200},
            {"wins", 0},
            {"total_games", 0},
            {"colour", "#FFFFFF"},
            {"name", "Unknown"}
        };
    }
}

// --- Getters / Setters ---

int DataHandler::get_elo(uint64_t player_id) const
{
    std::string id = std::to_string(player_id);

    if (!data.contains("players") || !data["players"].contains(id))
        return 1200;

    return data["players"][id].value("elo", 1200);
}

void DataHandler::set_elo(uint64_t player_id, int elo)
{
    ensure_player(player_id);
    data["players"][std::to_string(player_id)]["elo"] = elo;
}

int DataHandler::get_wins(uint64_t player_id) const
{
    std::string id = std::to_string(player_id);

    if (!data.contains("players") || !data["players"].contains(id))
        return 0;

    return data["players"][id].value("wins", 0);
}

void DataHandler::add_win(uint64_t player_id)
{
    ensure_player(player_id);
    data["players"][std::to_string(player_id)]["wins"] =
        get_wins(player_id) + 1;
}

int DataHandler::get_total_games(uint64_t player_id) const
{
    std::string id = std::to_string(player_id);

    if (!data.contains("players") || !data["players"].contains(id))
        return 0;

    return data["players"][id].value("total_games", 0);
}

void DataHandler::add_game(uint64_t player_id)
{
    ensure_player(player_id);
    data["players"][std::to_string(player_id)]["total_games"] =
        get_total_games(player_id) + 1;
}

std::string DataHandler::get_colour(uint64_t player_id) const
{
    std::string id = std::to_string(player_id);

    if (!data.contains("players") || !data["players"].contains(id))
        return "#FFFFFF";

    return data["players"][id].value("colour", "#FFFFFF");
}

void DataHandler::set_colour(uint64_t player_id, const std::string& colour)
{
    ensure_player(player_id);
    data["players"][std::to_string(player_id)]["colour"] = colour;
}

std::string DataHandler::get_name(uint64_t player_id) const
{
    std::string id = std::to_string(player_id);
    if (!data.contains("players") || !data["players"].contains(id))
        return "Unknown";
    return data["players"][id].value("name", "Unknown");
}

void DataHandler::set_name(uint64_t player_id, const std::string& name)
{
    ensure_player(player_id);
    data["players"][std::to_string(player_id)]["name"] = name;
}

std::vector<std::pair<uint64_t, int>> DataHandler::get_all_players() const
{
    std::vector<std::pair<uint64_t, int>> players;

    for (auto& el : data["players"].items())
    {
        uint64_t id  = std::stoull(el.key());
        int      elo = el.value().value("elo", 1200);
        players.push_back({id, elo});
    }

    std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return players;
}