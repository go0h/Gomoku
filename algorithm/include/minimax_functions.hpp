
#ifndef MINIMAX_MULTITHREAD_HPP
# define MINIMAX_MULTITHREAD_HPP

# define PLUS_INF  (1.0 / 0.0)
# define MINUS_INF (-1.0 / 0.0)

#include "utils.hpp"


double        evaluate_state(Board& state, size_t& is_win, t_color player, bool is_player_turn);

t_coord       minimax_thread(t_game_state& gs, t_color player, size_t depth);

double        minimax_thread(s_game_state& gs, size_t depth,
                             t_color player, t_color opponent,
                             double alpha, double betta,
                             bool is_player_turn);

void          set_move_and_catch(t_game_state& gs, size_t depth, size_t x, size_t y, t_color player);

void          remove_move_and_captures(t_game_state& gs, size_t depth, size_t x, size_t y, t_color player);

t_move_eval*  get_possible_moves(t_game_state& gs, size_t depth, t_color player);

t_move_eval   get_random_move(t_point* field, size_t side, t_color player);


#endif // MINIMAX_MULTITHREAD_HPP
