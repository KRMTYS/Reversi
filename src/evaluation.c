#include "evaluation.h"

#include <limits.h>

// 評価関数
static int evaluate(Board *board, Disk disk) {
    int diff_disk   = count_disks(board, disk) - count_disks(board, OPPONENT(disk));
    int valid_moves = count_valid_moves(board, disk);

    // 評価値: 石数の差 + 2 * 有効手数
    return diff_disk + 2 * valid_moves;
}

// NegaAlpha法による探索
static int negaalpha(Board *board,
                     Disk self, Disk current_turn,
                     Pos *next_move,
                     int alpha, int beta, int depth) {
    // 再帰探索の末尾
    if (depth == 0) {
        if (self == current_turn) {
            // 盤面の評価値を返す
            return evaluate(board, current_turn);
        } else {
            // 相手の手番では負の評価値を返す
            return -evaluate(board, current_turn);
        }
    }

    Pos move;
    bool had_valid_move = false;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (is_valid(board, current_turn, i)) {
            put_and_flip(board, current_turn, i);

            had_valid_move = true;

            // 再帰探索
            int score = -negaalpha(board,
                                   self, OPPONENT(current_turn),
                                   next_move,
                                   -beta, -alpha, (depth - 1));

            undo(board);

            // alphaカット: 下限値での枝狩り
            if (score > alpha) {
                alpha = score;
                move = i;

                // betaカット: 上限値での枝狩り
                if (alpha >= beta) {
                    return beta;
                }
            }
        }
    }

    // 有効手がないとき
    if (!had_valid_move) {
        if (!has_valid_move(board, OPPONENT(current_turn))) {
            // ゲーム終了: 評価値を返す
            alpha = evaluate(board, current_turn);
        } else {
            // パス: 手番を変更して探索を続ける
            alpha = -negaalpha(board,
                               self, OPPONENT(current_turn),
                               next_move,
                               -beta, -alpha, (depth - 1));
        }
    }

    *next_move = move;

    return alpha;
}

Pos search_move(Board *board, Disk self_disk, int depth) {
    Pos next_move;

    // NegaAlpha法による探索
    negaalpha(board, self_disk, self_disk, &next_move, -INT_MAX, INT_MAX, depth);

    return next_move;
}
