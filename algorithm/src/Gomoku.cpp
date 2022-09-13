
#include <iostream>

#include "utils.hpp"
#include "Gomoku.hpp"
#include "Minimax.hpp"


using json = nlohmann::json;

Gomoku::Gomoku(std::string mode, std::string color, std::string difficult, unsigned board_size) {

  _mode = mode == "PvP" ? PvP : PvE;
  _color = color == "WHITE" ? BLACK : WHITE;

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

  if (_mode == PvP)
    _swith_color();

  GomokuMethod res = { gm->name, arguments };
  return res.as_json().dump();
}

//TODO
MethodArgs::pointer Gomoku::_back(MethodArgs::pointer args) {

  Back* back = dynamic_cast<Back*>(args.get());

  t_color color = (back->color == "black") ? BLACK : WHITE;
  t_color opposite_color = color == BLACK ? WHITE : BLACK;

  _board(back->position) = EMPTY;

  for (std::string& capture : back->captures) {
    _board(capture) = opposite_color;
  }

  return args;
}

//TODO
MethodArgs::pointer Gomoku::_winner(MethodArgs::pointer args) {
  // args.reset(new EmptyArg());
  return args;
}

//TODO
MethodArgs::pointer Gomoku::_end_game(MethodArgs::pointer args) {
  return args;
}

#include "utils.hpp"

//TODO
MethodArgs::pointer Gomoku::_make_turn(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  t_color color = (turn->color == "black") ? BLACK : WHITE;

  _board(turn->position) = color;

  for (std::string& capture : turn->captures) {
    _board(capture) = EMPTY;
  }

  Minimax minimax = Minimax(_board, _color, 2);
  t_coord move = minimax.min_max(2);
  _board(move) = _color;

  MakeTurn* mt = new MakeTurn();

  mt->position = _board.coord_to_pos(move);

  mt->color = _colors[_color];
  mt->hints = std::vector<std::string>();
  mt->captures = std::vector<std::string>();

  return MethodArgs::pointer(mt);
}


//TODO
MethodArgs::pointer Gomoku::_print_hints(MethodArgs::pointer args) {
  return args;
}
