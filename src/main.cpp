#include <dpp/dpp.h>
#include <ctime>
#include <cstdlib>
#include <filesystem>

#include "gameManager.h"
#include "dataHandler.h"
#include "graphicsHandler.h"

#include "commands.h"

static GameManager gameManager;
static DataHandler dataHandler("../data/players.json");
static GraphicsHandler graphicsHandler;

uint64_t BOT_ID = 0;

void update_presence(dpp::cluster& bot, GameManager& gm)
{
    int count = gm.get_active_game_count();
    std::string status = std::to_string(count) + " active game";
    if (count != 1) status += "s";
    bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_game, status));
}

int main() 
{
    std::srand(std::time(nullptr));

    std::filesystem::create_directories("../data");
    std::filesystem::create_directories("../temp");

    const char* token_env = std::getenv("HEXBOT_TOKEN");
    if(!token_env){
        std::cout << "Failed to retrieve bot token" << std::endl;
        return 1;
    }

    dataHandler.load();
    
    dpp::cluster bot(token_env);

    bot.on_log(dpp::utility::cout_logger());
 
    bot.on_slashcommand([&](const dpp::slashcommand_t& event) {
    
        
    if (event.command.get_command_name() == "challenge")
    {
        dpp::message error;
        if(!validate_challenge(event, gameManager, error, BOT_ID))
        {
            event.reply(error);
            return;
        }
        event.thinking();
        event.edit_original_response(apply_challenge(event, gameManager, dataHandler, graphicsHandler, BOT_ID));
        update_presence(bot, gameManager);
    }
    else if (event.command.get_command_name() == "move")
    {
        dpp::message error;
        if(!validate_move(event, gameManager, error))
        {
            event.reply(error);
            return;
        }
        event.thinking();
        event.edit_original_response(apply_move(event, gameManager, dataHandler, graphicsHandler, BOT_ID));
        update_presence(bot, gameManager);
    }
        
    else if (event.command.get_command_name() == "colour")
    {
        event.thinking(true);
        event.edit_original_response(handle_setcolour(event, dataHandler));
    }
    else if (event.command.get_command_name() == "forfeit")
    {
        event.reply(handle_forfeit(event, gameManager, dataHandler));
        update_presence(bot, gameManager);
    }
    else if (event.command.get_command_name() == "stats")
    {
        event.thinking();
        event.edit_original_response(handle_stats(event, dataHandler));
    }
    else if (event.command.get_command_name() == "leaderboard")
    {
        event.thinking();
        event.edit_original_response(handle_leaderboard(event, dataHandler));
    }
    else if (event.command.get_command_name() == "name")
    {
        event.thinking(true);
        event.edit_original_response(handle_setname(event, dataHandler));
    }
    else if(event.command.get_command_name() == "swap")
    {
        dpp::message error;
        if(!validate_swap(event, gameManager, error))
        {
            event.reply(error);
            return;
        }
        event.thinking();
        event.edit_original_response(apply_swap(event, gameManager, dataHandler, graphicsHandler));
    }
    else if (event.command.get_command_name() == "help")
    {
        std::string help =
        "**How to play Hex**\n\n"
        "**Goal:** Connect your two coloured edges with an unbroken chain of hexagons.\n\n"
        "**Commands:**\n"
        "`/challenge @player`: start a game\n"
        "`/move A5`: place a hex (letter A-K, number 1-11)\n"
        "`/forfeit`: resign the current game\n"
        "`/setcolour #FF0013`: set your hex colour\n"
        "`/setname YourName`: set your display name\n"
        "`/stats`: view your stats\n"
        "`/stats @player`: view another player's stats\n\n"
        "Your border colour shows which edges you must connect. "
        "The bright border indicates whose turn it is.";

        event.reply(dpp::message(help).set_flags(dpp::m_ephemeral));
    }
    });
 
    bot.on_ready([&bot](const dpp::ready_t&) 
    {
        update_presence(bot, gameManager);
        BOT_ID = bot.me.id;
        
        if (dpp::run_once<struct register_bot_commands>()) 
        {
            dpp::slashcommand challenge("challenge", "Challenge another player.", bot.me.id);
            challenge.add_option(dpp::command_option(dpp::co_user, "player", "The other player.", true));

            dpp::slashcommand move_cmd("move", "Make a move.", bot.me.id);
            move_cmd.add_option(dpp::command_option(dpp::co_string, "position", "e.g, A5.", true));

            dpp::slashcommand setcolour_cmd("colour", "Set your hex colour.", bot.me.id);
            setcolour_cmd.add_option(dpp::command_option(dpp::co_string, "colour", "Hex colour e.g. #FF0013", true));

            dpp::slashcommand forfeit_cmd("forfeit", "End the game", bot.me.id);

            dpp::slashcommand stats_cmd("stats", "View player stats.", bot.me.id);
            stats_cmd.add_option(dpp::command_option(dpp::co_user, "player", "Player to view stats for.", false));

            dpp::slashcommand setname_cmd("name", "Set your display name.", bot.me.id);
            setname_cmd.add_option(dpp::command_option(dpp::co_string, "name", "Your display name.", true));

            dpp::slashcommand help_cmd("help", "Learn how to play.", bot.me.id);

            dpp::slashcommand leaderboard_cmd("leaderboard", "Top 10 players.", bot.me.id);

            dpp::slashcommand swap_cmp("swap", "Steal the first move.", bot.me.id);

            bot.global_bulk_command_create({challenge, move_cmd, setcolour_cmd, forfeit_cmd, stats_cmd, 
                                                    setname_cmd, help_cmd, leaderboard_cmd, swap_cmp});
        }
    });

    

    bot.start_timer([&bot](const dpp::timer){
        std::vector<uint64_t> timed_out = gameManager.get_timed_out_games(57600);
        for (uint64_t channel_id : timed_out)
        {
            bot.message_create(dpp::message(channel_id, "Game timed out after 16 hours."));
            gameManager.end_game(channel_id);
        }
        if(!timed_out.empty()) update_presence(bot, gameManager);
    }, 3600);
    
    bot.start(dpp::st_wait);
}