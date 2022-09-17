
#ifndef GOMOKU_HPP_
# define GOMOKU_HPP_

# define PLUS_INF  (1.0 / 0.0)
# define MINUS_INF (-1.0 / 0.0)

extern int DIRECTIONS[8][2];

#include <nlohmann/json.hpp>
#include "Board.hpp"
#include "gomoku_methods.hpp"


class Gomoku
{

public:

  typedef MethodArgs::pointer (Gomoku::*method)(MethodArgs::pointer);


  typedef enum    s_gomoku_mode {
    PvE,
    PvP
  }               t_gomoku_mode;

  typedef enum    s_difficult {
    EASY    = 4,
    MEDIUM  = 7,
    HARD    = 11
  }               t_difficult;

  typedef struct  s_move_eval
  {
    double    score;
    size_t    x;
    size_t    y;
  }               t_move_eval;

  using t_possible_moves = std::vector<t_move_eval>;

  typedef struct  s_depth_state
  {
    t_possible_moves      poss_moves;
    size_t                num_moves;
    std::vector<size_t>   depth_catches;
  }               t_depth_state;

  Gomoku();
  Gomoku(t_gomoku_mode mode, t_color color, t_difficult difficult, size_t board_size);
  ~Gomoku();

  std::string process(GomokuMethod::pointer gm);


  /*
   *  Minimax
   */
  MakeTurn*   minimax();
  double      minimax(Board& state, size_t depth, t_color player, t_color opponent, double low, double high);


private:

  /*
   *  Methods
   */
  MethodArgs::pointer _back(MethodArgs::pointer args);
  MethodArgs::pointer _start_game(MethodArgs::pointer args);
  MethodArgs::pointer _end_game(MethodArgs::pointer args);
  MethodArgs::pointer _make_turn(MethodArgs::pointer args);
  MethodArgs::pointer _print_hints(MethodArgs::pointer args);
  MethodArgs::pointer _winner(MethodArgs::pointer args);

  /*
   *  minimax utils
   */
  t_possible_moves& _get_possible_moves(size_t depth, t_color player);
  void              _set_move_and_catch(Board& state, size_t depth, size_t x, size_t y, t_color player);
  void              _remove_move_and_catches(Board& state, size_t depth, size_t x, size_t y, t_color player);


  /*
   *  Utils
   */
  void _print_config();
  void _switch_player() { _player = (_player == WHITE ? BLACK : WHITE); }


  t_gomoku_mode                 _mode;
  t_color                       _player;
  t_difficult                   _difficult;
  Board                         _board;
  t_depth_state*                _depth_state;
  int                           _captures[3] = { 0, 0, 0 };

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

double evaluate_state(Board& state, t_color player, t_color opponent, bool is_player_turn);

#endif // GOMOKU_HPP_
