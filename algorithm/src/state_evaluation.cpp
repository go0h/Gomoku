#define LiveOne 10
#define DeadOne 1;
#define LiveTwo 100;
#define DeadTwo 50;
#define LiveThree 1000;
#define DeadThree 500;
#define LiveFour 10000;
#define DeadFour 5000;
#define Five 100000;

#include <iostream>
#include "Gomoku.hpp"

int evaluateblock(int blocks, int pieces)
{
    if (blocks == 0)
    {
        switch (pieces)
        {
        case 1:
            return LiveOne;
        case 2:
            return LiveTwo;
        case 3:
            return LiveThree;
        case 4:
            return LiveFour;
        default:
            return Five;
        }
    }
    else if (blocks == 1)
    {
        switch (pieces)
        {
        case 1:
            return DeadOne;
        case 2:
            return DeadTwo;
        case 3:
            return DeadThree;
        case 4:
            return DeadFour;
        default:
            return Five;
        }
    }
    else
    {
        if (pieces >= 5)
        {
            return Five;
        }
        else
        {
            return 0;
        }
    }
}


int eval_field(
    t_point *field, const size_t &side, const t_color &player, const size_t restrictions[4])
{
    int score = 0;
    size_t min_y = restrictions[0];
    size_t min_x = restrictions[1];
    size_t max_y = restrictions[2];
    size_t max_x = restrictions[3];
    for (size_t y = min_y; y <= max_y; ++y)
    {
        for (size_t x = min_x; x <= max_x; ++x)
        {
            if (field[y * side + x] == player)
            {
                int block = 0;
                int piece = 1;
                // left
                if (x == 0 || field[y * side + x - 1] != EMPTY)
                {
                    ++block;
                }
                // pieceNum
                for (++x; x != side && field[y * side + x] == player; ++x)
                {
                    ++piece;
                }
                // right
                if (x == side || field[y * side + x] != EMPTY)
                {
                    block++;
                }
                score = score + evaluateblock(block, piece);
            }
        }
    }
    // std::cout << ", after gorisontal  " << score;

    for (size_t x = min_x; x <= max_x; ++x)
    {
        for (size_t y = min_y; y <= max_y; ++y)
        {
            if (field[y * side + x] == player)
            {
                int block = 0;
                int piece = 1;
                // left
                if (y == 0 || field[(y - 1) * side + x] != EMPTY)
                {
                    ++block;
                }
                // pieceNum
                for (++y; y != side && field[y * side + x] == player; ++y)
                {
                    ++piece;
                }
                // right
                if (y == side || field[y * side + x] != EMPTY)
                {
                    ++block;
                }
                score += evaluateblock(block, piece);
            }
        }
    }
    // std::cout << ", after vertical  " << score;

   for (size_t diag_y = min_y; diag_y <= max_y +  (max_x - min_x); ++diag_y)
    {
        int r = static_cast<int>(diag_y);
        size_t c = min_x;
        while (r >= static_cast<int>(min_y) && c <= max_x)
        {
            if (r <= static_cast<int>(max_y))
            {
                if (field[r * side + c] == player)
                {
                    int block = 0;
                    int piece = 1;
                    // left
                    if (c == 0 || r == static_cast<int>(side) - 1 || field[(r + 1) * side + c - 1] != EMPTY)
                    {
                        ++block;
                    }
                    // pieceNum
                    for (--r, ++c; r >= 0 && field[r * side + c] == player; --r, ++c)
                    {
                        ++piece;
                    }
                    // right
                    if (r < 0 || c == side || field[r * side + c] != EMPTY)
                    {
                        block++;
                    }
                    score += evaluateblock(block, piece);
                }
            }
            --r;
            ++c;
        }
    }
    // std::cout << ", after 1 diagonal  " << score;

    for (int diag_y = static_cast<int> (min_y)  - static_cast<int> (max_x) ; diag_y <= static_cast<int>(max_y); ++diag_y)
    {
        int r = diag_y;
        size_t c = min_x;
        while (r <= static_cast<int>(max_y) && c <= max_x)
        {
            if (r >= static_cast<int>(min_y) && r <= static_cast<int>(max_y))
            {
                if (field[r * side + c] == player)
                {
                    int block = 0;
                    int piece = 1;
                    // left
                    if (c == 0 || r == 0 || field[(r - 1) * side + c - 1] != EMPTY)
                    {
                        ++block;
                    }
                    // pieceNum
                    for (++r, ++c; r < static_cast<int>(side) && c < side && field[r * side + c] == player; ++r, ++c)
                    {
                        ++piece;
                    }
                    // right
                    if (r == static_cast<int>(side) || c == side || field[r * side + c] != EMPTY)
                    {
                        ++block;
                    }
                    score += evaluateblock(block, piece);
                }
            }
            ++r;
            ++c;
        }
    }
    // std::cout << ", after 2 diagonal  " << score << std::endl;
    return score;
}

int evaluate_state(Board &state, const t_color &player, const size_t restrictions[4])
{
    size_t side = state.getSide();
    t_point *field = state.getField();
    t_color opponent = (player == WHITE) ? BLACK : WHITE;

    // std::cout << "player ";
    int player_score = eval_field(field, side, player, restrictions);
    // std::cout << "opponent ";
    int opponent_score = eval_field(field, side, opponent, restrictions);
    int score = player_score - opponent_score;
    // std::cout << "player=" << player << ", player score=" << player_score << ", opponent_score=" << opponent_score << ", rest[0]" << restrictions[0] << ", rest[1]" << restrictions[1] << ", rest[2]" << restrictions[2] << ", rest[3]" << restrictions[3] << std::endl;
    return score;
}
