

#include "Gomoku.hpp"

int compare_moves_desc(const void* elem1, const void* elem2) {

  t_move_eval* m1 = (t_move_eval*)elem1;
  t_move_eval* m2 = (t_move_eval*)elem2;

	if (m1->score < m2->score)
    return 1;
  else if (m1->score > m2->score)
    return -1;
  return 0;
}

int compare_moves_asc(const void* elem1, const void* elem2) {

  t_move_eval* m1 = (t_move_eval*)elem1;
  t_move_eval* m2 = (t_move_eval*)elem2;

	if (m1->score < m2->score)
    return -1;
  else if (m1->score > m2->score)
    return 1;
  return 0;
}
