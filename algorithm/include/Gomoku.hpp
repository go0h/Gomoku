
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP_

#include "Board.hpp"
#include "gomoku_methods.hpp"
#include "Minimax.hpp"

#define NUM_THREADS 4

class Gomoku
{

public:

  typedef GomokuMethod (Gomoku::*method)(MethodArgs::pointer);

  typedef enum    s_gomoku_mode {
    PvE,
    PvP
  }               t_gomoku_mode;

  typedef enum    s_difficult {
    EASY    = 4,
    MEDIUM  = 6,
    HARD    = 8
  }               t_difficult;

  Gomoku();
  Gomoku(t_gomoku_mode mode, t_color color, t_difficult difficult, size_t board_size);
  ~Gomoku();

  std::string process(GomokuMethod::pointer gm);

private:

  /*
   *  Methods
   */
  GomokuMethod _back(MethodArgs::pointer args);
  GomokuMethod _start_game(MethodArgs::pointer args);
  GomokuMethod _end_game(MethodArgs::pointer args);
  GomokuMethod _make_turn(MethodArgs::pointer args);
  GomokuMethod _print_hints(MethodArgs::pointer args);
  GomokuMethod _winner(MethodArgs::pointer args);

  /*
   *  minimax utils
   */
  std::vector<std::string> _set_move_and_get_captures(Board& state, size_t x, size_t y, t_color player);
  MakeTurn*                _create_turn(std::vector<t_move_eval>& moves);

  /*
   *  Utils
   */
  void _print_config();

  t_gomoku_mode                 _mode;
  t_color                       _player;
  t_difficult                   _difficult;
  Board                         _board;
  int                           _captures[3] = { 0, 0, 0 };
  Minimax                       _minimax;

  std::map<std::string, method> _commands =
  {
    {"back",        &Gomoku::_back},
    {"start_game",  &Gomoku::_start_game},
    {"end_game",    &Gomoku::_end_game},
    {"make_turn",   &Gomoku::_make_turn},
    {"print_hints", &Gomoku::_print_hints},
    {"winner",      &Gomoku::_winner}
  };
  std::map<std::string, t_color> _str2color =
  {
    {"",        EMPTY},
    {"white",   WHITE},
    {"black",   BLACK}
  };
  std::vector<std::string> _color2str = { "", "white", "black" };
};

#endif // GOMOKU_HPP_
