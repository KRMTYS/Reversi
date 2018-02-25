#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "board.h"
#include "evaluation.h"

// 探索レベル
#define SEARCH_LEVEL 5

// 座標値-文字間変換
#define TO_INT_X(c) (c - 0x60)
#define TO_INT_Y(c) (c - 0x30)
#define TO_CHAR_X(x) (x + 0x60)
#define TO_CHAR_Y(y) (y + 0x30)

// 操作者
typedef enum
{
    PLAYER, // プレイヤー 
    COM     // COM
}
Operator;

void select_player(Operator*);
void input(Board*, Operator*);

int main()
{
    // 盤面
    Board board;

    init_board(&board);

    // 順に黒番、白番
    Operator op[2];

    select_player(op);

    // ターン数
    int turn = 1;

    while (true)
    {
        print_board(&board);

        State state = get_state(&board);

        if (state == DO_TURN)
        {
            printf("[%d] ", turn);
            input(&board, op);
            turn++;
        }
        else if (state == PASS)
        {
            printf("Pass\n");
        }
        else
        {
            printf("Finish\n\n");
            break;
        }
    }

    switch (judge(&board))
    {
        case BLACK:
            printf("* Black Wins *\n");
            break;
        case WHITE:
            printf("* White Wins *\n");
            break;
        default:
            printf("* Draw *\n");
            break;
    }

    return 0;
}

// プレイヤー選択
void select_player(Operator* op)
{
    printf("Select your turn\n");
    printf("1: Black 2: White 3: None (COM vs COM)\n");

    while (true)
    {
        printf(">> ");

        char input;

        fgets(&input, 2, stdin);
        fflush(stdin);

        int n = atoi(&input);

        if (n == 1)
        {
            op[0] = PLAYER;
            op[1] = COM;
            break;
        }
        else if (n == 2)
        {
            op[0] = COM;
            op[1] = PLAYER;
            break;
        }
        else if (n == 3)
        {
            op[0] = COM;
            op[1] = COM;
            break;
        }
    }
}

// 入力
void input(Board* board, Operator* op)
{
    int index;

    if (board->current_turn == BLACK)
    {
        printf("Black(@) >> ");
        index = 0;
    }
    else
    {
        printf("White(O) >> ");
        index = 1;
    }

    if (op[index] == PLAYER)
    {
        char input[2];
        int x, y;

        while (true)
        {
            // 入力した文字を1~8の(x, y)座標に変換
            fgets(input, 3, stdin);
            x = TO_INT_X(tolower(input[0]));
            y = TO_INT_Y(input[1]);
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
        Pos move;

        // negaalpha法での探索
        negaalpha(&move,
                  board->current_turn,
                  board->current_turn,
                  -INT_MAX,
                  INT_MAX,
                  SEARCH_LEVEL,
                  board);

        // プレイヤーと同様に入力座標を表示
        printf("%c%c\n", TO_CHAR_X(TO_X(move)), TO_CHAR_Y(TO_Y(move)));

        put_and_flip(move, board->current_turn, board);
    }

    printf("\n");
}