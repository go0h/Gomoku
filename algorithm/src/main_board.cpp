
#include <assert.h>
#include "utils.hpp"
#include "Board.hpp"
#include "Gomoku.hpp"


int main() {

  Board board = Board(19);

  board("a1") = WHITE;
  board("a19") = BLACK;
  board("s19") = WHITE;
  board("s1") = BLACK;
  board("f13") = BLACK;

  t_coord a19 = {0, 0};
  std::cout << board.coord_to_pos(a19) << std::endl;
  assert(board.coord_to_pos(a19) == "a19");

  t_coord s19 = {18, 0};
  std::cout << board.coord_to_pos(s19) << std::endl;
  assert(board.coord_to_pos(s19) == "s19");

  t_coord a1 = {0, 18};
  std::cout << board.coord_to_pos(a1) << std::endl;
  assert(board.coord_to_pos(a1) == "a1");

  t_coord s1 = {18, 18};
  std::cout << board.coord_to_pos(s1) << std::endl;
  assert(board.coord_to_pos(s1) == "s1");

  t_coord e11 = {4, 8};
  std::cout << board.coord_to_pos(e11) << std::endl;
  assert(board.coord_to_pos(e11) == "e11");


  Gomoku gomoku = Gomoku(Gomoku::PvE, WHITE, Gomoku::EASY, 19);

  t_coord best_move = gomoku.min_max();

  board(best_move) = WHITE;

  board("q1") = BLACK;

  t_coord q1 = {16, 18};
  board(q1) = WHITE;

  board.printBoard();

  std::cout << "Best move is " << board.coord_to_pos(best_move) << std::endl;

  return EXIT_SUCCESS;
}
