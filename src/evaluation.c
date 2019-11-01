#include "evaluation.h"

#include <limits.h>

int evaluate(Board *board, Disk disk) {
    int diff_disk   = count_disks(board, disk) - count_disks(board, OPPONENT(disk));
    int valid_moves = count_valid_moves(board, disk);

    // 評価値: 石数の差 + 2 * 有効手数
    return diff_disk + 2 * valid_moves;
}
