///
/// @file   main.c
/// @brief  リバーシメインルーチン
/// @author kentakuramochi
///

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#include "board.h"
#include "com.h"

const char option_str[] = "options:\n \
    -b) play with BLACK (first turn, by default)\n \
    -w) play with WHITE (second turn)\n \
    -c) COM vs COM\n \
    -h) show this help\n";

/// 
/// @fn     show_prompt
/// @brief  プロンプトを表示する
/// @param[in]  currnent   現在の手番
///
void show_prompt(Disk current) {
    if (current == BLACK) {
        printf("Black(@) >> ");
    } else {
        printf("White(O) >> ");
    }
}

/// 
/// @fn     get_input
/// @brief  プレイヤー入力を取得する
/// @param[in]  board   盤面
/// @param[in]  current 現在の手番
/// @return 入力に対応した座標インデックス（'A1' - 'H8'）
///
Pos get_input(Board *board, Disk current) {
    Pos move;

    while (true) {
        // col + row + '\0'
        char input[3];
        fgets(input, sizeof(input), stdin);

        // 標準出力に残る文字を読み飛ばす
        if(!feof(stdin)) {
            while (getchar() != '\n');
        }

        move = CHAR2POS(input[0], input[1]);

        // 石の設置判定
        if (Board_check_valid(board, current, move)) {
            break;
        } else {
            show_prompt(current);
        }
    }

    return move;
}

/// 
/// @fn     judge
/// @brief  勝敗を判定する
/// @param[in]  board   盤面
///
void judge(Board *board) {
    int n_black = Board_count_disk(board, BLACK);
    int n_white = Board_count_disk(board, WHITE);

    if (n_black > n_white) {
        printf("*** BLACK wins ***\n");
    } else if (n_black < n_white) {
        printf("*** WHITE wins ***\n");
    } else {
        printf("*** draw ***\n");
    }
}

int main(int argc, char *argv[]) {
    Disk player = BLACK;

    int opt;
    while ((opt = getopt(argc, argv, "bwch")) != -1) {
        switch (opt) {
            case 'b': player = BLACK; break;
            case 'w': player = WHITE; break;
            case 'c': player = EMPTY; break;
            case 'h':
                printf(option_str);
                return 0;
                break;
            default:
                printf("invalid option : \'%c\'", opt);
                return 1;
                break;
        }
    }

    Board *board = Board_create();

    Board_init(board);

    Com *com = Com_create();

    Disk current = BLACK;

    while (true) {
        Board_print(board, current);
        printf("@:%2d O:%2d\n", Board_count_disk(board, BLACK), Board_count_disk(board, WHITE));

        if (Board_has_valid_move(board, current)) {
            show_prompt(current);

            Pos move;
            if (current == player) {
                move = get_input(board, current);
            } else {
                move = Com_get_move(com, board, current);
                // プレイヤーと同様に入力座標を表示
                printf("%c%c\n", POS2COL(move), POS2ROW(move));
            }

            Board_put_and_flip(board, current, move);

        } else if (Board_has_valid_move(board, OPPONENT(current))) {
            printf("pass\n");
        } else {
            break;
        }

        current = OPPONENT(current);
    }

    judge(board);

    Board_delete(board);

    Com_delete(com);

    return 0;
}
