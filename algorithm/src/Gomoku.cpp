
#include "utils.hpp"
#include "Gomoku.hpp"
#include "Minimax.hpp"


using json = nlohmann::json;


Gomoku::Gomoku() :
  _mode(PvE),
  _player(WHITE),
  _difficult(EASY),
  _board(Board(19)) { }


std::string Gomoku::process(GomokuMethod::pointer gm) {

  method met = _commands[gm->name];
  MethodArgs::pointer arguments = ((this->*(met))(gm->arguments));

  if (_mode == PvP)
    _switch_player();

  _board.printBoard();
  std::cout << std::endl;

  GomokuMethod res = { gm->name, arguments };

  return res.as_json().dump();
}


MethodArgs::pointer Gomoku::_start_game(MethodArgs::pointer args) {

  StartGame* st = dynamic_cast<StartGame*>(args.get());

  _mode = st->mode == "PvP" ? PvP : PvE;
  _player = st->player_color == "WHITE" ? BLACK : WHITE;
  _difficult = EASY;

  if (st->difficult == "MEDIUM")
    _difficult = MEDIUM;
  else if (st->difficult == "HARD")
    _difficult = HARD;

  _board = Board(st->board_size);

  #ifdef DEBUG
    _print_config();
  #endif

  return args;
}


MethodArgs::pointer Gomoku::_back(MethodArgs::pointer args) {

  Back* back = dynamic_cast<Back*>(args.get());

  t_color restore_color = _str2color[back->color] == BLACK ? WHITE : BLACK;
  _board(back->position) = EMPTY;

  for (std::string& capture : back->captures) {
    _board(capture) = restore_color;
  }

  return args;
}


MethodArgs::pointer Gomoku::_winner(MethodArgs::pointer args) {
  return args;
}


MethodArgs::pointer Gomoku::_end_game(MethodArgs::pointer args) {
  return args;
}

#include "utils.hpp"

//TODO
MethodArgs::pointer Gomoku::_make_turn(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  _board(turn->position) = _str2color[turn->color];

  for (std::string& capture : turn->captures) {
    _board(capture) = EMPTY;
  }

  Minimax minimax = Minimax(_board, _player, _difficult);
  t_coord move = minimax.min_max();
  _board(move) = _player;

  MakeTurn* mt = new MakeTurn();

  mt->position = _board.coord_to_pos(move);

  mt->color = _color2str[_player];
  mt->hints = std::vector<std::string>();
  mt->captures = std::vector<std::string>();

  return MethodArgs::pointer(mt);
}


//TODO
MethodArgs::pointer Gomoku::_print_hints(MethodArgs::pointer args) {
  return args;
}
