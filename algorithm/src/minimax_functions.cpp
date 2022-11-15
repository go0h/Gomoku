
#include "Gomoku.hpp"
#include "Minimax.hpp"
#include "minimax_functions.hpp"
#include <iostream>

static double get_catch_score(int n)
{
  switch (n)
  {
  case 0:
    return 0;
  case 2:
    return 1000;
  case 4:
    return 5000;
  case 6:
    return 10000;
  case 8:
    return 50000;
  default:
    return 180000;
  }
}

std::string coord_to_pos_debug2(size_t x, size_t y, size_t side)
{
  return std::string(1, char(97 + x)) + std::to_string(side - y);
}

void changeRestrictions(const size_t &x, const size_t &y, const size_t oldRestrictions[4], size_t newRestrictions[4])
{
  newRestrictions[0] = std::min(oldRestrictions[0], y);
  newRestrictions[1] = std::min(oldRestrictions[1], x);
  newRestrictions[2] = std::max(oldRestrictions[2], y);
  newRestrictions[3] = std::max(oldRestrictions[3], x);
  return;
}

void minimax_thread_f(void *gs_ptr, int player, size_t depth, size_t restrictions[4])
{

  int alpha = MINUS_INF;
  int beta = PLUS_INF;
  int score = MINUS_INF;
  t_color opponent = (player == WHITE ? BLACK : WHITE);
  t_game_state *gs = (t_game_state *)gs_ptr;

  t_move_eval *possible_moves = gs->depth_state[depth].poss_moves;
  size_t num_moves = gs->depth_state[depth].num_moves;
  
  for (size_t i = 0; i != num_moves; ++i)
  {

    t_move_eval move = possible_moves[i];

    set_move_and_catch(gs, depth, move.x, move.y, (t_color)player, opponent, move.is_catch);

    size_t newRestrictions[4];
    changeRestrictions(move.x, move.y, restrictions, newRestrictions);

    score = -negamax_thread(gs, depth - 1, opponent, (t_color)player, -beta, -alpha, move.score, newRestrictions);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);

    // для отправки наилучших ходов
    possible_moves[i].score = score;

    remove_move_and_captures(gs, depth, move.x, move.y, (t_color)player, opponent, move.is_catch);
    if (score > alpha)
    {
      alpha = score;
    }
  }
}

int negamax_thread(t_game_state *gs, const size_t &depth, const t_color &player, const t_color &opponent,
                      int alpha, const int beta, const int &moveScore, const size_t restrictions[4])
{
  if (moveScore == WIN_DETECTED)
  {
    return -(WIN_STATE_SCORE + static_cast<int>(depth));
  }
  int bestScore = MINUS_INF;

  if (!depth)
  {
    return -evaluate_state(gs->board, opponent, restrictions);
  }

  t_move_eval *possible_moves = get_possible_moves(gs, depth, player, opponent, restrictions);
  size_t num_moves = gs->depth_state[depth].num_moves;

  if (!num_moves)
    return alpha;

  for (size_t i = 0; i != num_moves; ++i)
  {

    t_move_eval &move = possible_moves[i];

    set_move_and_catch(gs, depth, move.x, move.y, player, opponent, move.is_catch);

    size_t newRestrictions[4];
    changeRestrictions(move.x, move.y, restrictions, newRestrictions);

    int score = -negamax_thread(gs, depth - 1, opponent, player, -beta, -alpha, move.score, newRestrictions);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);

    remove_move_and_captures(gs, depth, move.x, move.y, player, opponent, move.is_catch);

    if (score > bestScore)
    {
      bestScore = score;
    }
    if (bestScore > alpha)
    {
      alpha = score;
    }

    if (alpha > beta)
    {
      break;
    }
  }
  return bestScore;
}

void set_move_and_catch(t_game_state *gs, const size_t &depth, const size_t &x_long, const size_t &y_long, const t_color &player, const t_color &opponent, const bool &is_catch)
{

  gs->board(x_long, y_long) = player;
  if (!is_catch)
  {
    return;
  }
  t_point *field = gs->board.getField();
  size_t side_long = gs->board.getSide();
  size_t *captures = gs->depth_state[depth].captures;
  size_t &num_captures = gs->depth_state[depth].num_captures;

    int side = static_cast<int>(side_long);
    int y = static_cast<int>(y_long);
    int x = static_cast<int>(x_long);


  for (int i = 0; i != 8; ++i)
  {

    int x_dir = DIRECTIONS[i][0];
    int y_dir = DIRECTIONS[i][1];

    int x3 = x + x_dir * 3;
    int y3 = y + y_dir * 3;

    if (x3 >= 0 && x3 < side && y3 >= 0 && y3 < side && field[y3 * side + x3] == player)
    {
      size_t p2 = (y + y_dir * 2) * side + (x + x_dir * 2);
      size_t p1 = (y + y_dir * 1) * side + (x + x_dir * 1);

      if (field[p1] == opponent && field[p2] == opponent)
      {
        field[p1] = EMPTY;
        field[p2] = EMPTY;
        captures[num_captures++] = p1;
        captures[num_captures++] = p2;
        gs->captures[player] += 2;
      }
    }
  }
  return;
}

void remove_move_and_captures(t_game_state *gs, const size_t &depth, const size_t &x, const size_t &y, const t_color &player, const t_color &opponent, const bool &is_catch)
{
  gs->board(x, y) = EMPTY;
  if (!is_catch)
  {
    return;
  }
  t_point *field = gs->board.getField();
  size_t *captures = gs->depth_state[depth].captures;
  size_t &num_captures = gs->depth_state[depth].num_captures;

  for (size_t i = 0; i < num_captures; ++i)
  {
    field[captures[i]] = opponent;
    gs->captures[player] -= 1;
  }
  num_captures = 0;
  return ;
}
