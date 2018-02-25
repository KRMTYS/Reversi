#include <limits.h>

#include "evaluation.h"

int evaluate(Disk disk, Board* board)
{
    // 石数の差
    int diff_disk = count_disks(disk, board) - count_disks(OPPONENT(disk), board);

    return diff_disk;
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

    bool had_valid_move = false;

    // 最大評価値
    int best_score = -INT_MAX;

    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        for (int x = 1; x <= SQUARE_SIZE; x++)
        {
            if (is_valid(x, y, current_turn, board))
            {
                put_and_flip(TO_POS(x, y), current_turn, board);
                had_valid_move = true;

                int score = -negamax(next_move, self_disk, OPPONENT(current_turn), depth - 1, board);

                if (score > best_score)
                {
                    best_score = score;
                    move = TO_POS(x, y);
                }

                undo(board);
            }
        }
    }

    // 先読み中有効手がないとき
    if (!had_valid_move)
    {
        // ゲーム終了のとき評価値を返す
        if (!has_valid_move(OPPONENT(current_turn), board))
        {
            return evaluate(current_turn, board);
        }
        else
        {
            // パスのとき手番を次に回す
            best_score = -negamax(next_move, self_disk, OPPONENT(current_turn), depth - 1, board);
        }
    }

    *next_move = move;

    return best_score;
}