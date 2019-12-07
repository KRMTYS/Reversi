///
/// @file   com.c
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#include "com.h"

#include <stdlib.h>
#include <limits.h>

///
/// @struct MoveList
/// @brief  候補手リスト
///
typedef struct MoveList_ {
    Pos pos;                ///< 座標
    struct MoveList_ *prev; ///< 前要素へのポインタ
    struct MoveList_ *next; ///< 次要素へのポインタ
} MoveList;

///
/// @struct MoveInfo
/// @brief  着手情報
///
typedef struct MoveInfo_ {
    MoveList *move; ///< 着手位置
    int value;      ///< 評価値
} MoveInfo;

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
    MoveList    moves[BOARD_SIZE * BOARD_SIZE]; ///< 候補手リスト
};

static void make_move_list(Com *com) {
    Pos list[] = {
        A1, A8, H8, H1,
        D3, D6, E3, E6, C4, C5, F4, F5,
        C3, C6, F3, F6,
        D2, D7, E2, E7, B4, B5, G4, G5,
        C2, C7, F2, F7, B3, B6, G3, G6,
        D1, D8, E1, E8, A4, A5, H4, H5,
        C1, C8, F1, F8, A3, A6, H3, H6,
        B2, B7, G2, G7,
        B1, B8, G1, G8, A2, A7, H2, H7,
        D4, D5, E4, E5,
        NONE
    };

    MoveList *prev = com->moves;
    prev->pos  = NONE;
    prev->prev = NULL;
    prev->next = NULL;

    for (int i = 0; (list[i] != NONE); i++) {
        if (Board_disk(com->board, list[i]) == EMPTY) {
            prev[1].pos = list[i];
            prev[1].prev = prev;
            prev[1].next = NULL;
            prev->next = &prev[1];
            prev++;
        }
    }

}

static void remove_list(MoveList *movelist) {
    if (movelist->prev) {
        movelist->prev->next = movelist->next;
    }
    if (movelist->next) {
        movelist->next->prev = movelist->prev;
    }
}

static void recover_list(MoveList *movelist) {
    if (movelist->prev) {
        movelist->prev->next = movelist;
    }
    if (movelist->next) {
        movelist->next->prev = movelist;
    }
}

Com *Com_create(Evaluator *evaluator) {
    Com *com = malloc(sizeof(Com));
    if (com == NULL) {
        return NULL;
    }

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

    Com_init(com);

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

static int sort_moves(Com *com, Disk color, MoveInfo *moveinfo) {
    int info_num = 0;
    MoveList *p;
    MoveInfo tmp_info, *best_info;

    for (p = com->moves->next; p; (p = p->next)) {
        if (Board_flip(com->board, color, p->pos) > 0) {
            moveinfo[info_num].move = p;
            moveinfo[info_num].value = Evaluator_evaluate(com->evaluator, com->board);
            info_num++;
            Board_unflip(com->board);
        }
    }
    if (color == WHITE) {
        for (int i = 0; i < info_num; i++) {
            moveinfo[i].value = -moveinfo[i].value;
        }
    }
    for (int i = 0; i < info_num; i++) {
        best_info = &moveinfo[i];

        for (int j = (i + 1); j < info_num; j++) {
            if (moveinfo[j].value > best_info->value) {
                best_info = &moveinfo[j];
            }
        }

        tmp_info    = *best_info;
        *best_info  = moveinfo[i];
        moveinfo[i] = tmp_info;
    }

    return info_num;
}

///
/// @fn     Com_mid_search
/// @brief  NegaAlpha法による中盤探索
/// @param[in]  com         COM
/// @param[in]  turn        現在の手番色
/// @param[in]  opponent    相手の手番色
/// @param[out] next_move   次手の座標
/// @param[in]  alpha       alphaカット閾値
/// @param[in]  beta        betaカット閾値
/// @param[in]  depth       残りの探索深さ
/// @return 盤面の評価値
///
static int Com_mid_search(Com *com, Disk turn, Disk opponent, Pos *next_move, int alpha, int beta, int depth) {
    // 再帰探索の末端
    if (depth == 0) {
        com->node++;
        // 評価値を返す
        return Evaluator_evaluate(com->evaluator, com->board);
    }

    Pos move;
    MoveList *p;
    int value;
    int max = alpha;
    bool has_move = false;

    MoveInfo info[BOARD_SIZE * BOARD_SIZE / 2];
    int info_num;

    *next_move = NONE;

    // 残り手数が2より多いとき候補手を並び替える
    if (depth > 2) {
        info_num = sort_moves(com, turn, info);

        if (info_num > 0) {
            *next_move = info[0].move->pos;
            has_move = true;
        }
        for (int i = 0; i < info_num; i++) {
            Board_flip(com->board, turn, info[i].move->pos);
            remove_list(info[i].move);
            value = -Com_mid_search(com, opponent, turn, &move, -beta, -max, (depth - 1));
            Board_unflip(com->board);
            recover_list(info[i].move);

            if (value > max) {
                max = value;
                *next_move = info[i].move->pos;
                if (max >= beta) {
                    return beta;
                }
            }
        }
    } else {
        // 候補手リストを探索
        for (p = com->moves->next; p; (p = p->next)) {
            if (Board_flip(com->board, turn, p->pos) > 0) {
                remove_list(p);
                if (!has_move) {
                    *next_move = p->pos;
                    has_move = true;
                }

                value = -Com_mid_search(com, opponent, turn, &move, -beta, -max, (depth - 1));

                Board_unflip(com->board);
                recover_list(p);

                // alphaカット: 下限値での枝刈り
                if (value > max) {
                    max = value;
                    *next_move = p->pos;
                    // betaカット: 上限値での枝刈り
                    if (max >= beta) {
                        return beta;
                    }
                }
            }
        }
    }

    if (!has_move) {
        if (!Board_can_play(com->board, opponent)) {
            // 互いに有効手ないときゲーム終了、石数差の評価値を返す
            *next_move = NONE;
            com->node++;
            max = DISK_VALUE * (Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent));
        } else {
            // 相手に有効手あるときパス、手番を変更して探索を続ける
            *next_move = NONE;
            max = -Com_mid_search(com, opponent, turn, &move, -beta, -max, (depth - 1));
        }
    }

    return max;
}

///
/// @fn     Com_end_search
/// @brief  NegaAlpha法による終盤探索
/// @param[in]  com         COM
/// @param[in]  turn        現在の手番色
/// @param[in]  opponent    相手の手番色
/// @param[out] next_move   次手の座標
/// @param[in]  alpha       alphaカット閾値
/// @param[in]  beta        betaカット閾値
/// @param[in]  depth       残りの探索深さ
/// @return 盤面の評価値
///
static int Com_end_search(Com *com, Disk turn, Disk opponent, Pos* next_move, int alpha, int beta, int depth) {
    if (depth == 0) {
        com->node++;
        return Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent);
    }

    Pos move;
    MoveList *p;
    int value;
    int max = alpha;
    bool has_move = false;

    // 残り1マスのとき返せる石数のみ調べる
    if (depth == 1) {
        com->node++;
        p = com->moves->next;
        value = Board_count_flips(com->board, turn, p->pos);
        max = Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent);
        if (value > 0) {
            *next_move = p->pos;
            return (max + value + value + 1);
        }
        value = Board_count_flips(com->board, opponent, com->moves->next->pos);
        if (value > 0) {
            *next_move = NONE;
            return (max - value - value - 1);
        }

        *next_move = NONE;

        return max;
    }

    *next_move = NONE;

    // 候補手リストを探索
    for (p = com->moves->next; p; (p = p->next)) {
        if (Board_flip(com->board, turn, p->pos) > 0) {
            remove_list(p);
            if (!has_move) {
                *next_move = p->pos;
                has_move = true;
            }

            int value = -Com_end_search(com, opponent, turn, &move, -beta, -max, (depth - 1));

            Board_unflip(com->board);
            recover_list(p);

            // alphaカット
            if (value > max) {
                max = value;
                *next_move = p->pos;
                // betaカット
                if (max >= beta) {
                    return beta;
                }
            }
        }
    }

    if (!has_move) {
        if (!Board_can_play(com->board, opponent)) {
            // 互いに有効手ないときゲーム終了、石数差の評価値を返す
            *next_move = NONE;
            com->node++;
            alpha = (Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent));
        } else {
            // 相手に有効手あるときパス、手番を変更して探索を続ける
            *next_move = NONE;
            alpha = -Com_end_search(com, opponent, turn, &move, -beta, -max, (depth - 1));
        }
    }

    return max;
}

Pos Com_get_nextmove(Com *com, Board *board, Disk turn, int *value) {
    Pos next_move;
    int color;
    int val;

    Board_copy(board, com->board);
    com->node = 0;

    int left = Board_count_disks(com->board, EMPTY);

    make_move_list(com);

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

int Com_count_nodes(const Com *com) {
    return com->node;
}
