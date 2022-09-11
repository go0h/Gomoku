
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP


#include <string>
#include <nlohmann/json.hpp>

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

  void print_config();

private:

  typedef std::string (*method)(nlohmann::json&);


  std::string _back(nlohmann::json& arguments);
  std::string _end_game(nlohmann::json& arguments);
  std::string _make_turn(nlohmann::json& arguments);
  std::string _print_hints(nlohmann::json& arguments);
  std::string _winner(nlohmann::json& arguments);


  t_gomoku_mode _mode;
  t_color       _color;
  t_difficult   _difficult;
  unsigned      _board_size;

};

#endif // GOMOKU_HPP_
