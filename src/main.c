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

const char option_str[] = "options:\n \
    -b) play with BLACK (first turn, by default)\n \
    -w) play with WHITE (second turn)\n \
    -c) COM vs COM\n \
    -h) show this help\n";

#define EVAL_FILE "eval.dat"

/// 
/// @fn     show_prompt
/// @brief  プロンプトを表示する
/// @param[in]  currnent   現在の手番
///
static void show_prompt(Disk current) {
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
static Pos get_input(Board *board, Disk current) {
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
static void judge(Board *board) {
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

static int get_rand(int max) {
    return (int)((double)max * rand() / (RAND_MAX + 1.0));
}

static void move_random(Board *board, Disk color) {
    while(!Board_flip(board, color, XY2POS(get_rand(BOARD_SIZE), get_rand(BOARD_SIZE))));
}

void learn(Board *board, Evaluator *evaluator, Com *com, int max_iter) {
    int  history[BOARD_SIZE * BOARD_SIZE];

    Com_set_level(com, 4, 12, 12);

    for (int i = 0; i < max_iter; i++) {
        Disk color = BLACK;
        Pos move;
        int turn = 0;
        int value;

        Board_init(board);

        for (int j = 0; j < 8; j++) {
            if (Board_has_valid_move(board, color)) {
                move_random(board, color);
                history[turn] = color;
                turn++;
            }
            color = OPPONENT(color);
        }

        while (true) {
            if (Board_has_valid_move(board, color)) {
                if ((Board_count_disk(board, EMPTY) > 12) && (get_rand(100) < 1)) {
                    move_random(board, color);
                } else {
                    move = Com_get_nextmove(com, board, color, &value);
                    Board_flip(board, color, move);
                }

                history[turn] = color;
                turn++;
            } else if (!Board_has_valid_move(board, OPPONENT(color))){
                break;
            }
            color = OPPONENT(color);
        }

        int result = (Board_count_disk(board, BLACK) - Board_count_disk(board, WHITE)) * DISK_VALUE;
        for (int j = Board_count_disk(board, EMPTY); j < 8; j++) {
            turn--;
            Board_unflip(board);
        }
        for (int j = Board_count_disk(board, EMPTY); j < (BOARD_SIZE * BOARD_SIZE - 12); j++) {
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
            printf("Learning ... %d / %d\n", (i + 1), max_iter);
            Evaluator_save(evaluator, EVAL_FILE);
        }
    }

    Evaluator_save(evaluator, EVAL_FILE);
    printf("Finished\n");
}

static void play(Board *board, Com *com, Disk player) {
    Disk current = BLACK;

    int val;

    while (true) {
        Board_print(board, current);
        printf("@:%2d O:%2d\n", Board_count_disk(board, BLACK), Board_count_disk(board, WHITE));

        if (Board_has_valid_move(board, current)) {
            show_prompt(current);

            Pos move;
            if (current == player) {
                move = get_input(board, current);
            } else {
                move = Com_get_nextmove(com, board, current, &val);
                // プレイヤーと同様に入力座標を表示
                printf("%c%c\n", POS2COL(move), POS2ROW(move));
            }

            Board_flip(board, current, move);

        } else if (Board_has_valid_move(board, OPPONENT(current))) {
            printf("pass\n");
        } else {
            break;
        }

        current = OPPONENT(current);
    }

    judge(board);
}

int main(int argc, char *argv[]) {
    Disk player   = BLACK;
    bool do_learn = false;
    int  max_iter;

    int opt;
    while ((opt = getopt(argc, argv, "bwcl:h")) != -1) {
        switch (opt) {
            case 'b': player = BLACK; break;
            case 'w': player = WHITE; break;
            case 'c': player = EMPTY; break;
            case 'l':
                do_learn = true;
                max_iter = atoi(optarg);
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

    Board *board = Board_create();
    Board_init(board);

    Evaluator *evaluator = Evaluator_create();
    Evaluator_init(evaluator);
    Evaluator_load(evaluator, EVAL_FILE);

    Com *com = Com_create(evaluator);
    Com_init(com);

    if (do_learn) {
        learn(board, evaluator, com, max_iter);
    } else {
        Com_set_level(com, 6, 10, 6);
        play(board, com, player);
    }

    Board_delete(board);

    Evaluator_delete(evaluator);

    Com_delete(com);

    return 0;
}
