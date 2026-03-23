#include "commands.h"


//-------------- helpers -------------
std::string image_url = "https://raw.githubusercontent.com/Dippinlow/hexgame-DPP/refs/heads/main/assets/image.png";
std::string get_display_name(const dpp::user& user)
{
    if (!user.global_name.empty()) return user.global_name;
    return user.username;
}

std::string render_board(const Game* game, DataHandler& dh, GraphicsHandler& gh)
{
    RenderData rd = game->get_render_data();
    std::string colour1 = dh.get_colour(game->get_player1());
    std::string colour2 = dh.get_colour(game->get_player2());
    std::string name1   = dh.get_name(game->get_player1());
    std::string name2   = dh.get_name(game->get_player2());
    return gh.render(rd, colour1, colour2, name1, name2);
}

dpp::message handle_game_cancel(uint64_t channel_id, GameManager& gm)
{
    gm.end_game(channel_id);
    return dpp::message("Game cancelled. No rating changes.");
}

//------------ validate commands ---------------

bool validate_challenge(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err, uint64_t bot_id)
{
    uint64_t player1    = event.command.get_issuing_user().id;
    uint64_t channel_id = event.command.channel_id;
    auto     player2    = std::get<dpp::snowflake>(event.get_parameter("player"));

    if (player1 == player2)
    {
        err = dpp::message("You can't challenge yourself.").set_flags(dpp::m_ephemeral);
        return false;
    }

    auto it = event.command.resolved.users.find(player2);
    if (it != event.command.resolved.users.end() && it->second.is_bot() && player2 != bot_id)
    {
        err = dpp::message("You can't challenge a bot.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (!gm.create_game(channel_id, player1, player2))
    {
        err = dpp::message("A game already exists in this channel.").set_flags(dpp::m_ephemeral);
        return false;
    }

    return true;
}

bool validate_move(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err)
{
    uint64_t channel_id = event.command.channel_id;
    uint64_t player     = event.command.get_issuing_user().id;

    Game* game = gm.get_game(channel_id);
    if (!game){
        err = dpp::message("No current game in this channel.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (game->get_player1() != player && game->get_player2() != player)
    {
        err = dpp::message("You are not a player in this game.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (game->get_current_player() != player){
        err = dpp::message("It's not your turn.").set_flags(dpp::m_ephemeral);
        return false;
    }

    std::string pos = std::get<std::string>(event.get_parameter("position"));

    if (!isalpha(pos[0]) || !isdigit(pos[1])){
        err = dpp::message("Invalid position.").set_flags(dpp::m_ephemeral);
        return false;
    }

    int col = toupper(pos[0]) - 'A';
    int row = std::stoi(pos.substr(1)) - 1;

    if (col < 0 || col > 10 || row < 0 || row > 10){
        err = dpp::message("Invalid position: use A-K and 1-11, e.g. E5.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (!game->is_empty(col, row)){
        err = dpp::message("Invalid move: that cell is already occupied.").set_flags(dpp::m_ephemeral);
        return false;
    }

    return true;
}

bool validate_swap(const dpp::slashcommand_t& event, GameManager& gm, dpp::message& err)
{
    uint64_t channel_id = event.command.channel_id;
    uint64_t player_id  = event.command.get_issuing_user().id;

    Game* game = gm.get_game(channel_id);
    if (!game){
        err = dpp::message("No current game in this channel.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (game->get_player1() != player_id && game->get_player2() != player_id){
        err = dpp::message("You are not a player in this game.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (game->get_move_count() != 1){
        err = dpp::message("Swap is only valid after the first move.").set_flags(dpp::m_ephemeral);
        return false;
    }

    if (game->get_current_player() != player_id){
        err = dpp::message("It's not your turn.").set_flags(dpp::m_ephemeral);
        return false;
    }

    return true;
}

// apply commands

dpp::message apply_challenge(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id)
{
    bool name_dirty = false;
    uint64_t player1    = event.command.get_issuing_user().id;
    if (dh.get_name(player1) == "Unknown")
    {
        dh.set_name(player1, get_display_name(event.command.get_issuing_user()));
        name_dirty = true;
    }

    dpp::snowflake player2 = std::get<dpp::snowflake>(event.get_parameter("player"));
    auto it = event.command.resolved.users.find(player2);
    if (it != event.command.resolved.users.end())
    {
        const dpp::user& player2_user = it->second;
        if (dh.get_name(player2) == "Unknown")
        {
            dh.set_name(player2, get_display_name(player2_user));
            name_dirty = true;
        }
    }
    if(name_dirty) dh.save();

    uint64_t channel_id = event.command.channel_id;

    Game* game = gm.get_game(channel_id);

    uint64_t first_player = game->get_current_player();

    dpp::message msg;

    if (game->get_current_player() == bot_id)
    {
        Move m = game->get_random_move();
        game->make_move(bot_id, m.i, m.j);
        msg.content = "Game started! HexBot went first. <@" + std::to_string(game->get_current_player()) + ">'s turn.";
    }
    else
    {
        msg.content = "Game started! <@" + std::to_string(first_player) + "> is going first.";
    }


    std::string path = render_board(game, dh, gh);
    msg.allowed_mentions.parse_users = true;
    msg.add_file("board.png", dpp::utility::read_file(path));
    return msg;
}

dpp::message apply_move(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id)
{
    uint64_t channel_id = event.command.channel_id;
    uint64_t player     = event.command.get_issuing_user().id;

    Game* game = gm.get_game(channel_id);

    std::string pos = std::get<std::string>(event.get_parameter("position"));
    int col = toupper(pos[0]) - 'A';
    int row = std::stoi(pos.substr(1)) - 1;

    game->make_move(player, col, row);

    if (!game->is_over() && game->get_current_player() == bot_id)
    {
        Move m = game->get_random_move();
        game->make_move(bot_id, m.i, m.j);
    }

    std::string path = render_board(game, dh, gh);

    if (game->is_over())
    {
        uint64_t winner_id = game->get_winner_id();
        uint64_t loser_id  = game->get_loser_id();
        return handle_game_over(channel_id, winner_id, loser_id, gm, dh);
    }

    dpp::message msg;
    msg.content = "<@" + std::to_string(game->get_current_player()) + ">'s turn.";
    msg.allowed_mentions.parse_users = true;
    msg.add_file("board.png", dpp::utility::read_file(path));
    return msg;
}

dpp::message apply_swap(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh, GraphicsHandler& gh, uint64_t bot_id)
{
    uint64_t channel_id = event.command.channel_id;
    uint64_t player_id  = event.command.get_issuing_user().id;

    Game* game = gm.get_game(channel_id);
    game->swap_players();

    if (game->get_current_player() == bot_id)
    {
        Move m = game->get_random_move();
        game->make_move(bot_id, m.i, m.j);
    }

    std::string path = render_board(game, dh, gh);

    dpp::message msg;
    msg.content = "<@" + std::to_string(player_id) + "> swapped! <@" + std::to_string(game->get_current_player()) + ">'s turn.";
    msg.allowed_mentions.parse_users = true;
    msg.add_file("board.png", dpp::utility::read_file(path));
    return msg;
}

dpp::message handle_game_over(uint64_t channel_id, uint64_t winner_id, uint64_t loser_id, GameManager& gm, DataHandler& dh)
{
    int winner_elo = dh.get_elo(winner_id);
    int loser_elo  = dh.get_elo(loser_id);

    double expected_winner = 1.0 / (1.0 + pow(10.0, (loser_elo - winner_elo) / 400.0));

    int k = 32;

    int delta = (int)round(k * (1.0 - expected_winner));
    int new_winner_elo = winner_elo + delta;
    int new_loser_elo  = loser_elo  - delta;

    dh.set_elo(winner_id, new_winner_elo);
    dh.set_elo(loser_id,  new_loser_elo);
    dh.add_win(winner_id);
    dh.add_game(winner_id);
    dh.add_game(loser_id);
    dh.save();

    dpp::message msg;

    std::string path = "../temp/board_" + std::to_string(channel_id) + ".png";

    msg.content  = "<@" + std::to_string(winner_id) + "> wins!\n";
    msg.content += "<@" + std::to_string(winner_id) + "> Rating: " + std::to_string(winner_elo) + " -> " + std::to_string(new_winner_elo) + "\n";
    msg.content += "<@" + std::to_string(loser_id)  + "> Rating: " + std::to_string(loser_elo)  + " -> " + std::to_string(new_loser_elo);
    msg.add_file("board.png", dpp::utility::read_file(path));
    msg.allowed_mentions.parse_users = true;
    gm.end_game(channel_id);

    return msg;
}

dpp::message handle_forfeit(const dpp::slashcommand_t& event, GameManager& gm, DataHandler& dh)
{
    uint64_t channel_id = event.command.channel_id;
    uint64_t player_id  = event.command.get_issuing_user().id;

    Game* game = gm.get_game(channel_id);
    if (!game)
        return dpp::message("No current game in this channel.").set_flags(dpp::m_ephemeral);

    if (game->get_player1() != player_id && game->get_player2() != player_id)
        return dpp::message("You are not a player in this game.").set_flags(dpp::m_ephemeral);

    if (game->get_move_count() < 2)
        return handle_game_cancel(channel_id, gm);

    uint64_t winner_id = (game->get_player1() == player_id) ? game->get_player2() : game->get_player1();

    return handle_game_over(channel_id, winner_id, player_id, gm, dh);
}
dpp::message handle_stats(const dpp::slashcommand_t& event, DataHandler& dh)
{
    uint64_t player_id = event.command.get_issuing_user().id;

    auto param = event.get_parameter("player");
    if (std::holds_alternative<dpp::snowflake>(param))
        player_id = std::get<dpp::snowflake>(param);

    int elo    = dh.get_elo(player_id);
    int wins   = dh.get_wins(player_id);
    int total  = dh.get_total_games(player_id);
    int losses = total - wins;
    std::string name   = dh.get_name(player_id);
    std::string colour = dh.get_colour(player_id);

    std::string winRate;
    char buf[16];
    if (total == 0) winRate = "N/A";
    else{
        snprintf(buf, sizeof(buf), "%.2f%%", (float)wins / (float)total * 100.0f);
        winRate = buf;
    }

    // convert #RRGGBB to integer
    std::string h = colour.substr(1);
    int colour_int = std::stoi(h, nullptr, 16);

    dpp::embed e;
    e.set_color(colour_int);
    e.set_title(name + "'s Stats");
    e.set_thumbnail(image_url);
    e.add_field("Rating",       std::to_string(elo),    true);
    e.add_field("Wins",         std::to_string(wins),   true);
    e.add_field("Losses",       std::to_string(losses), true);
    e.add_field("Total games",  std::to_string(total),  true);
    e.add_field("Win rate",     winRate,                 true);

    return dpp::message(event.command.channel_id, e);
}
dpp::message handle_leaderboard(const dpp::slashcommand_t& event, DataHandler& dh)
{
    auto players = dh.get_all_players();

    dpp::embed e;
    e.set_color(0x4F89FF);
    e.set_title("Hex Leaderboard");
    e.set_thumbnail(image_url);

    std::string board;
    int count = std::min((int)players.size(), 10);
    for (int i = 0; i < count; i++)
    {
        uint64_t id  = players[i].first;
        int      elo = players[i].second;
        std::string name = dh.get_name(id);
        board += std::to_string(i + 1) + ". " + name + ": " + std::to_string(elo) + "\n";
    }

    if (board.empty()) board = "No players yet.";
    e.add_field("Top Players", board, false);

    return dpp::message(event.command.channel_id, e);
}
dpp::message handle_setcolour(const dpp::slashcommand_t& event, DataHandler& dh)
{
    uint64_t player_id = event.command.get_issuing_user().id;
    std::string colour = std::get<std::string>(event.get_parameter("colour"));

    // basic validation
    if (colour.size() != 7 || colour[0] != '#')
        return dpp::message("Invalid colour: use hex format e.g. #FF0013.").set_flags(dpp::m_ephemeral);

    for (int i = 1; i < 7; i++)
        if (!isxdigit(colour[i]))
            return dpp::message("Invalid colour: use hex format e.g. #FF0013.").set_flags(dpp::m_ephemeral);

    dh.set_colour(player_id, colour);
    dh.save();
    return dpp::message("Colour updated to " + colour + ".").set_flags(dpp::m_ephemeral);
}

dpp::message handle_setname(const dpp::slashcommand_t& event, DataHandler& dh)
{
    uint64_t player_id = event.command.get_issuing_user().id;
    std::string name = std::get<std::string>(event.get_parameter("name"));

    dh.set_name(player_id, name);
    dh.save();
    return dpp::message("Name updated to " + name + ".").set_flags(dpp::m_ephemeral);
}
