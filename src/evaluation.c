#include <limits.h>

#include "evaluation.h"

int evaluate(Disk disk, Board* board)
{
    // 石差
    return count_disks(disk, board) - count_disks(REVERSE(disk), board);
}

int minmax(Pos* next_move, Disk self_disk, Disk current_turn, int depth, Board* board)
{
    if (depth == 0)
    {
        return evaluate(self_disk, board);
    }

    Pos move;

    // 子ノードの評価値
    int child_score = (self_disk == current_turn) ? INT_MIN : INT_MAX;

    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        for (int x = 1; x <= SQUARE_SIZE; x++)
        {
            if (is_valid(x, y, current_turn, board))
            {
                put_and_flip(TO_POS(x, y), current_turn, board);

                int score = minmax(next_move, self_disk, REVERSE(current_turn), depth-1, board);

                if (self_disk == current_turn)
                {
                    // 自己手番：最大評価値
                    if (score > child_score)
                    {
                        child_score = score;
                        move = TO_POS(x, y);
                    }
                }
                else
                {
                    // 相手手番：最小評価値
                    if (score < child_score)
                    {
                        child_score = score;
                        move = TO_POS(x, y);
                    }
                }

                undo(board);
            }
        }
    }

    // 先読み中
    if (!has_valid_move(current_turn, board))
    {
        // ゲーム終了したとき
        // 評価値を返す
        if (!has_valid_move(REVERSE(current_turn), board))
        {
            return evaluate(current_turn, board);
        }
        else
        {
            // パスのとき
            // 手番を次に回す
            child_score = minmax(next_move, self_disk, REVERSE(current_turn), depth-1, board);
        }
    }

    *next_move = move;

    return child_score;
}

int negamax(Pos* next_move, Disk self_disk, Disk current_turn, int depth, Board* board)
{
    if (depth == 0)
    {
        if (self_disk == current_turn)
        {
            return evaluate(current_turn, board);
        }
        // 相手の手番では負の評価値を返す
        else
        {
            return -evaluate(current_turn, board);
        }
    }

    Pos move;

    // 子ノードの評価値
    int child_score = INT_MIN;

    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        for (int x = 1; x <= SQUARE_SIZE; x++)
        {
            if (is_valid(x, y, current_turn, board))
            {
                put_and_flip(TO_POS(x, y), current_turn, board);

                int score = -negamax(next_move, self_disk, REVERSE(current_turn), depth - 1, board);

                if (score > child_score)
                {
                    child_score = score;
                    move = TO_POS(x, y);
                }

                undo(board);
            }
        }
    }

    // 先読み中
    if (!has_valid_move(current_turn, board))
    {
        // ゲーム終了したとき
        // 評価値を返す
        if (!has_valid_move(REVERSE(current_turn), board))
        {
            return evaluate(current_turn, board);
        }
        else
        {
            // パスのとき
            // 手番を次に回す
            child_score = -negamax(next_move, self_disk, REVERSE(current_turn), depth - 1, board);
        }
    }

    *next_move = move;

    return child_score;
}