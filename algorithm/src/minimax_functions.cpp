
#include "Minimax.hpp"
#include "minimax_functions.hpp"
#include <iostream>


static double get_catch_score(int n) {
	switch (n) {
		case 0:
			return 0;
		case 2:
			return 40;
		case 4:
			return 500;
		case 6:
			return 1000;
		case 8:
			return 2000;
		default:
			return 180000;
	}
}


void minimax_thread_f(void* gs_ptr, int player, size_t depth) {

  t_coord       best;
  double        alpha = MINUS_INF;
  double        score = alpha;
  t_color       opponent = (player == WHITE ? BLACK : WHITE);
  bool          is_player_turn = true;
  t_game_state* gs = (t_game_state*)gs_ptr;

  t_move_eval*  possible_moves = gs->depth_state[depth].poss_moves;
  size_t        num_moves = gs->depth_state[depth].num_moves;

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval move = possible_moves[i];

    set_move_and_catch(gs, depth, move.x, move.y, (t_color)player);

    score = minimax_thread(gs, depth - 1, opponent, (t_color)player, alpha, PLUS_INF, !is_player_turn);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);

    // для отправки наилучших ходов
    possible_moves[i].score = score;

    remove_move_and_captures(gs, depth, move.x, move.y, (t_color)player);

    if (-score > alpha) {
      alpha = -score;
      best = { move.x, move.y };
    }
  }
}


double minimax_thread(t_game_state* gs, size_t depth,
                      t_color player, t_color opponent,
                      double alpha, double betta,
                      bool is_player_turn) {

  size_t is_win = 0;
  alpha = evaluate_state(gs->board, is_win, player, is_player_turn);

  if (gs->captures[player] >= 10) {
    is_win = 1;
    alpha += 120000;
  }

  if (!depth || is_win)
    return alpha;

  t_move_eval* possible_moves = get_possible_moves(gs, depth, player);
  size_t       num_moves = gs->depth_state[depth].num_moves;

  if (!num_moves)
    return alpha;

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval& move = possible_moves[i];

    set_move_and_catch(gs, depth, move.x, move.y, player);

    double score = minimax_thread(gs, depth - 1, opponent, player, -betta, -alpha, !is_player_turn);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);

    remove_move_and_captures(gs, depth, move.x, move.y, player);

    if (-score > alpha)
      alpha = -score;

    if (alpha >= betta)
      return alpha;
  }
  return alpha;
}


void set_move_and_catch(t_game_state* gs, size_t depth, size_t x, size_t y, t_color player) {

  t_point*  field = gs->board.getField();
  size_t    side = gs->board.getSide();
  t_color   opponent = player == WHITE ? BLACK : WHITE;
  size_t*   captures = gs->depth_state[depth].captures;
  size_t&   num_captures = gs->depth_state[depth].num_captures;

  gs->board(x, y) = player;

  for (size_t i = 0; i != 8; ++i) {

    size_t x_dir = DIRECTIONS[i][0];
    size_t y_dir = DIRECTIONS[i][1];

    size_t x3 = x + x_dir * 3;
    size_t y3 = y + y_dir * 3;

    if (x3 < side && y3 < side && field[y3 * side + x3] == player) {
      size_t p2 = (y + y_dir * 2) * side + (x + x_dir * 2);
      size_t p1 = (y + y_dir * 1) * side + (x + x_dir * 1);

      if (field[p1] == opponent && field[p2] == opponent) {
        field[p1] = EMPTY;
        field[p2] = EMPTY;
        captures[num_captures++] = p1;
        captures[num_captures++] = p2;
        gs->captures[player] += 2;
      }
    }
  }
}


void remove_move_and_captures(t_game_state* gs, size_t depth, size_t x, size_t y, t_color player) {

  t_point*  field = gs->board.getField();
  t_color   opponent = player == WHITE ? BLACK : WHITE;
  size_t*   captures = gs->depth_state[depth].captures;
  size_t&   num_captures = gs->depth_state[depth].num_captures;

  gs->board(x, y) = EMPTY;
  for (size_t i = 0; i < num_captures; ++i) {
    field[captures[i]] = opponent;
    gs->captures[player] -= 1;
  }
  num_captures = 0;
}
