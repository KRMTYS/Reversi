///
/// @file   evaluator.c
/// @brief  盤面の評価
/// @author kentakuramochi
///

#include "evaluator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

///
/// @def    UPDATE_RATIO
/// @brief  評価値の更新率
///
#define UPDATE_RATIO 0.003

///
/// @def    PATTERN_IDX(N)
/// @brief  座標N個を使用するパターンのインデックス
///         各座標は3状態（黒石/白石/空）であるため各パターンはN^3の状態数を持つ
///

#define PATTERN_IDX_4(board, p1, p2, p3, p4) ( \
    ((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4)) \
)

#define PATTERN_IDX_5(board, p1, p2, p3, p4, p5) ( \
    (((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4)) * 3) \
    + Board_disk((board), (p5)) \
)

#define PATTERN_IDX_6(board, p1, p2, p3, p4, p5, p6) ( \
    ((((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4))) * 3 \
    + Board_disk((board), (p5))) * 3 + Board_disk((board), (p6)) \
)

#define PATTERN_IDX_7(board, p1, p2, p3, p4, p5, p6, p7) ( \
    (((((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4))) * 3 \
    + Board_disk((board), (p5))) * 3 + Board_disk((board), (p6))) * 3 + Board_disk((board), (p7)) \
)

#define PATTERN_IDX_8(board, p1, p2, p3, p4, p5, p6, p7, p8) ( \
    ((((((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4))) * 3 \
    + Board_disk((board), (p5))) * 3 + Board_disk((board), (p6))) * 3 + Board_disk((board), (p7))) * 3 + Board_disk((board), (p8)) \
)

#define PATTERN_IDX_9(board, p1, p2, p3, p4, p5, p6, p7, p8, p9) ( \
    (((((((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4))) * 3 \
    + Board_disk((board), (p5))) * 3 + Board_disk((board), (p6))) * 3 + Board_disk((board), (p7))) * 3 + Board_disk((board), (p8))) * 3 \
    + Board_disk((board), (p9)) \
)

#define PATTERN_IDX_10(board, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) ( \
    ((((((((Board_disk((board), (p1)) * 3 + Board_disk((board), (p2))) * 3 + Board_disk((board), (p3))) * 3 + Board_disk((board), (p4))) * 3 \
    + Board_disk((board), (p5))) * 3 + Board_disk((board), (p6))) * 3 + Board_disk((board), (p7))) * 3 + Board_disk((board), (p8))) * 3 \
    + Board_disk((board), (p9))) * 3 + Board_disk((board), (p10)) \
)


///
/// @enum   Pattern
/// @brief  評価対象のパターン
/// @note   Logistelloのパターンを元に簡略化されたもの
///         (http://www.es-cube.net/es-cube/reversi/sample/html/3_2.html)
///
typedef enum {
    PATTERN_HV2,        ///< hor./vert.2: 水平/垂直ラインパターン
    PATTERN_HV3,        ///< hor./vert.3
    PATTERN_HV4,        ///< hor./vert.4
    PATTERN_DIAG8,      ///< diag8: 対角線パターン
    PATTERN_DIAG7,      ///< diag7
    PATTERN_DIAG6,      ///< diag6
    PATTERN_DIAG5,      ///< diag5
    PATTERN_DIAG4,      ///< diag4
    PATTERN_EDGE8,      ///< edge: 辺パターン
    PATTERN_CORNER8,    ///< corner: 角パターン
    PATTERN_PARITY,     ///< パリティ
    PATTERN_NUM         ///< パターン数
} Pattern;


///
/// @enum   Pow3
/// @brief  3の冪乗数
/// @note   評価パターンの状態数に利用する
///
typedef enum {
    POW3_0  = 1,
    POW3_1  = 3,
    POW3_2  = 9,
    POW3_3  = 27,
    POW3_4  = 81,
    POW3_5  = 243,
    POW3_6  = 729,
    POW3_7  = 2187,
    POW3_8  = 6561,
    POW3_9  = 19683,
    POW3_10 = 59049
} Pow3;

// 各評価パターンの状態数
static const int pattern_size[] = {
    POW3_8, // hor./vert.2: A4-H4
    POW3_8, // hor./vert.3: A3-H3
    POW3_8, // hor./vert.4: A2-H2
    POW3_8, // diag8:       A1-H8
    POW3_7, // diag7:       A2-G8
    POW3_6, // diag6:       A3-F8
    POW3_5, // diag5:       A4-E8
    POW3_4, // diag4:       A5-D8
    POW3_8, // edge:        A1-G1 + B2
    POW3_8, // corner:      A1-C1 + A2-C2 + A3-B3
    2, // parity
    0  // dummy
};

///
/// @struct Evaluator_
/// @brief  評価器
///
struct Evaluator_ {
    int *values[PATTERN_NUM];   ///< 各パターンに対する評価値
    int mirror_line[POW3_8];    ///< 対称な列パターンを調べるための変数
    int mirror_corner[POW3_8];  ///< 対称なコーナーパターンを調べるための変数
};

Evaluator *Evaluator_create(void) {
    Evaluator *eval = malloc(sizeof(Evaluator));

    return eval;
}

void Evaluator_init(Evaluator *eval) {
    memset(eval, 0, sizeof(Evaluator));

    int mirror_in, mirror_out, coeff;
    int mirror_corner_coeff[] = {
        POW3_2, POW3_5, POW3_0, POW3_3, POW3_6, POW3_1, POW3_4, POW3_7
    };

    for (int i = 0; i < PATTERN_NUM; i++) {
        eval->values[i] = calloc(pattern_size[i], sizeof(int));
    }

    for (int i = 0; i < POW3_8; i++) {
        mirror_in  = i;
        mirror_out = 0;
        coeff = POW3_7;
        for (int j = 0; j < 8; j++) {
            mirror_out += mirror_in;
            mirror_in /= 3;
            coeff /= 3;
        }
        if (mirror_out < i) {
            eval->mirror_line[i] = mirror_out;
        } else {
            eval->mirror_line[i] = i;
        }
    }
    for (int i = 0; i < POW3_8; i++) {
        mirror_in  = i;
        mirror_out = 0;
        coeff = POW3_7;
        for (int j = 0; j < 8; j++) {
            mirror_out += mirror_in;
            mirror_in /= 3;
            coeff /= 3;
        }
        if (mirror_out < i) {
            eval->mirror_corner[i] = mirror_out;
        } else {
            eval->mirror_corner[i] = i;
        }
    }
}

void Evaluator_delete(Evaluator *eval) {
    if (eval == NULL) {
        return;
    }

    for (int i = 0; i < PATTERN_NUM; i++) {
        free(eval->values[i]);
        eval->values[i] = NULL;
    }

    free(eval);
    eval = NULL;
}

void Evaluator_load(Evaluator *eval, const char *file) {
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        return;
    }

    for (int i = 0; i < PATTERN_NUM; i++) {
        if (fread(eval->values[i], sizeof(int), pattern_size[i], fp) < (size_t)pattern_size[i]) {
            fclose(fp);
            return;
        }
    }

    fclose(fp);
}

void Evaluator_save(Evaluator *eval, const char *file) {
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        return;
    }

    for (int i = 0; i < PATTERN_NUM; i++) {
        if (fwrite(eval->values[i], sizeof(int), pattern_size[i], fp) < (size_t)pattern_size[i]) {
            fclose(fp);
            return;
        }
    }

    fclose(fp);
}

int Evaluator_evaluate(Evaluator *eval, Board *board) {
    int result = 0;

    result += eval->values[PATTERN_HV4][PATTERN_IDX_8(board, A4, B4, C4, D4, E4, F4, G4, H4)];
    result += eval->values[PATTERN_HV4][PATTERN_IDX_8(board, A5, B5, C5, D5, E5, F5, G5, H5)];
    result += eval->values[PATTERN_HV4][PATTERN_IDX_8(board, D1, D2, D3, D4, D5, D6, D7, D8)];
    result += eval->values[PATTERN_HV4][PATTERN_IDX_8(board, E1, E2, E3, E4, E5, E6, E7, E8)];

    result += eval->values[PATTERN_HV3][PATTERN_IDX_8(board, A3, B3, C3, D3, E3, F3, G3, H3)];
    result += eval->values[PATTERN_HV3][PATTERN_IDX_8(board, A6, B6, C6, D6, E6, F6, G6, H6)];
    result += eval->values[PATTERN_HV3][PATTERN_IDX_8(board, C1, C2, C3, C4, C5, C6, C7, C8)];
    result += eval->values[PATTERN_HV3][PATTERN_IDX_8(board, F1, F2, F3, F4, F5, F6, F7, F8)];

    result += eval->values[PATTERN_HV2][PATTERN_IDX_8(board, A2, B2, C2, D2, E2, F2, G2, H2)];
    result += eval->values[PATTERN_HV2][PATTERN_IDX_8(board, A7, B7, C7, D7, E7, F7, G7, H7)];
    result += eval->values[PATTERN_HV2][PATTERN_IDX_8(board, B1, B2, B3, B4, B5, B6, B7, B8)];
    result += eval->values[PATTERN_HV2][PATTERN_IDX_8(board, G1, G2, G3, G4, G5, G6, G7, G8)];

    result += eval->values[PATTERN_DIAG8][PATTERN_IDX_8(board, A1, B2, C3, D4, E5, F6, G7, H8)];
    result += eval->values[PATTERN_DIAG8][PATTERN_IDX_8(board, A8, B8, C6, D5, E4, F3, G2, H1)];

    result += eval->values[PATTERN_DIAG7][PATTERN_IDX_7(board, A2, B3, C4, D5, E6, F7, G8)];
    result += eval->values[PATTERN_DIAG7][PATTERN_IDX_7(board, B1, C2, D3, E4, F5, G6, H7)];
    result += eval->values[PATTERN_DIAG7][PATTERN_IDX_7(board, A7, B7, C5, D4, E3, F2, G1)];
    result += eval->values[PATTERN_DIAG7][PATTERN_IDX_7(board, B8, C7, D5, E5, F4, G3, H2)];

    result += eval->values[PATTERN_DIAG6][PATTERN_IDX_6(board, A3, B4, C5, D6, E7 ,F8)];
    result += eval->values[PATTERN_DIAG6][PATTERN_IDX_6(board, C1, D2, E3, F4, G5, H6)];
    result += eval->values[PATTERN_DIAG6][PATTERN_IDX_6(board, A6, B5, C4, D3, E2, F1)];
    result += eval->values[PATTERN_DIAG6][PATTERN_IDX_6(board, C8, D7, E6, F5, G4, H3)];

    result += eval->values[PATTERN_DIAG5][PATTERN_IDX_5(board, A4, B5, C6, D7, E8)];
    result += eval->values[PATTERN_DIAG5][PATTERN_IDX_5(board, D1, E2, F3, G4, H5)];
    result += eval->values[PATTERN_DIAG5][PATTERN_IDX_5(board, A5, B4, C3, D2, E1)];
    result += eval->values[PATTERN_DIAG5][PATTERN_IDX_5(board, D8, E7, F6, G5, H4)];

    result += eval->values[PATTERN_DIAG4][PATTERN_IDX_4(board, A5, B6, C7, D8)];
    result += eval->values[PATTERN_DIAG4][PATTERN_IDX_4(board, E1, F2, G3, H4)];
    result += eval->values[PATTERN_DIAG4][PATTERN_IDX_4(board, A4, B3, C2, D1)];
    result += eval->values[PATTERN_DIAG4][PATTERN_IDX_4(board, E8, F7, G6, H5)];

    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, A1, B1, C1, D1, E1, F1, G1, B2)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, H1, G1, F1, E1, D1, C1, B1, G2)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, A8, B8, C8, D8, E8, F8, G8, B7)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, H8, G8, F8, E8, D8, C8, B8, G7)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, A1, A2, A3, A4, A5, A6, A7, B2)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, A8, A7, A6, A5, A4, A3, A2, B7)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, H1, H2, H3, H4, H5, H6, H7, G2)];
    result += eval->values[PATTERN_EDGE8][PATTERN_IDX_8(board, H8, H7, H6, H5, H4, H3, H2, G7)];

    result += eval->values[PATTERN_CORNER8][PATTERN_IDX_8(board, A1, B1, C1, A2, B2, C2, A3, B3)];
    result += eval->values[PATTERN_CORNER8][PATTERN_IDX_8(board, H1, G1, F1, H2, G2, F2, H3, G3)];
    result += eval->values[PATTERN_CORNER8][PATTERN_IDX_8(board, A8, B8, C8, A7, B7, C7, A6, B6)];
    result += eval->values[PATTERN_CORNER8][PATTERN_IDX_8(board, H8, G8, F8, H7, G7, F7, H6, G6)];

    result += eval->values[PATTERN_PARITY][Board_count_disk(board, EMPTY) & 1];

    return result;
}

///
/// @fn     Evaluator_update_pattern
/// @brief  パターンの評価値を更新する
/// @param[in]  eval    評価器
/// @param[in]  pattern 更新するパターンの種類
/// @param[in]  idx     更新するパターンインデックス
/// @param[in]  mirror  更新パターンと対称なパターンインデックス
/// @param[in]  diff    評価値の差分
///
static void Evaluator_update_pattern(Evaluator *eval, int pattern, int idx, int mirror, int diff) {
    if ((MAX_PATTERN_VALUE - diff) < eval->values[pattern][idx]) {
        eval->values[pattern][idx] = MAX_PATTERN_VALUE;
    } else if ((-MAX_PATTERN_VALUE - diff) > eval->values[pattern][idx]) {
        eval->values[pattern][idx] = -MAX_PATTERN_VALUE;
    } else {
        eval->values[pattern][idx] += diff;
    }

    if (mirror >= 0) {
        eval->values[pattern][mirror] = eval->values[pattern][idx];
    }
}

void Evaluator_update(Evaluator *eval, Board *board, int value) {
    int diff = (int)((value - Evaluator_evaluate(eval, board)) * UPDATE_RATIO);

    int index = PATTERN_IDX_8(board, A4, B4, C4, D4, E4, F4, G4, H4);
    Evaluator_update_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, A5, B5, C5, D5, E5, F5, G5, H5);
    Evaluator_update_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, D1, D2, D3, D4, D5, D6, D7, D8);
    Evaluator_update_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, E1, E2, E3, E4, E5, E6, E7, E8);
    Evaluator_update_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);

    index = PATTERN_IDX_8(board, A3, B3, C3, D3, E3, F3, G3, H3);
    Evaluator_update_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, A6, B6, C6, D6, E6, F6, G6, H6);
    Evaluator_update_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, C1, C2, C3, C4, C5, C6, C7, C8);
    Evaluator_update_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, F1, F2, F3, F4, F5, F6, F7, F8);
    Evaluator_update_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);

    index = PATTERN_IDX_8(board, A2, B2, C2, D2, E2, F2, G2, H2);
    Evaluator_update_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, A7, B7, C7, D7, E7, F7, G7, H7);
    Evaluator_update_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, B1, B2, B3, B4, B5, B6, B7, B8);
    Evaluator_update_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, G1, G2, G3, G4, G5, G6, G7, G8);
    Evaluator_update_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);

    index = PATTERN_IDX_8(board, A1, B2, C3, D4, E5, F6, G7, H8);
    Evaluator_update_pattern(eval, PATTERN_DIAG8, eval->mirror_line[index], index, diff);
    index = PATTERN_IDX_8(board, A8, B8, C6, D5, E4, F3, G2, H1);
    Evaluator_update_pattern(eval, PATTERN_DIAG8, eval->mirror_line[index], index, diff);

    index = PATTERN_IDX_7(board, A2, B3, C4, D5, E6, F7, G8);
    Evaluator_update_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = PATTERN_IDX_7(board, B1, C2, D3, E4, F5, G6, H7);
    Evaluator_update_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = PATTERN_IDX_7(board, A7, B7, C5, D4, E3, F2, G1);
    Evaluator_update_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = PATTERN_IDX_7(board, B8, C7, D5, E5, F4, G3, H2);
    Evaluator_update_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);

    index = PATTERN_IDX_6(board, A3, B4, C5, D6, E7 ,F8);
    Evaluator_update_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = PATTERN_IDX_6(board, C1, D2, E3, F4, G5, H6);
    Evaluator_update_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = PATTERN_IDX_6(board, A6, B5, C4, D3, E2, F1);
    Evaluator_update_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = PATTERN_IDX_6(board, C8, D7, E6, F5, G4, H3);
    Evaluator_update_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);

    index = PATTERN_IDX_5(board, A4, B5, C6, D7, E8);
    Evaluator_update_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = PATTERN_IDX_5(board, D1, E2, F3, G4, H5);
    Evaluator_update_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = PATTERN_IDX_5(board, A5, B4, C3, D2, E1);
    Evaluator_update_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = PATTERN_IDX_5(board, D8, E7, F6, G5, H4);
    Evaluator_update_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);

    index = PATTERN_IDX_4(board, A5, B6, C7, D8);
    Evaluator_update_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = PATTERN_IDX_4(board, E1, F2, G3, H4);
    Evaluator_update_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = PATTERN_IDX_4(board, A4, B3, C2, D1);
    Evaluator_update_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = PATTERN_IDX_4(board, E8, F7, G6, H5);
    Evaluator_update_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);

    index = PATTERN_IDX_8(board, A1, B1, C1, D1, E1, F1, G1, B2);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, H1, G1, F1, E1, D1, C1, B1, G2);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, A8, B8, C8, D8, E8, F8, G8, B7);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, H8, G8, F8, E8, D8, C8, B8, G7);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, A1, A2, A3, A4, A5, A6, A7, B2);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, A8, A7, A6, A5, A4, A3, A2, B7);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, H1, H2, H3, H4, H5, H6, H7, G2);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = PATTERN_IDX_8(board, H8, H7, H6, H5, H4, H3, H2, G7);
    Evaluator_update_pattern(eval, PATTERN_EDGE8, index, -1, diff);

    index = PATTERN_IDX_8(board, A1, B1, C1, A2, B2, C2, A3, B3);
    Evaluator_update_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = PATTERN_IDX_8(board, H1, G1, F1, H2, G2, F2, H3, G3);
    Evaluator_update_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = PATTERN_IDX_8(board, A8, B8, C8, A7, B7, C7, A6, B6);
    Evaluator_update_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = PATTERN_IDX_8(board, H8, G8, F8, H7, G7, F7, H6, G6);
    Evaluator_update_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);

    Evaluator_update_pattern(eval, PATTERN_PARITY, (Board_count_disk(board, EMPTY) & 1), -1, diff);
}
