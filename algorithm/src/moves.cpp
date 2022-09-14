
#include "Minimax.hpp"


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


int DIRECTIONS[8][2] = {
	{-1, -1 }, { 0, -1 }, { 1, -1 },
	{-1,  0 }, /*  X  */  { 1,  0 },
	{-1,  1 }, { 0,  1 }, { 1,  1 },
};


int64_t FREE_THREE[3][3] = {
  {   5  ,    6  ,   6    },
  { _WWW_, _W_WW_, _WW_W_ },
  { _BBB_, _B_BB_, _BB_B_ }
};


int32_t CATCHES[3][2] = {
  {  0,     0  },
  { BW_B, B_WB },
  { W_BW, WB_W }
};

//TODO optimize
static size_t _get_free_threes(t_point* field, int side, int x, int y, int x_dir, int y_dir, t_color player) {

  size_t three_num = 0;

  for (size_t p_num = 0; p_num < 3; ++p_num) {

    size_t pattern = FREE_THREE[player][p_num];
    int is_free_tree = 1;

    for (int i = 1; i < FREE_THREE[0][p_num] - 1; ++i) {

      for (int j = 0; j < FREE_THREE[0][p_num]; ++j) {
        int x_ = x + j * x_dir;
        int y_ = y + j * y_dir;
        t_point pp = (pattern >> (j * 8)) & 0xFF;

        if (x_ > 0 && x_ < side && y_ > 0 && y_ < side && field[y_ * side + x_] == pp) {
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


size_t Minimax::_get_num_of_free_threes(size_t x, size_t y, t_color player) {

  size_t three_num = 0;

  if (_state(x, y) != EMPTY)
    return three_num;
  _state(x, y) = player;

  size_t side = _state.getSide();
  t_point* field = _state.getField();

  // horizontal
  if (x > 0 && x < side - 1)
    three_num += _get_free_threes(field, side, x, y, 1, 0, player);

  // vertical
  if (y > 0 && y < side - 1)
    three_num += _get_free_threes(field, side, x, y, 0, 1, player);

  if (x > 0 && x < side - 1 && y > 0 && y < side - 1) {

    // diagonal top-left <-> bottom-right
    three_num += _get_free_threes(field, side, x, y, 1, 1, player);

    // diagonal top-right <-> bottom-left
    three_num += _get_free_threes(field, side, x, y, 1, -1, player);
  }

  _state(x, y) = EMPTY;
  return three_num;
}


static size_t is_capture(t_point* field, size_t side, int x, int y, int x_dir, int y_dir, t_color player) {

  for (size_t j = 0; j < 2; ++j) {

    bool is_capture = true;

    for (size_t i = 0; i < 4; ++i) {
      size_t x_ = x + i * x_dir;
      size_t y_ = y + i * y_dir;

      t_point pp = (CATCHES[player][j] >> ((i * x_dir) * 8)) & 0xFF;

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


bool Minimax::_is_possible_capture(size_t x, size_t y, t_color player) {

  size_t side = _state.getSide();
  t_point* field = _state.getField();

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


bool Minimax::_not_forbidden(size_t x, size_t y, t_color player) {
  return _get_num_of_free_threes(x, y, player) < 2 && !_is_possible_capture(x, y, player);
}


Minimax::t_possible_moves Minimax::get_possible_moves(t_color player) {

  player = player == WHITE ? WHITE : BLACK;

  Minimax::t_possible_moves pm = Minimax::t_possible_moves();
  size_t side = _state.getSide();
  pm.reserve(side * side);

  for (size_t y = 0; y < side; ++y) {
    for (size_t x = 0; x < side; ++x) {

      if (_state(x, y) != EMPTY)
        continue;

      for (size_t i = 0; i < 8; ++i) {
        size_t _x = x + DIRECTIONS[i][0];
        size_t _y = y + DIRECTIONS[i][1];
        if (_x < side && _y < side && _state(_x, _y) != EMPTY) {

          if (_not_forbidden(x, y, player))
            pm.push_back({x, y});

          break;
        }
      }
    }
  }
  return pm;
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

  return uid(re, Dist::param_type{0, 100000000});
}
