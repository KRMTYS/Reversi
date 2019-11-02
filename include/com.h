#ifndef COM_H_
#define COM_H_

#include "board.h"

// 手の探索と取得
Pos com_search_move(Board *board, Disk self_disk);

#endif // COM_H_
