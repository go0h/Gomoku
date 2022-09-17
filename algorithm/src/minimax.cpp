
#include "Gomoku.hpp"
#include <iostream>


double get_catch_score(int n) {
	switch (n) {
		case 0:
			return 40;
		case 2:
			return 40;
		case 4:
			return 40;
		case 6:
			return 40;
		case 8:
			return 1700;
		default:
			return 180000;
	}
}

MakeTurn* Gomoku::minimax() {

  t_coord best;
  double  alpha = MINUS_INF;
  double  score = alpha;
  t_color opponent = (_player == WHITE ? BLACK : WHITE);

  t_possible_moves& possible_moves = _get_possible_moves(_difficult, _player);

  for (t_move_eval move: possible_moves) {

    _set_move_and_catch(_board, _difficult, move.x, move.y, _player);

    score = minimax(_board, _difficult - 1, opponent, _player, alpha, PLUS_INF);

    if (_depth_state[_difficult].depth_catches.size())
      score += get_catch_score(_depth_state[_difficult].depth_catches.size());

    _remove_move_and_catches(_board, _difficult, move.x, move.y, _player);

    if (-score > alpha) {
      alpha = -score;
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
double Gomoku::minimax(Board& state, size_t depth, t_color player, t_color opponent, double alpha, double betta) {

  bool is_win = false;
  double best_score = evaluate_state(state, player, opponent, _player == player);

  if (_captures[player] >= 10) {
    is_win = true;
    best_score = (player == _player) ? 100000 : -100000;
  }

  if (!depth || is_win)
    return best_score;

  t_possible_moves& possible_moves = _get_possible_moves(depth, player);

  if (possible_moves.empty())
    return best_score;

  for (t_move_eval move: possible_moves) {

    _set_move_and_catch(state, depth, move.x, move.y, player);

    double score = minimax(state, depth - 1, opponent, player, -betta, -alpha);

    if (_depth_state[_difficult].depth_catches.size()) {
      double catch_score = get_catch_score(_depth_state[_difficult].depth_catches.size());
      score = score + (player == _player) ? catch_score : -catch_score;
    }

    _remove_move_and_catches(state, depth, move.x, move.y, player);

    if (-score > alpha) {
      alpha = -score;
      best_score = alpha;
    }

    if (alpha >= betta)
      return best_score;
  }
  return best_score;
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
        _captures[player] += 2;
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
    _captures[player] -= 1;
  }
}
