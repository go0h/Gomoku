
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
#define MIN(x,y) (((x) <= (y)) ? (x) : (y))
#define ABS(x)   (((x) <= (0)) ? (-x) : (x))

#include <iostream>
#include <random>
#include "Gomoku.hpp"


typedef struct s_segment_info {
  double        score;
  unsigned      total_free;
  unsigned      prev_seq_free;
  unsigned      sequence_len;
  unsigned      post_seq_free;
}             t_segment_info;


void   evaluate(t_segment_info& seg_info) {

  // длина сегмента = 0 или нет свободных сторон
  if (seg_info.sequence_len < 5 && (!seg_info.prev_seq_free && !seg_info.prev_seq_free))
    return;

  switch (seg_info.sequence_len)
  {
  case 4:
    if (seg_info.prev_seq_free && seg_info.prev_seq_free) // с двух сторон свободное место
      seg_info.score += 10000;
    else                                                  // с одной стороны свободное место
      seg_info.score += 2000;
    return;
  case 3:
    // с двух сторон свободное место, и сумма > 2
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free > 2)) {
      seg_info.score += 2000;
    }
    // с двух сторон по одной свободной клетке
    else if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0) {
      seg_info.score += 1500;
    }
    // с одной стороны свободного места нет
    else {
      seg_info.score += 1000;
    }
    return;
  case 2:
    // с двух сторон свободное место, и сумма > 3
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free > 3)) {
      seg_info.score += 250;
    }
    // с двух сторон по одной свободной клетке
    else if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0) {
      seg_info.score += 150;
    }
    // с одной стороны свободного места нет
    else {
      seg_info.score += 75;
    }
    return;
  case 1:
    // с двух сторон свободное место, и сумма > 3
    if (seg_info.prev_seq_free > 0 && seg_info.post_seq_free > 0 && \
       (seg_info.prev_seq_free + seg_info.post_seq_free > 4)) {
      seg_info.score += 25;
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
    // 5 в ряд
    seg_info.score += 100000;
    break;
  }
}


void   evaluate_segment(t_segment_info& seg_info, bool is_player, bool is_empty) {

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
      evaluate(seg_info);

    seg_info.total_free += seg_info.prev_seq_free + seg_info.post_seq_free;
    seg_info.prev_seq_free = 0;
    seg_info.sequence_len = 0;
    seg_info.post_seq_free = 0;
  }
}


void   evaluate_line(t_segment_info& seg_info) {

  if (seg_info.sequence_len)
    evaluate(seg_info);

  seg_info.total_free += seg_info.prev_seq_free + seg_info.post_seq_free;
  seg_info.prev_seq_free = 0;
  seg_info.sequence_len = 0;
  seg_info.post_seq_free = 0;

}


double evaluate_diagonal(t_point* field, int side, t_color player, bool is_player_turn) {

  t_segment_info  diagonale;
  memset(&diagonale, 0, sizeof(t_segment_info));

  Board b = Board(side);

  for (int i = 0; i < side * 2 - 1; ++i) {
    int f_y = MIN(i, side - 1);
    int f_x = MAX(0, i - (side - 1));
    for (int j = 0; j < side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side) {
        break;
      }
      // std::cout << "pos = " << b.coord_to_pos(f_x + j, f_y - j) << std::endl;
      int pos = (f_y - j) * side + (f_x + j);
      evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY);
    }
    // std::cout << "-------------------------------" << std::endl;
    evaluate_line(diagonale);
  }

  double score = diagonale.score;
  memset(&diagonale, 0, sizeof(t_segment_info));


  for (int i = side - 1; i > -side; --i) {
    int f_y = ABS(MIN(0, i));
    int f_x = MAX(0, i);
    // top-left -> bottom-right
    for (long j = 0; j < side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;
      // std::cout << "pos = " << b.coord_to_pos(f_x + j, f_y + j) << std::endl;
      int pos = (f_y + j) * side + (f_x + j);
      evaluate_segment(diagonale, field[pos] == player, field[pos] == EMPTY);
    }

    // std::cout << "-------------------------------" << std::endl;
    evaluate_line(diagonale);
  }

  score += diagonale.score;

  return is_player_turn ? score * 1.2 : score;
}


double evaluate_horizontal_vertical(t_point* field, size_t side, t_color player, bool is_player_turn) {

  t_segment_info  horizontal;
  t_segment_info  vertical;
  memset(&horizontal, 0, sizeof(t_segment_info));
  memset(&vertical, 0, sizeof(t_segment_info));

  // Board b = Board(side);

  for (size_t y = 0; y < side; ++y) {
    for (size_t x = 0; x < side; ++x) {
      // if (y == side - 1)
      //   std::cout << "here" << std::endl;

      evaluate_segment(horizontal, field[y * side + x] == player, field[y * side + x] == EMPTY);
      evaluate_segment(vertical, field[x * side + y] == player, field[x * side + y] == EMPTY);

      // std::cout << b.coord_to_pos(x, y) << std::endl;
    }
    evaluate_line(horizontal);
    evaluate_line(vertical);
  }

  double score = horizontal.score + vertical.score;

  return is_player_turn ? score * 1.2 : score;
}


double evaluate_state(Board& state, size_t depth, t_color player, t_color opponent, bool is_player_turn) {

  size_t          side = state.getSide();
  t_point*        field = state.getField();

  depth = depth == 3 ? 0 : 1;

  // double player_hv_score = evaluate_horizontal_vertical(field, side, player, is_player_turn);
  // double player_d_score = evaluate_diagonal(field, side, player, is_player_turn);
  // double opponent_hv_score = evaluate_horizontal_vertical(field, opponent, player, !is_player_turn);
  // double opponent_d_score = evaluate_diagonal(field, side, opponent, !is_player_turn);

  // std::cout << "player_hv_score: " << player_hv_score << std::endl;
  // std::cout << "player_d_score: " << player_d_score << std::endl;
  // std::cout << "opponent_hv_score: " << opponent_hv_score << std::endl;
  // std::cout << "opponent_d_score: " << opponent_d_score << std::endl;

  double score = evaluate_horizontal_vertical(field, side, player, is_player_turn)
               + evaluate_diagonal(field, side, player, is_player_turn)
               - evaluate_horizontal_vertical(field, side, opponent, !is_player_turn)
               - evaluate_diagonal(field, side, opponent, !is_player_turn);

  // double score = player_hv_score + player_d_score - opponent_hv_score - opponent_d_score;

  return score;
}
