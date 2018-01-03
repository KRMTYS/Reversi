#include <stdio.h>

#include "board.h"

Stone board[100] = {};

int directions[] = {
    -10, -9, 1, 11, 10, 9, -1, -11
};

void init();
bool in_board(int, int);
bool is_none(int, int);
void draw_board();

void init()
{
    board[44] = WHITE;
    board[45] = BLACK;
    board[54] = BLACK;
    board[55] = WHITE;
}

bool in_board(int x, int y)
{
    return ((x >= 1) && (x <= 8) && (y >= 1) && (y <= 8)) ? true : false;
}

bool is_none(int x, int y)
{
    return (board[y * 10 + x] == NONE) ? true : false;
}

int search_all(Stone stone)
{
    int count = 0;

    for (int y = 1; y <= 8; y++)
    {
        for (int x = 1; x <= 8; x++)
        {
            if (board[y * 10 + x] == NONE)
            {
                count += search_8dir(x, y, stone, false);
            }
        }
    }

    return count;
}

int search_8dir(int x, int y, Stone stone, bool flip)
{
    int count = 0;

    for (int i = 0; i < 8; i++)
    {
        count += search_dir(x, y, directions[i], stone, flip);
    }

    return count;
}

int search_dir(int x, int y, int dir, Stone stone, bool flip)
{
    int indices[8] = {0};
    int count = 0;
    int nest = 1;

    while (1)
    {
        int next_index = y * 10 + x + dir * nest;
        Stone next = board[next_index];

        if (next == NONE)
        {
            count = 0;
            break;
        }
        else
        {
            if (next == stone)
            {
                break;
            }
            else
            {
                indices[count] = next_index;
                count++;
                nest++;
            }
        }
    }

    if ((count > 0) && flip)
    {
        for (int i = 0; i < count; i++)
        {
            board[indices[i]] = stone;
        }
    }

    return count;
}

int put_stone(int x, int y, Stone stone)
{
    int count = search_8dir(x, y, stone, true);

    if (count)
        board[y * 10 + x] = stone;

    return count;
}

int count_stones(Stone stone)
{
    int count = 0;

    for (int i = 0; i < 100; i++)
    {
        if (board[i] == stone)
            count++;
    }

    return count;
}

void draw_board()
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