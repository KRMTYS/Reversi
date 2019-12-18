///
/// @file   com.c
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#include "com.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

///
/// @def    MAX_VALUE
/// @brief  パターンの最大評価値
///
#define MAX_VALUE (DISK_VALUE * 200)

///
/// @struct MoveList
/// @brief  候補手リスト
/// @note   双方向リスト
///
typedef struct MoveList_ {
    int pos;                ///< 座標
    struct MoveList_ *prev; ///< 前要素へのポインタ
    struct MoveList_ *next; ///< 次要素へのポインタ
} MoveList;

///
/// @struct MoveInfo
/// @brief  着手情報
///
typedef struct MoveInfo_ {
    MoveList *move; ///< 着手位置
    int      value; ///< 評価値
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

static bool initialize(Com *com, Evaluator *eval);

static int Com_mid_search(Com *com, int turn, int opponent, int *next_move, bool pass, int alpha, int beta, int depth);
static int Com_end_search(Com *com, int turn, int opponent, int* next_move, bool pass, int alpha, int beta, int depth);

static void make_move_list(Com *com);
static void make_move_list(Com *com);
static void remove_list(MoveList *movelist);
static void recover_list(MoveList *movelist);
static int sort_moves(Com *com, int color, MoveInfo *moveinfo);

///
/// @fn     initialize
/// @brief  COMのメンバを初期化する
/// @param[in,out]  com     COM
/// @param[in,out]  eval    評価器
/// @retval true    初期化成功
/// @retval false   初期化失敗
///
static bool initialize(Com *com, Evaluator *eval)
{
    memset(com, 0, sizeof(Com));

    com->board = Board_create();
    if (!com->board) {
        return false;
    }

    com->evaluator = eval;
    if (!com->evaluator) {
        return false;
    }

    com->mid_depth   = 1;
    com->wld_depth   = 1;
    com->exact_depth = 1;
    com->node        = 0;

    return true;
}

Com *Com_create(Evaluator *eval)
{
    Com *com = malloc(sizeof(Com));

    if (com) {
        if (!initialize(com, eval)) {
            Com_delete(com);
            com = NULL;
        }
    }

    return com;
}

void Com_delete(Com *com)
{
    if (com->board) {
        Board_delete(com->board);
    }

    free(com);
    com = NULL;
}

void Com_set_level(Com *com, int mid_depth, int th_exact, int th_wld)
{
    com->mid_depth   = mid_depth;
    com->exact_depth = th_exact;
    com->wld_depth   = th_wld;
}

int Com_get_nextmove(Com *com, Board *board, int color, int *value)
{
    Board_copy(board, com->board);
    com->node = 0;

    int left = Board_count_disks(com->board, EMPTY);

    make_move_list(com);

    Board_init_pattern(com->board);

    int next_move;
    int col;
    int val;

    if (left <= com->exact_depth) {
        // 完全読み
        val = Com_end_search(com, color, Board_opponent(color), &next_move, false, -(BOARD_SIZE * BOARD_SIZE), (BOARD_SIZE * BOARD_SIZE), left);
        val *= DISK_VALUE;
    } else if (left <= com->wld_depth) {
        // 必勝読み
        val = Com_end_search(com, color, Board_opponent(color), &next_move, false, -(BOARD_SIZE * BOARD_SIZE), 1, left);
        val *= DISK_VALUE;
    } else {
        // 中盤探索
        // 盤面を反転し黒手番で評価する
        if (((color == WHITE) && (com->mid_depth % 2 == 0)) ||
            ((color == BLACK) && (com->mid_depth % 2 == 1))) {
            Board_reverse(com->board);
            col = Board_opponent(color);
        } else {
            col = color;
        }

        // 探索範囲を -MAX_VALUE - MAX_VALUE とする
        val = Com_mid_search(com, col, Board_opponent(col), &next_move, false, -MAX_VALUE, MAX_VALUE, com->mid_depth);
    }

    if (value) {
        *value = val;
    }

    return next_move;
}

///
/// @fn     Com_mid_search
/// @brief  NegaAlpha法による中盤探索
/// @param[in]  com         COM
/// @param[in]  turn        現在の手番色
/// @param[in]  opponent    相手の手番色
/// @param[out] next_move   次手の座標
/// @param[in]  pass        パス判定
/// @param[in]  alpha       alpha値（探索下限）
/// @param[in]  beta        beta値（探索上限）
/// @param[in]  depth       探索深さ
/// @return 盤面の評価値
/// @note   1手ごとにパターンを更新する
///
static int Com_mid_search(Com *com, int turn, int opponent, int *next_move, bool pass, int alpha, int beta, int depth)
{
    // 探索末端（リーフ）: 盤面の評価値を返す
    if (depth == 0) {
        com->node++;
        return Evaluator_evaluate(com->evaluator, com->board);
    }

    int move;
    int value;
    int max = alpha;
    bool can_move = false;
    MoveList *p;
    // 並び替え用着手情報
    MoveInfo info[BOARD_SIZE * BOARD_SIZE / 2];
    int info_num;

    *next_move = NONE;

    if (depth > 2) {
        // 残り手数が2より多いとき候補手を並び替える
        info_num = sort_moves(com, turn, info);

        if (info_num > 0) {
            *next_move = info[0].move->pos;
            can_move = true;
        }
        // 着手できる候補手数ぶん探索する
        for (int i = 0; i < info_num; i++) {
            Board_flip_pattern(com->board, turn, info[i].move->pos);
            remove_list(info[i].move);

            value = -Com_mid_search(com, opponent, turn, &move, false, -beta, -max, (depth - 1));

            Board_unflip_pattern(com->board);
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
        // 候補手リストを探索する
        for (p = com->moves->next; p; (p = p->next)) {
            // 着手できるとき着手する
            if (Board_flip_pattern(com->board, turn, p->pos) > 0) {
                remove_list(p);
                if (!can_move) {
                    *next_move = p->pos;
                    can_move = true;
                }

                // 子ノードを探索
                value = -Com_mid_search(com, opponent, turn, &move, false, -beta, -max, (depth - 1));

                // 盤面・候補手リストを戻す
                Board_unflip_pattern(com->board);
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

    if (!can_move) {
        if (pass) {
            // 互いに有効手ないときゲーム終了、評価値として石数差を返す
            *next_move = NONE;
            com->node++;
            max = DISK_VALUE * (Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent));
        } else {
            // 相手に有効手あるときパス、手番を変更して探索続ける
            *next_move = NONE;
            max = -Com_mid_search(com, opponent, turn, &move, true, -beta, -max, (depth - 1));
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
/// @param[in]  pass        パス判定
/// @param[in]  alpha       alpha値（探索下限）
/// @param[in]  beta        beta値（探索上限）
/// @param[in]  depth       探索深さ
/// @return 盤面の評価値
///
static int Com_end_search(Com *com, int turn, int opponent, int* next_move, bool pass, int alpha, int beta, int depth)
{
    // リーフ: 盤面評価値を返す
    if (depth == 0) {
        com->node++;
        return Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent);
    }

    int move;
    MoveList *p;
    int value;
    int max = alpha;
    bool can_move = false;
    MoveInfo info[BOARD_SIZE * BOARD_SIZE / 2];
    int info_num;

    // 残り1マスのとき、返せる石数のみ調べ石数差を計算する
    if (depth == 1) {
        com->node++;
        p = com->moves->next;

        // 着手前の評価値
        max  = Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent);

        // 空きマスに自手着手
        value = Board_count_flips(com->board, turn, p->pos);
        if (value > 0) {
            *next_move = p->pos;
            return (max + value + value + 1);
        }

        // 空きマスに相手着手
        value = Board_count_flips(com->board, opponent, com->moves->next->pos);
        if (value > 0) {
            *next_move = NONE;
            return (max - value - value - 1);
        }

        // 空きマスに着手できない
        *next_move = NONE;
        return max;
    }

    *next_move = NONE;

    // 残り8手を超える際候補手を並び替える
    if (depth > 8) {
        info_num = sort_moves(com, turn, info);

        if (info_num > 0) {
            *next_move = info[0].move->pos;
            can_move = true;
        }
        for (int i = 0; i < info_num; i++) {
            Board_flip_pattern(com->board, turn, info[i].move->pos);
            remove_list(info[i].move);

            value = -Com_mid_search(com, opponent, turn, &move, false, -beta, -max, (depth - 1));

            Board_unflip_pattern(com->board);
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
            // パターン更新は行わない
            if (Board_flip(com->board, turn, p->pos) > 0) {
                remove_list(p);

                if (!can_move) {
                    *next_move = p->pos;
                    can_move = true;
                }

                int value = -Com_end_search(com, opponent, turn, &move, false, -beta, -max, (depth - 1));

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
    }

    if (!can_move) {
        if (pass) {
            // 互いに有効手ないときゲーム終了、石数差の評価値を返す
            *next_move = NONE;
            com->node++;
            alpha = (Board_count_disks(com->board, turn) - Board_count_disks(com->board, opponent));
        } else {
            // 相手に有効手あるときパス、手番を変更して探索を続ける
            *next_move = NONE;
            alpha = -Com_end_search(com, opponent, turn, &move, true, -beta, -max, (depth - 1));
        }
    }

    return max;
}

int Com_count_nodes(const Com *com)
{
    return com->node;
}

///
/// @fn     make_move_list
/// @brief  候補手リストを作成する
/// @param[in,out]  com     COM
///
static void make_move_list(Com *com)
{
    //　候補手リスト
    // 初期状態で重要度の高い順に並べる
    int list[] = {
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

///
/// @fn     remove_list
/// @brief  候補手リストから1手削除する
/// @param[in,out]  movelist    候補手リスト
///
static void remove_list(MoveList *movelist)
{
    if (movelist->prev) {
        movelist->prev->next = movelist->next;
    }
    if (movelist->next) {
        movelist->next->prev = movelist->prev;
    }
}

///
/// @fn     recover_list
/// @brief  候補手リストに1手戻す
/// @param[in,out]  movelist    候補手リスト
/// @note   戻す手はremove_list()で削除されているもの
///
static void recover_list(MoveList *movelist)
{
    if (movelist->prev) {
        movelist->prev->next = movelist;
    }
    if (movelist->next) {
        movelist->next->prev = movelist;
    }
}

///
/// @fn     sort_moves
/// @brief  候補手を並べ替える
/// @param[in]  com         COM
/// @param[in]  color       手番
/// @param[out] moveinfo    着手情報
/// @return 着手できる候補手数
///
static int sort_moves(Com *com, int color, MoveInfo *moveinfo)
{
    int info_num = 0;
    MoveList *p;
    MoveInfo tmp_info, *best_info;

    // 候補手から着手できる手を探索し評価値をつける
    for (p = com->moves->next; p; (p = p->next)) {
        if (Board_flip_pattern(com->board, color, p->pos) > 0) {
            moveinfo[info_num].move = p;
            moveinfo[info_num].value = Evaluator_evaluate(com->evaluator, com->board);
            info_num++;
            Board_unflip_pattern(com->board);
        }
    }
    // 黒手番で評価するため白手番のとき評価値反転する
    if (color == WHITE) {
        for (int i = 0; i < info_num; i++) {
            moveinfo[i].value = -moveinfo[i].value;
        }
    }
    // 評価値に基づいて候補手を並び替える
    // 選択ソート
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
