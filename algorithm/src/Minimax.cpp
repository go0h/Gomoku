
#include "Minimax.hpp"


Minimax::Minimax(Board state, t_color player, size_t depth)
  : _state(state),
    _player(player),
    _depth(depth),
    _possible_moves(t_possible_moves(state.getSide() * state.getSide())) { }


Minimax::~Minimax() { }


t_coord Minimax::min_max() {

  t_move_eval best_move = min_max(_state, _depth, _player, (_player == WHITE ? BLACK : WHITE));

  return best_move.coord;
}


Minimax::t_move_eval Minimax::min_max(Board& state, size_t depth, t_color player, t_color opponent) {

  fill_possible_moves(player);

  t_coord coord = {0, 0};

  double best_score = evaluate_state(state, player);

  t_move_eval best = { best_score, coord };

  if (!depth || _possible_moves.empty()) {
    return best;
  }

  best.coord = _possible_moves[0];

  for (t_coord move: _possible_moves) {

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

  return best;
}
