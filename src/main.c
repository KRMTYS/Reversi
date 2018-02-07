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
    // 前ターンで置ける石数
    int prev_count = 0;
    // ターン数
    int turn = 1;

    srand((unsigned)time(NULL));

    // プレイヤーは先攻黒番
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

// ターン行動
bool play_turn(int turn,int* prev_count)
{
    Disk disk = (turn % 2 == 1) ? BLACK : WHITE;

    int count = count_all_disks(disk);

    // 返せる石がない
    if (count == 0)
    {
        // 両者返せなくなったときゲーム終了
        if (*prev_count == 0) return false;
        else printf("pass\n");
    }
    else
    {
        // 座標入力
        if (turn % 2 == 1) player_input(turn);
        else  program_input(turn);
    }

    *prev_count = count;

    printf("\n");

    return true;
}

// プレイヤー側入力
void player_input(int turn)
{
    char input[4];
    int x, y;

    do
    {
        do
        {
            printf("[%d] Black: ", turn);
            fgets(input, 4, stdin);

            // 入力した文字型変数を1~8の整数値に変換する
            x = tolower(input[0]) - '`';
            y = input[1] - '0';
        }
        // 石を置けるか
        while (!in_board(x, y) || !is_none(x, y));
    }
    // 石を返せるか
    while (put_disk(x, y, BLACK) == 0);
}

// プログラム側入力
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
        // 石を置けるか
        while (!in_board(x, y) || !is_none(x, y));
    }
    // 石を返せるか
    while (put_disk(x, y, WHITE) == 0);

    // プレイヤーと同様に入力座標を表示
    printf("%c%c\n", x + '`', y + '0');
}

// 石の集計
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