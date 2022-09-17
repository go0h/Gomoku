
#define MAX(x,y) (((x) >= (y)) ? (x)  : (y))
#define MIN(x,y) (((x) <= (y)) ? (x)  : (y))
#define ABS(x)   (((x) <= (0)) ? (-x) : (x))


#include <iostream>
#include <random>
#include "Gomoku.hpp"


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
