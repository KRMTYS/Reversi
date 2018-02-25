#include <limits.h>

#include "evaluation.h"

int evaluate(Disk disk, Board* board)
{
    // 石数の差
    int diff_disk = count_disks(disk, board) - count_disks(OPPONENT(disk), board);

    return diff_disk;
}

int negaalpha(Pos* next_move,
            Disk self_disk,
            Disk current_turn,
            int alpha,
            int beta,
            int depth,
            Board* board)
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

    for (int i = 0; i < SQUARE_LENGTH; i++)
    {
        if (is_valid(TO_X(i), TO_Y(i), current_turn, board))
        {
            put_and_flip(i, current_turn, board);

            had_valid_move = true;

            int score = -negaalpha(next_move,
                                   self_disk,
                                   OPPONENT(current_turn),
                                   -beta,
                                   -alpha,
                                   depth - 1,
                                   board);

            undo(board);

            if (score > alpha)
            {
                alpha = score;
                move = i;

                // betaカット
                if (alpha >= beta)
                {
                    return beta;
                }
            }
        }
    }

    // 先読み中有効手がないとき
    if (!had_valid_move)
    {
        // ゲーム終了のとき評価値を返す
        if (!has_valid_move(OPPONENT(current_turn), board))
        {
            alpha = evaluate(current_turn, board);
        }
        else
        {
            // パスのとき手番を次に回す
            alpha = -negaalpha(next_move,
                               self_disk,
                               OPPONENT(current_turn),
                               -beta,
                               -alpha,
                               depth - 1,
                               board);
        }
    }

    *next_move = move;

    return alpha;
}