#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"

bool play_turn(int, int*);
void player_input(int);
void program_input(int);
void judge();

int main()
{
    int prev_count = 1;
    int turn = 1;

    srand((unsigned)time(NULL));

    printf("You are Black\n");
    init();

    while (play_turn(turn, &prev_count))
    {
        print_board();
        turn++;
    }

    judge();

    return 0;
}

bool play_turn(int turn,int* prev_count)
{
    Disk disk = (turn % 2 == 1) ? BLACK : WHITE;

    int count = count_all_disks(disk);

    if (count == 0)
    {
        // finish a game if both of players couldn't put disk
        if (count == *prev_count) return false;
        else printf("pass\n");
    }
    else
    {
       if (turn % 2 == 1) player_input(turn);
       else  program_input(turn);
    }

    *prev_count = count;

    printf("\n");

    return true;
}

void player_input(int turn)
{
    int x, y;
    char input[4];

    do
    {
        do
        {
            printf("[%d] Black: ", turn);
            fgets(input, 4, stdin);
            
            x = toupper(input[0]) - '@';
            y = input[1] - '0';
        }
        while (!in_board(x, y) || !is_none(x, y));
    }
    while (put_disk(x, y, BLACK) == 0);
}

void program_input(int turn)
{
    printf("[%d] White: ", turn);

    int x, y;

    do
    {
        do
        {
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        }
        while (!in_board(x, y) || !is_none(x, y));
    }
    while (put_disk(x, y, WHITE) == 0);

    printf("%c%c\n", x + '@', y + '0');
}

// count disks and judge
void judge()
{
    int num_black = count_disks(BLACK);
    int num_white = count_disks(WHITE);

    printf("\n\n*********\n");
    printf("Black: %2d\n", num_black);
    printf("White: %2d\n", num_white);
    printf("*********\n");

    if (num_black > num_white)
    {
        printf("* You Win *\n");
    }
    else if (num_black < num_white)
    {
        printf("* You Lose *\n");
    }
    else
    {
        printf("* Draw *\n");
    }
}