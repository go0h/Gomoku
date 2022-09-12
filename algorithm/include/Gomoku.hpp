
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP


#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "gomoku_methods.hpp"


class Gomoku
{
public:

  typedef enum s_gomoku_mode {
    PvE,
    PvP
  }            t_gomoku_mode;

  typedef enum s_color {
    WHITE,
    BLACK
  }            t_color;

  typedef enum s_difficult {
    EASY,
    MEDIUM,
    HARD
  }            t_difficult;

  typedef MethodArgs::pointer (Gomoku::*method)(MethodArgs::pointer);


  Gomoku() {};
  Gomoku(std::string mode, std::string color, std::string difficult, unsigned board_size);
  ~Gomoku() {};

  std::string exec_method(GomokuMethod::pointer gm);


private:

  void _print_config();

  void _swith_color() { _color = (_color == WHITE ? BLACK : WHITE); }

  MethodArgs::pointer _back(MethodArgs::pointer args);
  MethodArgs::pointer _end_game(MethodArgs::pointer args);
  MethodArgs::pointer _make_turn(MethodArgs::pointer args);
  MethodArgs::pointer _print_hints(MethodArgs::pointer args);
  MethodArgs::pointer _winner(MethodArgs::pointer args);

  t_gomoku_mode                 _mode;
  t_color                       _color;
  t_difficult                   _difficult;
  unsigned                      _board_size;

  std::map<std::string, method> _commands =
  {
    {"back",        &Gomoku::_back},
    {"end_game",    &Gomoku::_end_game},
    {"make_turn",   &Gomoku::_make_turn},
    {"print_hints", &Gomoku::_print_hints},
    {"winner",      &Gomoku::_winner}
  };
  std::vector<std::string> _colors = { "white", "black" };

};

#endif // GOMOKU_HPP_
