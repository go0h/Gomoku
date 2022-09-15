

#ifndef MINIMAX_HPP_
# define MINIMAX_HPP_

#include <vector>
#include <memory>
#include "Board.hpp"
#include "utils.hpp"


class Minimax
{
public:

  using t_possible_moves = std::vector<t_coord>;

  typedef struct s_move_eval
  {
    double    score;
    t_coord   coord;
  }              t_move_eval;


  Minimax(Board state, t_color player, size_t depth);
  ~Minimax();

  t_coord     min_max();
  t_move_eval min_max(Board& state, size_t depth, t_color player, t_color opponent);

  t_possible_moves get_possible_moves(t_color player);

private:

  bool   _not_forbidden(size_t x, size_t y, t_color player);
  bool   _is_possible_capture(size_t x, size_t y, t_color player);
  size_t _get_num_of_free_threes(size_t x, size_t y, t_color player);

  Board                _state;
  t_color              _player;
  size_t               _depth;
  t_possible_moves     _possible_moves;

};

double evaluate_state(Board& state, t_color player);

#endif // MINIMAX_HPP_
