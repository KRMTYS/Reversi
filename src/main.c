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

typedef struct {
    int player_turn;
    int learn_iter;
} Setting;

const char option_str[] = "options\n \
    -b  play with BLACK (by default)\n \
    -w  play with WHITE\n \
    -c  COM vs COM\n \
    -l iterations\n\
        self-playing learning by specified iterations\n \
    -h  show this help\n";

#define EVAL_FILE "eval.dat"

static bool parse_options(int argc, char* argv[], Setting *setting);
static void print_board(const Board *board, const int color);
static char *get_stream(char *buffer, const int size, FILE *stream);
static void play(Board *board, Com *com, Setting *setting);

static bool parse_options(int argc, char* argv[], Setting *setting)
{
    setting->player_turn = BLACK;
    setting->learn_iter  = 0;

    int opt;
    while ((opt = getopt(argc, argv, "bwcl:h")) != -1) {
        switch (opt) {
            case 'b':
                setting->player_turn = BLACK;
                break;
            case 'w':
                setting->player_turn = WHITE;
                break;
            case 'c':
                setting->player_turn = EMPTY;
                break;
            case 'l':
                setting->learn_iter = atoi(optarg);
                break;
            case 'h':
                printf(option_str);
                return false;
                break;
            default:
                printf("invalid option : \'%c\'", opt);
                return false;
                break;
        }
    }

    return true;
}

/// 
/// @fn     print_board
/// @brief  盤面を表示する
/// @param[in]  board   盤面
/// @param[in]  color   現在の手番
///
static void print_board(const Board *board, const int color)
{
    printf("    A B C D E F G H \n");
    printf("  +-----------------+\n");
    for (int y = 0; y < BOARD_SIZE; y++) {
        printf("%d | ", (y + 1));
        for (int x = 0; x < BOARD_SIZE; x++) {
            int pos = Board_pos(x, y);
            switch (Board_disk(board, pos)) {
                case BLACK:
                    printf("X ");
                    break;
                case WHITE:
                    printf("O ");
                    break;
                default:
                    if (Board_can_flip(board, color, pos)) {
                        printf("* ");
                    } else {
                        printf("  ");
                    }
                    break;
            }
        }
        printf("|\n");
    }
    printf("  +-----------------+\n");
    printf("X: %d O: %d\n", Board_count_disks(board, BLACK), Board_count_disks(board, WHITE));
}

static char *get_stream(char *buffer, const int size, FILE *stream)
{
    char *result = fgets(buffer, size, stream);

    if (result != NULL) {
        for (int i = 0; i < size; i++) {
            if (buffer[i] == '\n') {
                buffer[i] = '\0';
            }
        }
    }

    return result;
}

///
/// @fn     play
/// @brief  ゲーム実行
/// @param[in]  board   盤面
///
static void play(Board *board, Com *com, Setting *setting)
{
    int  turn = BLACK;
    int  move;
    int  val;
    char buffer[32];

    Com_set_level(com, 6, 10, 6);

    while (true) {
        print_board(board, turn);

        if (Board_can_play(board, turn)) {
            printf(">> ");
            if (turn == setting->player_turn) {
                while (true) {
                    while (!get_stream(buffer, sizeof(buffer), stdin)) {
                        printf(">> ");
                    }

                    move = CHAR2POS(buffer[0], buffer[1]);
                    if (Board_can_flip(board, turn, move)) {
                        break;
                    }
                }
            } else {
                move = Com_get_nextmove(com, board, turn, &val);
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

    // 勝敗判定
    int diff = Board_count_disks(board, BLACK) - Board_count_disks(board, WHITE);
    if (diff > 0) {
        printf("* BLACK wins *\n");
    } else if (diff < 0) {
        printf("* WHITE wins *\n");
    } else {
        printf("* draw *\n");
    }
}

int main(int argc, char *argv[])
{
    Setting setting;
    if (!parse_options(argc, argv, &setting)) {
        exit(EXIT_FAILURE);
    }

    Board *board = Board_create();

    Evaluator *evaluator = Evaluator_create();
    Evaluator_load(evaluator, EVAL_FILE);

    Com *com = Com_create(evaluator);

    if (setting.learn_iter > 0) {
        learn(board, evaluator, com, setting.learn_iter, EVAL_FILE);
    } else {
        play(board, com, &setting);
    }

    Com_delete(com);

    Evaluator_delete(evaluator);

    Board_delete(board);

    return 0;
}
