///
/// @file   eval.c
/// @brief  盤面の評価
/// @author kentakuramochi
///

#include "eval.h"

#include <limits.h>

int evaluate(Board *board, Disk disk) {
    int diff_disk   = Board_count_disk(board, disk) - Board_count_disk(board, OPPONENT(disk));
    int valid_moves = Board_count_valid_moves(board, disk);

    // 評価値: 石数の差 + 2 * 有効手数
    return diff_disk + 2 * valid_moves;
}
