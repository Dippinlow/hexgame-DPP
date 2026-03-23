#include "game.h"
//#include <iostream>
#include <cstdlib>
#include <ctime>

// ----------------- Constructor -----------------
Game::Game(uint64_t channel_id_, uint64_t player1_id, uint64_t player2_id)
    : channel_id(channel_id_)
{
    players[0] = player1_id;
    players[1] = player2_id;
    
    axis_swapped = (std::rand() % 2) == 0;
    turn_index = (std::rand() % 2) == 0;

    for (int i = 0; i < 125; i++)
    {
        parent1[i]  = i;
        parent2[i]  = i;
        uf_rank1[i] = 0;
        uf_rank2[i] = 0;
    }

    for (int k = 0; k < 11; k++)
    {
        uf_unite(parent1, uf_rank1, k,        I_START);
        uf_unite(parent1, uf_rank1, 110 + k,  I_END);
        uf_unite(parent2, uf_rank2, k * 11,   J_START);
        uf_unite(parent2, uf_rank2, k * 11 + 10, J_END);
    }
    last_move_time = std::time(nullptr);
}

// ----------------- Accessors -----------------
uint64_t Game::get_channel_id() const {
    return channel_id;
}

uint64_t Game::get_current_player() const {
    return players[turn_index];
}

uint64_t Game::get_player1() const { return players[0]; }
uint64_t Game::get_player2() const { return players[1]; }

bool Game::is_over() const {
    return game_over;
}
uint64_t Game::get_winner_id() const {
    if (winner == -1) return 0;
    return players[winner];
}
uint64_t Game::get_loser_id() const {
    if (winner == -1) return 0;
    return players[1 - winner];
}

RenderData Game::get_render_data() const
{
    RenderData rd;
    rd.board        = board;
    rd.channel_id   = channel_id;
    rd.turn         = turn_index;
    rd.axis_swapped = axis_swapped;
    rd.game_over    = game_over;
    rd.winner       = winner;
    rd.last_i       = last_i;
    rd.last_j       = last_j;

    return rd;
}

int Game::get_move_count() const
{
    return move_count;
}

std::time_t Game::get_last_move_time() const
{
    return last_move_time;
}

bool Game::is_empty(int i, int j) const
{
    return board[i][j] == EMPTY;
}

// ----------------- Move handling -----------------
bool Game::validate_move(uint64_t player_id, int col, int row) const
{
    if(player_id != get_current_player()) return false;
    if (col < 0 || col >= 11 || row < 0 || row >= 11) return false;
    if (!is_empty(col, row)) return false;
    return true;
}

bool Game::make_move(uint64_t player_id, int i, int j)
{
    if (!validate_move(player_id, i, j)) return false;

    int p = turn_index;
    last_i = i;
    last_j = j;

    board[i][j] = (p == 0) ? PLAYER1 : PLAYER2;

    int* par = (p == 0) ? parent1 : parent2;
    if (axis_swapped) par = (p == 0) ? parent2 : parent1;

    int* rnk = (p == 0) ? uf_rank1 : uf_rank2;
    if (axis_swapped) rnk = (p == 0) ? uf_rank2 : uf_rank1;
    Cell mine = (p == 0) ? PLAYER1  : PLAYER2;

    int di[] = { 1, -1,  0,  0, -1,  1};
    int dj[] = { 0,  0,  1, -1,  1, -1};

    for (int n = 0; n < 6; n++)
    {
        int ni = i + di[n];
        int nj = j + dj[n];
        if (ni >= 0 && ni < 11 && nj >= 0 && nj < 11 && board[ni][nj] == mine)
            uf_unite(par, rnk, i * 11 + j, ni * 11 + nj);
    }

    check_win();
    if (!game_over)
    {
        last_move_time = std::time(nullptr);
        next_turn();

        move_count++;
    } 

    return true;
}

Move Game::get_random_move() const
{
    Move moves[121];
    int count = 0;
    for (int i = 0; i < 11; i++)
        for (int j = 0; j < 11; j++)
            if (board[i][j] == EMPTY)
                moves[count++] = {i, j};

    return moves[std::rand() % count];
}

void Game::swap_players()
{
    uint64_t tmp = players[0];
    players[0] = players[1];
    players[1] = tmp;
    //axis_swapped  = !axis_swapped;
    move_count++;
}

// ----------------- Turn handling -----------------
void Game::next_turn() {
    turn_index = (turn_index + 1) % 2;
}

// ----------------- Win check -----------------
void Game::check_win()
{
    int p = turn_index;

    int* par = (p == 0) ? parent1 : parent2;
    if (axis_swapped) par = (p == 0) ? parent2 : parent1;

    int vs = (p == 0) ? I_START : J_START;
    int ve = (p == 0) ? I_END   : J_END;
    if (axis_swapped)
    {
        vs = (p == 0) ? J_START : I_START;
        ve = (p == 0) ? J_END   : I_END;
    }

    if (uf_find(par, vs) == uf_find(par, ve))
    {
        game_over = true;
        winner = p;
    }
}

int Game::uf_find(int* parent, int a)
{
    if (parent[a] != a)
    {
        parent[a] = uf_find(parent, parent[a]);
    }
    return parent[a];
}

void Game::uf_unite(int* parent, int* uf_rank, int a, int b)
{
    int ra = uf_find(parent, a);
    int rb = uf_find(parent, b);
    if (ra == rb) return;

    if (uf_rank[ra] < uf_rank[rb])
    {
        int tmp = ra;
        ra = rb;
        rb = tmp;
    }
    parent[rb] = ra;
    if (uf_rank[ra] == uf_rank[rb]) uf_rank[ra]++;
}