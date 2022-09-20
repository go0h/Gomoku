

#include "Gomoku.hpp"

int compare_moves_desc(const void* elem1, const void* elem2) {

  Gomoku::t_move_eval* m1 = (Gomoku::t_move_eval*)elem1;
  Gomoku::t_move_eval* m2 = (Gomoku::t_move_eval*)elem2;

	if (m1->score < m2->score)
    return 1;
  else if (m1->score > m2->score)
    return -1;
  return 0;
}

int compare_moves_asc(const void* elem1, const void* elem2) {

  Gomoku::t_move_eval* m1 = (Gomoku::t_move_eval*)elem1;
  Gomoku::t_move_eval* m2 = (Gomoku::t_move_eval*)elem2;

	if (m1->score < m2->score)
    return -1;
  else if (m1->score > m2->score)
    return 1;
  return 0;
}


size_t *create_horizontal_positions(long side) {

  size_t i = 0;
  size_t* horizontal = new size_t[side * side];

  for (long y = 0; y != side; ++y) {
    for (long x = 0; x != side; ++x) {
      horizontal[i] = x * side + y;
      ++i;
    }
  }
  return horizontal;
}


size_t *create_vertical_positions(long side) {

  size_t k = 0;
  size_t* horizontal = new size_t[side * side];

  for (long y = 0; y != side; ++y) {
    for (long x = 0; x != side; ++x) {
      horizontal[k] = y * side + x;
      ++k;
    }
  }
  return horizontal;
}


size_t *create_diagonal_bl_tp_positions(long side) {

  size_t k = 0;
  size_t* diagonal = new size_t[side * side];

  for (long i = 0; i != side * 2 - 1; ++i) {
    long f_y = std::min(i, side - 1);
    long f_x = std::max(0L, i - (side - 1));
    for (long j = 0; j != side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side)
        break;
      diagonal[k] = (f_y - j) * side + (f_x + j);
      ++k;
    }
  }
  return diagonal;
}


size_t *create_diagonal_tl_br_positions(long side) {

  size_t k = 0;
  size_t* diagonal = new size_t[side * side];

  for (long i = side - 1; i > -side; --i) {
    long f_y = std::abs(std::min(0L, i));
    long f_x = std::max(0L, i);
    // top-left -> bottom-right
    for (long j = 0; j != side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;
      diagonal[k] = (f_y + j) * side + (f_x + j);
      ++k;
    }
  }
  return diagonal;
}
