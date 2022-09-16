
#include <iostream>
#include <random>
#include "Gomoku.hpp"


// TODO TEST ONLY
double evaluate_state(Board& state, t_color player) {

  state.getSide();
  player = (player == WHITE) ? BLACK : WHITE;

  static std::default_random_engine re {};
  using Dist = std::uniform_int_distribution<int>;

  static Dist uid {};

  return uid(re, Dist::param_type{0, 100000000});
}
