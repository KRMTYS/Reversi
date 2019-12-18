///
/// @file   board.c
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#include "board.h"

#include <stdlib.h>

///
/// @def    NUM_DISK
/// @brief  マスの総数（番兵含む）
///
#define NUM_DISK ((BOARD_SIZE + 1) * (BOARD_SIZE + 2) + 1)

///
/// @def    STACK_SIZE
/// @brief  スタック長
/// @note   (BOARD_SIZE - 2) * 3: 返せる最大の石数
///         3: 着手位置、相手の石色、返した石数
///         BOARD_SIZE * BOARD_SIZE - 4: 着手できる総マス数
/// 
#define STACK_SIZE (((BOARD_SIZE - 2) * 3 + 3) * BOARD_SIZE * BOARD_SIZE - 4)

///
/// @def    NUM_PATTERN_DIFF
/// @brief  1マスの変化で更新される最大パターン数
///
#define NUM_PATTERN_DIFF 6

///
/// @enum   Dir
/// @brief  盤面上での8方向インデックス
///
typedef enum {
    UPPER_LEFT  = -10,  ///< 左上
    UPPER       =  -9,  ///< 上
    UPPER_RIGHT =  -8,  ///< 右上
    LEFT        =  -1,  ///< 左
    RIGHT       =   1,  ///< 右
    LOWER_LEFT  =   8,  ///< 左下
    LOWER       =   9,  ///< 下
    LOWER_RIGHT =  10   ///< 右下
} Dir;

///
/// @struct Board_
/// @brief  リバーシ盤面
///
struct Board_ {
    int disks[NUM_DISK];                            ///< マス情報
    int stack[STACK_SIZE];                          ///< 着手情報スタック: （返した石の位置1 - N）、（着手位置）、（相手の石色）、（返した石数）
    int  *sp;                                       ///< スタックポインタ
    int  disk_num[3];                               ///< 石数（0: 黒 1: 白、2: 空）
    int  pattern[NUM_PATTERN_ID];                   ///< 盤面パターン状態
    int  pattern_id[NUM_DISK][NUM_PATTERN_DIFF];    ///< あるマスへの着手時に更新するパターンID
    int  pattern_diff[NUM_DISK][NUM_PATTERN_DIFF];  ///< あるマスへの着手時に更新するパターン状態の差分
};

///
/// @def    OPPONENT
/// @brief  逆色
///
#define OPPONENT(color) (BLACK + WHITE - color)

///
/// @def    STACK_PUSH
/// @brief  スタックへのプッシュ
///
#define STACK_PUSH(board, value) (*((board)->sp++) = (value))

///
/// @def    STACK_POP
/// @brief  スタックからのポップ
///
#define STACK_POP(board) (*(--(board)->sp))

static int flip_line(Board *board, int color, int pos, int dir);

static void add_pattern(Board *board, int id, const int *pos_list, int num);
static void init_pattern_diff(Board *board);

static void flip_square_black(Board *board, int pos);
static void flip_square_white(Board *board, int pos);
static void put_square_black(Board *board, int pos);
static void put_square_white(Board *board, int pos);

static int flip_line_pattern(Board *board, int color, int pos, int dir);

static int count_flips_line(const Board *board, int color, int pos, int dir);

Board *Board_create(void)
{
    Board *board = malloc(sizeof(Board));

    if (board) {
        init_pattern_diff(board);
        Board_init(board);
    }

    return board;
}

void Board_delete(Board *board)
{
    free(board);
    board = NULL;
}

void Board_init(Board *board)
{
    for (int i = 0; i < NUM_DISK; i++) {
        board->disks[i] = WALL;
    }

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            board->disks[Board_pos(x, y)] = EMPTY;
        }
    }

    // 石の初期配置
    board->disks[D4] = WHITE;
    board->disks[D5] = BLACK;
    board->disks[E4] = BLACK;
    board->disks[E5] = WHITE;

    board->sp = board->stack;

    // 石数の初期化
    board->disk_num[BLACK] = 2;
    board->disk_num[WHITE] = 2;
    board->disk_num[EMPTY] = (BOARD_SIZE * BOARD_SIZE) - 4;

    Board_init_pattern(board);
}

int Board_disk(const Board *board, int pos)
{
    return board->disks[pos];
}

int Board_count_disks(const Board *board, int color)
{
    return board->disk_num[color];
}

///
/// @fn     flip_line
/// @brief  一方向へ石を返す
/// @param[in,out]  board   盤面
/// @param[in]      color   手番
/// @param[in]      pos     着手座標
/// @param[in]      dir     探索方向
/// @return 返した石数
///
static int flip_line(Board *board, int color, int pos, int dir)
{
    int op = Board_opponent(color);
    int cur_pos;

    int count = 0;

    cur_pos = pos + dir;
    if (board->disks[cur_pos] != op) {
        return 0;
    }

    // 一方向へ逆色を探索
    // ループアンローリングで高速化する
    cur_pos += dir;
    if (board->disks[cur_pos] == op) {
        cur_pos += dir;
        if (board->disks[cur_pos] == op) {
            cur_pos += dir;
            if (board->disks[cur_pos] == op) {
                cur_pos += dir;
                if (board->disks[cur_pos] == op) {
                    cur_pos += dir;
                    if (board->disks[cur_pos] == op) {
                        cur_pos += dir;
                        if (board->disks[cur_pos] == op) {
                            return 0;
                        }
                        // 端が同色石のとき反転、スタックへ記録
                        cur_pos -= dir;
                        count++;
                        board->disks[cur_pos] = color;
                        STACK_PUSH(board, cur_pos);
                    } else if (board->disks[cur_pos] != color) {
                        return 0;
                    }
                    cur_pos -= dir;
                    count++;
                    board->disks[cur_pos] = color;
                    STACK_PUSH(board, cur_pos);
                } else if (board->disks[cur_pos] != color) {
                    return 0;
                }
                cur_pos -= dir;
                count++;
                board->disks[cur_pos] = color;
                STACK_PUSH(board, cur_pos);
            } else if (board->disks[cur_pos] != color) {
                return 0;
            }
            cur_pos -= dir;
            count++;
            board->disks[cur_pos] = color;
            STACK_PUSH(board, cur_pos);
        } else if (board->disks[cur_pos] != color) {
            return 0;
        }
        cur_pos -= dir;
        count++;
        board->disks[cur_pos] = color;
        STACK_PUSH(board, cur_pos);
    } else if (board->disks[cur_pos] != color) {
        return 0;
    }
    cur_pos -= dir;
    count++;
    board->disks[cur_pos] = color;
    STACK_PUSH(board, cur_pos);

    return count;
}

int Board_flip(Board *board, int color, int pos)
{
    if (board->disks[pos] != EMPTY) {
        return 0;
    }

    int count = 0;

    // マスごとに探索方向を限定し高速化する
    switch (pos) {
        case C1:
        case C2:
        case D1:
        case D2:
        case E1:
        case E2:
        case F1:
        case F2:
            count += flip_line(board, color, pos, LEFT);
            count += flip_line(board, color, pos, RIGHT);
            count += flip_line(board, color, pos, LOWER_LEFT);
            count += flip_line(board, color, pos, LOWER);
            count += flip_line(board, color, pos, LOWER_RIGHT);
            break;
        case C8:
        case C7:
        case D8:
        case D7:
        case E8:
        case E7:
        case F8:
        case F7:
            count += flip_line(board, color, pos, UPPER_LEFT);
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, UPPER_RIGHT);
            count += flip_line(board, color, pos, LEFT);
            count += flip_line(board, color, pos, RIGHT);
            break;
        case A3:
        case A4:
        case A5:
        case A6:
        case B3:
        case B4:
        case B5:
        case B6:
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, UPPER_RIGHT);
            count += flip_line(board, color, pos, RIGHT);
            count += flip_line(board, color, pos, LOWER);
            count += flip_line(board, color, pos, LOWER_RIGHT);
            break;
        case H3:
        case H4:
        case H5:
        case H6:
        case G3:
        case G4:
        case G5:
        case G6:
            count += flip_line(board, color, pos, UPPER_LEFT);
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, LEFT);
            count += flip_line(board, color, pos, LOWER_LEFT);
            count += flip_line(board, color, pos, LOWER);
            break;
        case A1:
        case A2:
        case B1:
        case B2:
            count += flip_line(board, color, pos, RIGHT);
            count += flip_line(board, color, pos, LOWER);
            count += flip_line(board, color, pos, LOWER_RIGHT);
            break;
        case A8:
        case A7:
        case B8:
        case B7:
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, UPPER_RIGHT);
            count += flip_line(board, color, pos, RIGHT);
            break;
        case H1:
        case H2:
        case G1:
        case G2:
            count += flip_line(board, color, pos, LEFT);
            count += flip_line(board, color, pos, LOWER_LEFT);
            count += flip_line(board, color, pos, LOWER);
            break;
        case H8:
        case H7:
        case G8:
        case G7:
            count += flip_line(board, color, pos, UPPER_LEFT);
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, LEFT);
            break;
        default:
            count += flip_line(board, color, pos, UPPER_LEFT);
            count += flip_line(board, color, pos, UPPER);
            count += flip_line(board, color, pos, UPPER_RIGHT);
            count += flip_line(board, color, pos, LEFT);
            count += flip_line(board, color, pos, RIGHT);
            count += flip_line(board, color, pos, LOWER_LEFT);
            count += flip_line(board, color, pos, LOWER);
            count += flip_line(board, color, pos, LOWER_RIGHT);
            break;
    }

    if (count > 0) {
        board->disks[pos] = color;
        // スタックへ記録
        STACK_PUSH(board, pos);
        STACK_PUSH(board, Board_opponent(color));
        STACK_PUSH(board, count);

        board->disk_num[color] += (count + 1);
        board->disk_num[Board_opponent(color)] -= count;
        board->disk_num[EMPTY]--;
    }

    return count;
}

int Board_unflip(Board *board)
{
    if (board->sp <= board->stack) {
        return 0;
    }

    int count = STACK_POP(board);
    int color = STACK_POP(board);

    board->disks[STACK_POP(board)] = EMPTY;

    for (int i = 0; i < count; i++) {
        board->disks[STACK_POP(board)] = color;
    }

    board->disk_num[color] += count;
    board->disk_num[Board_opponent(color)] -= (count + 1);
    board->disk_num[EMPTY]++;

    return count;
}

void Board_init_pattern(Board *board)
{
    for (int i = 0; i < NUM_PATTERN_ID; i++) {
        board->pattern[i] = 0;
    }

    for (int i = 0; i < NUM_DISK; i++) {
        if (board->disks[i] == BLACK) {
            put_square_black(board, i);
        } else if (board->disks[i] == WHITE) {
            put_square_white(board, i);
        }
    }
}

int Board_pattern(const Board *board, int id)
{
    return board->pattern[id];
}

///
/// @fn     add_pattern
/// @brief  指定パターンの状態を登録する
/// @param[in,out]  board       評価器
/// @param[in]      id          パターンID
/// @param[in]      pos_list    パターンを構成するマスのリスト
/// @param[in]      num         パターンを構成するマス数
///
static void add_pattern(Board *board, int id, const int *pos_list, int num)
{
    int i, j;
    int n = 1;

    for (i = 0; i < num; i++) {
        for (j = 0; board->pattern_diff[pos_list[i]][j] != 0; j++);

        // pos_list[i]に着手したときidのパターンをn増やす
        board->pattern_id[pos_list[i]][j] = id;
        board->pattern_diff[pos_list[i]][j] = n;
        n *= 3;
    }
}

///
/// @fn     init_pattern_diff
/// @brief  盤面パターン差分を初期化する
/// @param[in,out]  board   評価器
///
static void init_pattern_diff(Board *board)
{
    int i, j;
    // 各パターンの使用マス
    int pattern_list[][9] = {
        { A4, B4, C4, D4, E4, F4, G4, H4, -1 },
        { A5, B5, C5, D5, E5, F5, G5, H5, -1 },
        { D1, D2, D3, D4, D5, D6, D7, D8, -1 },
        { E1, E2, E3, E4, E5, E6, E7, E8, -1 },
        { A3, B3, C3, D3, E3, F3, G3, H3, -1 },
        { A6, B6, C6, D6, E6, F6, G6, H6, -1 },
        { C1, C2, C3, C4, C5, C6, C7, C8, -1 },
        { F1, F2, F3, F4, F5, F6, F7, F8, -1 },
        { A2, B2, C2, D2, E2, F2, G2, H2, -1 },
        { A7, B7, C7, D7, E7, F7, G7, H7, -1 },
        { B1, B2, B3, B4, B5, B6, B7, B8, -1 },
        { G1, G2, G3, G4, G5, G6, G7, G8, -1 },
        { A1, B2, C3, D4, E5, F6, G7, H8, -1 },
        { A8, B7, C6, D5, E4, F3, G2, H1, -1 },
        { A2, B3, C4, D5, E6, F7, G8, -1 },
        { B1, C2, D3, E4, F5, G6, H7, -1 },
        { A7, B6, C5, D4, E3, F2, G1, -1 },
        { B8, C7, D6, E5, F4, G3, H2, -1 },
        { A3, B4, C5, D6, E7, F8, -1 },
        { C1, D2, E3, F4, G5, H6, -1 },
        { A6, B5, C4, D3, E2, F1, -1 },
        { C8, D7, E6, F5, G4, H3, -1 },
        { A4, B5, C6, D7, E8, -1 },
        { D1, E2, F3, G4, H5, -1 },
        { A5, B4, C3, D2, E1, -1 },
        { D8, E7, F6, G5, H4, -1 },
        { A5, B6, C7, D8, -1 },
        { E1, F2, G3, H4, -1 },
        { A4, B3, C2, D1, -1 },
        { E8, F7, G6, H5, -1 },
        { B2, G1, F1, E1, D1, C1, B1, A1, -1 },
        { G2, B1, C1, D1, E1, F1, G1, H1, -1 },
        { B7, G8, F8, E8, D8, C8, B8, A8, -1 },
        { G7, B8, C8, D8, E8, F8, G8, H8, -1 },
        { B2, A7, A6, A5, A4, A3, A2, A1, -1 },
        { B7, A2, A3, A4, A5, A6, A7, A8, -1 },
        { G2, H7, H6, H5, H4, H3, H2, H1, -1 },
        { G7, H2, H3, H4, H5, H6, H7, H8, -1 },
        { B3, A3, C2, B2, A2, C1, B1, A1, -1 },
        { G3, H3, F2, G2, H2, F1, G1, H1, -1 },
        { B6, A6, C7, B7, A7, C8, B8, A8, -1 },
        { G6, H6, F7, G7, H7, F8, G8, H8, -1 },
        { -1 }
    };

    for (i = 0; i < NUM_DISK; i++) {
        for (j = 0; j < NUM_PATTERN_DIFF; j++) {
            board->pattern_id[i][j] = 0;
            board->pattern_diff[i][j] = 0;
        }
    }

    // 各パターンを登録する
    for (i = 0; pattern_list[i][0] >= 0; i++) {
        for (j = 0; pattern_list[i][j] >= 0; j++);
        add_pattern(board, i, pattern_list[i], j);
    }
}

///
/// @fn     flip_square_black
/// @brief  パターン更新：白石から黒石へ反転
/// @param[in,out]  board   盤面
/// @param[out]     pos     着手座標
///
static void flip_square_black(Board *board, int pos)
{
    board->disks[pos] = BLACK;
    // 最大6つの関連パターンについて状態数を更新する
    board->pattern[board->pattern_id[pos][0]] -= board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] -= board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] -= board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] -= board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] -= board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] -= board->pattern_diff[pos][5];
}

///
/// @fn     flip_square_white
/// @brief  パターン更新：黒石から白石へ反転
/// @param[in,out]  board   盤面
/// @param[out]     pos     着手座標
///
static void flip_square_white(Board *board, int pos)
{
    board->disks[pos] = WHITE;
    board->pattern[board->pattern_id[pos][0]] += board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] += board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] += board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] += board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] += board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] += board->pattern_diff[pos][5];
}

///
/// @fn     put_square_black
/// @brief  パターン更新：黒石を着手
/// @param[in,out]  board   盤面
/// @param[out]     pos     座標
///
static void put_square_black(Board *board, int pos)
{
    board->disks[pos] = BLACK;
    board->pattern[board->pattern_id[pos][0]] += board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] += board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] += board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] += board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] += board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] += board->pattern_diff[pos][5];
}

///
/// @fn     put_square_white
/// @brief  パターン更新：白石を着手
/// @param[in,out]  board   盤面
/// @param[out]     pos     座標
///
static void put_square_white(Board *board, int pos)
{
    board->disks[pos] = WHITE;
    board->pattern[board->pattern_id[pos][0]] += (board->pattern_diff[pos][0] + board->pattern_diff[pos][0]);
    board->pattern[board->pattern_id[pos][1]] += (board->pattern_diff[pos][1] + board->pattern_diff[pos][1]);
    board->pattern[board->pattern_id[pos][2]] += (board->pattern_diff[pos][2] + board->pattern_diff[pos][2]);
    board->pattern[board->pattern_id[pos][3]] += (board->pattern_diff[pos][3] + board->pattern_diff[pos][3]);
    board->pattern[board->pattern_id[pos][4]] += (board->pattern_diff[pos][4] + board->pattern_diff[pos][4]);
    board->pattern[board->pattern_id[pos][5]] += (board->pattern_diff[pos][5] + board->pattern_diff[pos][5]);
}

///
/// @fn     remove_square_black
/// @brief  パターン更新：黒石を戻す
/// @param[in,out]  board   盤面
/// @param[out]     pos     座標
///
static void remove_square_black(Board *board, int pos)
{
    board->disks[pos] = EMPTY;
    board->pattern[board->pattern_id[pos][0]] -= board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] -= board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] -= board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] -= board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] -= board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] -= board->pattern_diff[pos][5];
}

///
/// @fn     remove_square_white
/// @brief  パターン更新：白石を戻す
/// @param[in,out]  board   盤面
/// @param[out]     pos     座標
///
static void remove_square_white(Board *board, int pos)
{
    board->disks[pos] = EMPTY;
    board->pattern[board->pattern_id[pos][0]] -= (board->pattern_diff[pos][0] + board->pattern_diff[pos][0]);
    board->pattern[board->pattern_id[pos][1]] -= (board->pattern_diff[pos][1] + board->pattern_diff[pos][1]);
    board->pattern[board->pattern_id[pos][2]] -= (board->pattern_diff[pos][2] + board->pattern_diff[pos][2]);
    board->pattern[board->pattern_id[pos][3]] -= (board->pattern_diff[pos][3] + board->pattern_diff[pos][3]);
    board->pattern[board->pattern_id[pos][4]] -= (board->pattern_diff[pos][4] + board->pattern_diff[pos][4]);
    board->pattern[board->pattern_id[pos][5]] -= (board->pattern_diff[pos][5] + board->pattern_diff[pos][5]);
}

///
/// @fn     flip_line_pattern
/// @brief  パターン更新しつつ一方向へ石を返す
/// @param[in,out]  board   盤面
/// @param[in]      color   手番
/// @param[in]      pos     着手座標
/// @param[in]      dir     探索方向
/// @return 返した石数
///
static int flip_line_pattern(Board *board, int color, int pos, int dir)
{
    void (*func_flip)(Board *, int);

    // パターン更新関数を手番で変える
    if (color == BLACK) {
        func_flip = flip_square_black;
    } else {
        func_flip = flip_square_white;
    }

    int op = Board_opponent(color);
    int cur_pos;

    int count = 0;
    cur_pos = pos + dir;
    if (board->disks[cur_pos] != op) {
        return 0;
    }

    // ループアンローリング
    cur_pos += dir;
    if (board->disks[cur_pos] == op) {
        cur_pos += dir;
        if (board->disks[cur_pos] == op) {
            cur_pos += dir;
            if (board->disks[cur_pos] == op) {
                cur_pos += dir;
                if (board->disks[cur_pos] == op) {
                    cur_pos += dir;
                    if (board->disks[cur_pos] == op) {
                        cur_pos += dir;
                        if (board->disks[cur_pos] == op) {
                            return 0;
                        }
                        cur_pos -= dir;
                        count++;
                        func_flip(board, cur_pos);
                        STACK_PUSH(board, cur_pos);
                    } else if (board->disks[cur_pos] != color) {
                        return 0;
                    }
                    cur_pos -= dir;
                    count++;
                    func_flip(board, cur_pos);
                    STACK_PUSH(board, cur_pos);
                } else if (board->disks[cur_pos] != color) {
                    return 0;
                }
                cur_pos -= dir;
                count++;
                func_flip(board, cur_pos);
                STACK_PUSH(board, cur_pos);
            } else if (board->disks[cur_pos] != color) {
                return 0;
            }
            cur_pos -= dir;
            count++;
            func_flip(board, cur_pos);
            STACK_PUSH(board, cur_pos);
        } else if (board->disks[cur_pos] != color) {
            return 0;
        }
        cur_pos -= dir;
        count++;
        func_flip(board, cur_pos);
        STACK_PUSH(board, cur_pos);
    } else if (board->disks[cur_pos] != color) {
        return 0;
    }
    cur_pos -= dir;
    count++;
    func_flip(board, cur_pos);
    STACK_PUSH(board, cur_pos);

    return count;
}

int Board_flip_pattern(Board *board, int color, int pos)
{
    if (board->disks[pos] != EMPTY) {
        return 0;
    }

    int count = 0;

    switch (pos) {
        case C1:
        case C2:
        case D1:
        case D2:
        case E1:
        case E2:
        case F1:
        case F2:
            count += flip_line_pattern(board, color, pos, LEFT);
            count += flip_line_pattern(board, color, pos, RIGHT);
            count += flip_line_pattern(board, color, pos, LOWER_LEFT);
            count += flip_line_pattern(board, color, pos, LOWER);
            count += flip_line_pattern(board, color, pos, LOWER_RIGHT);
            break;
        case C8:
        case C7:
        case D8:
        case D7:
        case E8:
        case E7:
        case F8:
        case F7:
            count += flip_line_pattern(board, color, pos, UPPER_LEFT);
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, UPPER_RIGHT);
            count += flip_line_pattern(board, color, pos, LEFT);
            count += flip_line_pattern(board, color, pos, RIGHT);
            break;
        case A3:
        case A4:
        case A5:
        case A6:
        case B3:
        case B4:
        case B5:
        case B6:
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, UPPER_RIGHT);
            count += flip_line_pattern(board, color, pos, RIGHT);
            count += flip_line_pattern(board, color, pos, LOWER);
            count += flip_line_pattern(board, color, pos, LOWER_RIGHT);
            break;
        case H3:
        case H4:
        case H5:
        case H6:
        case G3:
        case G4:
        case G5:
        case G6:
            count += flip_line_pattern(board, color, pos, UPPER_LEFT);
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, LEFT);
            count += flip_line_pattern(board, color, pos, LOWER_LEFT);
            count += flip_line_pattern(board, color, pos, LOWER);
            break;
        case A1:
        case A2:
        case B1:
        case B2:
            count += flip_line_pattern(board, color, pos, RIGHT);
            count += flip_line_pattern(board, color, pos, LOWER);
            count += flip_line_pattern(board, color, pos, LOWER_RIGHT);
            break;
        case A8:
        case A7:
        case B8:
        case B7:
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, UPPER_RIGHT);
            count += flip_line_pattern(board, color, pos, RIGHT);
            break;
        case H1:
        case H2:
        case G1:
        case G2:
            count += flip_line_pattern(board, color, pos, LEFT);
            count += flip_line_pattern(board, color, pos, LOWER_LEFT);
            count += flip_line_pattern(board, color, pos, LOWER);
            break;
        case H8:
        case H7:
        case G8:
        case G7:
            count += flip_line_pattern(board, color, pos, UPPER_LEFT);
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, LEFT);
            break;
        default:
            count += flip_line_pattern(board, color, pos, UPPER_LEFT);
            count += flip_line_pattern(board, color, pos, UPPER);
            count += flip_line_pattern(board, color, pos, UPPER_RIGHT);
            count += flip_line_pattern(board, color, pos, LEFT);
            count += flip_line_pattern(board, color, pos, RIGHT);
            count += flip_line_pattern(board, color, pos, LOWER_LEFT);
            count += flip_line_pattern(board, color, pos, LOWER);
            count += flip_line_pattern(board, color, pos, LOWER_RIGHT);
            break;
    }

    if (count > 0) {
        if (color == BLACK) {
            put_square_black(board, pos);
        } else {
            put_square_white(board, pos);
        }
        STACK_PUSH(board, pos);
        STACK_PUSH(board, Board_opponent(color));
        STACK_PUSH(board, count);

        board->disk_num[color] += (count + 1);
        board->disk_num[Board_opponent(color)] -= count;
        board->disk_num[EMPTY]--;
    }

    return count;
}

int Board_unflip_pattern(Board *board)
{
    if (board->sp <= board->stack) {
        return 0;
    }

    int count  = STACK_POP(board);
    int color = STACK_POP(board);

    if (color == BLACK) {
        remove_square_white(board, STACK_POP(board));
        for (int i = 0; i < count; i++) {
            flip_square_black(board, STACK_POP(board));
        }
    } else {
        remove_square_black(board, STACK_POP(board));
        for (int i = 0; i < count; i++) {
            flip_square_white(board, STACK_POP(board));
        }
    }

    board->disk_num[color] += count;
    board->disk_num[Board_opponent(color)] -= (count + 1);
    board->disk_num[EMPTY]++;

    return count;
}

///
/// @fn     count_flips_line
/// @brief  一方向へ返せる石数を調べる
/// @param[in,out]  board   盤面
/// @param[in]      color   手番
/// @param[in]      pos     着手座標
/// @param[in]      dir     探索方向
/// @return 返せる石数
///
static int count_flips_line(const Board *board, int color, int pos, int dir)
{
    int cur_pos;
    int op = Board_opponent(color);

    // 一方向へ逆色石を探索、端が同色のときカウント
    int count = 0;
    for (cur_pos = (pos + dir); board->disks[cur_pos] == op; cur_pos += dir) {
        count++;
    }
    if (board->disks[cur_pos] != color) {
        return 0;
    }

    return count;
}

int Board_count_flips(const Board *board, int color, int pos)
{
    if (board->disks[pos] != EMPTY) {
        return 0;
    }

    int count = 0;

    switch (pos) {
        case C1:
        case C2:
        case D1:
        case D2:
        case E1:
        case E2:
        case F1:
        case F2:
            count += count_flips_line(board, color, pos, LEFT);
            count += count_flips_line(board, color, pos, RIGHT);
            count += count_flips_line(board, color, pos, LOWER_LEFT);
            count += count_flips_line(board, color, pos, LOWER);
            count += count_flips_line(board, color, pos, LOWER_RIGHT);
            break;
        case C8:
        case C7:
        case D8:
        case D7:
        case E8:
        case E7:
        case F8:
        case F7:
            count += count_flips_line(board, color, pos, UPPER_LEFT);
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, UPPER_RIGHT);
            count += count_flips_line(board, color, pos, LEFT);
            count += count_flips_line(board, color, pos, RIGHT);
            break;
        case A3:
        case A4:
        case A5:
        case A6:
        case B3:
        case B4:
        case B5:
        case B6:
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, UPPER_RIGHT);
            count += count_flips_line(board, color, pos, RIGHT);
            count += count_flips_line(board, color, pos, LOWER);
            count += count_flips_line(board, color, pos, LOWER_RIGHT);
            break;
        case H3:
        case H4:
        case H5:
        case H6:
        case G3:
        case G4:
        case G5:
        case G6:
            count += count_flips_line(board, color, pos, UPPER_LEFT);
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, LEFT);
            count += count_flips_line(board, color, pos, LOWER_LEFT);
            count += count_flips_line(board, color, pos, LOWER);
            break;
        case A1:
        case A2:
        case B1:
        case B2:
            count += count_flips_line(board, color, pos, RIGHT);
            count += count_flips_line(board, color, pos, LOWER);
            count += count_flips_line(board, color, pos, LOWER_RIGHT);
            break;
        case A8:
        case A7:
        case B8:
        case B7:
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, UPPER_RIGHT);
            count += count_flips_line(board, color, pos, RIGHT);
            break;
        case H1:
        case H2:
        case G1:
        case G2:
            count += count_flips_line(board, color, pos, LEFT);
            count += count_flips_line(board, color, pos, LOWER_LEFT);
            count += count_flips_line(board, color, pos, LOWER);
            break;
        case H8:
        case H7:
        case G8:
        case G7:
            count += count_flips_line(board, color, pos, UPPER_LEFT);
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, LEFT);
            break;
        default:
            count += count_flips_line(board, color, pos, UPPER_LEFT);
            count += count_flips_line(board, color, pos, UPPER);
            count += count_flips_line(board, color, pos, UPPER_RIGHT);
            count += count_flips_line(board, color, pos, LEFT);
            count += count_flips_line(board, color, pos, RIGHT);
            count += count_flips_line(board, color, pos, LOWER_LEFT);
            count += count_flips_line(board, color, pos, LOWER);
            count += count_flips_line(board, color, pos, LOWER_RIGHT);
            break;
    }

    return count;
}

bool Board_can_flip(const Board *board, int disk, int pos)
{
    if (board->disks[pos] != EMPTY) {
        return false;
    }

    // 各方向へ着手できるかを確認
    if (count_flips_line(board, disk, pos, UPPER_LEFT) > 0)  return true;
    if (count_flips_line(board, disk, pos, UPPER) > 0)       return true;
    if (count_flips_line(board, disk, pos, UPPER_RIGHT) > 0) return true;
    if (count_flips_line(board, disk, pos, LEFT) > 0)        return true;
    if (count_flips_line(board, disk, pos, RIGHT) > 0)       return true;
    if (count_flips_line(board, disk, pos, LOWER_LEFT) > 0)  return true;
    if (count_flips_line(board, disk, pos, LOWER) > 0)       return true;
    if (count_flips_line(board, disk, pos, LOWER_RIGHT) > 0) return true;

    return false;
}

void Board_copy(const Board *src, Board *dst)
{
    *dst = *src;
    // スタックポインタ位置の調整
    dst->sp = dst->stack + (src->sp - src->stack);
}

void Board_reverse(Board *board)
{
    for (int pos = 0; pos < NUM_DISK; pos++) {
        if (board->disks[pos] == BLACK) {
            board->disks[pos] = WHITE;
            board->disk_num[BLACK]--;
            board->disk_num[WHITE]++;
        } else if (board->disks[pos] == WHITE) {
            board->disks[pos] = BLACK;
            board->disk_num[WHITE]--;
            board->disk_num[BLACK]++;
        }
    }

    // スタック情報を反転する
    for (int *p = board->sp; p > board->stack; ) {
        p--;
        int count = *p;             // 返した石数を取得
        p--;
        *p = Board_opponent(*p);    // 着手色を反転
        p -= (count + 1);           // （着手+石数）ぶんスタックをさかのぼる
    }

    Board_init_pattern(board);
}

bool Board_can_play(const Board *board, int color)
{
    // 着手可能な座標があるか走査
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Board_can_flip(board, color, Board_pos(x, y))) {
                return true;
            }
        }
    }

    return false;
}

int Board_pos(int x, int y)
{
    return ((y + 1) * (BOARD_SIZE + 1) + (x + 1));
}

int Board_x(int pos)
{
    return (pos % (BOARD_SIZE + 1) - 1);
}

int Board_y(int pos)
{
    return (pos / (BOARD_SIZE + 1) - 1);
}

int Board_opponent(int color)
{
    return  OPPONENT(color);
}
