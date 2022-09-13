
#include <iostream>

#include "utils.hpp"
#include "Gomoku.hpp"


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
  _board = Board(_board_size);

  #ifdef DEBUG
    _print_config();
  #endif
}


std::string Gomoku::exec_method(GomokuMethod::pointer gm) {

  method met = _commands[gm->name];
  MethodArgs::pointer arguments = ((this->*(met))(gm->arguments));

  if (_mode == PvE)
    _swith_color();

  GomokuMethod res = { gm->name, arguments };
  return res.as_json().dump();
}


MethodArgs::pointer Gomoku::_back(MethodArgs::pointer args) {
  // args.reset(new EmptyArg());
  return args;
}


MethodArgs::pointer Gomoku::_winner(MethodArgs::pointer args) {
  // args.reset(new EmptyArg());
  return args;
}


MethodArgs::pointer Gomoku::_end_game(MethodArgs::pointer args) {
  return args;
}

#include "utils.hpp"

//TODO
MethodArgs::pointer Gomoku::_make_turn(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  remove_move(turn->position);

  MakeTurn* mt = new MakeTurn();
  mt->position = get_and_remove_random_move();
  mt->color = _colors[_color];
  mt->hints = std::vector<std::string>();
  mt->hints.push_back(get_random_move());
  mt->captures = std::vector<std::string>();

  return MethodArgs::pointer(mt);
  return args;
}


//TODO
MethodArgs::pointer Gomoku::_print_hints(MethodArgs::pointer args) {
  return args;
}
