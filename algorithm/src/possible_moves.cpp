
#define _WWW_   0x0001010100
#define _BBB_   0x0002020200

#define _W_WW_  0x000100010100
#define _B_BB_  0x000200020200

#define _WW_W_  0x000101000100
#define _BB_B_  0x000202000200

#define BW_B    0x02010002
#define B_WB    0x02000102

#define W_BW    0x01000201
#define WB_W    0x01020001

#include <iostream>
#include <random>
#include "Gomoku.hpp"


using Dist = std::uniform_int_distribution<size_t>;


long DIRECTIONS[8][2] = {
	{-1, -1 }, { 0, -1 }, { 1, -1 },
	{-1,  0 }, /*  X  */  { 1,  0 },
	{-1,  1 }, { 0,  1 }, { 1,  1 },
};


long FREE_THREE[3][3] = {
  {   5  ,    6  ,   6    },
  { _WWW_, _W_WW_, _WW_W_ },
  { _BBB_, _B_BB_, _BB_B_ }
};


long CATCHES[3][2] = {
  {  0,     0  },
  { BW_B, B_WB },
  { W_BW, WB_W }
};

//TODO optimize
size_t get_free_three_by_dir(t_point* field, long side, long x, long y, long x_dir, long y_dir, t_color player) {

  size_t three_num = 0;

  for (size_t p_num = 0; p_num < 3; ++p_num) {

    size_t pattern = FREE_THREE[player][p_num];

    for (long i = 1; i < FREE_THREE[0][p_num]; ++i) {
        long is_free_tree = 1;
        long x_p = x - i * x_dir;
        long y_p = y - i * y_dir;

      for (long j = 0; j < FREE_THREE[0][p_num]; ++j) {
        long x_ = x_p + j * x_dir;
        long y_ = y_p + j * y_dir;
        t_point pp = (pattern >> (j * 8)) & 0xFF;

        if (x_ > -1 && x_ < side && y_ > -1 && y_ < side && field[y_ * side + x_] == pp) {
          continue;
        } else {
          is_free_tree = 0;
          break;
        }
      }
      three_num += is_free_tree;
    }
  }
  return three_num;
}


size_t get_num_of_free_threes(t_point* field, size_t side, size_t x, size_t y, t_color player) {

  size_t three_num = 0;

  if (field[y * side + x] != EMPTY)
    return three_num;
  field[y * side + x] = player;

  // horizontal
  if (x > 0 && x < side - 1)
    three_num += get_free_three_by_dir(field, side, x, y, 1, 0, player);

  // vertical
  if (y > 0 && y < side - 1)
    three_num += get_free_three_by_dir(field, side, x, y, 0, 1, player);

  if (x > 0 && x < side - 1 && y > 0 && y < side - 1) {

    // diagonal top-left <-> bottom-right
    three_num += get_free_three_by_dir(field, side, x, y, 1, 1, player);

    // diagonal top-right <-> bottom-left
    three_num += get_free_three_by_dir(field, side, x, y, 1, -1, player);
  }

  field[y * side + x] = EMPTY;
  return three_num;
}


size_t is_capture(t_point* field, size_t side, long x, long y, long x_dir, long y_dir, t_color player) {

  for (size_t j = 0; j < 2; ++j) {

    bool is_capture = true;

    for (size_t i = 0; i < 4; ++i) {
      size_t x_ = x + i * x_dir;
      size_t y_ = y + i * y_dir;

      t_point pp = (CATCHES[player][j] >> (i * 8)) & 0xFF;

      if (x_ < side && y_ < side && field[y_ * side + x_] == pp)
        continue;
      else {
        is_capture = false;
        break;
      };
    }
    if (is_capture)
      return true;
  }
  return false;
}


bool is_possible_capture(t_point* field, size_t side, size_t x, size_t y, t_color player) {

  // horizontal
  if (is_capture(field, side, x - 1, y, 1, 0, player) || is_capture(field, side, x - 2, y, 1, 0, player))
    return true;

  // vertical
  if (is_capture(field, side, x, y - 1, 0, 1, player) || is_capture(field, side,x, y - 2, 0, 1, player))
    return true;

  // diagonal top-left -> bottom-right
  if (is_capture(field, side, x - 1, y - 1, 1, 1, player) || is_capture(field, side, x - 2, y - 2, 1, 1, player))
    return true;

  // diagonal top-right -> bottom-left
  if (is_capture(field, side, x + 1, y - 1, -1, 1, player) || is_capture(field, side, x + 2, y - 2, -1, 1, player))
    return true;

  return false;
}


bool not_forbidden(t_point* field, size_t side, size_t x, size_t y, t_color player) {
  return get_num_of_free_threes(field, side, x, y, player) < 2 && \
        !is_possible_capture(field, side, x, y, player);
}


double pre_evaluate_step(t_point* field, size_t side, size_t x, size_t y, t_color player) {

	double score = 0.0;

	for (long i = 0; i < 8; ++i) {

    size_t _x = x;
	  size_t _y = y;
		long x_dir = DIRECTIONS[i][0];
		long y_dir = DIRECTIONS[i][1];

    size_t points_in_row = 0;

    for (;;) {
			_x += x_dir;
			_y += y_dir;
			if (_x < side &&  _y < side && field[_y * side + x] == player)
				points_in_row = (points_in_row + 1) * 2;
			else
				break;
		}
		score += points_in_row;
	}
	return score;
}


bool compare_moves(Gomoku::t_move_eval& m1, Gomoku::t_move_eval& m2) {
  return m1.score > m2.score;
}


Gomoku::t_move_eval get_random_move(t_point* field, size_t side, t_color player) {

  static std::default_random_engine re {};
  static Dist                       uid {};

  for(size_t i = 0; i < side * side; ++i) {
    size_t x = uid(re, Dist::param_type{0, side - 1});
    size_t y = uid(re, Dist::param_type{0, side - 1});

    if (field[y * side + x] == EMPTY && not_forbidden(field, side, x, y, player))
      return { 0.0, x, y };
  }
  return { MINUS_INF, 0, 0 };
}


Gomoku::t_possible_moves& Gomoku::_get_possible_moves(size_t depth, t_color player) {

  size_t side = _board.getSide();
  t_point* field = _board.getField();

  t_possible_moves& pm = _depth_state[depth].poss_moves;
  pm.clear();

  for (size_t y = 0; y < side; ++y) {
    for (size_t x = 0; x < side; ++x) {

      if (field[y * side + x] != EMPTY)
        continue;

      for (size_t i = 0; i < 8; ++i) {
        size_t _x = x + DIRECTIONS[i][0];
        size_t _y = y + DIRECTIONS[i][1];
        if (_x < side && _y < side && field[_y * side + _x] != EMPTY) {
          if (not_forbidden(field, side, x, y, player)) {
            double score = pre_evaluate_step(field, side, x, y, player);
            pm.push_back({ score, x, y});
          }
          break;
        }
      }
    }
  }
  if (pm.empty())
    pm.push_back(get_random_move(field, side, player));

  std::sort(pm.begin(), pm.end(), compare_moves);
  return pm;
}
