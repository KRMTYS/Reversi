///
/// @file   com.c
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#include "com.h"

#include <stdlib.h>
#include <limits.h>

///
/// @struct Com_
/// @brief  COM思考ルーチン
///
struct Com_ {
    Board       *board;         ///< 盤面
    Evaluator   *evaluator;     ///< 評価器
    int         mid_depth;      ///< 中盤探索深さ
    int         wld_depth;      ///< 必勝読み深さ
    int         exact_depth;    ///< 完全読み深さ
    int         node;           ///< 探索したノード数
};

Com *Com_create(Evaluator *evaluator) {
    Com *com = malloc(sizeof(Com));
    if (com == NULL) return NULL;

    com->board = Board_create();
    if (com->board == NULL) {
        Com_delete(com);
        return NULL;
    }

    com->evaluator = evaluator;
    if (com->evaluator == NULL) {
        Com_delete(com);
        return NULL;
    }

    return com;
}

void Com_delete(Com *com) {
    if (com == NULL) {
        return;
    }

    Board_delete(com->board);
    Evaluator_delete(com->evaluator);

    free(com);
    com = NULL;
}

void Com_init(Com *com) {
    com->mid_depth   = 1;
    com->wld_depth   = 1;
    com->exact_depth = 1;
    com->node        = 0;
}

void Com_set_level(Com *com, int mid_depth, int th_exact, int th_wld) {
    com->mid_depth   = mid_depth;
    com->exact_depth = th_exact;
    com->wld_depth   = th_wld;
}

///
/// @fn     negaalpha
/// @brief  NegaAlpha法による再帰探索
/// @param[in]  com         COM
/// @param[in]  turn        現在の手番色
/// @param[in]  opponent    相手の手番色
/// @param[out] next_move   次手の座標
/// @param[in]  alpha       alphaカット閾値
/// @param[in]  beta        betaカット閾値
/// @param[in]  depth       残りの探索深さ
/// @return 盤面の評価値
///
static int negaalpha(Com *com, Disk turn, Disk opponent, Pos *next_move, int alpha, int beta, int depth) {
    // 再帰探索の末端
    if (depth == 0) {
        com->node++;
        // 評価値を返す
        return Evaluator_evaluate(com->evaluator, com->board);
    }

    Pos move;
    bool had_valid_move = false;

    *next_move = NONE;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_LENGTH; x++) {
            if (Board_check_valid(com->board, turn, XY2POS(x, y))) {
                Board_flip(com->board, turn, XY2POS(x, y));
                if (!had_valid_move) {
                    *next_move = XY2POS(x, y);
                    had_valid_move = true;
                }

                // 再帰探索
                int score = -negaalpha(com, opponent, turn, &move, -beta, -alpha, (depth - 1));

                Board_unflip(com->board);

                // alphaカット: 下限値での枝刈り
                if (score > alpha) {
                    alpha = score;
                    *next_move = XY2POS(x, y);
                    // betaカット: 上限値での枝刈り
                    if (alpha >= beta) {
                        return beta;
                    }
                }
            }
        }
    }

    if (!had_valid_move) {
        if (!Board_has_valid_move(com->board, opponent)) {
            // 互いに有効手ないときゲーム終了、石数差の評価値を返す
            *next_move = NONE;
            com->node++;
            alpha = DISK_VALUE * (Board_count_disk(com->board, turn) - Board_count_disk(com->board, opponent));
        } else {
            // 相手に有効手あるときパス、手番を変更して探索を続ける
            *next_move = NONE;
            alpha = -negaalpha(com, opponent, turn, &move, -beta, -alpha, (depth - 1));
        }
    }

    return alpha;
}

static int Com_end_search(Com *com, Disk turn, Disk opponent, Pos* move, int alpha, int beta, int depth) {
    return negaalpha(com, turn, opponent, move, alpha, beta, depth);
}

static int Com_mid_search(Com *com, Disk turn, Disk opponent, Pos* move, int alpha, int beta, int depth) {
    return negaalpha(com, turn, opponent, move, alpha, beta, depth);
}

Pos Com_get_nextmove(Com *com, Board *board, Disk turn, int *value) {
    Pos next_move;
    int color;
    int val;

    Board_copy(board, com->board);
    com->node = 0;

    int left = Board_count_disk(com->board, EMPTY);

    if (left <= com->exact_depth) {
        val = Com_end_search(com, turn, OPPONENT(turn), &next_move, -(BOARD_SIZE * BOARD_SIZE), (BOARD_SIZE * BOARD_SIZE), left);
        val *= DISK_VALUE;
    } else if (left <= com->wld_depth) {
        val = Com_end_search(com, turn, OPPONENT(turn), &next_move, -(BOARD_SIZE * BOARD_SIZE), 1, left);
        val *= DISK_VALUE;
    } else {
        if (((turn == WHITE) && (com->mid_depth % 2 == 0)) ||
            ((turn == BLACK) && (com->mid_depth % 2 == 1))) {
                Board_reverse(com->board);
                color = OPPONENT(turn);
            } else {
                color = turn;
            }

            // 評価値の上限/下限を十分大きな正負値とする
            val = Com_mid_search(com, color, OPPONENT(color), &next_move, -MAX_PATTERN_VALUE, MAX_PATTERN_VALUE, com->mid_depth);
    }

    if (value) {
        *value = val;
    }

    return next_move;
}

int Com_count_nodes(Com *com) {
    return com->node;
}
