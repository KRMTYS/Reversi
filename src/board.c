#include <stdio.h>

#include "board.h"

void init_board(Board* board)
{
    for (int i = 0; i < SQUARE_LENGTH; i++)
    {
        if (is_on_board(TO_X(i), TO_Y(i)))
        {
            board->square[i] = EMPTY;
        }
        else
        {
            board->square[i] = WALL;
        }
    }

    // 石の初期配置
    board->square[D4] = WHITE;
    board->square[D5] = BLACK;
    board->square[E4] = BLACK;
    board->square[E5] = WHITE;

    board->current_turn = BLACK;

    // スタックの初期化
    for (int i = 0; i < STACK_LENGTH; i++)
    {
        board->stack[i] = 0;
    }

    board->sp = board->stack;
}

bool is_on_board(int x, int y)
{
    return ((x >= 1) && (x <= SQUARE_SIZE) && (y >= 1) && (y <= SQUARE_SIZE)) ? true : false;
}

bool is_empty(int x, int y, Board* board)
{
    return (board->square[TO_POS(x, y)] == EMPTY) ? true : false;
}

bool is_valid(int x, int y, Disk disk, Board* board)
{
    if (!is_on_board(x, y) || !is_empty(x, y, board))
    {
        return false;
    }

    if (count_flip_disks(TO_POS(x, y), disk, board) > 0)
    {
        return true;
    }
    
    return false;
}

bool has_valid_move(Disk disk, Board* board)
{
    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        for(int x = 1; x <= SQUARE_SIZE; x++)
        {
            Pos pos = TO_POS(x, y);

            if (board->square[pos] == EMPTY)
            {
                // 返せる石が一つでもあれば有効手をもつ
                if (count_flip_disks(pos, disk, board) > 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

State get_state(Board* board)
{
    Disk current = board->current_turn;

    if (has_valid_move(current, board))
    {
        return DO_TURN;
    }
    else if (has_valid_move(OPPONENT(current), board))
    {
        // 手番変更
        board->current_turn = OPPONENT(board->current_turn);
        return PASS;
    }

    return FINISH;
}

int count_flip_disks_line(Pos pos, Disk disk, Dir dir, Board* board)
{
    int count = 0;

    // 同色石まで探索
    for (int i = pos + dir; board->square[i] != disk ; i += dir)
    {
        if (board->square[i] == EMPTY || board->square[i] == WALL)
        {
            return 0;
        }

        count++;
    }

    return count;
}

int count_flip_disks(Pos pos, Disk disk, Board* board)
{
    int count = 0;
    
    count += count_flip_disks_line(pos, disk, UPPER, board);
    count += count_flip_disks_line(pos, disk, UPPER_RIGHT, board);
    count += count_flip_disks_line(pos, disk, UPPER_LEFT, board);
    count += count_flip_disks_line(pos, disk, RIGHT, board);
    count += count_flip_disks_line(pos, disk, LEFT, board);
    count += count_flip_disks_line(pos, disk, LOWER, board);
    count += count_flip_disks_line(pos, disk, LOWER_RIGHT, board);
    count += count_flip_disks_line(pos, disk, LOWER_LEFT, board);

    return count;
}

int count_disks(Disk disk, Board* board)
{
    int count = 0;

    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        for (int x = 1; x <= SQUARE_SIZE; x++)
        {
            if (board->square[TO_POS(x, y)] == disk)
            {
                count++;
            }
        }
    }

    return count;
}

int flip_line(Pos pos, Disk disk, Dir dir, Board* board)
{
    int count = 0;
    int n;

    // 同色石まで探索
    for (n = pos + dir; board->square[n] != disk ; n += dir)
    {
        // 空き/壁のとき返す石なし
        if (board->square[n] == EMPTY || board->square[n] == WALL)
        {
            return 0;
        }

        count++;
    }

    n -= dir;

    // 石を返す
    while (n != pos)
    {
        board->square[n] = disk;
        push(n, board);
        n -= dir;
    }

    return count;
}

int put_and_flip(Pos pos, Disk disk, Board* board)
{
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

    board->current_turn = OPPONENT(board->current_turn);

    return count;
}

void undo(Board* board)
{
    board->square[pop(board)] = EMPTY;

    int n = pop(board);

    for (int i = 0; i < n; i++)
    {
        board->square[pop(board)] *= -1;
    }

    board->current_turn = OPPONENT(board->current_turn);
}

void print_board(Board* board)
{
    printf("    a b c d e f g h \n");
    printf("  +-----------------+\n");

    for (int y = 1; y <= SQUARE_SIZE; y++)
    {
        printf("%d | ", y);

        for (int x = 1; x <= SQUARE_SIZE; x++)
        {
            switch (board->square[TO_POS(x, y)])
            {
                case WHITE:
                    printf(WHITE_STR);
                    break;
                case BLACK:
                    printf(BLACK_STR);
                    break;
                default:
                    if (is_valid(x, y, board->current_turn, board))
                    {
                        printf(VALID_STR);
                    }
                    else
                    {
                        printf(EMPTY_STR);
                    }
                    break;
            }
        }

        printf("|\n");
    }
    printf("  +-----------------+\n");
}

Disk judge(Board* board)
{
    int num_black = count_disks(BLACK, board);
    int num_white = count_disks(WHITE, board);

    if (num_black > num_white)
    {
        return BLACK;
    }
    else if (num_black < num_white)
    {
        return WHITE;
    }
    else
    {
        return EMPTY;
    }
}

void push(int n, Board* board)
{
    *(board->sp++) = n;
}

int pop(Board* board)
{
    return *(--board->sp);
}