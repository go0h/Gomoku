#ifndef MINIMAX_HPP_
# define MINIMAX_HPP_

#include "ThreadPool.hpp"
#include "utils.hpp"

extern int DIRECTIONS[8][2];

class Minimax
{

public:
  Minimax(size_t num_threads);
  ~Minimax();

  std::vector<t_move_eval> run(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures);

private:

  void                     _init_game_states(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures);
  std::vector<t_move_eval> _get_sorted_moves(size_t depth);

  size_t         _num_threads;
  ThreadPool     _pool;
  t_game_state*  _game_state;
};

#endif // MINIMAX_HPP_
