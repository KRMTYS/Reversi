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

const char option_str[] = "options\n \
    -b  play with BLACK (by default)\n \
    -w  play with WHITE\n \
    -c  COM vs COM\n \
    -l iterations\n\
        self-playing learning by specified iterations\n \
    -h  show this help\n";

#define EVAL_FILE "eval.dat"

/// 
/// @fn     show_prompt
/// @brief  プロンプトを表示する
/// @param[in]  currnent   現在の手番
///
static void show_prompt(Disk turn) {
    if (turn == BLACK) {
        printf("Black(@) >> ");
    } else {
        printf("White(O) >> ");
    }
}

static void print_board(const Board *board, const Disk turn) {
    printf("    A B C D E F G H \n");
    printf("  +-----------------+\n");
    for (int y = 0; y < BOARD_SIZE; y++) {
        printf("%d | ", (y + 1));
        for (int x = 0; x < BOARD_SIZE; x++) {
            Pos pos = XY2POS(x, y);
            switch (Board_disk(board, pos)) {
                case WHITE:
                    printf("O ");
                    break;
                case BLACK:
                    printf("@ ");
                    break;
                default:
                    if (Board_can_flip(board, turn, pos)) {
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
/// @param[in]  turn 現在の手番
/// @return 入力に対応した座標インデックス（'A1' - 'H8'）
///
static Pos get_input(Board *board, Disk turn) {
    Pos move;

    while (true) {
        char input[3];
        fgets(input, sizeof(input), stdin);

        // 標準出力に残る文字を読み飛ばす
        if(!feof(stdin)) {
            while (getchar() != '\n');
        }

        move = CHAR2POS(input[0], input[1]);

        // 着手判定
        if (Board_can_flip(board, turn, move)) {
            break;
        } else {
            print_board(board, turn);
            show_prompt(turn);
        }
    }

    return move;
}

/// 
/// @fn     judge
/// @brief  勝敗を判定する
/// @param[in]  board   盤面
///
static void judge(Board *board) {
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

static int get_rand(int max) {
    return (int)((double)max * rand() / (RAND_MAX + 1.0));
}

static void move_random(Board *board, Disk color) {
    while(!Board_flip(board, color, XY2POS(get_rand(BOARD_SIZE), get_rand(BOARD_SIZE))));
}

static void learn(Board *board, Evaluator *evaluator, Com *com, int iteration) {
    int  history[BOARD_SIZE * BOARD_SIZE];

    Com_set_level(com, 4, 12, 12);

    for (int i = 0; i < iteration; i++) {
        Disk color = BLACK;
        Pos move;
        int turn = 0;
        int value;

        Board_init(board);

        for (int j = 0; j < 8; j++) {
            if (Board_can_play(board, color)) {
                move_random(board, color);
                history[turn] = color;
                turn++;
            }
            color = OPPONENT(color);
        }

        while (true) {
            if (Board_can_play(board, color)) {
                if ((Board_count_disks(board, EMPTY) > 12) && (get_rand(100) < 1)) {
                    move_random(board, color);
                } else {
                    move = Com_get_nextmove(com, board, color, &value);
                    Board_flip(board, color, move);
                }

                history[turn] = color;
                turn++;
            } else if (!Board_can_play(board, OPPONENT(color))){
                break;
            }
            color = OPPONENT(color);
        }

        int result = (Board_count_disks(board, BLACK) - Board_count_disks(board, WHITE)) * DISK_VALUE;
        for (int j = Board_count_disks(board, EMPTY); j < 8; j++) {
            turn--;
            Board_unflip(board);
        }
        for (int j = Board_count_disks(board, EMPTY); j < (BOARD_SIZE * BOARD_SIZE - 12); j++) {
            turn--;
            Board_unflip(board);
            if (history[turn] == BLACK) {
                Evaluator_update(evaluator, board, result);
            } else {
                Board_reverse(board);
                Evaluator_update(evaluator, board, -result);
                Board_reverse(board);
            }
        }

        if ((i + 1) % 100 == 0) {
            printf("Learning ... %d / %d\n", (i + 1), iteration);
            Evaluator_save(evaluator, EVAL_FILE);
        }
    }

    Evaluator_save(evaluator, EVAL_FILE);
    printf("Finished\n");
}

static void play(Board *board, Com *com, Disk player) {
    Disk turn = BLACK;

    int val;

    while (true) {
        print_board(board, turn);

        if (Board_can_play(board, turn)) {
            show_prompt(turn);

            Pos move;
            if (turn == player) {
                move = get_input(board, turn);
            } else {
                move = Com_get_nextmove(com, board, turn, &val);
                // プレイヤーと同様に入力座標を表示
                printf("%c%c\n", POS2COL(move), POS2ROW(move));
            }

            Board_flip(board, turn, move);

        } else if (Board_can_play(board, OPPONENT(turn))) {
            printf("pass\n");
        } else {
            break;
        }

        turn = OPPONENT(turn);
    }

    judge(board);
}

int main(int argc, char *argv[]) {
    Disk player   = BLACK;
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
        learn(board, evaluator, com, iteration);
    } else {
        Com_set_level(com, 6, 10, 6);
        play(board, com, player);
    }

    Board_delete(board);

    Com_delete(com);

    return 0;
}
