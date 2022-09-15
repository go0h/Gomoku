
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP_

#include <nlohmann/json.hpp>
#include "gomoku_methods.hpp"
#include "Board.hpp"


class Gomoku
{

public:

  typedef enum s_gomoku_mode {
    PvE,
    PvP
  }            t_gomoku_mode;

  typedef enum s_difficult {
    EASY       = 3,
    MEDIUM     = 7,
    HARD       = 11
  }            t_difficult;

  typedef MethodArgs::pointer (Gomoku::*method)(MethodArgs::pointer);


  Gomoku();
  ~Gomoku() {};

  std::string process(GomokuMethod::pointer gm);


private:

  void _print_config();
  void _switch_player() { _player = (_player == WHITE ? BLACK : WHITE); }

  MethodArgs::pointer _back(MethodArgs::pointer args);
  MethodArgs::pointer _start_game(MethodArgs::pointer args);
  MethodArgs::pointer _end_game(MethodArgs::pointer args);
  MethodArgs::pointer _make_turn(MethodArgs::pointer args);
  MethodArgs::pointer _print_hints(MethodArgs::pointer args);
  MethodArgs::pointer _winner(MethodArgs::pointer args);

  t_gomoku_mode                 _mode;
  t_color                       _player;
  t_difficult                   _difficult;
  Board                         _board;

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
