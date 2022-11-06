
#include "utils.hpp"
#include "Gomoku.hpp"

using json = nlohmann::json;


Gomoku::Gomoku() :
  _mode(PvE),
  _player(WHITE),
  _difficult(EASY),
  _board(Board(19)),
  _minimax(Minimax(NUM_THREADS)) { }


Gomoku::Gomoku(t_gomoku_mode mode, t_color color, t_difficult difficult, size_t board_size) :
  _mode(mode),
  _player(color),
  _difficult(difficult),
  _board(Board(board_size)),
  _minimax(Minimax(NUM_THREADS)) { }


Gomoku::~Gomoku() { }


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
  if (_player == WHITE || _mode == PvP) {
    return { "start_game", args };
  }

  t_color opponent = _player == WHITE ? BLACK : WHITE;

  // если бот начинает первым
  std::vector<t_move_eval> moves = _minimax.run(
    _board, _difficult, _player, _captures[_player], _captures[opponent]);

  MakeTurn* move = _create_turn(moves);

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

  std::vector<t_move_eval> moves = _minimax.run(
    _board, _difficult, _player, _captures[_player], _captures[opponent]);

  MakeTurn* move = _create_turn(moves);

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

  std::vector<t_move_eval> moves = _minimax.run(
    _board, _difficult, _player, _captures[_player], _captures[opponent]);

  Hints* hint = new Hints();

  for (size_t i = 0; i < 5 && i < moves.size(); ++i) {
    hint->hints.push_back(_board.coord_to_pos(moves[i].x, moves[i].y));
  }

  return { "print_hints", MethodArgs::pointer(hint) };
}


MakeTurn* Gomoku::_create_turn(std::vector<t_move_eval>& moves) {

  MakeTurn*    m = new MakeTurn();

  t_move_eval best_move = *moves.begin();
  moves.erase(moves.begin());

  m->position = _board.coord_to_pos(best_move.x, best_move.y);
  m->color = _color2str[_player];
  m->captures = _set_move_and_get_captures(_board, best_move.x, best_move.y, _player);

  for (size_t i = 0; i != moves.size(); ++i) {
    m->hints.push_back(_board.coord_to_pos(moves[i].x, moves[i].y));
  }

  return m;
}


std::vector<std::string> Gomoku::_set_move_and_get_captures(Board& state, size_t x, size_t y, t_color player) {

  size_t                   side = state.getSide();
  t_color                  opponent = player == WHITE ? BLACK : WHITE;
  std::vector<std::string> captures = std::vector<std::string>();

  state(x, y) = player;

  for (size_t i = 0; i != 8; ++i) {

    size_t x_dir = DIRECTIONS[i][0];
    size_t y_dir = DIRECTIONS[i][1];

    size_t x3 = x + x_dir * 3;
    size_t y3 = y + y_dir * 3;

    if (x3 < side && y3 < side && state(x3, y3) == player) {

      size_t y1 = (y + y_dir * 1);
      size_t x1 = (x + x_dir * 1);

      size_t y2 = (y + y_dir * 2);
      size_t x2 = (x + x_dir * 2);

      if (state(x1, y1) == opponent && state(x2, y2) == opponent) {
        state(x1, y1) = EMPTY;
        state(x2, y2) = EMPTY;
        captures.push_back(state.coord_to_pos(x1, y1));
        captures.push_back(state.coord_to_pos(x2, y2));
      }
    }
  }

  return captures;
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
