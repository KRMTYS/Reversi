#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <getopt.h>

#include "board.h"
#include "evaluation.h"

const char option_str[] = "options:\n \
    -b) play with BLACK (first turn, by default)\n \
    -w) play with WHITE (second turn)\n \
    -c) COM vs COM\n \
    -h) show this help\n";

// 探索レベル
#define SEARCH_LEVEL 5

// 座標値-文字間変換
#define TO_INT_X(c)  (toupper((c)) - 'A' + 1)
#define TO_INT_Y(c)  ((c) - '0')
#define TO_CHAR_X(x) ((x) + 'A' - 1)
#define TO_CHAR_Y(y) ((y) + '0')

// プロンプト表示
void show_prompt(Disk current_turn) {
    if (current_turn == BLACK) {
        printf("Black(@) >> ");
    } else {
        printf("White(O) >> ");
    }
}

// プレイヤーの入力を取得
Pos get_input(Board *board) {
    Pos move;

    while (true) {
        // col + row + '\0'
        char input[3];
        fgets(input, sizeof(input), stdin);

        if(!feof(stdin)) {
            // 標準出力に残る文字を読み飛ばす
            while (getchar() != '\n');
        }

        int x = TO_INT_X(input[0]);
        int y = TO_INT_Y(input[1]);

        move = TO_POS(x, y);

        // 石の設置判定
        if (is_valid(board, board->turn, move)) {
            break;
        } else {
            show_prompt(board->turn);
        }
    }

    return move;
}

// 勝敗判定
void judge(Board *board) {
    int n_black = count_disks(board, BLACK);
    int n_white = count_disks(board, WHITE);

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

    // 盤面
    Board board;

    init_board(&board);

    while (true) {
        print_board(&board);

        if (has_valid_move(&board, board.turn)) {
            show_prompt(board.turn);

            Pos move;
            if (board.turn == player) {
                move = get_input(&board);
            } else {
                move = search_move(&board, board.turn, SEARCH_LEVEL);

                // プレイヤーと同様に入力座標を表示
                printf("%c%c\n", TO_CHAR_X(TO_X(move)), TO_CHAR_Y(TO_Y(move)));
            }
            put_and_flip(&board, board.turn, move);
        } else if (has_valid_move(&board, OPPONENT(board.turn))) {
            printf("pass\n");
            change_turn(&board);
        } else {
            break;
        }
    }

    judge(&board);

    return 0;
}
