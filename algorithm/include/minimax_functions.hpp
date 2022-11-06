
#ifndef MINIMAX_MULTITHREAD_HPP
# define MINIMAX_MULTITHREAD_HPP

// # define PLUS_INF  1000000
// # define MINUS_INF -1000000
// # define MAX_MOVES 7
// # define EXPANSION_STEP 2
// # define WIN_DETECTED -1

#include "utils.hpp"

int evaluate_state(Board &state, const t_color & player, const size_t restrictions[4]) ;

void          minimax_thread_f(void* gs_ptr, int player, size_t depth, size_t restrictions[4]);

int        negamax_thread(t_game_state* gs, const size_t & depth, const t_color  & player, const t_color & opponent,
                             int alpha, const int beta, const int & moveScore, const size_t restrictions[4]);

void          set_move_and_catch(t_game_state* gs, const size_t & depth, const size_t & x, const size_t & y, const t_color & player, const t_color & opponent, const bool & is_catch);

void          remove_move_and_captures(t_game_state* gs, const size_t & depth, const  size_t & x, const size_t & y, const t_color & player, const t_color & opponent, const bool & is_catch);

t_move_eval*  get_possible_moves(t_game_state* gs, const size_t & depth, const t_color & player, const t_color & opponent, const size_t restrictions[4]);

t_move_eval   get_random_move(t_point* field, const size_t & side, const t_color & player, const t_color & opponent);

void setRestrictions(const t_point * field, const size_t &side, size_t restrictions[4]) ;

double evalute_move(t_point * field, const size_t &side, const t_color & player, const t_color & opponent, size_t & y, size_t & x, const int capture[3], bool & is_catch);

#endif // MINIMAX_MULTITHREAD_HPP
