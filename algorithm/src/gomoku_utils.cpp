
#include "Gomoku.hpp"
#include "utils.hpp"


void Gomoku::_print_config() {

  std::cout << MAGENTA << "Gomoku game [" << this << "] with parameters:" << std::endl;
  std::cout << GREEN << "Game mode:\t" << CYAN << (_mode == PvP ? "PvP" : "PvE") << std::endl;
  std::cout << GREEN << "Player color:\t" << CYAN << (_player == WHITE ? "WHITE" : "BLACK") << std::endl;

  std::string difficult = "EASY";
  if (_difficult == MEDIUM)
    difficult = "MEDIUM";
  else if (_difficult == HARD)
    difficult = "HARD";

  std::cout << GREEN << "Difficult:\t" << CYAN << difficult << std::endl;
  std::cout << GREEN << "Board size:\t" << CYAN << _board.getSide() << RESET << std::endl;
}

