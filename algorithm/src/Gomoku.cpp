
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
  _depth_state = nullptr;
}


std::string Gomoku::process(GomokuMethod::pointer gm) {

  GomokuMethod  res;

  if (_mode == PvE) {

    method method_ptr = _commands[gm->name];
    res = ((this->*(method_ptr))(gm->arguments));

  } else {

    if (gm->name == "make_turn")
      gm->name = "print_hints";

    method method_ptr = _commands[gm->name];
    res = ((this->*(method_ptr))(gm->arguments));
  }

  #ifdef DEBUG
    _board.printBoard();
    std::cout << std::endl;
  #endif

  return res.as_json().dump();
}


GomokuMethod Gomoku::_start_game(MethodArgs::pointer args) {

  StartGame* st = dynamic_cast<StartGame*>(args.get());

  _mode = st->mode == "PvP" ? PvP : PvE;
  _player = st->player_color == "WHITE" ? BLACK : WHITE;

  // если мод PvP подсказки всегда на EASY
  if (st->difficult == "EASY" || _mode == PvP)
    _difficult = EASY;
  else if (st->difficult == "MEDIUM")
    _difficult = MEDIUM;
  else
    _difficult = HARD;

  _board = Board(st->board_size);
  memset(_captures, 0, sizeof(_captures));

  #ifdef DEBUG
    _print_config();
  #endif

  // первый ход ничего не делаем, если начинаем вторыми или игра PvP
  if (_player == WHITE || _mode == PvP)
    return { "start_game", args };

  // если бот начинает первым
  t_coord best = minimax();

  _set_move_and_catch(_board, _difficult, best.x, best.y, _player);

  MakeTurn* move = _create_turn(best);

  return { "make_turn", MethodArgs::pointer(move) };
}


GomokuMethod Gomoku::_back(MethodArgs::pointer args) {

  Back* back = dynamic_cast<Back*>(args.get());

  t_color restore_color = _str2color[back->color] == BLACK ? WHITE : BLACK;
  _board(back->position) = EMPTY;

  for (std::string& capture : back->captures) {
    _board(capture) = restore_color;
  }
  _captures[_str2color[back->color]] -= back->captures.size();

  return { "back", args };
}


GomokuMethod Gomoku::_winner(MethodArgs::pointer args) {
  return { "winner", args };
}


GomokuMethod Gomoku::_end_game(MethodArgs::pointer args) {
  return { "end_game", args };
}


GomokuMethod Gomoku::_make_turn(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  t_color opponent = _str2color[turn->color];
  _board(turn->position) = opponent;

  for (std::string& capture : turn->captures) {
    _board(capture) = EMPTY;
  }
  _captures[opponent] += turn->captures.size();

  t_coord best = minimax();

  _set_move_and_catch(_board, _difficult, best.x, best.y, _player);

  MakeTurn* move = _create_turn(best);

  return { "make_turn", MethodArgs::pointer(move) };
}


GomokuMethod Gomoku::_print_hints(MethodArgs::pointer args) {

  MakeTurn* turn = dynamic_cast<MakeTurn*>(args.get());

  t_color opponent = _str2color[turn->color];
  _board(turn->position) = opponent;

  for (std::string& capture : turn->captures) {
    _board(capture) = EMPTY;
  }
  _captures[opponent] += turn->captures.size();

  _player = opponent == WHITE ? BLACK : WHITE;

  minimax();

  qsort(_depth_state[_difficult].poss_moves, _depth_state[_difficult].num_moves,
        sizeof(t_move_eval), &compare_moves_asc);

  Hints* hint = new Hints();

  for (size_t i = 0; i < 5 && i < _depth_state[_difficult].num_moves; ++i) {
    t_move_eval move = _depth_state[_difficult].poss_moves[i];
    hint->hints.push_back(_board.coord_to_pos(move.x, move.y));
  }

  return { "print_hints", MethodArgs::pointer(hint) };
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
  std::cout << GREEN << "Player captures:\t" << CYAN << _captures[_player] << std::endl;

  std::cout << GREEN << "Opponent color:\t\t" << CYAN << (_player == WHITE ? "BLACK" : "WHITE") << std::endl;
  std::cout << GREEN << "Opponent captures:\t" << CYAN << \
    _captures[(_player == WHITE ? BLACK : WHITE)] << RESET << std::endl;
}
