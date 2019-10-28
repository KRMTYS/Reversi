#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <getopt.h>

#include "board.h"
#include "evaluation.h"

const char option_str[] = "options:\n \
    -b) start with black (first turn, by default)\n \
    -w) start with white (second turn)\n \
    -c) COM vs COM\n \
    -h) show this strings\n";

// 探索レベル
#define SEARCH_LEVEL 5

// 座標値-文字間変換
#define TO_INT_X(c)     ((c) - 0x60)
#define TO_INT_Y(c)     ((c) - 0x30)
#define TO_CHAR_X(x)    ((x) + 0x60)
#define TO_CHAR_Y(y)    ((y) + 0x30)

// 操作者
typedef enum {
    PLAYER, // プレイヤー 
    COM     // COM
} Operator;

// プロンプト表示
void show_prompt(int turn_num, Disk current_turn) {
    printf("[%d] ", turn_num);

    if (current_turn == BLACK) {
        printf("Black(@) >> ");
    } else {
        printf("White(O) >> ");
    }
}

// 勝敗判定
void judge(Board *board) {
    int n_black = count_disks(board, BLACK);
    int n_white = count_disks(board, WHITE);

    if (n_black > n_white) {
        printf("* Black Wins *\n");
    } else if (n_black < n_white) {
        printf("* White Wins *\n");
    } else {
        printf("* Draw *\n");
    }
}

// 入力
void input(Board* board, Operator* op) {
    int index = (board->current_turn == BLACK) ? 0 : 1;

    show_prompt(board->turn_num, board->current_turn);

    // PLAYER
    if (op[index] == PLAYER) {
        // 入力バッファ長は適当
        char input[10];
        int x, y;

        while (true) {
            fgets(input, sizeof(input), stdin);
            fflush(stdin);
            x = TO_INT_X(tolower(input[0]));
            y = TO_INT_Y(input[1]);

            // 石の設置判定
            if (is_valid(board, board->current_turn, x, y)) {
                break;
            } else {
                printf("cannot put: retry\n");
                show_prompt(board->turn_num, board->current_turn);
            }
        }
        put_and_flip(board, board->current_turn, TO_POS(x, y));
    // COM
    } else {
        Pos move = search_move(board, board->current_turn, SEARCH_LEVEL);

        // プレイヤーと同様に入力座標を表示
        printf("%c%c\n", TO_CHAR_X(TO_X(move)), TO_CHAR_Y(TO_Y(move)));

        put_and_flip(board, board->current_turn, move);
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    // 順に黒番、白番
    Operator op[2] = { PLAYER, COM };

    int opt;
    while ((opt = getopt(argc, argv, "bwch")) != -1) {
        switch (opt) {
            case 'b':
                op[0] = PLAYER;
                op[1] = COM;
                break;
            case 'w':
                op[0] = COM;
                op[1] = PLAYER;
                break;
            case 'c':
                op[0] = COM;
                op[1] = COM;
                break;
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

        State state = get_state(&board);

        if (state == DO_TURN) {
            input(&board, op);
        } else if (state == PASS) {
            printf("Pass\n");
        } else {
            printf("Finish\n\n");
            break;
        }
    }

    judge(&board);

    return 0;
}
