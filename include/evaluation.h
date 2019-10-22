#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "board.h"

// 評価関数
int evaluate(Disk disk, Board *board);

// 探索した手の取得
Pos search_move(Disk self_disk, int depth, Board *board);

// NegaAlpha法による探索
int negaalpha(Pos *next_move,
              Disk self_disk,
              Disk current_turn,
              int alpha, int beta, int depth,
              Board *board);

#endif // EVALUATION_H_
