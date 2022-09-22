#ifndef MINIMAX_HPP_
# define MINIMAX_HPP_

#include <vector>
#include "utils.hpp"

extern ssize_t DIRECTIONS[8][2];

class Minimax
{

public:
  Minimax(size_t num_threads);
  ~Minimax();

  std::vector<t_move_eval> run(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures);

private:

  void init_game_states(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures);

  size_t         _num_threads;
  t_game_state*  _game_state;
};

#endif // MINIMAX_HPP_
