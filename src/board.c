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

// スタック長
// 返せる最大の石数：(BOARD_SIZE - 2) * 3
// 着手位置、相手の石色、返した石数の情報：3
#define STACK_SIZE (((BOARD_SIZE - 2) * 3 + 3) * BOARD_SIZE * BOARD_SIZE - 4)

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
    Disk disks[NUM_DISK];   ///< マス情報
    int  stack[STACK_SIZE];     ///< 返した石を記録するスタック
                                ///< （返した石の位置1）...（~N）、（着手位置）、（相手の石色）、（返した石数）
    int  *sp;                   ///< スタックポインタ
    int  disk_num[3];           ///< 石数（0: 黒 1: 白、2: 空）
    int  pattern[NUM_PATTERN];
    int  pattern_id[NUM_DISK][NUM_PATTERN_DIFF];
    int  pattern_diff[NUM_DISK][NUM_PATTERN_DIFF];
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

static int flip_line(Board *board, Disk color, Pos pos, Dir dir);

static void add_pattern(Board *board, int id, const Pos *pos_list, int num);
static void init_pattern_diff(Board *board);

static void flip_square_black(Board *board, Pos pos);
static void flip_square_white(Board *board, Pos pos);
static void put_square_black(Board *board, Pos pos);
static void put_square_white(Board *board, Pos pos);

static int flip_line_pattern(Board *board, Disk disk, Pos pos, Dir dir);

static int count_flips_line(const Board *board, Disk disk, Pos pos, Dir dir);

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

    Board_init_pattern(board);
}

Disk Board_disk(const Board *board, Pos pos)
{
    return board->disks[pos];
}

int Board_count_disks(const Board *board, Disk color)
{
    return board->disk_num[color];
}

static int flip_line(Board *board, Disk color, Pos pos, Dir dir)
{
    Pos  cur_pos;
    Disk op = OPPONENT(color);

    // 相手の石を探索
    for (cur_pos = (pos + dir); board->disks[cur_pos] == op; cur_pos += dir);

    // 終端が自分の石色であれば着手位置までたどりながら石を返す
    int count = 0;
    if (board->disks[cur_pos] == color) {
        for (cur_pos -= dir; board->disks[cur_pos] == op; cur_pos -= dir) {
            board->disks[cur_pos] = color;
            STACK_PUSH(board, cur_pos);
            count++;
        }
    }

    return count;
}

int Board_flip(Board *board, Disk color, Pos pos)
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
        STACK_PUSH(board, pos);
        STACK_PUSH(board, OPPONENT(color));
        STACK_PUSH(board, count);

        board->disk_num[color] += (count + 1);
        board->disk_num[OPPONENT(color)] -= count;
        board->disk_num[EMPTY]--;
    }

    return count;
}

int Board_unflip(Board *board)
{
    if (board->sp <= board->stack) {
        return 0;
    }

    int  count = STACK_POP(board);
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

void Board_init_pattern(Board *board)
{
    for (int i = 0; i < NUM_PATTERN; i++) {
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

static void add_pattern(Board *board, int id, const Pos *pos_list, int num)
{
    int i, j;
    int n = 1;

    for (i = 0; i < num; i++) {
        for (j = 0; board->pattern_diff[pos_list[i]][j] != 0; j++);

        board->pattern_id[pos_list[i]][j] = id;
        board->pattern_diff[pos_list[i]][j] = n;

        n *= 3;
    }
}

static void init_pattern_diff(Board *board)
{
    int i, j;
    Pos pattern_list[][9] = {
        { A4, B4, C4, D4, E4, F4, G4, H4, NONE },
        { A5, B5, C5, D5, E5, F5, G5, H5, NONE },
        { D1, D2, D3, D4, D5, D6, D7, D8, NONE },
        { E1, E2, E3, E4, E5, E6, E7, E8, NONE },
        { A3, B3, C3, D3, E3, F3, G3, H3, NONE },
        { A6, B6, C6, D6, E6, F6, G6, H6, NONE },
        { C1, C2, C3, C4, C5, C6, C7, C8, NONE },
        { F1, F2, F3, F4, F5, F6, F7, F8, NONE },
        { A2, B2, C2, D2, E2, F2, G2, H2, NONE },
        { A7, B7, C7, D7, E7, F7, G7, H7, NONE },
        { B1, B2, B3, B4, B5, B6, B7, B8, NONE },
        { G1, G2, G3, G4, G5, G6, G7, G8, NONE },
        { A1, B2, C3, D4, E5, F6, G7, H8, NONE },
        { A8, B7, C6, D5, E4, F3, G2, H1, NONE },
        { A2, B3, C4, D5, E6, F7, G8, NONE },
        { B1, C2, D3, E4, F5, G6, H7, NONE },
        { A7, B6, C5, D4, E3, F2, G1, NONE },
        { B8, C7, D6, E5, F4, G3, H2, NONE },
        { A3, B4, C5, D6, E7, F8, NONE },
        { C1, D2, E3, F4, G5, H6, NONE },
        { A6, B5, C4, D3, E2, F1, NONE },
        { C8, D7, E6, F5, G4, H3, NONE },
        { A4, B5, C6, D7, E8, NONE },
        { D1, E2, F3, G4, H5, NONE },
        { A5, B4, C3, D2, E1, NONE },
        { D8, E7, F6, G5, H4, NONE },
        { A5, B6, C7, D8, NONE },
        { E1, F2, G3, H4, NONE },
        { A4, B3, C2, D1, NONE },
        { E8, F7, G6, H5, NONE },
        { B2, G1, F1, E1, D1, C1, B1, A1, NONE },
        { G2, B1, C1, D1, E1, F1, G1, H1, NONE },
        { B7, G8, F8, E8, D8, C8, B8, A8, NONE },
        { G7, B8, C8, D8, E8, F8, G8, H8, NONE },
        { B2, A7, A6, A5, A4, A3, A2, A1, NONE },
        { B7, A2, A3, A4, A5, A6, A7, A8, NONE },
        { G2, H7, H6, H5, H4, H3, H2, H1, NONE },
        { G7, H2, H3, H4, H5, H6, H7, H8, NONE },
        { B3, A3, C2, B2, A2, C1, B1, A1, NONE },
        { G3, H3, F2, G2, H2, F1, G1, H1, NONE },
        { B6, A6, C7, B7, A7, C8, B8, A8, NONE },
        { G6, H6, F7, G7, H7, F8, G8, H8, NONE },
        { NONE }
    };

    for (i = 0; i < NUM_DISK; i++) {
        for (j = 0; j < NUM_PATTERN_DIFF; j++) {
            board->pattern_id[i][j] = 0;
            board->pattern_diff[i][j] = 0;
        }
    }
    for (i = 0; pattern_list[i][0] != NONE; i++) {
        for (j = 0; pattern_list[i][j] != NONE; j++);
        add_pattern(board, i, pattern_list[i], j);
    }
}

static void flip_square_black(Board *board, Pos pos)
{
    board->disks[pos] = BLACK;
    board->pattern[board->pattern_id[pos][0]] -= board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] -= board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] -= board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] -= board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] -= board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] -= board->pattern_diff[pos][5];
}

static void flip_square_white(Board *board, Pos pos)
{
    board->disks[pos] = WHITE;
    board->pattern[board->pattern_id[pos][0]] += board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] += board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] += board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] += board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] += board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] += board->pattern_diff[pos][5];
}

static void put_square_black(Board *board, Pos pos)
{
    board->disks[pos] = BLACK;
    board->pattern[board->pattern_id[pos][0]] += board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] += board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] += board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] += board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] += board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] += board->pattern_diff[pos][5];
}

static void put_square_white(Board *board, Pos pos)
{
    board->disks[pos] = WHITE;
    board->pattern[board->pattern_id[pos][0]] += (board->pattern_diff[pos][0] + board->pattern_diff[pos][0]);
    board->pattern[board->pattern_id[pos][1]] += (board->pattern_diff[pos][1] + board->pattern_diff[pos][1]);
    board->pattern[board->pattern_id[pos][2]] += (board->pattern_diff[pos][2] + board->pattern_diff[pos][2]);
    board->pattern[board->pattern_id[pos][3]] += (board->pattern_diff[pos][3] + board->pattern_diff[pos][3]);
    board->pattern[board->pattern_id[pos][4]] += (board->pattern_diff[pos][4] + board->pattern_diff[pos][4]);
    board->pattern[board->pattern_id[pos][5]] += (board->pattern_diff[pos][5] + board->pattern_diff[pos][5]);
}

static void remove_square_black(Board *board, Pos pos)
{
    board->disks[pos] = EMPTY;
    board->pattern[board->pattern_id[pos][0]] -= board->pattern_diff[pos][0];
    board->pattern[board->pattern_id[pos][1]] -= board->pattern_diff[pos][1];
    board->pattern[board->pattern_id[pos][2]] -= board->pattern_diff[pos][2];
    board->pattern[board->pattern_id[pos][3]] -= board->pattern_diff[pos][3];
    board->pattern[board->pattern_id[pos][4]] -= board->pattern_diff[pos][4];
    board->pattern[board->pattern_id[pos][5]] -= board->pattern_diff[pos][5];
}

static void remove_square_white(Board *board, Pos pos)
{
    board->disks[pos] = EMPTY;
    board->pattern[board->pattern_id[pos][0]] -= (board->pattern_diff[pos][0] + board->pattern_diff[pos][0]);
    board->pattern[board->pattern_id[pos][1]] -= (board->pattern_diff[pos][1] + board->pattern_diff[pos][1]);
    board->pattern[board->pattern_id[pos][2]] -= (board->pattern_diff[pos][2] + board->pattern_diff[pos][2]);
    board->pattern[board->pattern_id[pos][3]] -= (board->pattern_diff[pos][3] + board->pattern_diff[pos][3]);
    board->pattern[board->pattern_id[pos][4]] -= (board->pattern_diff[pos][4] + board->pattern_diff[pos][4]);
    board->pattern[board->pattern_id[pos][5]] -= (board->pattern_diff[pos][5] + board->pattern_diff[pos][5]);
}

static int flip_line_pattern(Board *board, Disk disk, Pos pos, Dir dir)
{
    Pos cur_pos;
    Disk op = OPPONENT(disk);
    void (*func_flip)(Board *, Pos);

    if (disk == BLACK) {
        func_flip = flip_square_black;
    } else {
        func_flip = flip_square_white;
    }

    for (cur_pos = (pos + dir); board->disks[cur_pos] == op; cur_pos += dir);

    int count = 0;
    if (board->disks[cur_pos] == disk) {
        for (cur_pos -= dir; board->disks[cur_pos] == op; cur_pos -= dir) {
            func_flip(board, pos);
            STACK_PUSH(board, cur_pos);
            count++;
        }
    }

    return count;
}

int Board_flip_pattern(Board *board, Disk color, Pos pos)
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
        STACK_PUSH(board, OPPONENT(color));
        STACK_PUSH(board, count);

        board->disk_num[color] += (count + 1);
        board->disk_num[OPPONENT(color)] -= count;
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
    Disk color = STACK_POP(board);

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
    board->disk_num[OPPONENT(color)] -= (count + 1);
    board->disk_num[EMPTY]++;

    return count;
}

static int count_flips_line(const Board *board, Disk disk, Pos pos, Dir dir)
{
    int cur_pos;
    Disk op = OPPONENT(disk);

    // 相手の石を探索
    int count = 0;
    for (cur_pos = pos + dir; board->disks[cur_pos] == op; cur_pos += dir) {
        count++;
    }
    // 終端が自色石でないときカウントしない
    if (board->disks[cur_pos] != disk) {
        return 0;
    }

    return count;
}

int Board_count_flips(const Board *board, Disk disk, Pos pos)
{
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

bool Board_can_flip(const Board *board, Disk disk, Pos pos)
{
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

void Board_copy(const Board *src, Board *dst)
{
    *dst = *src;
    dst->sp = dst->stack + (src->sp - src->stack);
}

void Board_reverse(Board *board)
{
    for (Pos pos = 0; pos < NUM_DISK; pos++) {
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

    Board_init_pattern(board);
}

bool Board_can_play(const Board *board, Disk disk)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Board_can_flip(board, disk, XY2POS(x, y))) {
                return true;
            }
        }
    }

    return false;
}

Pos Board_pos(int x, int y)
{
    return (((y) + 1) * (BOARD_SIZE + 1) + ((x) + 1));
}

int Board_x(Pos pos)
{
    return (pos % (BOARD_SIZE + 1) - 1);
}

int Board_y(Pos pos)
{
    return (pos / (BOARD_SIZE + 1) - 1);
}

int Board_opponent(Disk color)
{
    return  (1 ^ color);
}
