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

static int get_rand(int max)
{
    return (int)((double)max * rand() / (RAND_MAX + 1.0));
}

static void move_random(Board *board, const int color)
{
    while (!Board_flip(board, color, Board_pos(get_rand(BOARD_SIZE), get_rand(BOARD_SIZE))));
}

void learn(Board *board, Evaluator *evaluator, Com *com, const int iteration, const char* file)
{
    int  history[BOARD_SIZE * BOARD_SIZE];

    Com_set_level(com, 4, 12, 12);

    printf("Start learning\n");

    for (int i = 0; i < iteration; i++) {
        Board_init(board);

        int color = BLACK;
        int move;
        int turn = 0;
        int value;

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

        int result = (Board_count_disks(board, BLACK) - Board_count_disks(board, WHITE)) * DISK_VALUE;
        for (int j = Board_count_disks(board, EMPTY); j < 8; j++) {
            turn--;
            Board_unflip(board);
        }
        for (int j = Board_count_disks(board, EMPTY); j < (BOARD_SIZE * BOARD_SIZE - 12); j++) {
            turn--;
            Board_unflip(board);
            if (history[turn] == BLACK) {
                // 局面の登録
                Evaluator_add(evaluator, board, result);
            } else {
                Board_reverse(board);
                // 局面の登録
                Evaluator_add(evaluator, board, -result);
                Board_reverse(board);
            }
        }

        // パラメータ更新
        if ((i + 1) % 10 == 0) {
            Evaluator_update(evaluator);
        }

        if ((i + 1) % 100 == 0) {
            printf("Learning ... %d / %d\n", (i + 1), iteration);
            Evaluator_save(evaluator, file);
        }
    }

    Evaluator_save(evaluator, file);
    printf("Finished\n");
}
