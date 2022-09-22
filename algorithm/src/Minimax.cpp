
#include "Minimax.hpp"
#include "minimax_functions.hpp"


Minimax::Minimax(size_t num_threads) :
  _num_threads(num_threads)
{

  _game_state = new t_game_state[_num_threads];

  for (size_t i = 0; i < _num_threads; ++i) {

    t_depth_state* ds = new t_depth_state[10];

    for (size_t j = 0; j < 10; ++j) {
      ds[j].num_moves = 0;
      ds[j].poss_moves = new t_move_eval[361];

      ds[j].num_captures = 0;
      ds[j].captures = new size_t[16];
    }

    _game_state[i].depth_state = ds;
  }
}


Minimax::~Minimax()
{
  for (size_t i = 0; i < _num_threads; ++i) {

    t_depth_state* ds = _game_state[i].depth_state;

    for (size_t j = 0; j < 10; ++i) {
      delete[] ds[j].poss_moves;
      delete[] ds[j].captures;
    }
    delete[] ds;
  }

  delete[] _game_state;
}


void Minimax::init_game_states(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures) {

  t_color opponent = (player == WHITE ? BLACK : WHITE);

  // t_move_eval* pm = get_possible_moves(_game_state[0], depth, player);
  // size_t&      num_moves = _game_state[0].depth_state->num_moves;

  for (size_t i = 0; i < _num_threads; ++i) {
    _game_state[i].board = board;
    _game_state[i].captures[player] = p_captures;
    _game_state[i].captures[opponent] = o_captures;

    t_depth_state* ds = _game_state[i].depth_state;

    for (size_t j = 0; j <= depth; ++j) {
      ds[j].num_moves = 0;
      ds[j].num_captures = 0;
    }
  }

  get_possible_moves(_game_state[0], depth, player);
}


std::vector<t_move_eval> Minimax::run(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures) {

  init_game_states(board, depth, player, p_captures, o_captures);

  minimax_thread(_game_state[0], player, depth);

  std::vector<t_move_eval> moves = std::vector<t_move_eval>();

  t_move_eval* ps = _game_state[0].depth_state[depth].poss_moves;
  size_t num_moves = _game_state[0].depth_state[depth].num_moves;

  qsort(ps, num_moves, sizeof(t_move_eval), &compare_moves_asc);

  for (size_t i = 0; i < num_moves; ++i) {
    moves.push_back(ps[i]);
  }

  return moves;
}
