///
/// @file   board.c
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#include "board.h"

#include <stdio.h>
#include <stdlib.h>

// スタック長、適当
#define STACK_LENGTH 60 * 20

///
/// @struct Board_
/// @brief  リバーシ盤面
///
struct Board_ {
    Disk disks[BOARD_LENGTH]; ///< マス
    int stack[STACK_LENGTH];  ///< 手を記録するスタック
                              ///< (返した石の位置1), ... , (返した石の数), (置いた石の位置)
                              ///< の順に記録される
    int *sp;                  ///< スタックポインタ
};

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

// スタック操作
static void push_stack(Board *board, int n) {
    *(board->sp++) = n;
}

static int pop_stack(Board *board) {
    return *(--board->sp);
}


Board *Board_create(void) {
    Board *board = malloc(sizeof(Board));

    return board;
}

Board *Board_delete(Board *board) {
    free(board);
    board = NULL;
}

void Board_init(Board *board) {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        if ((i < A1) || (i > H8) || (i % 9 == 0) ) {
            board->disks[i] = WALL;
        } else {
            board->disks[i] = EMPTY;
        }
    }

    // 石の初期配置
    board->disks[D4] = WHITE;
    board->disks[D5] = BLACK;
    board->disks[E4] = BLACK;
    board->disks[E5] = WHITE;

    // スタックの初期化
    for (int i = 0; i < STACK_LENGTH; i++) {
        board->stack[i] = 0;
    }

    board->sp = board->stack;
}

///
/// @fn     is_on_board
/// @brief  座標の判定
/// @param[in]  pos 座標
/// @retval true    指定した座標は盤上である
/// @retval false   指定した座標は盤上ではない
///
static bool is_on_board(Pos pos) {
    return ((pos >= A1) && (pos <= H8) && (pos % 9 != 0)) ? true : false;
}

///
/// @fn     is_empty
/// @brief  空マスの判定
/// @param[in]  board   座標
/// @param[in]  pos     座標
/// @retval true    指定した座標は空マスである
/// @retval false   指定した座標は空マスではない
///
static bool is_empty(Board *board, Pos pos) {
    return (board->disks[pos] == EMPTY) ? true : false;
}

bool Board_check_valid(Board *board, Disk disk, Pos pos) {
    if (!is_on_board(pos) || !is_empty(board, pos)) {
        return false;
    }

    if (Board_count_flip_disks(board, disk, pos) > 0) {
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
/// @brief  一方向の返せる石数のカウント
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @param[in]  dir     探索方向
/// @return 返せる石数
///
static int count_flip_disks_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int count = 0;

    // 同色石まで探索
    for (int i = pos + dir; board->disks[i] != disk ; i += dir) {
        // 空き/壁があるとき返せない
        if ((board->disks[i] == EMPTY) || (board->disks[i] == WALL)) {
            return 0;
        }

        count++;
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
/// @brief  一方向の石を反転
/// @param[in,out]  board   盤面
/// @param[in]      disk    手番
/// @param[in]      pos     座標
/// @param[in]      dir     探索方向
/// @return 返した石数
///
static int flip_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int count = 0;
    int n;

    // 同色石まで探索
    for (n = pos + dir; board->disks[n] != disk ; n += dir) {
        // 空き/壁があるとき返せない
        if ((board->disks[n] == EMPTY) || (board->disks[n] == WALL)) {
            return 0;
        }

        count++;
    }

    n -= dir;

    // 石を返す
    while (n != (int)pos) {
        board->disks[n] = disk;
        push_stack(board, n);
        n -= dir;
    }

    return count;
}

int Board_put_and_flip(Board *board, Disk disk, Pos pos) {
    int count = 0;
    
    count += flip_line(board, disk, pos, UPPER);
    count += flip_line(board, disk, pos, UPPER_RIGHT);
    count += flip_line(board, disk, pos, UPPER_LEFT);
    count += flip_line(board, disk, pos, RIGHT);
    count += flip_line(board, disk, pos, LEFT);
    count += flip_line(board, disk, pos, LOWER);
    count += flip_line(board, disk, pos, LOWER_RIGHT);
    count += flip_line(board, disk, pos, LOWER_LEFT);

    board->disks[pos] = disk;

    push_stack(board, count);
    push_stack(board, pos);

    return count;
}

void Board_undo(Board *board) {
    board->disks[pop_stack(board)] = EMPTY;

    int n = pop_stack(board);

    for (int i = 0; i < n; i++) {
        board->disks[pop_stack(board)] *= -1;
    }
}

void Board_print(Board *board, Disk current) {
    printf("    A B C D E F G H \n");
    printf("  +-----------------+\n");

    for (int y = 1; y <= BOARD_SIZE; y++) {
        printf("%d | ", y);
        for (int x = 1; x <= BOARD_SIZE; x++) {
            switch (board->disks[xy_to_pos(x, y)]) {
                case WHITE: printf("O "); break;
                case BLACK: printf("@ "); break;
                default:
                    if (Board_check_valid(board, current, xy_to_pos(x, y))) {
                        printf("* ");
                    }
                    else {
                        printf("- ");
                    }
                    break;
            }
        }

        printf("|\n");
    }
    printf("  +-----------------+\n");

    printf("@:%2d O:%2d\n", Board_count_disk(board, BLACK), Board_count_disk(board, WHITE));
}
