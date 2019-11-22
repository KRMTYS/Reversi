///
/// @file   com.h
/// @brief  COM思考ルーチンの動作
/// @author kentakuramochi
///

#ifndef COM_H_
#define COM_H_

#include "board.h"

typedef struct Com_ Com;

///
/// @fn     Com_create
/// @brief  COM思考ルーチンを生成する
/// @return COM
///
Com *Com_create(void);

///
/// @fn     Com_delete
/// @brief  COM思考ルーチンを破棄する
/// @param[in]  com     COM
///
void Com_delete(Com *com);

///
/// @fn     Com_get_move
/// @brief  次の手を取得する
/// @param[in]  com     COM
/// @param[in]  board   盤面
/// @param[in]  turn    手番
/// @return COMの次手
///
Pos Com_get_move(Com *com, Board *board, Disk turn);

#endif // COM_H_
