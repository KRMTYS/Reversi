///
/// @file   board.c
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#include "board.h"

#include <stdlib.h>

// スタック長
// 返せる最大の石数：(BOARD_SIZE - 2) * 3
// 着手位置、相手の石色、返した石数の情報：3
#define STACK_SIZE (((BOARD_SIZE - 2) * 3 + 3) * BOARD_SIZE * BOARD_SIZE - 4)

///
/// @struct Board_
/// @brief  リバーシ盤面
///
struct Board_ {
    Disk disks[BOARD_LENGTH];   ///< マス情報
    int  stack[STACK_SIZE];     ///< 返した石を記録するスタック
                                ///< （返した石の位置1）...（~N）、（着手位置）、（相手の石色）、（返した石数）
    int  *sp;                   ///< スタックポインタ
    int  disk_num[3];           ///< 石数（0: 黒 1: 白、2: 空）
};

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

///
/// @enum   Dir
/// @brief  盤面上での8方向インデックス
///
typedef enum {
    UPPER       =  -9,  ///< 上
    UPPER_RIGHT =  -8,  ///< 右上
    UPPER_LEFT  = -10,  ///< 左上
    RIGHT       =   1,  ///< 右
    LEFT        =  -1,  ///< 左
    LOWER       =   9,  ///< 下
    LOWER_RIGHT =  10,  ///< 右下
    LOWER_LEFT  =   8   ///< 左下
} Dir;

static int flip_line(Board *board, Disk disk, Pos pos, Dir dir);
static int count_flips_line(const Board *board, Disk disk, Pos pos, Dir dir);

Board *Board_create(void) {
    Board *board = malloc(sizeof(Board));
    if (board) {
        Board_init(board);
    }

    return board;
}

void Board_delete(Board *board) {
    free(board);
    board = NULL;
}

void Board_init(Board *board) {
    for (Pos pos = 0; pos < BOARD_LENGTH; pos++) {
        board->disks[pos] = WALL;
    }

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            board->disks[XY2POS(x, y)] = EMPTY;
        }
    }

    // 石の初期配置
    board->disks[D4] = WHITE;
    board->disks[D5] = BLACK;
    board->disks[E4] = BLACK;
    board->disks[E5] = WHITE;

    board->sp = board->stack;

    board->disk_num[BLACK] = 2;
    board->disk_num[WHITE] = 2;
    board->disk_num[EMPTY] = (BOARD_SIZE * BOARD_SIZE) - 4;
}

Disk Board_disk(const Board *board, Pos pos) {
    return board->disks[pos];
}

int Board_count_disks(const Board *board, Disk disk) {
    return board->disk_num[disk];
}

static int flip_line(Board *board, Disk disk, Pos pos, Dir dir) {
    Pos cur_pos;
    int count = 0;
    Disk op = OPPONENT(disk);

    // 相手の石を探索
    for (cur_pos = (pos + dir); board->disks[cur_pos] == op; cur_pos += dir);

    // 終端が自分の石色であれば着手位置までたどりながら石を返す
    if (board->disks[cur_pos] == disk) {
        for (cur_pos -= dir; board->disks[cur_pos] == op; cur_pos -= dir) {
            board->disks[cur_pos] = disk;
            STACK_PUSH(board, cur_pos);
            count++;
        }
    }

    return count;
}

int Board_flip(Board *board, Disk disk, Pos pos) {
    int count = 0;
    
    if (board->disks[pos] != EMPTY) {
        return 0;
    }

    switch (pos) {
        case C1:
        case C2:
        case D1:
        case D2:
        case E1:
        case E2:
        case F1:
        case F2:
            count += flip_line(board, disk, pos, RIGHT);
            count += flip_line(board, disk, pos, LOWER_RIGHT);
            count += flip_line(board, disk, pos, LOWER);
            count += flip_line(board, disk, pos, LOWER_LEFT);
            count += flip_line(board, disk, pos, LEFT);
            break;
        case C8:
        case C7:
        case D8:
        case D7:
        case E8:
        case E7:
        case F8:
        case F7:
            count += flip_line(board, disk, pos, LEFT);
            count += flip_line(board, disk, pos, UPPER_LEFT);
            count += flip_line(board, disk, pos, UPPER);
            count += flip_line(board, disk, pos, UPPER_RIGHT);
            count += flip_line(board, disk, pos, RIGHT);
            break;
        case A3:
        case A4:
        case A5:
        case A6:
        case B3:
        case B4:
        case B5:
        case B6:
            count += flip_line(board, disk, pos, UPPER);
            count += flip_line(board, disk, pos, UPPER_RIGHT);
            count += flip_line(board, disk, pos, RIGHT);
            count += flip_line(board, disk, pos, LOWER_LEFT);
            count += flip_line(board, disk, pos, LOWER);
            break;
        case H3:
        case H4:
        case H5:
        case H6:
        case G3:
        case G4:
        case G5:
        case G6:
            count += flip_line(board, disk, pos, LOWER);
            count += flip_line(board, disk, pos, LOWER_LEFT);
            count += flip_line(board, disk, pos, LEFT);
            count += flip_line(board, disk, pos, UPPER_LEFT);
            count += flip_line(board, disk, pos, UPPER);
            break;
        case A1:
        case A2:
        case B1:
        case B2:
            count += flip_line(board, disk, pos, RIGHT);
            count += flip_line(board, disk, pos, LOWER_RIGHT);
            count += flip_line(board, disk, pos, LOWER);
            break;
        case A8:
        case A7:
        case B8:
        case B7:
            count += flip_line(board, disk, pos, UPPER);
            count += flip_line(board, disk, pos, UPPER_RIGHT);
            count += flip_line(board, disk, pos, RIGHT);
            break;
        case H1:
        case H2:
        case G1:
        case G2:
            count += flip_line(board, disk, pos, LOWER);
            count += flip_line(board, disk, pos, LOWER_LEFT);
            count += flip_line(board, disk, pos, LEFT);
            break;
        case H8:
        case H7:
        case G8:
        case G7:
            count += flip_line(board, disk, pos, LEFT);
            count += flip_line(board, disk, pos, UPPER_LEFT);
            count += flip_line(board, disk, pos, UPPER);
            break;
        default:
            count += flip_line(board, disk, pos, UPPER);
            count += flip_line(board, disk, pos, UPPER_RIGHT);
            count += flip_line(board, disk, pos, RIGHT);
            count += flip_line(board, disk, pos, LOWER_RIGHT);
            count += flip_line(board, disk, pos, LOWER);
            count += flip_line(board, disk, pos, LOWER_LEFT);
            count += flip_line(board, disk, pos, LEFT);
            count += flip_line(board, disk, pos, UPPER_LEFT);
            break;
    }

    if (count > 0) {
        board->disks[pos] = disk;
        STACK_PUSH(board, pos);
        STACK_PUSH(board, OPPONENT(disk));
        STACK_PUSH(board, count);

        board->disk_num[disk] += (count + 1);
        board->disk_num[OPPONENT(disk)] -= count;
        board->disk_num[EMPTY]--;
    }

    return count;
}

int Board_unflip(Board *board) {
    if (board->sp <= board->stack) {
        return 0;
    }

    int count  = STACK_POP(board);
    Disk color = STACK_POP(board);

    board->disks[STACK_POP(board)] = EMPTY;

    for (int i = 0; i < count; i++) {
        board->disks[STACK_POP(board)] = color;
    }

    board->disk_num[color] += count;
    board->disk_num[OPPONENT(color)] -= (count + 1);
    board->disk_num[EMPTY]++;

    return count;
}

static int count_flips_line(const Board *board, Disk disk, Pos pos, Dir dir) {
    int cur_pos;
    int count = 0;
    Disk op = OPPONENT(disk);

    // 相手の石を探索
    for (cur_pos = pos + dir; board->disks[cur_pos] == op; cur_pos += dir) {
        count++;
    }
    // 終端が自色石でないときカウントしない
    if (board->disks[cur_pos] != disk) {
        return 0;
    }

    return count;
}

int Board_count_flips(const Board *board, Disk disk, Pos pos) {
    int count = 0;

    count += count_flips_line(board, disk, pos, UPPER);
    count += count_flips_line(board, disk, pos, UPPER_RIGHT);
    count += count_flips_line(board, disk, pos, UPPER_LEFT);
    count += count_flips_line(board, disk, pos, RIGHT);
    count += count_flips_line(board, disk, pos, LEFT);
    count += count_flips_line(board, disk, pos, LOWER);
    count += count_flips_line(board, disk, pos, LOWER_RIGHT);
    count += count_flips_line(board, disk, pos, LOWER_LEFT);

    return count;
}

bool Board_can_flip(const Board *board, Disk disk, Pos pos) {
    if (board->disks[pos] != EMPTY) {
        return false;
    }

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

bool Board_can_play(const Board *board, Disk disk) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Board_can_flip(board, disk, XY2POS(x, y))) {
                return true;
            }
        }
    }

    return false;
}

void Board_copy(const Board *src, Board *dst) {
    *dst = *src;
    dst->sp = dst->stack + (src->sp - src->stack);
}

void Board_reverse(Board *board) {
    for (Pos pos = 0; pos < BOARD_LENGTH; pos++) {
        switch (board->disks[pos]) {
            case BLACK:
                board->disks[pos] = WHITE;
                board->disk_num[BLACK]--;
                board->disk_num[WHITE]++;
                break;
            case WHITE:
                board->disks[pos] = BLACK;
                board->disk_num[WHITE]--;
                board->disk_num[BLACK]++;
                break;
            default:
                break;
        }
    }

    for (int *p = board->sp; p > board->stack; ) {
        p--;
        int count = *p;     // 返した石数
        p--;
        *p = OPPONENT(*p);  // 色を反転
        p -= (count + 1);   // （石数+着手）分さかのぼる
    }
}
