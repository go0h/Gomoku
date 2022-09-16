
#include "Gomoku.hpp"
#include <iostream>


MakeTurn* Gomoku::minimax() {

  t_coord best;
  t_color opponent = (_player == WHITE ? BLACK : WHITE);

  t_possible_moves& possible_moves = _get_possible_moves(_difficult, _player);

	double low = MINUS_INF;

  t_move_eval best_move = possible_moves[0];


  for (t_move_eval move: possible_moves) {

    _set_move_and_catch(_board, _difficult, move.x, move.y, _player);

    best_move = minimax(_board, _difficult - 1, opponent, _player, low, PLUS_INF);

    _remove_move_and_catches(_board, _difficult, move.x, move.y, _player);

    if (best_move.score > low) {
      low = best_move.score;
      best = { move.x, move.y };
    }
  }
  _set_move_and_catch(_board, _difficult, best.x, best.y, _player);


  // TODO
  MakeTurn* m = new MakeTurn();

  m->position = _board.coord_to_pos(best);
  m->color = _color2str[_player];
  for (t_move_eval move: possible_moves) {
    if (move.x != best.x || move.y != best.y)
      m->hints.push_back(_board.coord_to_pos(move.x, move.y));
  }
  for (size_t pos: _depth_state[_difficult].depth_catches) {
    std::string capture = _board.coord_to_pos(pos % _board.getSide(), pos / _board.getSide());
    std::cout << capture << std::endl;
    m->captures.push_back(capture);
  }
  return m;
}

/** MINIMAX with AlphaBetta */
Gomoku::t_move_eval Gomoku::minimax(Board& state, size_t depth, t_color player, t_color opponent, double low, double high) {

  t_possible_moves& possible_moves = _get_possible_moves(depth, player);

  double best_score = evaluate_state(state, player);

  t_move_eval best = { best_score, 0, 0 };

  if (!depth || possible_moves.empty()) {
    return best;
  }

  best = possible_moves[0];

  for (t_move_eval move: possible_moves) {

    _set_move_and_catch(state, depth, move.x, move.y, player);

    t_move_eval me = minimax(state, depth - 1, opponent, player, -high, -low);

    if (-me.score > best_score) {
      low = -me.score;
      best = { low, move.x, move.y };
    }

    _remove_move_and_catches(state, depth, move.x, move.y, player);

    if (low >= high)
      return best;
  }
  return best;
}
