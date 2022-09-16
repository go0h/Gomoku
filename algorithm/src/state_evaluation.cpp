
#include <iostream>
#include <random>
#include "Gomoku.hpp"


void Gomoku::_set_move_and_catch(Board& state, size_t depth, size_t x, size_t y, t_color player) {

  t_point*              field = state.getField();
  size_t                side = state.getSide();
  t_color               opponent = player == WHITE ? BLACK : WHITE;
  std::vector<size_t>&  catches = _depth_state[depth].depth_catches;

  catches.clear();
  state(x, y) = player;

  for (size_t i = 0; i < 8; ++i) {

    size_t x_dir = DIRECTIONS[i][0];
    size_t y_dir = DIRECTIONS[i][1];

    size_t x3 = x + x_dir * 3;
    size_t y3 = y + y_dir * 3;

    if (x3 < side && y3 < side && field[y3 * side + x3] == player) {
      size_t p2 = (y + y_dir * 2) * side + (x + x_dir * 2);
      size_t p1 = (y + y_dir * 1) * side + (x + x_dir * 1);

      if (field[p1] == opponent && field[p2] == opponent) {
        if (depth == _difficult) {
          std::cout << "Catch: " << state.coord_to_pos(p1 % side, p1 / side) << \
            ", " << state.coord_to_pos(p2 % side, p2 / side) << std::endl;
        }
        field[p1] = EMPTY;
        field[p2] = EMPTY;
        catches.push_back(p1);
        catches.push_back(p2);
      }
    }
  }
}

void Gomoku::_remove_move_and_catches(Board& state, size_t depth, size_t x, size_t y, t_color player) {

  t_point* field = state.getField();
  t_color opponent = player == WHITE ? BLACK : WHITE;
  std::vector<size_t>& catches = _depth_state[depth].depth_catches;

  state(x, y) = EMPTY;
  for (size_t pos: catches) {
    field[pos] = opponent;
  }
}


// TODO TEST ONLY
double evaluate_state(Board& state, t_color player) {

  state.getSide();
  player = (player == WHITE) ? BLACK : WHITE;

  static std::default_random_engine re {};
  using Dist = std::uniform_int_distribution<int>;

  static Dist uid {};

  return uid(re, Dist::param_type{0, 100000000});
}
