
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
    _print_config();
  #endif
}


std::string Gomoku::exec_method(Arguments::pointer args) {

  method met = _commands[args->getName()];
  std::string result = ((this->*(met))(args))->to_json_string();

  if (_mode == PvE)
    _swith_color();

  return result;
}


Arguments::pointer Gomoku::_back(Arguments::pointer args) {
  args.reset(new EmptyArg());
  return args;
}


Arguments::pointer Gomoku::_winner(Arguments::pointer args) {
  args.reset(new EmptyArg());
  return args;
}


Arguments::pointer Gomoku::_end_game(Arguments::pointer args) {
  return args;
}

#include "utils.hpp"

//TODO
Arguments::pointer Gomoku::_make_turn(Arguments::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  remove_move(turn->getPosition());
  std::string position = get_and_remove_random_move();
  std::string color = _colors[_color];
  std::vector<std::string> hints = std::vector<std::string>();
  hints.push_back(get_random_move());

  return Arguments::pointer(new MakeTurn(
    color, position, std::vector<std::string>(), hints)
  );
}


//TODO
Arguments::pointer Gomoku::_print_hints(Arguments::pointer args) {
  return args;
}
