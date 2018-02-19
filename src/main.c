#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "evaluation.h"

void input(Board*);

int main()
{
    // 盤面
    Board board;

    srand((unsigned)time(NULL));

    init_board(&board);

    // ゲーム状態
    State state;

    // ターン数
    int turn = 1;

    while ((state = get_state(&board)) != FINISH)
    {
        print_board(&board);

        printf("[%d] ", turn);

        switch(state)
        {
            case DO_TURN:
                input(&board);
                turn++;
                break;
            default:
                printf("Pass\n");
                break;
        }
    }

    judge(&board);

    return 0;
}

// 入力
void input(Board* board)
{
    if (board->current_turn == BLACK)
    {
        char input[2];
        int x, y;

        printf("Black >> ");

        while (true)
        {
            // 入力した文字を1~8の(x, y)座標に変換
            fgets(input, 3, stdin);
            x = tolower(input[0]) - '`';
            y = input[1] - '0';
            fflush(stdin);

            // 石の設置判定
            if (is_valid(x, y, board->current_turn, board))
            {
                break;
            }
            else
            {
                printf("Cannot put here: input again >> ");
            }
        }

        put_and_flip(TO_POS(x, y), board->current_turn, board);
    }
    else
    {
        printf("White >> ");

        Pos move;

        // negamax法での先読み評価
        // 5手先まで
        negamax(&move, board->current_turn, board->current_turn, 5, board);

        // プレイヤーと同様に入力座標を表示
        printf("%c%c\n", TO_X(move) + '`', TO_Y(move) + '0');

        put_and_flip(move, board->current_turn, board);
    }

    printf("\n");
}