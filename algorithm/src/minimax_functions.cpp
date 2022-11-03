
#include "Minimax.hpp"
#include "minimax_functions.hpp"
#include <iostream>
#define WIN_DETECTED -1

static double get_catch_score(int n) {
  switch (n) {
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

std::string coord_to_pos_debug2(size_t x, size_t y, size_t side) {
  return std::string(1, char(97 + x)) + std::to_string(side - y);
}

void minimax_thread_f(void* gs_ptr, int player, size_t depth) {

  double        alpha = MINUS_INF;
  double beta = PLUS_INF;
  double        score = MINUS_INF;
  t_color       opponent = (player == WHITE ? BLACK : WHITE);
  // bool          is_player_turn = true;
  t_game_state* gs = (t_game_state*)gs_ptr;

  
  t_move_eval*  possible_moves = gs->depth_state[depth].poss_moves;
  size_t        num_moves = gs->depth_state[depth].num_moves;

//  std::cout << "minimax_thread_f player="<< player << ", depth="<< depth << ", is_player_turn=" << is_player_turn << ", num_moves=" << num_moves << std::endl;
  

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval move = possible_moves[i];

    set_move_and_catch(gs, depth, move.x, move.y, (t_color)player);

    score = - negamax_thread(gs, depth - 1, opponent, (t_color)player, -beta, -alpha, move.score);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);

    // для отправки наилучших ходов
    possible_moves[i].score = score;

    remove_move_and_captures(gs, depth, move.x, move.y, (t_color)player);
    // std::cout << "minimax_thread_f player="<< player << ", depth="<< depth << ", score=" << score << ", x=" << coord_to_pos_debug2(move.x, move.y,gs->board.getSide())  << std::endl;
    if (score > alpha) {
      alpha = score;
    }
  }
}


double negamax_thread(t_game_state* gs, size_t depth,
                      t_color player, t_color opponent,
                      double alpha, double beta, double moveScore) {

  // std::cout << std::endl << "negamax_thread player="<< opponent << ", depth="<< depth << std::endl;
  //  gs->board.printBoard();
  if (moveScore == WIN_DETECTED) {
      // if (opponent == 2){
      // std::cout << "WIN_DETECTED player="<< opponent << ", depth=" << depth << ", move score="<< moveScore << ", alpha="<< alpha <<  ", return=" << -(100000 + static_cast<int>(depth)) << std::endl;
  //    gs->board.printBoard();
      // }
    return -(100000 + + static_cast<int>(depth));        
  }                
  double bestScore = MINUS_INF;                        
  // size_t is_win = 0;
  size_t restrictions[4];
  // side = gs->board.getSide();
  // t_point*      field = gs->board.getField();
  setRestrictions(gs->board.getField(), gs->board.getSide(), restrictions) ;
  // alpha = evaluate_state(gs->board, is_win, player, is_player_turn);
  //alpha = evaluate_state(gs->board, player, restrictions) ;
  // alpha = -moveScore;
  // std::cout << "move score="<< moveScore << "alpha="<< alpha <<  std::endl;
  
  // if (gs->captures[player] >= 10) {
  //   is_win = 1;
  //   alpha += 120000;
  // }

  if (!depth) {
    bestScore =  evaluate_state(gs->board, opponent, restrictions);
    // std::cout << "leaf player="<< opponent << ", depth="<< depth << ", score=" << bestScore << std::endl;
  //gs->board.printBoard();
    return bestScore;
  }

  t_move_eval* possible_moves = get_possible_moves(gs, depth, player);
  size_t       num_moves = gs->depth_state[depth].num_moves;

  // std::cout << "minimax_thread get_possible_moves, player="<< player <<    ", depth="<< depth << ", is_player_turn=" << is_player_turn <<    ", num_moves=" << num_moves <<  std::endl;
  
  if (!num_moves)
    return alpha;

  for (size_t i = 0; i != num_moves; ++i) {

    t_move_eval& move = possible_moves[i];
      
    set_move_and_catch(gs, depth, move.x, move.y, player);

    double score = - negamax_thread(gs, depth - 1, opponent, player, -beta, -alpha, move.score);

    if (gs->depth_state[depth].num_captures)
      score += get_catch_score(gs->captures[player]);
    // std::cout << "player="<< player << ", depth="<< depth << ", score=" << score << ", x=" << coord_to_pos_debug2(move.x, move.y,gs->board.getSide())  << std::endl;

    remove_move_and_captures(gs, depth, move.x, move.y, player);


    if (score > bestScore) {
      bestScore = score;
    }
      if (bestScore > alpha) {
        alpha = score;
      }
    
    if (alpha >= beta){
      // bestScore = score;
      break;
    }
    // if (score >= beta)
    //   return beta;
  }
  return bestScore;
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
