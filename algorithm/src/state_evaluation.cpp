
#define MAX(x,y) (((x) >= (y)) ? (x)  : (y))
#define MIN(x,y) (((x) <= (y)) ? (x)  : (y))
#define ABS(x)   (((x) <= (0)) ? (-x) : (x))


#include <iostream>
#include <random>
#include "Gomoku.hpp"

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


// SECOND IMPLEMENTATION

static long evaluate(long line, bool is_player_turn) {

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
    return is_player_turn ? 10000 : 5000;
  case _WW_W_:
    return is_player_turn ? 10000 : 5000;
  default:
    break;
  }

  // FIVE WITH SHIFT 1 BYTE
  switch ((line >> 8) & 0xFFFFFFFFFF)
  {
  case WWWWW:
    return 100000;
  case W_WWW:
    return is_player_turn ? 20000 : 10000;
  case WWW_W:
    return is_player_turn ? 20000 : 10000;
  case WW_WW:
    return is_player_turn ? 20000 : 10000;
  case W_W_W:
    return is_player_turn ? 10000 : 5000;
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


static long evaluate_line(long& line, bool is_player_turn) {

  // если конец линии ставим как будто там противник
  if (line & 0x01)
    line = (line << 8) | 0x02;


  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0x0102) == 0x0102 || (line & 0x0100) == 0x0100) {
    line = 0x02;
    return evaluate(line, is_player_turn);
  }

  // линия начинается с противника
  line = 0x02;
  return 0;
}


static long evaluate_segment(long& line, bool is_player, bool is_empty, bool is_player_turn) {

  line = line << 8;

  if (!is_empty)
    line = line | (is_player ? 0x01 : 0x02);

  //расчет только если предыдущая фишка была наша, а текущая нет
  if ((line & 0x0102) == 0x0102 || line & 0x0100)
    return evaluate(line, is_player_turn);

  return 0;
}


static long evaluate_diagonal(t_point* field, long side, t_color player, bool is_player_turn) {

  long diagonale = 0x02;
  long score     = 0;

  for (long i = 0; i < side * 2 - 1; ++i) {
    long f_y = MIN(i, side - 1);
    long f_x = MAX(0, i - (side - 1));
    for (long j = 0; j < side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side)
        break;
      long pos = (f_y - j) * side + (f_x + j);
      score += evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY, is_player_turn);
    }
    score += evaluate_line(diagonale, is_player_turn);
  }

  for (long i = side - 1; i > -side; --i) {
    long f_y = ABS(MIN(0, i));
    long f_x = MAX(0, i);
    // top-left -> bottom-right
    for (long j = 0; j < side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;

      long pos = (f_y + j) * side + (f_x + j);
      score += evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY, is_player_turn);
    }
    score += evaluate_line(diagonale, is_player_turn);
  }

  return score;
}


long evaluate_horizontal_vertical(t_point* field, long side, t_color player, bool is_player_turn) {

  long horizontal  = 0x02;
  long vertical    = 0x02;
  long score       = 0;

  for (long y = 0; y < side; ++y) {
    for (long x = 0; x < side; ++x) {

      long pos1 = y * side + x;
      long pos2 = x * side + y;

      score += evaluate_segment(horizontal, field[pos1] == player, field[pos1] == EMPTY, is_player_turn);
      score += evaluate_segment(vertical, field[pos2] == player, field[pos2] == EMPTY, is_player_turn);
    }
    score += evaluate_line(horizontal, is_player_turn);
    score += evaluate_line(vertical, is_player_turn);
  }

  return score;
}


double evaluate_state(Board& state, t_color player, t_color opponent, bool is_player_turn) {

  size_t   side = state.getSide();
  t_point* field = state.getField();

  return (double)(evaluate_horizontal_vertical(field, side, player, is_player_turn)
                + evaluate_diagonal(field, side, player, is_player_turn)
                - evaluate_horizontal_vertical(field, side, opponent, !is_player_turn)
                - evaluate_diagonal(field, side, opponent, !is_player_turn));

  // double player_hv_score = evaluate_horizontal_vertical(field, side, player, is_player_turn);
  // double player_d_score = evaluate_diagonal(field, side, player, is_player_turn);
  // double opponent_hv_score = evaluate_horizontal_vertical(field, side, opponent, !is_player_turn);
  // double opponent_d_score = evaluate_diagonal(field, side, opponent, !is_player_turn);

  // std::cout << "player_hv_score: " << player_hv_score << std::endl;
  // std::cout << "player_d_score: " << player_d_score << std::endl;
  // std::cout << "opponent_hv_score: " << opponent_hv_score << std::endl;
  // std::cout << "opponent_d_score: " << opponent_d_score << std::endl;

  // double score = player_hv_score + player_d_score - opponent_hv_score - opponent_d_score;

  // return score;
}


/* FIRST IMPLEMENTATION

typedef struct s_segment_info {
  double        score;
  long          total_free;
  long          prev_seq_free;
  long          sequence_len;
  long          post_seq_free;
}             t_segment_info;


static void   evaluate(t_segment_info& seg_info, bool is_player_turn) {

  // длина сегмента = 0 или нет свободных сторон
  if (seg_info.sequence_len < 5 && (!seg_info.prev_seq_free && !seg_info.prev_seq_free))
    return;

  switch (seg_info.sequence_len)
  {
  case 4:
    if (seg_info.prev_seq_free && seg_info.prev_seq_free) // с двух сторон свободное место
      seg_info.score += (is_player_turn ? 20000 : 15000);
    else                                                  // с одной стороны свободное место
      seg_info.score += (is_player_turn ? 15000 : 5000);
    return;
  case 3:
    // с двух сторон свободное место, и сумма > 2
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free) > 2) {
      seg_info.score += (is_player_turn ? 4000 : 2000);
    }
    // с двух сторон по одной свободной клетке
    else if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0) {
      seg_info.score += (is_player_turn ? 2000 : 1500);
    }
    // с одной стороны свободного места нет
    else {
      seg_info.score += (is_player_turn ? 1250 : 1000);
    }
    return;
  case 2:
    // с двух сторон свободное место, и сумма > 3
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free) > 3) {
      seg_info.score += (is_player_turn ? 250 : 200);
    }
    // с двух сторон по одной свободной клетке
    else if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0) {
      seg_info.score += (is_player_turn ? 180 : 150);
    }
    // с одной стороны свободного места нет
    else {
      seg_info.score += (is_player_turn ? 80 : 75);
    }
    return;
  case 1:
    // с двух сторон свободное место, и сумма > 3
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free > 4)) {
      seg_info.score += (is_player_turn ? 25 : 15);;
    }
    // с двух сторон по одной свободной клетке
    else if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0) {
      seg_info.score += 10;
    }
    // с одной стороны свободного места нет
    else {
      seg_info.score += 5;
    }
    return;
  default:
    std::cout << "Have five " << (is_player_turn ? "Player" : "Opponent") << std::endl;
    // 5 в ряд
    seg_info.score += 100000;
    break;
  }
}


static void   evaluate_segment(t_segment_info& seg_info, bool is_player, bool is_empty, bool is_player_turn) {

  if (is_empty && !seg_info.sequence_len) {
    ++seg_info.prev_seq_free;
  }
  else if (is_player) {
    ++seg_info.sequence_len;
  }
  else if (is_empty) {
    seg_info.post_seq_free++;
  }
  else if (!is_player) {

    if (seg_info.sequence_len)
      evaluate(seg_info, is_player_turn);

    seg_info.total_free += seg_info.prev_seq_free + seg_info.post_seq_free;
    seg_info.prev_seq_free = 0;
    seg_info.sequence_len = 0;
    seg_info.post_seq_free = 0;
  }
}


static void   evaluate_line(t_segment_info& seg_info, bool is_player_turn) {

  if (seg_info.sequence_len)
    evaluate(seg_info, is_player_turn);

  seg_info.total_free += seg_info.prev_seq_free + seg_info.post_seq_free;
  seg_info.prev_seq_free = 0;
  seg_info.sequence_len = 0;
  seg_info.post_seq_free = 0;

}


static double evaluate_diagonal(t_point* field, long side, t_color player, bool is_player_turn) {

  t_segment_info  diagonale = { 0, 0, 0, 0, 0 };

  for (long i = 0; i < side * 2 - 1; ++i) {
    long f_y = MIN(i, side - 1);
    long f_x = MAX(0, i - (side - 1));
    for (long j = 0; j < side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side)
        break;
      long pos = (f_y - j) * side + (f_x + j);
      evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY, is_player_turn);
    }
    evaluate_line(diagonale, is_player_turn);
  }

  for (long i = side - 1; i > -side; --i) {
    long f_y = ABS(MIN(0, i));
    long f_x = MAX(0, i);
    // top-left -> bottom-right
    for (long j = 0; j < side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;

      long pos = (f_y + j) * side + (f_x + j);
      evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY, is_player_turn);
    }
    evaluate_line(diagonale, is_player_turn);
  }

  return diagonale.score;
}


double evaluate_horizontal_vertical(t_point* field, long side, t_color player, bool is_player_turn) {

  t_segment_info  horizontal = { 0, 0, 0, 0, 0 };
  t_segment_info  vertical   = { 0, 0, 0, 0, 0 };

  for (long y = 0; y < side; ++y) {
    for (long x = 0; x < side; ++x) {

      long pos1 = y * side + x;
      long pos2 = x * side + y;

      evaluate_segment(horizontal, field[pos1] == player, field[pos1] == EMPTY, is_player_turn);
      evaluate_segment(vertical, field[pos2] == player, field[pos2] == EMPTY, is_player_turn);
    }
    evaluate_line(horizontal, is_player_turn);
    evaluate_line(vertical, is_player_turn);
  }

  return horizontal.score + vertical.score;
}


double evaluate_state(Board& state, t_color player, t_color opponent, bool is_player_turn) {

  size_t   side = state.getSide();
  t_point* field = state.getField();

  return evaluate_horizontal_vertical(field, side, player, is_player_turn)
       + evaluate_diagonal(field, side, player, is_player_turn)
       - evaluate_horizontal_vertical(field, side, opponent, !is_player_turn)
       - evaluate_diagonal(field, side, opponent, !is_player_turn);
}
*/
