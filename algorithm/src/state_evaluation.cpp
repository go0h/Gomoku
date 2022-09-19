
#define MAX(x,y) (((x) >= (y)) ? (x)  : (y))
#define MIN(x,y) (((x) <= (y)) ? (x)  : (y))
#define ABS(x)   (((x) <= (0)) ? (-x) : (x))

// SIX
#define _WWWW_ 0x000101010100
#define _WWWWB 0x000101010102
#define BWWWW_ 0x020101010100
#define _W_WW_ 0x000100010100
#define _WW_W_ 0x000101000100

// FIVE WITH SHIFT 2 BITS
#define WWWWW  0x0101010101
#define W_WWW  0x0100010101
#define WWW_W  0x0101010001
#define WW_WW  0x0101000101
#define W_W_W  0x0100010001

// FIVE
#define _WWW_  0x0001010100
#define BWWW_  0x0201010100
#define _WWWB  0x0001010102
#define _W_W_  0x0001000100

// FOUR
#define _WW_   0x00010100
#define BWW_   0x02010100
#define _WWB   0x00010102

// THREE
#define _W_    0x000100
#define BW_    0x020100
#define _WB    0x000102

#include <iostream>
#include "Gomoku.hpp"


static long evaluate(long line, size_t& is_win, bool is_player_turn) {

  // SIX
  switch (line & 0xFFFFFFFFFFFF)
  {
  case _WWWW_:
    return is_player_turn ? 20000 : 15000;
  case _WWWWB:
    return is_player_turn ? 15000 : 5000;
  case BWWWW_:
    return is_player_turn ? 15000 : 5000;
  case _W_WW_:
    return is_player_turn ? 10000 : 2500;
  case _WW_W_:
    return is_player_turn ? 10000 : 2500;
  default:
    break;
  }

  // FIVE WITH SHIFT 1 BYTE
  switch ((line >> 8) & 0xFFFFFFFFFF)
  {
  case WWWWW:
    is_win = is_player_turn ? 1 : 0;
    return 100000;
  case W_WWW:
    return is_player_turn ? 20000 : 2500;
  case WWW_W:
    return is_player_turn ? 20000 : 2500;
  case WW_WW:
    return is_player_turn ? 20000 : 2500;
  case W_W_W:
    return is_player_turn ? 5000 : 1500;
  default:
    break;
  }

  // FIVE
  switch (line & 0xFFFFFFFFFF)
  {
  case _WWW_:
    return is_player_turn ? 4000 : 2000;
  case BWWW_:
    return is_player_turn ? 2000 : 1500;
  case _WWWB:
    return is_player_turn ? 2000 : 1500;
  case WW_WW:
    return is_player_turn ? 20000 : 10000;
  case W_W_W:
    return is_player_turn ? 10000 : 5000;
  case _W_W_:
    return is_player_turn ? 1000 : 500;
  default:
    break;
  }

  // FOUR
  switch (line & 0xFFFFFFFF)
  {
  case _WW_:
    return is_player_turn ? 250 : 200;
  case BWW_:
    return is_player_turn ? 80 : 75;
  case _WWB:
    return is_player_turn ? 80 : 75;
  default:
    break;
  }

  // THREE
  switch (line & 0xFFFFFF)
  {
  case _W_:
    return is_player_turn ? 25 : 15;
  case BW_:
    return 10;
  case _WB:
    return 10;
  default:
    break;
  }

  return 0;
}


static long evaluate_line(long& line, size_t& is_win, bool is_player_turn) {

  // если конец линии ставим как будто там противник
  if (line & 0x01)
    line = (line << 8) | 0x02;


  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0x0102) == 0x0102 || (line & 0x0100) == 0x0100) {
    long score = evaluate(line, is_win, is_player_turn);
    line = 0x02;
    return score;
  }

  // линия начинается с противника
  line = 0x02;
  return 0;
}


static long evaluate_segment(long& line, size_t& is_win, bool is_player, size_t point, bool is_player_turn) {

  line = line << 8;

  if (point)
    line = line | (is_player ? 0x01 : 0x02);

  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0x0102) == 0x0102 || line & 0x0100)
    return evaluate(line, is_win, is_player_turn);

  return 0;
}


static long evaluate_diagonal(t_point* field, long side, size_t& is_win, t_color player, bool is_player_turn) {

  long diagonale = 0x02;
  long score     = 0;

  for (long i = 0; i != side * 2 - 1; ++i) {
    long f_y = MIN(i, side - 1);
    long f_x = MAX(0, i - (side - 1));
    for (long j = 0; j != side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side)
        break;
      long pos = (f_y - j) * side + (f_x + j);
      score += evaluate_segment(diagonale, is_win, field[pos] == player, field[pos], is_player_turn);
    }
    score += evaluate_line(diagonale, is_win, is_player_turn);
  }

  for (long i = side - 1; i > -side; --i) {
    long f_y = ABS(MIN(0, i));
    long f_x = MAX(0, i);
    // top-left -> bottom-right
    for (long j = 0; j != side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;

      long pos = (f_y + j) * side + (f_x + j);
      score += evaluate_segment(diagonale, is_win, field[pos] == player, field[pos], is_player_turn);
    }
    score += evaluate_line(diagonale, is_win, is_player_turn);
  }

  return score;
}


static long evaluate_horizontal_vertical(t_point* field, long side, size_t& is_win, t_color player, bool is_player_turn) {

  long horizontal  = 0x02;
  long vertical    = 0x02;
  long score       = 0;

  for (long y = 0; y != side; ++y) {
    for (long x = 0; x != side; ++x) {

      long pos1 = x * side + y;
      long pos2 = y * side + x;

      score += evaluate_segment(horizontal, is_win, field[pos1] == player, field[pos1], is_player_turn);
      score += evaluate_segment(vertical, is_win, field[pos2] == player, field[pos2], is_player_turn);
    }
    score += evaluate_line(horizontal, is_win, is_player_turn);
    score += evaluate_line(vertical, is_win, is_player_turn);
  }

  return score;
}


double evaluate_state(Board& state, size_t& is_win, t_color player, bool is_player_turn) {

  size_t   side = state.getSide();
  t_point* field = state.getField();
  t_color  opponent = (player == WHITE) ? BLACK : WHITE;

  return (double)(evaluate_horizontal_vertical(field, side, is_win, player, is_player_turn)
                + evaluate_diagonal(field, side, is_win, player, is_player_turn)
                - evaluate_horizontal_vertical(field, side, is_win, opponent, !is_player_turn)
                - evaluate_diagonal(field, side, is_win, opponent, !is_player_turn));

  // double player_hv_score = evaluate_horizontal_vertical(field, side, is_win, player, is_player_turn);
  // double player_d_score = evaluate_diagonal(field, side, is_win, player, is_player_turn);
  // double opponent_hv_score = evaluate_horizontal_vertical(field, side, is_win, opponent, !is_player_turn);
  // double opponent_d_score = evaluate_diagonal(field, side, is_win, opponent, !is_player_turn);

  // std::cout << "player_hv_score: " << player_hv_score << std::endl;
  // std::cout << "player_d_score: " << player_d_score << std::endl;
  // std::cout << "opponent_hv_score: " << opponent_hv_score << std::endl;
  // std::cout << "opponent_d_score: " << opponent_d_score << std::endl;

  // double score = player_hv_score + player_d_score - opponent_hv_score - opponent_d_score;

  // return score;
}
