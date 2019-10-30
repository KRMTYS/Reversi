#include "board.h"

#include <stdio.h>

// スタック操作
static void push_stack(Board *board, int n) {
    *(board->sp++) = n;
}

static int pop_stack(Board *board) {
    return *(--board->sp);
}

static bool is_on_board(Pos pos) {
    return ((pos >= A1) && (pos <= H8) && (pos % 9 != 0)) ? true : false;
}

static bool is_empty(Board *board, Pos pos) {
    return (board->squares[pos] == EMPTY) ? true : false;
}

void init_board(Board *board) {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (is_on_board(i)) {
            board->squares[i] = EMPTY;
        } else {
            board->squares[i] = WALL;
        }
    }

    // 石の初期配置
    board->squares[D4] = WHITE;
    board->squares[D5] = BLACK;
    board->squares[E4] = BLACK;
    board->squares[E5] = WHITE;

    // 黒先攻
    board->current_turn = BLACK;

    board->turn = 1;

    // スタックの初期化
    for (int i = 0; i < STACK_LENGTH; i++) {
        board->stack[i] = 0;
    }

    board->sp = board->stack;
}

bool is_valid(Board *board, Disk disk, Pos pos) {
    if (!is_on_board(pos) || !is_empty(board, pos)) {
        return false;
    }

    if (count_flip_disks(board, disk, pos) > 0) {
        return true;
    }

    return false;
}

bool has_valid_move(Board *board, Disk disk) {
    return (count_valid_moves(board, disk) > 0) ? true : false;
}

int count_valid_moves(Board *board, Disk disk) {
    int count = 0;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (is_valid(board, disk, i)) {
            count++;
        }
    }

    return count;
}

void change_turn(Board *board, int n) {
    board->current_turn = OPPONENT(board->current_turn);
    board->turn += n;
}

bool can_play(Board *board) {
    if (!has_valid_move(board, board->current_turn)) {
        if (!has_valid_move(board, OPPONENT(board->current_turn))) {
            return false;
        }
    }

    return true;
}

static int count_flip_disks_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int count = 0;

    // 同色石まで探索
    for (int i = pos + dir; board->squares[i] != disk ; i += dir) {
        // 空き/壁があるとき返せない
        if ((board->squares[i] == EMPTY) || (board->squares[i] == WALL)) {
            return 0;
        }

        count++;
    }

    return count;
}

int count_flip_disks(Board *board, Disk disk, Pos pos) {
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

int count_disks(Board *board, Disk disk) {
    int count = 0;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (board->squares[i] == disk) {
            count++;
        }
    }

    return count;
}

static int flip_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int count = 0;
    int n;

    // 同色石まで探索
    for (n = pos + dir; board->squares[n] != disk ; n += dir) {
        // 空き/壁があるとき返せない
        if ((board->squares[n] == EMPTY) || (board->squares[n] == WALL)) {
            return 0;
        }

        count++;
    }

    n -= dir;

    // 石を返す
    while (n != (int)pos) {
        board->squares[n] = disk;
        push_stack(board, n);
        n -= dir;
    }

    return count;
}

int put_and_flip(Board *board, Disk disk, Pos pos) {
    int count = 0;
    
    count += flip_line(board, disk, pos, UPPER);
    count += flip_line(board, disk, pos, UPPER_RIGHT);
    count += flip_line(board, disk, pos, UPPER_LEFT);
    count += flip_line(board, disk, pos, RIGHT);
    count += flip_line(board, disk, pos, LEFT);
    count += flip_line(board, disk, pos, LOWER);
    count += flip_line(board, disk, pos, LOWER_RIGHT);
    count += flip_line(board, disk, pos, LOWER_LEFT);

    board->squares[pos] = disk;

    push_stack(board, count);
    push_stack(board, pos);

    change_turn(board, 1);

    return count;
}

void undo(Board *board) {
    board->squares[pop_stack(board)] = EMPTY;

    int n = pop_stack(board);

    for (int i = 0; i < n; i++) {
        board->squares[pop_stack(board)] *= -1;
    }

    change_turn(board, -1);
}

void print_board(Board *board) {
    printf("    a b c d e f g h \n");
    printf("  +-----------------+\n");

    for (int y = 1; y <= BOARD_SIZE; y++) {
        printf("%d | ", y);

        for (int x = 1; x <= BOARD_SIZE; x++) {
            switch (board->squares[TO_POS(x, y)]) {
                case WHITE:
                    printf(WHITE_STR);
                    break;
                case BLACK:
                    printf(BLACK_STR);
                    break;
                default:
                    if (is_valid(board, board->current_turn, TO_POS(x, y))) {
                        printf(VALID_STR);
                    }
                    else {
                        printf(EMPTY_STR);
                    }
                    break;
            }
        }

        printf("|\n");
    }
    printf("  +-----------------+\n");

    printf("@:%2d O:%2d\n", count_disks(board, BLACK), count_disks(board, WHITE));
}
