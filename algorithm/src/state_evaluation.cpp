#define LiveOne 10
#define DeadOne 5
#define LiveTwo 100
#define DeadTwo 50
#define LiveThree 1000
#define DeadThree 500
#define LiveFour 10000
#define DeadFour 5000
#define Five 100000

#include "Gomoku.hpp"

int evaluateblock(int blocks, int pieces, int additionalMove)
{
    if (blocks == 0)
    {
        switch (pieces)
        {
        case 1:
             if (additionalMove)
                return -LiveTwo;
             else
                return LiveOne;
        case 2:
             if (additionalMove)
                return -LiveThree;
             else
                return LiveTwo;
        case 3:
             if (additionalMove)
                return -LiveFour;
             else
                return LiveThree;
        case 4:
            if (additionalMove)
                return -Five;
            else
                return LiveFour;
        default:
            if (additionalMove)
                return -Five;
            else
                return WIN_STATE_SCORE;
        }
    }
    else if (blocks == 1)
    {
        switch (pieces)
        {
        case 1:
            if (additionalMove)
                return DeadTwo;
            else
                return DeadOne;
        case 2:
            if (additionalMove)
                return DeadThree;
            else
                return DeadTwo;
        case 3:
            if (additionalMove)
                return DeadFour;
            else
                return DeadThree;
        case 4:
            if (additionalMove)
                return -Five;
            else
                return DeadFour;
        default:
            if (additionalMove)
                return -Five;
            else
                return WIN_STATE_SCORE;
        }
    }
    else
    {
        if (pieces == 4 && additionalMove)
        {
            return Five;
        }
        if (pieces >= 5 && additionalMove == 0)
        {
            return WIN_STATE_SCORE;
        }
        else
        {
            return 0;
        }
    }
}

int eval_field(t_point *field, const size_t &side, const t_color &player, const size_t restrictions[4], size_t additionalMove)
{
    int score = 0;
    size_t min_y = restrictions[0];
    size_t min_x = restrictions[1];
    size_t max_y = restrictions[2];
    size_t max_x = restrictions[3];
    for (size_t y = min_y; y <= max_y; ++y)
    {
        size_t cells = 0;
        size_t player_cells = 0;
        for (size_t x = min_x; x <= max_x; ++x)
        {
            if (field[y * side + x] == player)
            {
                ++cells;
                ++player_cells;
                int block = 0;
                int piece = 1;
                // left
                if (x == 0 || field[y * side + x - 1] != EMPTY)
                {
                    ++block;
                }
                // count point
                for (++x; x != side && field[y * side + x] == player; ++x)
                {
                    ++piece;
                    ++cells;
                    ++player_cells;
                }
                // right
                if (x == side || field[y * side + x] != EMPTY)
                {
                    block++;
                }
                score += evaluateblock(block, piece, additionalMove);
                if (additionalMove == 0 && piece < 5 && cells > 4 && cells - player_cells == 1)
                {
                    block = 1;
                    piece = 4;
                    score += evaluateblock(block, piece, additionalMove);
                }
                if (additionalMove && player_cells == 3 && cells == 4)
                {
                    score += evaluateblock(0, player_cells, additionalMove);
                }

                --x;
            }
            else if (field[y * side + x] == EMPTY && cells)
            {
                ++cells;
            }
            else
            {
                cells = 0;
                player_cells = 0;
            }
        }
    }

    for (size_t x = min_x; x <= max_x; ++x)
    {
        size_t cells = 0;
        size_t player_cells = 0;
        for (size_t y = min_y; y <= max_y; ++y)
        {
            if (field[y * side + x] == player)
            {
                ++cells;
                ++player_cells;
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
                    ++cells;
                    ++player_cells;
                }
                // right
                if (y == side || field[y * side + x] != EMPTY)
                {
                    ++block;
                }
                score += evaluateblock(block, piece, additionalMove);
                if (additionalMove == 0 && piece < 5 && cells > 4 && cells - player_cells == 1)
                {
                    block = 1;
                    piece = 4;
                    score += evaluateblock(block, piece, additionalMove);
                }
                if (additionalMove && player_cells == 3 && cells == 4)
                {
                    score += evaluateblock(0, player_cells, additionalMove);
                }

                --y;
            }
            else if (field[y * side + x] == EMPTY && cells)
            {
                ++cells;
            }
            else
            {
                cells = 0;
                player_cells = 0;
            }
        }
    }

    for (size_t diag_y = min_y; diag_y <= max_y + (max_x - min_x); ++diag_y)
    {
        size_t cells = 0;
        size_t player_cells = 0;
        int r = static_cast<int>(diag_y);
        size_t c = min_x;
        while (r >= static_cast<int>(min_y) && c <= max_x)
        {
            if (r <= static_cast<int>(max_y))
            {
                if (field[r * side + c] == player)
                {
                    ++cells;
                    ++player_cells;
                    int block = 0;
                    int piece = 1;
                    if (c == 0 || r == static_cast<int>(side) - 1 || field[(r + 1) * side + c - 1] != EMPTY)
                    {
                        ++block;
                    }
                    for (--r, ++c; r >= 0 && field[r * side + c] == player; --r, ++c)
                    {
                        ++piece;
                        ++cells;
                        ++player_cells;
                    }
                    if (r < 0 || c == side || field[r * side + c] != EMPTY)
                    {
                        block++;
                    }
                    score += evaluateblock(block, piece, additionalMove);
                if (additionalMove == 0 && piece < 5 && cells > 4 && cells - player_cells == 1)
                {
                    block = 1;
                    piece = 4;
                    score += evaluateblock(block, piece, additionalMove);
                }
                if (additionalMove && player_cells == 3 && cells == 4)
                {
                    score += evaluateblock(0, player_cells, additionalMove);
                }

                    ++r;
                    --c;
                }
                else if (field[r * side + c] == EMPTY && cells)
                {
                    ++cells;
                }
                else
                {
                    cells = 0;
                    player_cells = 0;
                }
            }
            --r;
            ++c;
        }
    }

    for (int diag_y = static_cast<int>(min_y) - static_cast<int>(max_x); diag_y <= static_cast<int>(max_y); ++diag_y)
    {
        size_t cells = 0;
        size_t player_cells = 0;
        int r = diag_y;
        size_t c = min_x;
        while (r <= static_cast<int>(max_y) && c <= max_x)
        {
            if (r >= static_cast<int>(min_y) && r <= static_cast<int>(max_y))
            {
                if (field[r * side + c] == player)
                {
                    ++cells;
                    ++player_cells;
                    int block = 0;
                    int piece = 1;
                    if (c == 0 || r == 0 || field[(r - 1) * side + c - 1] != EMPTY)
                    {
                        ++block;
                    }
                    for (++r, ++c; r < static_cast<int>(side) && c < side && field[r * side + c] == player; ++r, ++c)
                    {
                        ++piece;
                        ++cells;
                        ++player_cells;                        
                    }
                    if (r == static_cast<int>(side) || c == side || field[r * side + c] != EMPTY)
                    {
                        ++block;
                    }
                    score += evaluateblock(block, piece, additionalMove);
                if (additionalMove == 0 && piece < 5 && cells > 4 && cells - player_cells == 1)
                {
                    block = 1;
                    piece = 4;
                    score += evaluateblock(block, piece, additionalMove);
                }
                if (additionalMove && player_cells == 3 && cells == 4)
                {
                    score += evaluateblock(0, player_cells, additionalMove);
                }

                    --r;
                    --c;                    
                }
                else if (field[r * side + c] == EMPTY && cells)
                {
                    ++cells;
                }
                else
                {
                    cells = 0;
                    player_cells = 0;
                }                
            }
            ++r;
            ++c;
        }
    }
    return score;
}

int evaluate_state(Board &state, const t_color &player, const size_t restrictions[4])
{
    size_t side = state.getSide();
    t_point *field = state.getField();
    t_color opponent = (player == WHITE) ? BLACK : WHITE;

    int player_score = eval_field(field, side, player, restrictions, 0);
    int opponent_score = eval_field(field, side, opponent, restrictions, 1);
    int score = player_score + opponent_score;
    return score;
}
