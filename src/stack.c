///
/// @file   stack.c
/// @brief  簡易スタック
/// @author kentakuramochi
///

#include "stack.h"

#include <stdlib.h>

///
/// @struct Stack_
/// @brief  簡易スタック
/// @note   データ型はintのみ
///
struct Stack_ {
    int *data;  ///< データ
    int max;    ///< 最大サイズ
    int size;   ///< スタックサイズ
};

Stack *Stack_create(const int maxsize) {
    Stack *stack = malloc(sizeof(Stack));
    if (stack == NULL) {
        return NULL;
    }

    stack->data = malloc(maxsize * sizeof(int));
    if (stack->data == NULL) {
        free(stack);
        stack = NULL;
        return NULL;
    }

    stack->max  = maxsize;
    stack->size = 0;

    return stack;
}

void Stack_delete(Stack *stack) {
    if (stack == NULL) {
        return;
    }

    free(stack->data);
    stack->data = NULL;

    free(stack);
    stack = NULL;
}

void Stack_init(Stack *stack) {
    stack->size = 0;
}

int Stack_push(Stack *stack, const int data) {
    if (stack->size >= stack->max) {
        return 1;
    }

    stack->data[stack->size] = data;
    stack->size++;

    return 0;
}

int Stack_pop(Stack *stack, int *data) {
    if (stack->size <= 0) {
        return 1;
    }

    stack->size--;
    *data = stack->data[stack->size];

    return 0;
}
