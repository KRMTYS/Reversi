#include <stdio.h>

#include "board.h"

// 盤面
// 8x8マス+周囲の番兵
Disk board[100];

// 周囲8方向へのインデックス差分
int dir[] = {
    -10, // 上
     -9, // 右上
      1, // 右
     11, // 右下
     10, // 下
      9, // 左下
     -1, // 左
    -11  // 左上
};

void init_board()
{
    for (int i = 0; i < 100; i++)
    {
        board[i] = BLANK;
    }

    // 石の初期配置
    board[44] = WHITE;
    board[45] = BLACK;
    board[54] = BLACK;
    board[55] = WHITE;
}

bool is_on_board(int x, int y)
{
    return ((x >= 1) && (x <= 8) && (y >= 1) && (y <= 8)) ? true : false;
}

bool is_blank(int x, int y)
{
    return (board[y * 10 + x] == BLANK) ? true : false;
}

bool is_valid_move(int x, int y, Disk disk)
{
    if (!is_on_board(x, y) || !is_blank(x, y))
    {
        return false;
    }

    for (int i = 0; i < 8; i++)
    {
        if (count_straight_reversal_disks(x, y, dir[i], disk) > 0)
        {
            return true;
        }
    }

    return false;
}

int count_valid_moves(Disk disk)
{
    int valid_moves = 0;

    for (int y = 1; y <= 8; y++)
    {
        for (int x = 1; x <= 8; x++)
        {
            if (is_valid_move(x, y, disk))
            {
                valid_moves++;
            }
        }
    }

    return valid_moves;
}

int count_all_reversal_disks(Disk disk)
{
    int count = 0;

    for (int y = 1; y <= 8; y++)
    {
        for (int x = 1; x <= 8; x++)
        {
            if (board[y * 10 + x] == BLANK)
            {
                count += count_reversal_disks(x, y, disk);
            }
        }
    }

    return count;
}

int count_reversal_disks(int x, int y, Disk disk)
{
    int count = 0;

    for (int i = 0; i < 8; i++)
    {
        count += count_straight_reversal_disks(x, y, dir[i], disk);
    }

    return count;
}

int count_straight_reversal_disks(int x, int y, int dir, Disk disk)
{
    int count = 0;

    for (int i = y * 10 + x + dir; ; i += dir)
    {
        // 次の石
        Disk next = board[i];

        // 空き/盤面端のとき返せる石はない
        if (next == BLANK)
        {
            count = 0;
            break;
        }
        // 同色の石であるときカウント終了
        else if (next == disk)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    return count;
}

int count_disks(Disk disk)
{
    int count = 0;

    for (int y = 1; y <= 8; y++)
    {
        for (int x = 1; x <= 8; x++)
        {
            if (board[y * 10 + x] == disk)
            {
                count++;
            }
        }
    }

    return count;
}

void put_and_flip(int x, int y, Disk disk)
{
    for (int i = 0; i < 8; i++)
    {
        int count = count_straight_reversal_disks(x, y, dir[i], disk);

        for (int j = 1; j <= count; j++)
        {
            board[y * 10 + x + j * dir[i]] = disk;
        }
    }

    board[y * 10 + x] = disk;
}

void print_board()
{
    printf("    a b c d e f g h \n");
    printf("  +-----------------+\n");

    for (int y = 1; y <= 8; y++)
    {
        printf("%d | ", y);

        for (int x = 1; x <= 8; x++)
        {
            switch (board[y * 10 + x])
            {
                case WHITE:
                    printf("O ");
                    break;
                case BLACK:
                    printf("@ ");
                    break;
                default:
                    printf("- ");
                    break;
            }
        }

        printf("|\n");
    }
    printf("  +-----------------+\n");
}