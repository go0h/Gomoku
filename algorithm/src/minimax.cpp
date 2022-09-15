
#include "Gomoku.hpp"
#include <iostream>

t_coord Gomoku::min_max() {

  t_possible_moves possible_moves = _get_possible_moves(_player);

  t_move_eval best_move = min_max(_board, _difficult, _player, (_player == WHITE ? BLACK : WHITE));

  return { best_move.x, best_move.y };
}


Gomoku::t_move_eval Gomoku::min_max(Board& state, size_t depth, t_color player, t_color opponent) {

  t_possible_moves possible_moves = _get_possible_moves(player);

  double best_score = evaluate_state(state, player);

  t_move_eval best = { best_score, 0, 0 };

  if (!depth || possible_moves.empty()) {
    return best;
  }

  best = possible_moves[0];

  for (t_move_eval& move: possible_moves) {

    state(move.x, move.y) = player;

    t_move_eval me = min_max(state, depth - 1, opponent, player);
    state(move.x, move.y) = EMPTY;

    if (player == _player) {
      if (me.score > best_score)
        best = { best_score, move.x, move.y };
    } else {
      if (me.score < best_score)
        best = { best_score, move.x, move.y };
    }
  }
  return best;
}
