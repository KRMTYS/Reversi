///
/// @file   board.c
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#include "board.h"

#include <stdio.h>
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
                                ///< （返した石の位置1）（~N）... 、（着手位置）、（相手の石色）、（返した石数）
    int  *sp;                   ///< スタックポインタ
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

Board *Board_create(void) {
    Board *board = malloc(sizeof(Board));

    return board;
}

void Board_delete(Board *board) {
    free(board);
    board = NULL;
}

void Board_init(Board *board) {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        board->disks[i] = WALL;
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
}

Disk Board_disk(Board *board, Pos pos) {
    return board->disks[pos];
}

bool Board_check_valid(Board *board, Disk disk, Pos pos) {
    if ((board->disks[pos] == EMPTY) && Board_count_flip_disks(board, disk, pos) > 0) {
        return true;
    }

    return false;
}

bool Board_has_valid_move(Board *board, Disk disk) {
    return (Board_count_valid_moves(board, disk) > 0) ? true : false;
}

int Board_count_valid_moves(Board *board, Disk disk) {
    int count = 0;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (Board_check_valid(board, disk, i)) {
            count++;
        }
    }

    return count;
}

///
/// @fn     count_flip_disks_line
/// @brief  一方向に返せる石数を数える
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @param[in]  dir     探索方向
/// @return 返せる石数
///
static int count_flip_disks_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int i;
    int count = 0;
    Disk op = OPPONENT(disk);

    // 相手の石を探索
    for (i = pos + dir; board->disks[i] == op; i += dir) {
        count++;
    }
    // 終端が自色石でないときカウントしない
    if (board->disks[i] != disk) {
        return 0;
    }

    return count;
}

int Board_count_disk(Board *board, Disk disk) {
    int count = 0;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (board->disks[i] == disk) {
            count++;
        }
    }

    return count;
}

int Board_count_flip_disks(Board *board, Disk disk, Pos pos) {
    int count = 0;

    count += count_flip_disks_line(board, disk, pos, UPPER);
    count += count_flip_disks_line(board, disk, pos, UPPER_RIGHT);
    count += count_flip_disks_line(board, disk, pos, UPPER_LEFT);
    count += count_flip_disks_line(board, disk, pos, RIGHT);
    count += count_flip_disks_line(board, disk, pos, LEFT);
    count += count_flip_disks_line(board, disk, pos, LOWER);
    count += count_flip_disks_line(board, disk, pos, LOWER_RIGHT);
    count += count_flip_disks_line(board, disk, pos, LOWER_LEFT);

    return count;
}

///
/// @fn     flip_line
/// @brief  一方向に石を反転する
/// @param[in,out]  board   盤面
/// @param[in]      disk    手番
/// @param[in]      pos     座標
/// @param[in]      dir     探索方向
/// @return 返した石数
///
static int flip_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int n;
    int count = 0;
    Disk op = OPPONENT(disk);

    // 相手の石を探索
    for (n = pos + dir; board->disks[n] == op; n += dir);

    // 終端が自分の石色であれば着手位置までたどりながら石を返す
    if (board->disks[n] == disk) {
        for (n -= dir; board->disks[n] == op; n -= dir) {
            board->disks[n] = disk;
            STACK_PUSH(board, n);
            count++;
        }
    }

    return count;
}

int Board_flip(Board *board, Disk disk, Pos pos) {
    int count = 0;
    
    count += flip_line(board, disk, pos, UPPER);
    count += flip_line(board, disk, pos, UPPER_RIGHT);
    count += flip_line(board, disk, pos, UPPER_LEFT);
    count += flip_line(board, disk, pos, RIGHT);
    count += flip_line(board, disk, pos, LEFT);
    count += flip_line(board, disk, pos, LOWER);
    count += flip_line(board, disk, pos, LOWER_RIGHT);
    count += flip_line(board, disk, pos, LOWER_LEFT);

    if (count > 0) {
        board->disks[pos] = disk;
        STACK_PUSH(board, pos);
        STACK_PUSH(board, OPPONENT(disk));
        STACK_PUSH(board, count);
    }

    return count;
}

void Board_unflip(Board *board) {
    if (board->sp <= board->stack) {
        return;
    }

    int count  = STACK_POP(board);
    Disk color = STACK_POP(board);

    board->disks[STACK_POP(board)] = EMPTY;

    for (int i = 0; i < count; i++) {
        board->disks[STACK_POP(board)] = color;
    }
}

void Board_copy(Board *src, Board *dst) {
    *dst = *src;
    dst->sp = dst->sp + (src->sp - src->stack);
}

void Board_reverse(Board *board) {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        switch (board->disks[i]) {
            case BLACK: board->disks[i] = WHITE; break;
            case WHITE: board->disks[i] = BLACK; break;
            default: break;
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

void Board_print(Board *board, Disk current) {
    printf("    A B C D E F G H \n");
    printf("  +-----------------+\n");

    for (int y = 0; y < BOARD_SIZE; y++) {
        printf("%d | ", y);
        for (int x = 0; x < BOARD_SIZE; x++) {
            Pos pos = XY2POS(x, y);
            switch (board->disks[pos]) {
                case WHITE: printf("O "); break;
                case BLACK: printf("@ "); break;
                default:
                    if (Board_check_valid(board, current, pos)) {
                        printf("* ");
                    } else {
                        printf("- ");
                    }
                    break;
            }
        }
        printf("|\n");
    }

    printf("  +-----------------+\n");
}
