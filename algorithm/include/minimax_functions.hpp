
#ifndef MINIMAX_MULTITHREAD_HPP
# define MINIMAX_MULTITHREAD_HPP

# define PLUS_INF  1000000
# define MINUS_INF -1000000

#include "utils.hpp"


// double        evaluate_state(Board& state, size_t& is_win, t_color player, bool is_player_turn);

int evaluate_state(Board &state, const t_color & player, const size_t restrictions[4]) ;

void          minimax_thread_f(void* gs_ptr, int player, size_t depth);

double        negamax_thread(t_game_state* gs, size_t depth,
                             t_color player, t_color opponent,
                             double alpha, double beta, double moveScore);

void          set_move_and_catch(t_game_state* gs, size_t depth, size_t x, size_t y, t_color player);

void          remove_move_and_captures(t_game_state* gs, size_t depth, size_t x, size_t y, t_color player);

t_move_eval*  get_possible_moves(t_game_state* gs, size_t depth, t_color player);

t_move_eval   get_random_move(t_point* field, size_t side, t_color player);

void setRestrictions(const t_point * field, const size_t &side, size_t restrictions[4]) ;

double evalute_move(t_point * field, const size_t &side, const t_color & player, size_t & y, size_t & x, const int capture[3]);

#endif // MINIMAX_MULTITHREAD_HPP
