#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"

void input(Board*);

int main()
{
    // 盤面
    Board board;

    srand((unsigned)time(NULL));

    init_board(&board);

    while (true)
    {
        print_board(&board);

        State state = get_state(&board);

        if (state == DO_TURN)
        {
            input(&board);
        }
        else if (state == PASS)
        {
            printf("Pass\n");
        }
        else
        {
            break;
        }

    }

    judge(&board);

    return 0;
}

// 入力
void input(Board* board)
{
    char input[4];
    int x, y;

    if (board->current_turn == BLACK)
    {
        printf("Black >> ");
        do
        {
            // 入力した文字型変数を1~8の整数値に変換する
            fgets(input, 4, stdin);
            x = tolower(input[0]) - '`';
            y = input[1] - '0';
        }
        // 石の設置判定
        while (!is_valid_move(x, y, board->current_turn, board));
    }
    else
    {
        printf("White >> ");
        do
        {
            // ランダムな位置に置く
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        }
        while (!is_valid_move(x, y, board->current_turn, board));

        // プレイヤーと同様に入力座標を表示
        printf("%c%c\n", x + '`', y + '0');
    }

    put_and_flip(TO_POS(x, y), board->current_turn, board);

    printf("\n");
}