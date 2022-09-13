
#include "Minimax.hpp"


Minimax::Minimax(Board state, t_color player, size_t depth)
  : _state(state),
    _player(player),
    _depth(depth) { }


Minimax::~Minimax() { }


t_coord Minimax::min_max(size_t depth) {

  t_move_eval best_move = min_max(_state, depth, _player, (_player == WHITE ? BLACK : WHITE));

  return best_move.coord;
}


Minimax::t_move_eval Minimax::min_max(Board& state, size_t depth, t_color player, t_color opponent) {

  t_coord coord = {0, 0};
  double best_score = -1.0;
  t_move_eval best = { best_score, coord };

  if (!depth)
    return best;

  t_possible_moves* possible_moves = get_possible_moves(state);

  if (!possible_moves->size()) {
    return best;
  }

  best.coord = *possible_moves->begin();

  best_score = evaluate_state(state, player);

  for (auto it = possible_moves->begin(); it != possible_moves->end(); ++it) {

    t_coord move = *it;

    state(move) = player;

    t_move_eval me = min_max(state, depth - 1, opponent, player);
    state(move) = EMPTY;

    if (player == _player) {
      if (me.score > best_score)
        best = { best_score, move };
    } else {
      if (me.score < best_score)
        best = { best_score, move };
    }
  }

  delete possible_moves;

  return best;
}


//TODO find possible moves
Minimax::t_possible_moves* get_possible_moves(Board& state) {

  Minimax::t_possible_moves* possible_moves = new Minimax::t_possible_moves();

  for (size_t y = 0; y < state.getSide(); ++y) {
    for (size_t x = 0; x < state.getSide(); ++x) {
      if (state(x, y) == EMPTY) {
        possible_moves->push_back({x, y});
      }
    }
  }

  return possible_moves;
}

#include <ctime>
#include <random>

// TODO TEST ONLY
double evaluate_state(Board& state, t_color player) {

  state.getSide();
  player = (player == WHITE) ? BLACK : WHITE;

  static std::default_random_engine re {};
  using Dist = std::uniform_int_distribution<int>;

  static Dist uid {};

  return uid(re, Dist::param_type{0,100000000});
}
