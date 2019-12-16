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
#include "evaluator.h"
#include "learn.h"

const char option_str[] = "options\n \
    -b  play with BLACK (by default)\n \
    -w  play with WHITE\n \
    -c  COM vs COM\n \
    -l iterations\n\
        self-playing learning by specified iterations\n \
    -h  show this help\n";

#define EVAL_FILE "eval.dat"

static void show_prompt(const int color);
static void print_board(const Board *board, const int color);
static int get_input(Board *board, int color);
static void play(Board *board, Com *com, int player);
static void judge(const Board *board);

/// 
/// @fn     show_prompt
/// @brief  プロンプトを表示する
/// @param[in]  color   現在の手番
///
static void show_prompt(const int color)
{
    if (color== BLACK) {
        printf("Black(X) >> ");
    } else {
        printf("White(O) >> ");
    }
}

static void print_board(const Board *board, const int color)
{
    printf("    A B C D E F G H \n");
    printf("  +-----------------+\n");
    for (int y = 0; y < BOARD_SIZE; y++) {
        printf("%d | ", (y + 1));
        for (int x = 0; x < BOARD_SIZE; x++) {
            int pos = Board_pos(x, y);
            switch (Board_disk(board, pos)) {
                case WHITE:
                    printf("O ");
                    break;
                case BLACK:
                    printf("@ ");
                    break;
                default:
                    if (Board_can_flip(board, color, pos)) {
                        printf("* ");
                    } else {
                        printf("- ");
                    }
                    break;
            }
        }
        printf("|\n");
    }
    printf("  +-----------------+\n");
    printf("@:%2d O:%2d\n", Board_count_disks(board, BLACK), Board_count_disks(board, WHITE));
}

/// 
/// @fn     get_input
/// @brief  プレイヤー入力を取得する
/// @param[in]  board   盤面
/// @param[in]  color   現在の手番
/// @return 入力に対応した座標インデックス（'A1' - 'H8'）
///
static int get_input(Board *board, int color)
{
    int move;

    while (true) {
        char input[3];
        fgets(input, sizeof(input), stdin);

        // 標準出力に残る文字を読み飛ばす
        if(!feof(stdin)) {
            while (getchar() != '\n');
        }

        move = CHAR2POS(input[0], input[1]);

        // 着手判定
        if (Board_can_flip(board, color, move)) {
            break;
        } else {
            print_board(board, color);
            show_prompt(color);
        }
    }

    return move;
}

/// 
/// @fn     judge
/// @brief  勝敗を判定する
/// @param[in]  board   盤面
///
static void judge(const Board *board)
{
    int n_black = Board_count_disks(board, BLACK);
    int n_white = Board_count_disks(board, WHITE);

    if (n_black > n_white) {
        printf("*** BLACK wins ***\n");
    } else if (n_black < n_white) {
        printf("*** WHITE wins ***\n");
    } else {
        printf("*** draw ***\n");
    }
}

static void play(Board *board, Com *com, int player)
{
    int turn = BLACK;

    int val;

    while (true) {
        print_board(board, turn);

        if (Board_can_play(board, turn)) {
            show_prompt(turn);

            int move;
            if (turn == player) {
                move = get_input(board, turn);
            } else {
                move = Com_get_nextmove(com, board, turn, &val);
                // プレイヤーと同様に入力座標を表示
                printf("%c%c\n", POS2COL(move), POS2ROW(move));
            }

            Board_flip(board, turn, move);

        } else if (Board_can_play(board, Board_opponent(turn))) {
            printf("pass\n");
        } else {
            break;
        }

        turn = Board_opponent(turn);
    }

    judge(board);
}

int main(int argc, char *argv[])
{
    int player   = BLACK;
    bool learning = false;
    int  iteration;

    int opt;
    while ((opt = getopt(argc, argv, "bwcl:h")) != -1) {
        switch (opt) {
            case 'b':
                player = BLACK;
                break;
            case 'w':
                player = WHITE;
                break;
            case 'c':
                player = EMPTY;
                break;
            case 'l':
                learning  = true;
                iteration = atoi(optarg);
                break;
            case 'h':
                printf(option_str);
                return 0;
                break;
            default:
                printf("invalid option : \'%c\'", opt);
                return -1;
                break;
        }
    }

    Board *board = Board_create();

    Evaluator *evaluator = Evaluator_create();
    Evaluator_load(evaluator, EVAL_FILE);

    Com *com = Com_create(evaluator);

    if (learning) {
        learn(board, evaluator, com, iteration, EVAL_FILE);
    } else {
        Com_set_level(com, 6, 10, 6);
        play(board, com, player);
    }

    Com_delete(com);

    Evaluator_delete(evaluator);

    Board_delete(board);

    return 0;
}
