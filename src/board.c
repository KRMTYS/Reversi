#include <stdio.h>

#include "board.h"

Disk board[100] = {};

int directions[] = {
    -10, // upper
    -9,  // upper right
    1,   // right
    11,  // lower right
    10,  // lower
    9,   // lower left
    -1,  // left
    -11  // upper left
};

void init()
{
    // starting position
    board[44] = WHITE;
    board[45] = BLACK;
    board[54] = BLACK;
    board[55] = WHITE;

    print_board();
}

bool in_board(int x, int y)
{
    return ((x >= 1) && (x <= 8) && (y >= 1) && (y <= 8)) ? true : false;
}

bool is_none(int x, int y)
{
    return (board[y * 10 + x] == NONE) ? true : false;
}

// count all disks
int count_all_disks(Disk disk)
{
    int count = 0;

    for (int y = 1; y <= 8; y++)
    {
        for (int x = 1; x <= 8; x++)
        {
            if (board[y * 10 + x] == NONE)
            {
                count += count_around_disks(x, y, disk, false);
            }
        }
    }

    return count;
}

// count disks for around 8 directions 
int count_around_disks(int x, int y, Disk disk, bool flip)
{
    int count = 0;

    for (int i = 0; i < 8; i++)
    {
        count += count_straight_disks(x, y, directions[i], disk, flip);
    }

    return count;
}

// count disks for one direction
int count_straight_disks(int x, int y, int dir, Disk disk, bool flip)
{
    int indices[8] = {0};
    int count = 0;
    int nest = 1;

    while (1)
    {
        int next_index = y * 10 + x + dir * nest;
        Disk next = board[next_index];

        if (next == NONE)
        {
            count = 0;
            break;
        }
        else
        {
            if (next == disk) break;
            else
            {
                indices[count] = next_index;
                count++;
                nest++;
            }
        }
    }

    // flip disks
    if ((count > 0) && flip)
    {
        for (int i = 0; i < count; i++)
        {
            board[indices[i]] = disk;
        }
    }

    return count;
}

int put_disk(int x, int y, Disk disk)
{
    int count = count_around_disks(x, y, disk, true);

    if (count) board[y * 10 + x] = disk;

    return count;
}

int count_disks(Disk disk)
{
    int count = 0;

    for (int i = 0; i < 100; i++)
    {
        if (board[i] == disk) count++;
    }

    return count;
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