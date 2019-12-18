///
/// @file   learn.c
/// @brief  自己対局による学習
/// @author kentakuramochi
///

#include "learn.h"

#include <stdio.h>
#include <stdlib.h>

static int get_rand(int max);

static void move_random(Board *board, const int color);

///
/// @fn     get_rand
/// @brief  指定した値未満の整数乱数を取得する
/// @param[in]  max 乱数上限値 (0 <= rand < max)
///
static int get_rand(int max)
{
    return (int)((double)max * rand() / (RAND_MAX + 1.0));
}

///
/// @fn     move_random
/// @brief  ランダムな位置に着手する
/// @param[in,out]  board   盤面
/// @param[in]      color   手番色
///
static void move_random(Board *board, const int color)
{
    while (!Board_flip(board, color, Board_pos(get_rand(BOARD_SIZE), get_rand(BOARD_SIZE))));
}

void learn(Board *board, Evaluator *evaluator, Com *com, const int iteration, const char* file)
{
    // 着手履歴
    int  history[BOARD_SIZE * BOARD_SIZE];

    // 探索深さは適当
    // 中盤: 4手読み、終盤: 12手読み
    Com_set_level(com, 4, 12, 12);

    printf("Start learning\n");

    for (int i = 0; i < iteration; i++) {
        Board_init(board);

        int color = BLACK;
        int move;
        int turn = 0;
        int value;

        // 初期8手はランダムに着手する
        for (int j = 0; j < 8; j++) {
            if (Board_can_play(board, color)) {
                move_random(board, color);
                history[turn] = color;
                turn++;
            }
            color = Board_opponent(color);
        }

        while (true) {
            if (Board_can_play(board, color)) {
                // ランダム着手: 空きマス12以上のとき、1%の確率
                if ((Board_count_disks(board, EMPTY) > 12) && (get_rand(100) < 1)) {
                    move_random(board, color);
                } else {
                    move = Com_get_nextmove(com, board, color, &value);
                    Board_flip(board, color, move);
                }

                history[turn] = color;
                turn++;
            } else if (!Board_can_play(board, Board_opponent(color))){
                break;
            }
            color = Board_opponent(color);
        }

        // 評価値: 終局時の石数差
        int result = (Board_count_disks(board, BLACK) - Board_count_disks(board, WHITE)) * DISK_VALUE;

        // 終盤探索する最後8手は評価に含めないため除く
        for (int j = Board_count_disks(board, EMPTY); j < 8; j++) {
            turn--;
            Board_unflip(board);
        }

        for (int j = Board_count_disks(board, EMPTY); j < (BOARD_SIZE * BOARD_SIZE - 12); j++) {
            turn--;
            Board_unflip(board);
            if (history[turn] == BLACK) {
                // 石数差を評価値として局面を登録する
                Evaluator_add(evaluator, board, result);
            } else {
                // パラメータ調整は黒番局面で揃える: 色反転し負の評価値で登録する
                Board_reverse(board);
                Evaluator_add(evaluator, board, -result);
                Board_reverse(board);
            }
        }

        // 評価パラメータの更新: 10局単位
        if ((i + 1) % 10 == 0) {
            Evaluator_update(evaluator);
        }

        // 評価パラメータの保存: 100局単位
        if ((i + 1) % 100 == 0) {
            printf("Learning ... %d / %d\n", (i + 1), iteration);
            Evaluator_save(evaluator, file);
        }
    }

    Evaluator_save(evaluator, file);
    printf("Finished\n");
}
