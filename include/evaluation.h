#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

// 評価関数
int evaluate(Disk, Board*);

// 探索した手の取得
Pos search_move(Disk, int, Board*);

// NegaAlpha法による探索
int negaalpha(Pos*, Disk, Disk, int, int, int, Board*);

#endif