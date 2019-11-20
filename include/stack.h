///
/// @file   stack.h
/// @brief  簡易スタック
/// @author kentakuramochi
///

///
/// @typedef    Stack
/// @brief      簡易スタック
/// @note       データ型はintのみ
///
typedef struct Stack_ Stack;

///
/// @fn     Stack_create
/// @brief  スタックを生成する
/// @param[in]  stack   スタック
/// @param[in]  maxsize スタックの最大サイズ
/// @return 初期化済みスタック
///
Stack *Stack_create(const int maxsize);

///
/// @fn     Stack_delete
/// @brief  スタックを破棄する
/// @param[in]  stack   スタック
///
void Stack_delete(Stack *stack);

///
/// @fn     Stack_init
/// @brief  スタックを初期化する
/// @param[in]  stack   スタック
///
void Stack_init(Stack *stack);

///
/// @fn     Stack_push
/// @brief  スタックへプッシュする
/// @param[in]  stack   スタック
/// @param[in]  maxsize スタックの最大サイズ
/// @retval 0   成功
/// @retval 1   失敗
/// @note データ型はintのみ
///
int Stack_push(Stack *stack, const int data);

///
/// @fn     Stack_pop
/// @brief  スタックからポップする
/// @param[in]  maxsize スタックの最大サイズ
/// @param[out] data    ポップしたデータ
/// @retval 0   成功
/// @retval 1   失敗
/// @note データ型はintのみ
///
int Stack_pop(Stack *stack, int *data);
