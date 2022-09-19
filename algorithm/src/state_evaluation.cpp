
// SIX
#define _WWWW_ 0b000101010100    // 20000 (15000)
#define _WWWWB 0b000101010110    // 15000 (5000)
#define BWWWW_ 0b100101010100    // 15000 (5000)
#define _W_WW_ 0b000100010100    // 7250  (2500)
#define _WW_W_ 0b000101000100    // 7250  (2500)

// FIVE WITH SHIFT 2 BITS
#define WWWWW  0b0101010101      // 100000
#define W_WWW  0b0100010101      // 20000 (7500)
#define WWW_W  0b0101010001      // 20000 (7500)
#define WW_WW  0b0101000101      // 20000 (7500)
#define W_W_W  0b0100010001      // 3500  (1500)

// FIVE
#define _WWW_  0b0001010100      // 4000  (2000)
#define BWWW_  0b1001010100      // 2000  (1500)
#define _WWWB  0b0001010110      // 2000  (1500)
#define _W_W_  0b0001000100      // 1000  (500)

// FOUR
#define _WW_   0b00010100        // 250   (200)
#define BWW_   0b10010100        // 80    (75)
#define _WWB   0b00010110        // 80    (75)

// THREE
#define _W_    0b000100          // 25    (15)
#define BW_    0b100100          // 10
#define _WB    0b000110          // 10

#define PLAYER   0b01
#define OPPONENT 0b10

#define SHIFT    2

#include <iostream>
#include "Gomoku.hpp"


static long evaluate(long line, size_t& is_win, bool is_player_turn) {

  // SIX
  switch (line & 0b111111111111)
  {
  case _WWWW_:
    return is_player_turn ? 20000 : 15000;
  case _WWWWB:
    return is_player_turn ? 15000 : 7500;
  case BWWWW_:
    return is_player_turn ? 15000 : 7500;
  case _W_WW_:
    return is_player_turn ? 7250 : 2500;
  case _WW_W_:
    return is_player_turn ? 7250 : 2500;
  default:
    break;
  }

  // FIVE WITH SHIFT 1 BYTE
  switch ((line >> SHIFT) & 0b1111111111)
  {
  case WWWWW:
    is_win = 1; // is_player_turn ? 1 : 0;
    return 100000;
  case W_WWW:
    return is_player_turn ? 20000 : 3500;
  case WWW_W:
    return is_player_turn ? 20000 : 3500;
  case WW_WW:
    return is_player_turn ? 20000 : 3500;
  case W_W_W:
    return is_player_turn ? 3500 : 1500;
  default:
    break;
  }

  // FIVE
  switch (line & 0b1111111111)
  {
  case _WWW_:
    return is_player_turn ? 4000 : 2000;
  case BWWW_:
    return is_player_turn ? 2000 : 1500;
  case _WWWB:
    return is_player_turn ? 2000 : 1500;
  case WW_WW:
    return is_player_turn ? 20000 : 3500;
  case W_W_W:
    return is_player_turn ? 3500 : 2000;
  case _W_W_:
    return is_player_turn ? 1000 : 500;
  default:
    break;
  }

  // FOUR
  switch (line & 0b11111111)
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
  switch (line & 0b111111)
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
  if (line & PLAYER)
    line = (line << SHIFT) | OPPONENT;


  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0b0110) == 0b0110 || (line & 0b0100) == 0b0100) {
    long score = evaluate(line, is_win, is_player_turn);
    line = OPPONENT;
    return score;
  }

  // линия начинается с противника
  line = OPPONENT;
  return 0;
}


static long evaluate_segment(long& line, size_t& is_win, bool is_player, size_t point, bool is_player_turn) {

  line = line << SHIFT;

  if (point)
    line = line | (is_player ? PLAYER : OPPONENT);

  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0b0110) == 0b0110 || line & 0b0100)
    return evaluate(line, is_win, is_player_turn);

  return 0;
}


static long evaluate_diagonal(t_point* field, long side, size_t& is_win, t_color player, bool is_player_turn) {

  long diagonale = OPPONENT;
  long score     = 0;

  for (long i = 0; i != side * 2 - 1; ++i) {
    long f_y = std::min(i, side - 1);
    long f_x = std::max(0L, i - (side - 1));
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
    long f_y = std::abs(std::min(0L, i));
    long f_x = std::max(0L, i);
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

  long horizontal  = OPPONENT;
  long vertical    = OPPONENT;
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
