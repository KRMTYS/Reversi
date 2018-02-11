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

bool in_board(int x, int y)
{
    return ((x >= 1) && (x <= 8) && (y >= 1) && (y <= 8)) ? true : false;
}

bool is_blank(int x, int y)
{
    return (board[y * 10 + x] == BLANK) ? true : false;
}

bool can_put_disk(int x, int y, Disk disk)
{
    if (!in_board(x, y) || !is_blank(x, y))
    {
        return false;
    }

    for (int i = 0; i < 8; i++)
    {
        if (count_linear_reversal_disks(x, y, dir[i], disk, false) > 0)
        {
            return true;
        }
    }

    return false;
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
                count += count_reversal_disks(x, y, disk, false);
            }
        }
    }

    return count;
}

int count_reversal_disks(int x, int y, Disk disk, bool flip)
{
    int count = 0;

    for (int i = 0; i < 8; i++)
    {
        count += count_linear_reversal_disks(x, y, dir[i], disk, flip);
    }

    return count;
}

int count_linear_reversal_disks(int x, int y, int dir, Disk disk, bool flip)
{
    // 返せる石の座標
    // 8つまで記憶
    int indices[8] = {0};
    int count = 0;
    // 探索深さ
    int nest = 1;

    while (true)
    {
        int next_index = y * 10 + x + dir * nest;
        Disk next = board[next_index];

        // 空き/盤面端に到達したとき返せない
        if (next == BLANK)
        {
            count = 0;
            break;
        }
        else
        {
            if (next == disk)
            {
                break;
            }
            // 相手の石があるとき座標をカウント
            else
            {
                indices[count] = next_index;
                count++;
                nest++;
            }
        }
    }

    // フラグ立つとき石を返す
    if ((count > 0) && flip)
    {
        for (int i = 0; i < count; i++)
        {
            board[indices[i]] = disk;
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

void put_disk(int x, int y, Disk disk)
{
    count_reversal_disks(x, y, disk, true);

    board[y * 10 + x] = disk;
}

void print_board()
{
    printf("  A B C D E F G H \n");

    for (int y = 1; y <= 8; y++)
    {
        printf("%d ", y);
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
        printf("\n");
    }
}