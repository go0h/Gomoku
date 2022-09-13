

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

  t_coord     min_max(size_t depth);
  t_move_eval min_max(Board& state, size_t depth, t_color player, t_color opponent);

private:

  Board        _state;
  t_color      _player;
  size_t       _depth;
};


Minimax::t_possible_moves* get_possible_moves(Board& state);

double evaluate_state(Board& state, t_color player);

#endif // MINIMAX_HPP_
