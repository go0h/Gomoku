
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP


#include <string>
#include <nlohmann/json.hpp>
#include "Arguments.hpp"

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


  Gomoku() {};
  Gomoku(std::string mode, std::string color, std::string difficult, unsigned board_size);
  ~Gomoku() {};


  std::string exec_method(std::string& method, nlohmann::json& arguments);

  std::string exec_method(Arguments& arguments);

  void print_config();

private:

  Arguments* _back(Arguments& args);
  Arguments* _end_game(Arguments& args);
  Arguments* _make_turn(Arguments& args);
  Arguments* _print_hints(Arguments& args);
  Arguments* _winner(Arguments& args);

  t_gomoku_mode _mode;
  t_color       _color;
  t_difficult   _difficult;
  unsigned      _board_size;

};

#endif // GOMOKU_HPP_
