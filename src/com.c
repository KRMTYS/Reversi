///
/// @file   com.c
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#include "com.h"

#include <limits.h>

#include "eval.h"

///
/// @def    SEARCH_DEPTH
/// @brief  探索する手番数
///
#define SEARCH_DEPTH 6

///
/// @fn     negaalpha
/// @brief  NegaAlpha法による再帰探索
/// @param[in]  board           盤面
/// @param[in]  self            COMの手番色
/// @param[in]  current_turn    現在の手番色
/// @param[out] next_move       次手の座標
/// @param[in]  alpha           alphaカット閾値
/// @param[in]  beta            betaカット閾値
/// @param[in]  depth           残りの探索深さ
/// @return 盤面の評価値
///
static int negaalpha(Board *board,
                     Disk self, Disk current_turn,
                     Pos *next_move,
                     int alpha, int beta, int depth) {
    // 再帰探索の末端
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

            // alphaカット: 下限値での枝刈り
            if (score > alpha) {
                alpha = score;
                move = i;

                // betaカット: 上限値での枝刈り
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

Pos com_search_move(Board *board, Disk self_disk) {
    Pos next_move;

    // NegaAlpha法による探索
    negaalpha(board, self_disk, self_disk, &next_move, -INT_MAX, INT_MAX, SEARCH_DEPTH);

    return next_move;
}
