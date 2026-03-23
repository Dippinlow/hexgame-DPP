#pragma once

#include <dpp/dpp.h>
#include "gameManager.h"
#include "dataHandler.h"
#include "graphicsHandler.h"


bool validate_move(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err);
bool validate_challenge(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err, uint64_t bot_id);
bool validate_swap(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err);

dpp::message apply_move(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id);
dpp::message apply_challenge(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id);
dpp::message apply_swap(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id);

dpp::message handle_forfeit(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh);
dpp::message handle_stats(const dpp::slashcommand_t& event, DataHandler& dh);
dpp::message handle_setcolour(const dpp::slashcommand_t& event, DataHandler& dh);
dpp::message handle_setname(const dpp::slashcommand_t& event, DataHandler& dh);
dpp::message handle_leaderboard(const dpp::slashcommand_t& event, DataHandler& dh);
dpp::message handle_game_over(uint64_t channel_id, uint64_t winner_id, uint64_t loser_id, GameManager& gm, DataHandler& dh);