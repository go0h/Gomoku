
#include <assert.h>
#include "utils.hpp"
#include "Board.hpp"
#include "Gomoku.hpp"

size_t get_num_of_free_threes(t_point* field, size_t side, size_t x, size_t y, t_color player);
bool   is_possible_capture(t_point* field, size_t side, size_t x, size_t y, t_color player);

double evaluate_state(Board& state, t_color player, t_color opponent, bool is_player_turn);


Board get_board_with_pieces(std::map<std::string, t_color> positions) {

  Board board = Board(19);

  for (auto it = positions.begin(); it != positions.end(); ++it) {
    board(it->first) = it->second;
  }
  return board;
}

void test_coordinates() {

  Board board = Board(19);

  t_coord a19 = {0, 0};
  assert(board.coord_to_pos(a19) == "a19");

  t_coord s19 = {18, 0};
  assert(board.coord_to_pos(s19) == "s19");

  t_coord a1 = {0, 18};
  assert(board.coord_to_pos(a1) == "a1");

  t_coord s1 = {18, 18};
  assert(board.coord_to_pos(s1) == "s1");

  t_coord e11 = {4, 8};
  assert(board.coord_to_pos(e11) == "e11");

  std::cout << __func__ << " - OK" << std::endl;
}


bool is_forbidden_turn_pos(Board& board, std::string pos, t_color color) {

  t_coord coord = board.pos_to_coord(pos);

  return get_num_of_free_threes(board.getField(), board.getSide(), coord.x, coord.y, color) > 1;
}


void test_horizontal_double_three() {

  std::map<std::string, t_color> positions = {
    {"b11", BLACK}, {"c11", BLACK}, {"g11", BLACK}, {"h11", BLACK},
    {"b12", WHITE}, {"c12", WHITE}, {"g12", WHITE}, {"h12", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "e11", BLACK));
  assert(is_forbidden_turn_pos(b, "e12", WHITE));


  assert(is_forbidden_turn_pos(b, "f11", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "f12", WHITE) == false);

  assert(is_forbidden_turn_pos(b, "d11", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "d12", WHITE) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_vertical_double_three() {

  std::map<std::string, t_color> positions = {
    {"e18", BLACK}, {"e17", BLACK}, {"e13", BLACK}, {"e12", BLACK},
    {"f18", WHITE}, {"f17", WHITE}, {"f13", WHITE}, {"f12", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "e15", BLACK));
  assert(is_forbidden_turn_pos(b, "f15", WHITE));


  assert(is_forbidden_turn_pos(b, "e14", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "f14", WHITE) == false);

  assert(is_forbidden_turn_pos(b, "e16", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "f16", WHITE) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_diagonal_top_left__bottom_right_double_three() {

  std::map<std::string, t_color> positions = {
    {"b17", BLACK}, {"c16", BLACK}, {"g12", BLACK}, {"h11", BLACK},
    {"c18", WHITE}, {"d17", WHITE}, {"h13", WHITE}, {"i12", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "e14", BLACK));
  assert(is_forbidden_turn_pos(b, "f15", WHITE));

  assert(is_forbidden_turn_pos(b, "d15", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "e16", WHITE) == false);

  assert(is_forbidden_turn_pos(b, "f13", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "g14", WHITE) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_diagonal_bottom_left__top_right_double_three() {

  std::map<std::string, t_color> positions = {
    {"b3", BLACK}, {"c4", BLACK}, {"g8", BLACK}, {"h9", BLACK},
    {"c2", WHITE}, {"d3", WHITE}, {"h7", WHITE}, {"i8", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "e6", BLACK));
  assert(is_forbidden_turn_pos(b, "f5", WHITE));

  assert(is_forbidden_turn_pos(b, "d5", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "e4", WHITE) == false);

  assert(is_forbidden_turn_pos(b, "f7", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "g6", WHITE) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_forbidden_from_subject() {

  std::map<std::string, t_color> positions = {
    {"f14", BLACK}, {"g15", WHITE}, {"g13", BLACK}, {"f13", WHITE},
    {"j11", BLACK}, {"j13", WHITE}, {"k11", BLACK}, {"h10", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "i11", BLACK));


  assert(is_forbidden_turn_pos(b, "h12", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "l11", WHITE) == false);
  assert(is_forbidden_turn_pos(b, "e15", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_forbidden_diagonal_horizontal() {

  std::map<std::string, t_color> positions = {
    {"g9", BLACK},  {"i9", WHITE},  {"h10", BLACK}, {"k10", WHITE},
    {"k12", BLACK}, {"h12", WHITE}, {"l12", BLACK}, {"l13", WHITE}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_forbidden_turn_pos(b, "j12", BLACK));

  assert(is_forbidden_turn_pos(b, "i11", BLACK) == false);
  assert(is_forbidden_turn_pos(b, "m12", WHITE) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


bool is_possible_capture_pos(Board& b, std::string pos, t_color player) {

  t_coord c = b.pos_to_coord(pos);

  return is_possible_capture(b.getField(), b.getSide(), c.x, c.y, player);

}


void test_possible_horizontal_capture_left() {

  std::map<std::string, t_color> positions = {
    {"a9", BLACK}, {"b9", WHITE}, {"d9", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "c9", WHITE));
  assert(is_possible_capture_pos(b, "c9", BLACK) == false);

  positions = {
    {"a9", BLACK}, {"c9", WHITE}, {"d9", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "b9", WHITE));
  assert(is_possible_capture_pos(b, "b9", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_horizontal_capture_right() {

  std::map<std::string, t_color> positions = {
    {"s9", BLACK}, {"r9", WHITE}, {"p9", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "q9", WHITE));
  assert(is_possible_capture_pos(b, "q9", BLACK) == false);

  positions = {
    {"s9", BLACK}, {"q9", WHITE}, {"p9", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "r9", WHITE));
  assert(is_possible_capture_pos(b, "r9", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_vertical_capture_top() {

  std::map<std::string, t_color> positions = {
    {"a19", BLACK}, {"a18", WHITE}, {"a16", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "a17", WHITE));
  assert(is_possible_capture_pos(b, "a17", BLACK) == false);

  positions = {
    {"a19", BLACK}, {"a17", WHITE}, {"a16", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "a18", WHITE));
  assert(is_possible_capture_pos(b, "a18", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_vertical_capture_bottom() {

  std::map<std::string, t_color> positions = {
    {"a1", BLACK}, {"a2", WHITE}, {"a4", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "a3", WHITE));
  assert(is_possible_capture_pos(b, "a3", BLACK) == false);

  positions = {
    {"a1", BLACK}, {"a3", WHITE}, {"a4", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "a2", WHITE));
  assert(is_possible_capture_pos(b, "a2", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_diagonal_capture_top_left() {

  std::map<std::string, t_color> positions = {
    {"a19", BLACK}, {"b18", WHITE}, {"d16", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "c17", WHITE));
  assert(is_possible_capture_pos(b, "c17", BLACK) == false);

  positions = {
    {"a19", BLACK}, {"c17", WHITE}, {"d16", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "b18", WHITE));
  assert(is_possible_capture_pos(b, "b18", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_diagonal_capture_bottom_right() {

  std::map<std::string, t_color> positions = {
    {"s1", BLACK}, {"r2", WHITE}, {"p4", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "q3", WHITE));
  assert(is_possible_capture_pos(b, "q3", BLACK) == false);

  positions = {
    {"s1", BLACK}, {"q3", WHITE}, {"p4", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "r2", WHITE));
  assert(is_possible_capture_pos(b, "r2", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_diagonal_capture_bottom_left() {

  std::map<std::string, t_color> positions = {
    {"a1", BLACK}, {"b2", WHITE}, {"d4", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "c3", WHITE));
  assert(is_possible_capture_pos(b, "c3", BLACK) == false);

  positions = {
    {"a1", BLACK}, {"c3", WHITE}, {"d4", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "b2", WHITE));
  assert(is_possible_capture_pos(b, "b2", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_possible_diagonal_capture_top_right() {

  std::map<std::string, t_color> positions = {
    {"s19", BLACK}, {"r18", WHITE}, {"p16", BLACK}
  };

  Board b = get_board_with_pieces(positions);

  assert(is_possible_capture_pos(b, "q17", WHITE));
  assert(is_possible_capture_pos(b, "q17", BLACK) == false);

  positions = {
    {"s19", BLACK}, {"q17", WHITE}, {"p16", BLACK}
  };

  b = get_board_with_pieces(positions);
  assert(is_possible_capture_pos(b, "r18", WHITE));
  assert(is_possible_capture_pos(b, "r18", BLACK) == false);

  std::cout << __func__ << " - OK" << std::endl;
}


void test_evaluation() {

  std::map<std::string, t_color> positions = {

    // vertical
    {"a1", BLACK}, {"a2", BLACK}, {"a3", BLACK}, {"a4", BLACK}, {"a5", BLACK},

    // horizont
    {"a1", BLACK}, {"b1", BLACK}, {"c1", BLACK}, {"d1", BLACK}, {"e1", BLACK},

    // diagonal bottom left - top right
    {"a1", BLACK}, {"b2", BLACK}, {"c3", BLACK}, {"d4", BLACK}, {"e5", BLACK},

    // top-left -> bottom-right
    {"a19", BLACK}, {"b18", BLACK}, {"c17", BLACK}, {"d16", BLACK}, {"e15", BLACK}

  };

  Board b = get_board_with_pieces(positions);

  double score = evaluate_state(b, BLACK, WHITE, true);
  std::cout << score << std::endl;
  assert(score >= 400000);

  score = evaluate_state(b, WHITE, BLACK, true);
  std::cout << score << std::endl;
  assert(score < -400000);

  std::cout << __func__ << " - OK" << std::endl;
}



int main() {

  test_coordinates();

  /* FREE THREE TESTS */
  test_horizontal_double_three();
  test_vertical_double_three();
  test_diagonal_top_left__bottom_right_double_three();
  test_diagonal_bottom_left__top_right_double_three();
  test_forbidden_from_subject();
  test_forbidden_diagonal_horizontal();


  /* CAPTURES TESTS */
  test_possible_horizontal_capture_left();
  test_possible_horizontal_capture_right();
  test_possible_vertical_capture_top();
  test_possible_vertical_capture_bottom();
  test_possible_diagonal_capture_top_left();
  test_possible_diagonal_capture_bottom_right();
  test_possible_diagonal_capture_bottom_left();
  test_possible_diagonal_capture_top_right();

  test_evaluation();

  return EXIT_SUCCESS;
}
