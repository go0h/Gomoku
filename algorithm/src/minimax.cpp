
#include "Gomoku.hpp"
#include <iostream>


MakeTurn* Gomoku::minimax() {

  t_coord best;
  t_color opponent = (_player == WHITE ? BLACK : WHITE);

  t_possible_moves possible_moves = _get_possible_moves(_player);

	double low = MINUS_INF;

  t_move_eval best_move = possible_moves[0];

  for (t_move_eval move: possible_moves) {

    _board(move.x, move.y) = _player;

    best_move = minimax(_board, _difficult - 1, opponent, _player, low, PLUS_INF);

    _board(move.x, move.y) = EMPTY;

    if (best_move.score > low) {
      low = best_move.score;
      best = { move.x, move.y };
    }
  }

  // TODO
  MakeTurn* m = new MakeTurn();

  m->position = _board.coord_to_pos(best);
  m->color = _color2str[_player];
  for (t_move_eval move: possible_moves) {
    if (move.x != best.x || move.y != best.y)
      m->hints.push_back(_board.coord_to_pos(move.x, move.y));
  }
  return m;
}

/** MINIMAX with AlphaBetta */
Gomoku::t_move_eval Gomoku::minimax(Board& state, size_t depth, t_color player, t_color opponent, double low, double high) {

  t_possible_moves possible_moves = _get_possible_moves(player);

  double best_score = evaluate_state(state, player);

  t_move_eval best = { best_score, 0, 0 };

  if (!depth || possible_moves.empty()) {
    return best;
  }

  best = possible_moves[0];

  for (t_move_eval move: possible_moves) {

    state(move.x, move.y) = player;

    t_move_eval me = minimax(state, depth - 1, opponent, player, -high, -low);
    state(move.x, move.y) = EMPTY;

    if (-me.score > best_score) {
      low = -me.score;
      best = { low, move.x, move.y };
    }
    if (low >= high)
      return best;
  }
  return best;
}
