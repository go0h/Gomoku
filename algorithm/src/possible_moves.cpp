
#define _XXX_   0b0001010100
#define _X_XX_  0b000100010100
#define _XX_X_  0b000101000100

#define XO_X    0b10010010
#define X_OX    0b10000110

#include <iostream>
#include <random>
#include "utils.hpp"
#include "minimax_functions.hpp"


using Dist = std::uniform_int_distribution<size_t>;


long DIRECTIONS[8][2] = {
	{-1, -1 }, { 0, -1 }, { 1, -1 },
	{-1,  0 }, /*  X  */  { 1,  0 },
	{-1,  1 }, { 0,  1 }, { 1,  1 },
};


static size_t is_free_tree(size_t line) {

  switch (line & 0b111111111111)
  {
  case _X_XX_:
    return 1;
  case _XX_X_:
    return 1;
  default:
    break;
  }

  if (((line >> 2) & 0b1111111111) == _XXX_)
    return 1;

  return 0;
}


static size_t get_free_three_by_dir(t_point* field, long side,
                                    long x, long y, long x_dir, long y_dir, t_color opponent) {

  size_t three_num = 0;

  for (long i = 1; i != 6; ++i) {

      size_t line = 0;
      long x_p = x - i * x_dir;
      long y_p = y - i * y_dir;

    for (long j = 0; j != 6; ++j) {
      long x_ = x_p + j * x_dir;
      long y_ = y_p + j * y_dir;
      long pos = (y_p + j * y_dir) * side + x_p + j * x_dir;

      line = line << 2;

      if (x_ > -1 && x_ < side && y_ > -1 && y_ < side && field[pos] != opponent) {
        line = line | (field[pos] ? 0b01 : 0b00);
        continue;
      } else {
        break;
      }
    }
    three_num += is_free_tree(line);
  }
  return three_num;
}


size_t get_num_of_free_threes(t_point* field, size_t side, size_t x, size_t y, t_color player) {

  size_t  three_num = 0;
  t_color opponent = player == WHITE ? BLACK : WHITE;
  size_t  pos = y * side + x;

  if (field[pos] != EMPTY)
    return three_num;
  field[pos] = player;

  // horizontal
  if (x > 0 && x < side - 1)
    three_num += get_free_three_by_dir(field, side, x, y, 1, 0, opponent);

  // vertical
  if (y > 0 && y < side - 1)
    three_num += get_free_three_by_dir(field, side, x, y, 0, 1, opponent);

  if (x > 0 && x < side - 1 && y > 0 && y < side - 1) {

    // diagonal top-left <-> bottom-right
    three_num += get_free_three_by_dir(field, side, x, y, 1, 1, opponent);

    // diagonal top-right <-> bottom-left
    three_num += get_free_three_by_dir(field, side, x, y, 1, -1, opponent);
  }

  field[pos] = EMPTY;
  return three_num;
}


static size_t is_capture(t_point* field, size_t side, long x, long y, long x_dir, long y_dir, t_color player) {

  size_t line = 0;

  for (size_t i = 0; i != 4; ++i) {
    size_t x_ = x + i * x_dir;
    size_t y_ = y + i * y_dir;

    if (x_ < side && y_ < side) {
      long pos = y_ * side + x_;

      line = line << 2;
      if (field[pos])
        line = line | (field[pos] == player ? 0b01 : 0b10);
    } else {
      return false;
    }
  }
  return line == X_OX || line == XO_X;
}


bool is_possible_capture(t_point* field, size_t side, size_t x, size_t y, t_color player) {

  // horizontal
  if (is_capture(field, side, x - 1, y, 1, 0, player) || is_capture(field, side, x - 2, y, 1, 0, player))
    return true;

  // vertical
  if (is_capture(field, side, x, y - 1, 0, 1, player) || is_capture(field, side, x, y - 2, 0, 1, player))
    return true;

  // diagonal top-left -> bottom-right
  if (is_capture(field, side, x - 1, y - 1, 1, 1, player) || is_capture(field, side, x - 2, y - 2, 1, 1, player))
    return true;

  // diagonal top-right -> bottom-left
  if (is_capture(field, side, x + 1, y - 1, -1, 1, player) || is_capture(field, side, x + 2, y - 2, -1, 1, player))
    return true;

  return false;
}


static bool not_forbidden(t_point* field, size_t side, size_t x, size_t y, t_color player) {
  return get_num_of_free_threes(field, side, x, y, player) < 2 && \
        !is_possible_capture(field, side, x, y, player);
}


t_move_eval get_random_move(t_point* field, size_t side, t_color player) {

  static std::default_random_engine re {};
  static Dist                       uid {};

  for(size_t i = 0; i != side * side; ++i) {
    size_t x = uid(re, Dist::param_type{0, side - 1});
    size_t y = uid(re, Dist::param_type{0, side - 1});

    if (field[y * side + x] == EMPTY && not_forbidden(field, side, x, y, player))
      return { 0.0, x, y };
  }
  return { 0.0, 0, 0 };
}


t_move_eval*  get_possible_moves(t_game_state* gs, size_t depth, t_color player) {

  size_t        side = gs->board.getSide();
  t_point*      field = gs->board.getField();
  t_move_eval*  pm = gs->depth_state[depth].poss_moves;
  size_t&       num_moves = gs->depth_state[depth].num_moves;
  size_t        win_flag = 0;
  num_moves = 0;

  for (size_t y = 0; y != side; ++y) {
    for (size_t x = 0; x != side; ++x) {

      if (field[y * side + x] != EMPTY)
        continue;

      for (size_t i = 0; i < 8; ++i) {
        size_t _x = x + DIRECTIONS[i][0];
        size_t _y = y + DIRECTIONS[i][1];
        if (_x < side && _y < side && field[_y * side + _x] != EMPTY) {
          if (not_forbidden(field, side, x, y, player)) {
            field[y * side + x] = player;
            double score = evaluate_state(gs->board, win_flag, player, false);
            field[y * side + x] = EMPTY;
            pm[num_moves] = { score, x, y };
            num_moves++;
          }
          break;
        }
      }
    }
  }
  if (!num_moves) {
    pm[num_moves] = get_random_move(field, side, player);
    num_moves++;
  }
  qsort(pm, num_moves, sizeof(t_move_eval), &compare_moves_desc);
  num_moves = num_moves - (num_moves / 3);
  return pm;
}
