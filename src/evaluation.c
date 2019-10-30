#include "evaluation.h"

#include <limits.h>

// 評価関数
int evaluate(Board *board, Disk disk) {
    // 石数の差
    int diff_disk = count_disks(board, disk) - count_disks(board, OPPONENT(disk));

    // 有効手の数
    int valid_moves = count_valid_moves(board, disk);

    return diff_disk + 2 * valid_moves;
}

// NegaAlpha法による探索
int negaalpha(Board *board,
              Disk self_disk, Disk current_turn,
              Pos *next_move,
              int alpha, int beta,
              int depth) {
    if (depth == 0) {
        if (self_disk == current_turn) {
            return evaluate(board, current_turn);
        }
        // 相手の手番では負の評価値を返す
        else {
            return -evaluate(board, current_turn);
        }
    }

    Pos move;

    bool had_valid_move = false;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (is_valid(board, current_turn, i)) {
            put_and_flip(board, current_turn, i);

            had_valid_move = true;

            int score = -negaalpha(board,
                                   self_disk, OPPONENT(current_turn),
                                   next_move,
                                   -beta, -alpha,
                                   (depth - 1));

            undo(board);

            if (score > alpha) {
                alpha = score;
                move = i;

                // betaカット
                if (alpha >= beta) {
                    return beta;
                }
            }
        }
    }

    // 先読み中有効手がないとき
    if (!had_valid_move) {
        // ゲーム終了のとき評価値を返す
        if (!has_valid_move(board, OPPONENT(current_turn))) {
            alpha = evaluate(board, current_turn);
        } else {
            // パスのとき手番を次に回す
            alpha = -negaalpha(board,
                               self_disk, OPPONENT(current_turn),
                               next_move,
                               -beta, -alpha,
                               (depth - 1));
        }
    }

    *next_move = move;

    return alpha;
}

Pos search_move(Board *board, Disk self_disk, int depth) {
    Pos next_move;

    //negaalpha(&next_move, self_disk, self_disk, -INT_MAX, INT_MAX, depth, board);
    negaalpha(board, self_disk, self_disk, &next_move, -INT_MAX, INT_MAX, depth);

    return next_move;
}
