
#include <iostream>

#include "utils.hpp"
#include "Gomoku.hpp"
#include "Arguments.hpp"

using json = nlohmann::json;

Gomoku::Gomoku(std::string mode, std::string color, std::string difficult, unsigned board_size) {

  _mode = mode == "PvP" ? PvP : PvE;
  _color = color == "WHITE" ? WHITE : BLACK;

  if (difficult == "EASY")
    _difficult = EASY;
  else if (difficult == "MEDIUM")
    _difficult = MEDIUM;
  else
    _difficult = HARD;

  _board_size = board_size;

  #ifdef DEBUG
    print_config();
  #endif
}


std::string Gomoku::exec_method(std::string& method, nlohmann::json& arguments) {

  std::cout << "Method: " << CYAN << method << RESET << std::endl;
  std::cout << "With arguments:" << std::endl;
  for (auto& [key, value] : arguments.items()) {
    std::cout << CYAN << key << ":\t" << GREEN << value << RESET << std::endl;
  }


  json responce;
  responce["method"] = method;
  responce["arguments"] = json::object();

  return responce.dump();
}

void Gomoku::print_config() {

  std::cout << MAGENTA << "Gomoku game [" << this << "] with parameters:" << std::endl;
  std::cout << GREEN << "Game mode:\t" << CYAN << (_mode == PvP ? "PvP" : "PvE") << std::endl;
  std::cout << GREEN << "Player color:\t" << CYAN << (_color == WHITE ? "WHITE" : "BLACK") << std::endl;

  std::string difficult = "EASY";
  if (_difficult == MEDIUM)
    difficult = "MEDIUM";
  else if (_difficult == HARD)
    difficult = "HARD";

  std::cout << GREEN << "Difficult:\t" << CYAN << difficult << std::endl;
  std::cout << GREEN << "Board size:\t" << CYAN << _board_size << RESET << std::endl;
}
