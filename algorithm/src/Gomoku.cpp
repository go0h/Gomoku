
#include "utils.hpp"
#include "Gomoku.hpp"

using json = nlohmann::json;


Gomoku::Gomoku() :
  _mode(PvE),
  _player(WHITE),
  _difficult(EASY),
  _board(Board(19))
{
  _depth_state = new t_depth_state[HARD + 1];

  _score_states = std::map<size_t, double>();

  for (size_t i = 0; i <= HARD; ++i) {
    _depth_state[i].num_moves = 0;
    _depth_state[i].poss_moves = new t_move_eval[_board.getSide() * _board.getSide()];

    _depth_state[i].num_captures = 0;
    _depth_state[i].captures = new size_t[16];
  }

}


Gomoku::Gomoku(t_gomoku_mode mode, t_color color, t_difficult difficult, size_t board_size) :
  _mode(mode),
  _player(color),
  _difficult(difficult),
  _board(Board(board_size))
{
  _depth_state = new t_depth_state[HARD + 1];

  _score_states = std::map<size_t, double>();

  for (size_t i = 0; i <= HARD; ++i) {
    _depth_state[i].num_moves = 0;
    _depth_state[i].poss_moves = new t_move_eval[_board.getSide() * _board.getSide()];

    _depth_state[i].num_captures = 0;
    _depth_state[i].captures = new size_t[16];
  }

}

Gomoku::~Gomoku() {

  for (size_t i = 0; i <= HARD; ++i) {
    delete[] _depth_state[i].poss_moves;
    delete[] _depth_state[i].captures;
  }

  delete[] _depth_state;
}


std::string Gomoku::process(GomokuMethod::pointer gm) {
  method met = _commands[gm->name];
  MethodArgs::pointer arguments = ((this->*(met))(gm->arguments));

  if (_mode == PvP)
    _switch_player();

  #ifdef DEBUG
    _board.printBoard();
    std::cout << std::endl;
  #endif

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
  memset(_captures, 0, sizeof(_captures));

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
  _captures[_str2color[back->color]] += back->captures.size();

  return args;
}


MethodArgs::pointer Gomoku::_winner(MethodArgs::pointer args) {
  return args;
}


MethodArgs::pointer Gomoku::_end_game(MethodArgs::pointer args) {
  return args;
}


MethodArgs::pointer Gomoku::_make_turn(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  t_color opponent = _str2color[turn->color];
  _board(turn->position) = opponent;

  for (std::string& capture : turn->captures) {
    _board(capture) = EMPTY;
  }
  _captures[opponent] += turn->captures.size();

  MakeTurn* move = minimax();

  _board(move->position) = _player;
  for (std::string& capture : move->captures) {
    _board(capture) = EMPTY;
    _captures[_player] += 1;
  }

  return MethodArgs::pointer(move);
}


//TODO
MethodArgs::pointer Gomoku::_print_hints(MethodArgs::pointer args) {
  return args;
}


void Gomoku::_print_config() {

  std::cout << MAGENTA << "Gomoku game [" << this << "] with parameters:" << std::endl;
  std::cout << GREEN << "Game mode:\t\t" << CYAN << (_mode == PvP ? "PvP" : "PvE") << std::endl;

  std::string difficult = "EASY";
  if (_difficult == MEDIUM)
    difficult = "MEDIUM";
  else if (_difficult == HARD)
    difficult = "HARD";

  std::cout << GREEN << "Difficult:\t\t" << CYAN << difficult << std::endl;
  std::cout << GREEN << "Board size:\t\t" << CYAN << _board.getSide() << std::endl;

  std::cout << GREEN << "Player color:\t\t" << CYAN << (_player == WHITE ? "WHITE" : "BLACK") << std::endl;
  std::cout << GREEN << "Player captures:\t\t" << CYAN << _captures[_player] << std::endl;

  std::cout << GREEN << "Opponent color:\t\t" << CYAN << (_player == WHITE ? "BLACK" : "WHITE") << std::endl;
  std::cout << GREEN << "Opponent captures:\t" << CYAN << \
    _captures[(_player == WHITE ? BLACK : WHITE)] << RESET << std::endl;
}
