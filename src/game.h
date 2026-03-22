#pragma once
#include <cstdint>
#include <ctime>
#include "renderData.h"

enum Cell : unsigned char {
    EMPTY = 0,
    PLAYER1 = 1,
    PLAYER2 = 2
};
struct Move {
    int i, j;
};

class Game 
{
public:
    Game(uint64_t channel_id, uint64_t player1_id, uint64_t player2_id);

    uint64_t get_channel_id() const;
    uint64_t get_current_player() const;

    uint64_t get_player1() const;
    uint64_t get_player2() const;

    bool is_over() const;
    bool is_empty(int i, int j) const;
    
    uint64_t get_winner_id() const; // 0 if no winner yet
    uint64_t get_loser_id() const; // 0 if no winner yet

    RenderData get_render_data() const;

    bool make_move(uint64_t player_id, int col, int row);

    int get_move_count() const;
    
    std::time_t get_last_move_time() const;

    Move get_random_move() const;

    void swap_players();


private:
    uint64_t channel_id;
    uint64_t players[2];

    int turn_index = 0;    // 0 = player1, 1 = player2
    bool game_over = false;
    bool axis_swapped = false;
    int winner = -1;

    int last_i = -1;
    int last_j = -1;

    int move_count = 0;

    std::time_t last_move_time;

    unsigned char board[11][11] = {{EMPTY}};

    bool validate_move(uint64_t player_id, int col, int row) const;
    void check_win();
    void next_turn();

    int parent1[125];
    int parent2[125];
    int uf_rank1[125];
    int uf_rank2[125];

    inline static const int I_START = 121;
    inline static const int I_END   = 122;
    inline static const int J_START = 123;
    inline static const int J_END   = 124;

    int uf_find(int* parent, int x);
    void uf_unite(int* parent, int* uf_rank, int x, int y);
};