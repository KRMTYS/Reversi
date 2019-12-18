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

///
/// @struct Setting
/// @brief  ゲーム設定
///
typedef struct {
    int player_turn;    ///< プレイヤー手番
    int learn_iter;     ///< 学習回数
} Setting;

const char option_str[] = "options\n \
    -b  play with BLACK (by default)\n \
    -w  play with WHITE\n \
    -c  COM vs COM\n \
    -l iterations\n\
        self-playing learning by specified iterations\n \
    -h  show this help\n";

///
/// @def    EVAL_FILE
/// @brief  学習評価値の出力ファイル名
///
#define EVAL_FILE "eval.dat"

static bool parse_options(int argc, char* argv[], Setting *setting);

static char *get_stream(char *buffer, const int size, FILE *stream);

static void print_board(const Board *board, const int color);
static void play(Board *board, Com *com, Setting *setting);

///
/// @fn     parse_options
/// @brief  コマンドライン引数からパラメータ設定する
/// @param[in]  argc    argc (main)
/// @param[in]  argv    argv (main)
/// @param[out] setting ゲーム設定
/// @retval true    パラメータ設定成功
/// @retval false   設定失敗
///
static bool parse_options(int argc, char* argv[], Setting *setting)
{
    setting->player_turn = BLACK;
    setting->learn_iter  = 0;

    int opt;
    while ((opt = getopt(argc, argv, "bwcl:h")) != -1) {
        switch (opt) {
            case 'b':
                // -b: プレイヤー手番黒（先攻）
                setting->player_turn = BLACK;
                break;
            case 'w':
                // -w: プレイヤー手番白（後攻）
                setting->player_turn = WHITE;
                break;
            case 'c':
                // -c: COM対COM (debug)
                setting->player_turn = EMPTY;
                break;
            case 'l':
                // -l iteration: 指定回数の学習
                setting->learn_iter = atoi(optarg);
                break;
            case 'h':
                // -h: ヘルプ表示
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
/// @fn     get_stream
/// @brief  ストリームから行単位で文字列を取得する
/// @param[out] buffer  文字列を格納するバッファ
/// @param[in]  size    バッファ長
/// @param[in]  stream  入力ストリーム
/// @return 取得文字列の先頭ポインタ（取得失敗時NULL）
///
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
                        // 有効手の位置を表示する
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

///
/// @fn     play
/// @brief  ゲームを実行する
/// @param[in]  board   盤面
/// @param[in]  com     思考ルーチン
/// @param[in]  setting ゲーム設定
///
static void play(Board *board, Com *com, Setting *setting)
{
    int  turn = BLACK;
    int  move;
    int  val;
    // 入力バッファ
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

    // 勝敗の判定処理
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
