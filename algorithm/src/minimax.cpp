
#include "Gomoku.hpp"
#include <iostream>


static double get_catch_score(int n) {
	switch (n) {
		case 0:
			return 0;
		case 2:
			return 40;
		case 4:
			return 500;
		case 6:
			return 1000;
		case 8:
			return 2000;
		default:
			return 180000;
	}
}

MakeTurn* Gomoku::minimax() {

  t_coord best;
  double  alpha = MINUS_INF;
  double  score = alpha;
  t_color opponent = (_player == WHITE ? BLACK : WHITE);

  t_move_eval* possible_moves = _get_possible_moves(_difficult, _player);
  size_t       num_moves = _depth_state[_difficult].num_moves;

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval move = possible_moves[i];

    _set_move_and_catch(_board, _difficult, move.x, move.y, _player);

    score = minimax(_board, _difficult - 1, opponent, _player, alpha, PLUS_INF);

    if (_depth_state[_difficult].num_captures)
      score += get_catch_score(_captures[_player]);

    _remove_move_and_captures(_board, _difficult, move.x, move.y, _player);

    if (-score > alpha) {
      alpha = -score;
      best = { move.x, move.y };
    }
  }
  _set_move_and_catch(_board, _difficult, best.x, best.y, _player);

  return _create_turn(best);
}


/** MINIMAX with AlphaBetta */
double Gomoku::minimax(Board& state, size_t depth, t_color player, t_color opponent, double alpha, double betta) {

  size_t is_win = 0;
  alpha = evaluate_state(state, is_win, player, _player == player);

  if (_captures[player] >= 10) {
    is_win = 1;
    alpha += 120000;
  }

  if (!depth || is_win)
    return alpha;

  t_move_eval* possible_moves = _get_possible_moves(depth, player);
  size_t       num_moves = _depth_state[depth].num_moves;

  if (!num_moves)
    return alpha;

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval& move = possible_moves[i];

    _set_move_and_catch(state, depth, move.x, move.y, player);

    double score = minimax(state, depth - 1, opponent, player, -betta, -alpha);

    if (_depth_state[depth].num_captures)
      score += get_catch_score(_captures[player]);

    _remove_move_and_captures(state, depth, move.x, move.y, player);

    if (-score > alpha)
      alpha = -score;

    if (alpha >= betta)
      return alpha;
  }
  return alpha;
}


void Gomoku::_set_move_and_catch(Board& state, size_t depth, size_t x, size_t y, t_color player) {

  t_point*  field = state.getField();
  size_t    side = state.getSide();
  t_color   opponent = player == WHITE ? BLACK : WHITE;
  size_t*   captures = _depth_state[depth].captures;
  size_t&   num_captures = _depth_state[depth].num_captures;

  state(x, y) = player;

  for (size_t i = 0; i != 8; ++i) {

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
        captures[num_captures++] = p1;
        captures[num_captures++] = p2;
        _captures[player] += 2;
      }
    }
  }
}


void Gomoku::_remove_move_and_captures(Board& state, size_t depth, size_t x, size_t y, t_color player) {

  t_point*  field = state.getField();
  t_color   opponent = player == WHITE ? BLACK : WHITE;
  size_t*   captures = _depth_state[depth].captures;
  size_t&   num_captures = _depth_state[depth].num_captures;

  state(x, y) = EMPTY;
  for (size_t i = 0; i < num_captures; ++i) {
    field[captures[i]] = opponent;
    _captures[player] -= 1;
  }
  num_captures = 0;
}

MakeTurn* Gomoku::_create_turn(t_coord best_move) {

  MakeTurn*    m = new MakeTurn();
  t_move_eval* possible_moves = _get_possible_moves(_difficult, _player);
  size_t       num_moves = _depth_state[_difficult].num_moves;

  m->position = _board.coord_to_pos(best_move);
  m->color = _color2str[_player];

  for (size_t i = 0; i != num_moves; ++i) {
    t_move_eval move = possible_moves[i];

    if (move.x != best_move.x || move.y != best_move.y)
      m->hints.push_back(_board.coord_to_pos(move.x, move.y));
  }

  for (size_t i = 0; i < _depth_state[_difficult].num_captures; ++i) {

    size_t x = _depth_state[_difficult].captures[i] % _board.getSide();
    size_t y = _depth_state[_difficult].captures[i] / _board.getSide();

    m->captures.push_back(_board.coord_to_pos(x, y));
  }
  _depth_state[_difficult].num_captures = 0;
  return m;
}
