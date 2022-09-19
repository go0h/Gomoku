
#include <algorithm>
#include <random>
#include "utils.hpp"


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

  size_t i = 0;
  size_t* horizontal = new size_t[side * side];

  for (long y = 0; y != side; ++y) {
    for (long x = 0; x != side; ++x) {
      horizontal[i] = y * side + x;
      ++i;
    }
  }
  return horizontal;
}

size_t *create_diagonal_bl_tp_positions(long side) {

  size_t i = 0;
  size_t* diagonal = new size_t[side * side];

  for (long i = 0; i != side * 2 - 1; ++i) {
    long f_y = std::min(i, side - 1);
    long f_x = std::max(0L, i - (side - 1));
    for (long j = 0; j != side; ++j) {
      // bottom-left -> top-right
      if (f_y - j <= -1 || f_x + j >= side)
        break;
      diagonal[i] = (f_y - j) * side + (f_x + j);
      ++i;
    }
  }
  return diagonal;
}


size_t *create_diagonal_tl_br_positions(long side) {

  size_t i = 0;
  size_t* diagonal = new size_t[side * side];

  for (long i = side - 1; i > -side; --i) {
    long f_y = std::abs(std::min(0L, i));
    long f_x = std::max(0L, i);
    // top-left -> bottom-right
    for (long j = 0; j != side; ++j) {
      if (f_y + j >= side || f_x + j >= side)
        break;
      diagonal[i] = (f_y + j) * side + (f_x + j);
      ++i;
    }
  }
  return diagonal;
}
