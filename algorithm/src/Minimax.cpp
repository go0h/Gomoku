
#include "Minimax.hpp"
#include "minimax_functions.hpp"
#include <functional>
#include <cstring>

Minimax::Minimax(size_t num_threads) :
  _num_threads(num_threads),
  _pool(ThreadPool(num_threads))
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


void Minimax::_init_game_states(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures) {

  t_color opponent = (player == WHITE ? BLACK : WHITE);

  // инициализируем состояния игры для каждого потока
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

  // находим возможные позиции для первого хода
  t_move_eval* pm = get_possible_moves(&_game_state[0], depth, player);

  size_t all_moves = _game_state[0].depth_state[depth].num_moves;
  size_t one_thread_moves = all_moves / _num_threads;

  // распределяем позиции равномерно для каждого потока
  for (size_t i = 0; i < _num_threads; ++i) {

    t_depth_state* ds = _game_state[i].depth_state;

    std::memcpy(ds[depth].poss_moves,
               &pm[i * one_thread_moves],
                one_thread_moves * sizeof(t_move_eval));

    ds[depth].num_moves = one_thread_moves;
  }

  // дописываем по одному ходу из остатка в состояния
  if (one_thread_moves * _num_threads != all_moves) {

    size_t distr_num = one_thread_moves * _num_threads;
    size_t rest = all_moves - distr_num;

    for (size_t i = 0; i < rest; ++i) {

      t_depth_state* ds = _game_state[i].depth_state;
      size_t n = ds[depth].num_moves;

      std::memcpy(&ds[depth].poss_moves[n], &pm[distr_num], sizeof(t_move_eval));
      ds[depth].num_moves += 1;

      distr_num++;
    }
  }
}


std::vector<t_move_eval> Minimax::_get_sorted_moves(size_t depth) {

  t_move_eval* ps = _game_state[0].depth_state[depth].poss_moves;
  size_t& num_moves = _game_state[0].depth_state[depth].num_moves;

  // собираем со всех игровых состояний ходы
  for (size_t i = 1; i < _num_threads; ++i) {

    t_move_eval* thr_moves = _game_state[i].depth_state[depth].poss_moves;
    size_t num_thr_moves = _game_state[i].depth_state[depth].num_moves;

    std::memcpy(&ps[num_moves], thr_moves, num_thr_moves * sizeof(t_move_eval));
    num_moves += num_thr_moves;
  }

  // сортируем по возрастанию
  qsort(ps, num_moves, sizeof(t_move_eval), &compare_moves_asc);

  std::vector<t_move_eval> moves = std::vector<t_move_eval>();

  for (size_t i = 0; i < num_moves; ++i) {
    moves.push_back(ps[i]);
  }

  return moves;
}

std::vector<t_move_eval> Minimax::run(Board board, size_t depth, t_color player, size_t p_captures, size_t o_captures) {

  _init_game_states(board, depth, player, p_captures, o_captures);

  for (size_t i = 0; i < _num_threads; ++i) {
    auto f = std::bind(minimax_thread_f, &_game_state[i], (int)player, depth);
    _pool.add_task(f);
  }
  _pool.wait_all();

  return _get_sorted_moves(depth);
}
