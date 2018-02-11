#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"

bool play_turn(int, int*);
void input(Disk, bool);
void judge();

int main()
{
    // 前ターンで置ける石数
    int prev_count = 0;
    // ターン数
    int turn = 1;

    srand((unsigned)time(NULL));

    init_board();
    print_board();

    while (play_turn(turn, &prev_count))
    {
        print_board();
        turn++;
    }

    judge();

    return 0;
}

// ターン行動
bool play_turn(int turn, int* prev_count)
{
    // 石色
    Disk disk = (turn % 2 == 1) ? BLACK : WHITE;
    char* disk_str = (disk == BLACK) ? "BLACK" : "WHITE";

    int count = count_all_reversal_disks(disk);
    // 返せる石がない
    if (count == 0)
    {
        // 両者返せなくなったときゲーム終了
        if (*prev_count == 0)
        {
            return false;
        }
        else
        {
            // パス
            printf("[%d] %s: pass\n", turn, disk_str);
        }
    }
    else
    {
        // 入力
        printf("[%d] %s: ", turn, disk_str);

        // 黒番がプレイヤー
        input(disk, (disk == BLACK));
    }

    *prev_count = count;

    printf("\n");

    return true;
}

// 入力
void input(Disk disk, bool is_player)
{
    char input[4];
    int x, y;

    // プレイヤー/プログラムで動作切替
    if (is_player)
    {
        do
        {
            // 入力した文字型変数を1~8の整数値に変換する
            fgets(input, 4, stdin);
            x = tolower(input[0]) - '`';
            y = input[1] - '0';
        }
        // 石の設置判定
        while (!can_put_disk(x, y, disk));
    }
    else
    {
        do
        {
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        }
        while (!can_put_disk(x, y, disk));

        // プレイヤーと同様に入力座標を表示
        printf("%c%c\n", x + '`', y + '0');
    }

    put_disk(x, y, disk);
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
        printf("* BLACK Wins *\n");
    }
    else if (num_black < num_white)
    {
        printf("* WHITE Wins *\n");
    }
    else
    {
        printf("* Draw *\n");
    }
}