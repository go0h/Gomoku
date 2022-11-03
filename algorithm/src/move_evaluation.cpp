//#include <array>
#include <iostream>
#include "Gomoku.hpp"
#define DIRECTION_SIZE 9
#define DIRECTION_STOP 3
#define WIN_DETECTED -1


void setRestrictions(const t_point *field, const size_t &side, size_t restrictions[4])
{
    int min_y = static_cast<int>(side) + 1;
    int min_x = static_cast<int>(side) + 1;
    int max_y = -1;
    int max_x = -1;
    for (int y = 0; y < static_cast<int>(side); ++y)
    {
        for (int x = 0; x < static_cast<int>(side); ++x)
        {
            if (field[y * side + x] != 0)
            {
                min_x = std::min(min_x, x);
                max_x = std::max(max_x, x);
                min_y = std::min(min_y, y);
                max_y = std::max(max_y, y);
            }
        }
    }
    restrictions[0] = static_cast<size_t>(min_y);
    restrictions[1] = static_cast<size_t>(min_x);
    restrictions[2] = static_cast<size_t>(max_y);
    restrictions[3] = static_cast<size_t>(max_x);
    return;
}

void setDirections(const t_point *field, const size_t &side, const size_t &y, const size_t &x, t_point directions[4][9])
{
    int directionsIndex[4];
    for (int i = 0; i != 4; ++i)
    {
        directionsIndex[i] = 0;
    }

    for (int i = -4; i < 5; i++)
    {
        if (static_cast<int>(y) + i >= 0 && static_cast<int>(y) + i < static_cast<int>(side))
        {
            directions[0][directionsIndex[0]] = field[(y + i) * side + x];
            ++directionsIndex[0];
            if (static_cast<int>(y) + i >= 0 && static_cast<int>(x) + i < static_cast<int>(side))
            {
                directions[1][directionsIndex[1]] = field[(y + i) * side + x + i];
                ++directionsIndex[1];
            }
        }
        if (static_cast<int>(x) + i >= 0 && static_cast<int>(x) + i < static_cast<int>(side))
        {
            directions[2][directionsIndex[2]] = field[y * side + x + i];
            ++directionsIndex[2];
            if (static_cast<int>(y) - i >= 0 && static_cast<int>(y) - i < static_cast<int>(side))
            {
                directions[3][directionsIndex[3]] = field[(y - i) * side + x + i];
                ++directionsIndex[3];
            }
        }
    }
    for (int i = 0; i != 4; ++i)
    {
        if (directionsIndex[i] != DIRECTION_SIZE)
        {
            directions[i][directionsIndex[i]] = DIRECTION_STOP;
        }
    }
    return;
}

int get_sequence_score(const int &player_count, const int &opponent_count)
{
    if (player_count + opponent_count == 0)
    {
        return 0;
    }
    if (player_count != 0 && opponent_count == 0)
    {
        return player_count;
    }
    if (player_count == 0 && opponent_count != 0)
    {
        return -opponent_count;
    }
    if (player_count != 0 && opponent_count != 0)
    {
        return 10;
    }
    return 0;
}

int eval_sequence(int seq)
{
    switch (seq)
    {
    case 20:
        return 45000;
    case 100:
        return 800000;
    case 0:
        return 7;
    case 1:
        return 35;
    case 2:
        return 800;
    case 3:
        return 15000;
    case 4:
        return 800000;
    case -1:
        return 15;
    case -2:
        return 400;
    case -3:
        return 30000;
    case -4:
        return 100000;
    case 10:
        return 0;
    }
    return 0;
}

int evaluate_direction(const t_point direction[9], const t_color player)
{
    int score = 0;

    t_color opponent = (player == WHITE) ? BLACK : WHITE;

    for (int i = 0; (i + 4) < DIRECTION_SIZE; ++i)
    {
        int player_count = 0;
        int opponent_count = 0;
        if (direction[i] == DIRECTION_STOP)
        {
            return score;
        }
        for (int j = 0; j < 5; ++j)
        {

            if (direction[i + j] == DIRECTION_STOP)
            {
                return score;
            }

            if (direction[i + j] == player)
            {
                ++player_count;
            }
            else if (direction[i + j] == opponent)
            {
                ++opponent_count;
            }
        }
        score += eval_sequence(get_sequence_score(player_count, opponent_count));
        if ((score >= 800000))
        {
            return WIN_DETECTED;
        }
    }
    return score;
}

int evaluate_captures(t_point *field, const size_t &side, const t_color &player, size_t &y, size_t &x, int capture)
{
    int score = 0;
    int player_count = 0;
    t_color opponent = player == WHITE ? BLACK : WHITE;

    for (size_t i = 0; i != 8; ++i)
    {

        size_t x_dir = DIRECTIONS[i][0];
        size_t y_dir = DIRECTIONS[i][1];

        size_t x3 = x + x_dir * 3;
        size_t y3 = y + y_dir * 3;

        if (x3 < side && y3 < side && field[y3 * side + x3] == player)
        {
            size_t p2 = (y + y_dir * 2) * side + (x + x_dir * 2);
            size_t p1 = (y + y_dir * 1) * side + (x + x_dir * 1);

            if (field[p1] == opponent && field[p2] == opponent)
            {
                capture += 2;
                if (capture > 8)
                {
                    player_count = 100; // catch final double
                }
                else
                {
                    player_count = 20; // catch double
                }
                score += eval_sequence(get_sequence_score(player_count, 0));
                if ((score >= 800000))
                {
                    return WIN_DETECTED;
                }
            }
        }
    }
    return score;
}

std::string coord_to_pos_debug_move(size_t x, size_t y, size_t side)
{
  return std::string(1, char(97 + x)) + std::to_string(side - y);
}

void    printField(t_point *field, const size_t &_side)
{
  for (size_t y = 0; y != _side; ++y) {
    for (size_t x = 0; x != _side; ++x) {
      char p = '.';
      if (field[y * _side + x] == WHITE)
        p = 'W';
      else if (field[y * _side + x] == BLACK)
        p = 'B';
      printf("%-2c", p);
    }
    printf("\n");
  }
}


double evalute_move(t_point *field, const size_t &side, const t_color &player, size_t &y, size_t &x, const int capture[3])
{
    int score = 0;
    int scoreTmp = 0;
    t_point directions[4][9];
    setDirections(field, side, y, x, directions);

    for (int i = 0; i < 4; ++i)
    {
        scoreTmp = evaluate_direction(directions[i], player);
        if (scoreTmp == WIN_DETECTED)
        {
            // std::cout << "evalute_move + WIN_DETECTED, player=" << player << ", coord= " << coord_to_pos_debug_move( x,  y, side) << std::endl;
            // printField(field, side);
            return WIN_DETECTED;
        }
        score += scoreTmp;
    }

    scoreTmp = evaluate_captures(field, side, player, y, x, capture[player]);
    if (scoreTmp == WIN_DETECTED)
    {
            // std::cout << "evalute_move + WIN_DETECTED CAPTURE, player=" << player << ", coord= " << coord_to_pos_debug_move( x,  y, side) << std::endl;
            // printField(field, side);

        return WIN_DETECTED;
    }
    score += scoreTmp;
    return score;
}
