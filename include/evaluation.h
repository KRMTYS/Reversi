#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "board.h"

// 探索した手の取得
Pos search_move(Disk self_disk, int depth, Board *board);

#endif // EVALUATION_H_
