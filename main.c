#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"

Stone player[] = {
    WHITE, BLACK};
int turn;

bool is_finished(int*);
void do_turn(Stone);
void parse_command(int *, int *);
void decide_winner();

int main()
{
    int count = 0;
    int prev_count = 1;

    turn = 1;

    init();
    draw_board();

    while (1)
    {
        count = search_all(player[turn % 2]);
        if (count == 0)
        {
            if (prev_count == 0)
            {
                printf("finished\n");
                break;
            }
            printf("pass\n");
            prev_count = count;
            continue;
        }

        do_turn(player[turn % 2]);
        draw_board();
        turn++;
    }

    decide_winner();

    return 0;
}

bool is_finished(int* prev_count)
{
    int count = search_all(player[turn % 2]);
    if (count == 0)
    {
        if (*prev_count == 0)
        {
            printf("finished\n");
            return true;
        }
        else
        {
            printf("pass\n");
            *prev_count = count;
        }
    }

    return false;
}

void do_turn(Stone stone)
{
    char *str_turn = (stone == BLACK) ? "Black" : "White";

    int x, y;

    while (1)
    {
        printf("[%d] %s: ", turn, str_turn);
        parse_command(&x, &y);

        if (in_board(x, y) || is_none(x, y))
        {
            if (put_stone(x, y, stone))
            {
                break;
            }
        }
    }
}

void parse_command(int *x, int *y)
{
    char command[4];
    fgets(command, 4, stdin);

    if (!isalpha(command[0]) && !isdigit(command[1]))
        return;

    *x = toupper(command[0]) - '@';
    *y = command[1] - '0';
}

void decide_winner()
{
    int num_black = count_stones(BLACK);
    int num_white = count_stones(WHITE);

    printf("***\n");
    printf("B : W = %d : %d\n", num_black, num_white);

    if (num_black > num_white)
    {
        printf("* Black Wins *\n");
    }
    else if (num_black < num_white)
    {
        printf("* White Wins *\n");
    }
    else
    {
        printf("* Draw *\n");
    }
}