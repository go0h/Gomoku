
#include <algorithm>
#include <random>
#include "utils.hpp"


std::vector<std::string> MOVES = std::vector<std::string>();


void fill_moves(unsigned board_size) {
  for (unsigned r = 0; r < board_size; ++r) {
    for (unsigned c = 0; c < board_size; ++c) {
      MOVES.push_back(std::string(1, (char)(97 + c)) + std::string(1, (char)(49 + (r % 9))));
    }
  }
  auto rng = std::default_random_engine {};
  std::shuffle(std::begin(MOVES), std::end(MOVES), rng);
}


std::string get_random_move() {
  return MOVES[0];
}


std::string get_and_remove_random_move() {
  std::string move = *MOVES.begin();
  remove_move(move);
  return move;
}


void remove_move(std::string move) {
  MOVES.erase(std::find(MOVES.begin(), MOVES.end(), move));
}
