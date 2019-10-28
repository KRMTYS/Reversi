#include "board.h"

#include <stdio.h>

// スタック操作
static void push(int n, Board *board) {
    *(board->sp++) = n;
}

static int pop(Board *board) {
    return *(--board->sp);
}

static bool is_on_board(int x, int y) {
    return ((x >= 1) && (x <= SQUARE_SIZE) && (y >= 1) && (y <= SQUARE_SIZE)) ? true : false;
}

static bool is_empty(int x, int y, Board *board) {
    return (board->square[TO_POS(x, y)] == EMPTY) ? true : false;
}

void init_board(Board *board) {
    for (int i = 0; i < SQUARE_LENGTH; i++) {
        if (is_on_board(TO_X(i), TO_Y(i))) {
            board->square[i] = EMPTY;
        } else {
            board->square[i] = WALL;
        }
    }

    // 石の初期配置
    board->square[D4] = WHITE;
    board->square[D5] = BLACK;
    board->square[E4] = BLACK;
    board->square[E5] = WHITE;

    // 黒先攻
    board->current_turn = BLACK;

    board->turn_num = 1;

    // スタックの初期化
    for (int i = 0; i < STACK_LENGTH; i++) {
        board->stack[i] = 0;
    }

    board->sp = board->stack;
}

bool is_valid(Board *board, Disk disk, int x, int y) {
    if (!is_on_board(x, y) || !is_empty(x, y, board)) {
        return false;
    }

    if (count_flip_disks(board, disk, TO_POS(x, y)) > 0) {
        return true;
    }

    return false;
}

bool has_valid_move(Board *board, Disk disk) {
    return (count_valid_moves(board, disk) > 0) ? true : false;
}

int count_valid_moves(Board *board, Disk disk) {
    int count = 0;

    for (int i = 0; i < SQUARE_LENGTH; i++) {
        if (is_valid(board, disk, TO_X(i), TO_Y(i))) {
            count++;
        }
    }

    return count;
}

void change_turn(Board *board, int n) {
    board->current_turn = OPPONENT(board->current_turn);
    board->turn_num += n;
}

State get_state(Board *board) {
    Disk current = board->current_turn;

    if (has_valid_move(board, current)) {
        return DO_TURN;
    } else if (has_valid_move(board, OPPONENT(current))) {
        change_turn(board, 0);
        return PASS;
    }

    return FINISH;
}

static int count_flip_disks_line(Board *board, Disk disk, Pos pos, Dir dir) {
    int count = 0;

    // 同色石まで探索
    for (int i = pos + dir; board->square[i] != disk ; i += dir) {
        // 空き/壁があるとき返せない
        if ((board->square[i] == EMPTY) || (board->square[i] == WALL)) {
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

    for (int i = 0; i < SQUARE_LENGTH; i++) {
        if (board->square[i] == disk) {
            count++;
        }
    }

    return count;
}

static int flip_line(Pos pos, Disk disk, Dir dir, Board *board) {
    int count = 0;
    int n;

    // 同色石まで探索
    for (n = pos + dir; board->square[n] != disk ; n += dir) {
        // 空き/壁があるとき返せない
        if ((board->square[n] == EMPTY) || (board->square[n] == WALL)) {
            return 0;
        }

        count++;
    }

    n -= dir;

    // 石を返す
    while (n != (int)pos) {
        board->square[n] = disk;
        push(n, board);
        n -= dir;
    }

    return count;
}

int put_and_flip(Board *board, Disk disk, Pos pos) {
    int count = 0;
    
    count += flip_line(pos, disk, UPPER, board);
    count += flip_line(pos, disk, UPPER_RIGHT, board);
    count += flip_line(pos, disk, UPPER_LEFT, board);
    count += flip_line(pos, disk, RIGHT, board);
    count += flip_line(pos, disk, LEFT, board);
    count += flip_line(pos, disk, LOWER, board);
    count += flip_line(pos, disk, LOWER_RIGHT, board);
    count += flip_line(pos, disk, LOWER_LEFT, board);

    board->square[pos] = disk;

    push(count, board);
    push(pos, board);

    change_turn(board, 1);

    return count;
}

void undo(Board *board) {
    board->square[pop(board)] = EMPTY;

    int n = pop(board);

    for (int i = 0; i < n; i++) {
        board->square[pop(board)] *= -1;
    }

    change_turn(board, -1);
}

void print_board(Board *board) {
    printf("    a b c d e f g h \n");
    printf("  +-----------------+\n");

    for (int y = 1; y <= SQUARE_SIZE; y++) {
        printf("%d | ", y);

        for (int x = 1; x <= SQUARE_SIZE; x++) {
            switch (board->square[TO_POS(x, y)]) {
                case WHITE:
                    printf(WHITE_STR);
                    break;
                case BLACK:
                    printf(BLACK_STR);
                    break;
                default:
                    if (is_valid(board, board->current_turn, x, y)) {
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
