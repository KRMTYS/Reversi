#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

// 評価関数
int evaluate(Disk, Board*);

// NegaMax法による探索
int negamax(Pos*, Disk, Disk, int, Board*);

#endif