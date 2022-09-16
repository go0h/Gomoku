
#include "Gomoku.hpp"
#include <iostream>


MakeTurn* Gomoku::minimax() {

  t_coord best;
  t_color opponent = (_player == WHITE ? BLACK : WHITE);

  t_possible_moves& possible_moves = _get_possible_moves(_difficult, _player);

	double low = MINUS_INF;

  t_move_eval best_move = { low, 0, 0 };


  for (t_move_eval move: possible_moves) {

    _set_move_and_catch(_board, _difficult, move.x, move.y, _player);

    best_move = minimax(_board, _difficult - 1, opponent, _player, low, PLUS_INF);

    if (_depth_state[_difficult].depth_catches.size()) {
      best_move.score += 10000;
    }

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

  double best_score = evaluate_state(state, depth, player, opponent);

  t_move_eval best = { best_score, 0, 0 };

  if (!depth || possible_moves.empty()) {
    return best;
  }

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
