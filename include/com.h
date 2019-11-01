#ifndef COM_H_
#define COM_H_

#include "board.h"
#include "evaluation.h"

// 手の探索と取得
Pos com_search_move(Board *board, Disk self_disk, int depth);

#endif // COM_H_
