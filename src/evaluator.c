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
#define UPDATE_RATIO 0.005

///
/// @def    MAX_PATTERN_VALUE
/// @brief  評価値の上限値
///
#define MAX_PATTERN_VALUE (DISK_VALUE * 20)

///
/// @def    MIN_FREQUENCY
/// @brief  評価値の更新頻度
///
#define MIN_FREQUNECY 10

///
/// @enum   Pattern
/// @brief  評価対象のパターン
/// @note   Logistelloのパターンを元に簡略化されたもの
///         (http://www.es-cube.net/es-cube/reversi/sample/html/3_2.html)
///
typedef enum {
    PATTERN_HV4,        ///< hor./vert.4: 水平/垂直ラインパターン
    PATTERN_HV3,        ///< hor./vert.3
    PATTERN_HV2,        ///< hor./vert.2
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
    POW3_8, // hor./vert.4: A4-H4
    POW3_8, // hor./vert.3: A3-H3
    POW3_8, // hor./vert.2: A2-H2
    POW3_8, // diag8:       A1-H8
    POW3_7, // diag7:       A2-G8
    POW3_6, // diag6:       A3-F8
    POW3_5, // diag5:       A4-E8
    POW3_4, // diag4:       A5-D8
    POW3_8, // edge:        A1-G1 + B2
    POW3_8, // corner:      A1-C1 + A2-C2 + A3-B3
    2,      // parity
    0       // dummy
};

///
/// @struct Evaluator_
/// @brief  評価器
///
struct Evaluator_ {
    int    *values[PATTERN_NUM];   ///< 各パターンに対する評価値
    int    *pattern_num[PATTERN_NUM];  ///< パターンの出現回数
    double *pattern_sum[PATTERN_NUM];   ///< 評価値差分の合計
    int    mirror_line[POW3_8];    ///< 対称な列パターンを調べるための変数
    int    mirror_corner[POW3_8];  ///< 対称なコーナーパターンを調べるための変数
};

static bool initialize(Evaluator *eval);
static void finalize(Evaluator *eval);

static void add_pattern(Evaluator* eval, int pattern, int id, int mirror, double diff);

static void update_pattern(Evaluator *eval, int pattern, int id);

static bool initialize(Evaluator *eval)
{
    memset(eval, 0, sizeof(Evaluator));

    for (int i = 0; i < PATTERN_NUM; i++) {
        eval->values[i] = calloc(pattern_size[i], sizeof(int));
        if (!eval->values[i]) {
            return false;
        }

        eval->pattern_num[i] = calloc(pattern_size[i], sizeof(int));
        if (!eval->pattern_num[i]) {
            return false;
        }

        eval->pattern_sum[i] = calloc(pattern_size[i], sizeof(double));
        if (!eval->pattern_sum[i]) {
            return false;
        }
    }

    int mirror_in, mirror_out, coeff;
    int mirror_corner_coeff[] = {
        POW3_2, POW3_5, POW3_0, POW3_3, POW3_6, POW3_1, POW3_4, POW3_7
    };

    for (int i = 0; i < POW3_8; i++) {
        mirror_in  = i;
        mirror_out = 0;
        coeff = POW3_7;
        for (int j = 0; j < 8; j++) {
            mirror_out += mirror_in % 3 * coeff;
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
            mirror_out += mirror_in % 3 * mirror_corner_coeff[j];
            mirror_in /= 3;
        }
        if (mirror_out < i) {
            eval->mirror_corner[i] = mirror_out;
        } else {
            eval->mirror_corner[i] = i;
        }
    }

    return true;
}

static void finalize(Evaluator *eval)
{
    for (int i = 0; i < PATTERN_NUM; i++) {
        if (eval->pattern_sum[i]) {
            free(eval->pattern_sum[i]);
        }

        if (eval->pattern_num[i]) {
            free(eval->pattern_num[i]);
        }

        if (eval->values[i]) {
            free(eval->values[i]);
        }
    }
}

Evaluator *Evaluator_create(void)
{
    Evaluator *eval = malloc(sizeof(Evaluator));

    if (eval) {
        if (!initialize(eval)) {
            finalize(eval);
            eval = NULL;
        }
    }

    return eval;
}


void Evaluator_delete(Evaluator *eval)
{
    if (!eval) {
        return;
    }

    finalize(eval);

    free(eval);
    eval = NULL;
}

bool Evaluator_load(Evaluator *eval, const char *file)
{
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        return false;
    }

    for (int i = 0; i < PATTERN_NUM; i++) {
        if (fread(eval->values[i], sizeof(int), pattern_size[i], fp) < (size_t)pattern_size[i]) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);

    return true;
}

bool Evaluator_save(Evaluator *eval, const char *file)
{
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        return false;
    }

    for (int i = 0; i < PATTERN_NUM; i++) {
        if (fwrite(eval->values[i], sizeof(int), pattern_size[i], fp) < (size_t)pattern_size[i]) {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);

    return true;
}

int Evaluator_evaluate(Evaluator *eval, const Board *board)
{
    int result = 0;

    result += eval->values[PATTERN_HV4][Board_pattern(board, PATTERN_HV4_1)];
    result += eval->values[PATTERN_HV4][Board_pattern(board, PATTERN_HV4_2)];
    result += eval->values[PATTERN_HV4][Board_pattern(board, PATTERN_HV4_3)];
    result += eval->values[PATTERN_HV4][Board_pattern(board, PATTERN_HV4_4)];

    result += eval->values[PATTERN_HV3][Board_pattern(board, PATTERN_HV3_1)];
    result += eval->values[PATTERN_HV3][Board_pattern(board, PATTERN_HV3_2)];
    result += eval->values[PATTERN_HV3][Board_pattern(board, PATTERN_HV3_3)];
    result += eval->values[PATTERN_HV3][Board_pattern(board, PATTERN_HV3_4)];

    result += eval->values[PATTERN_HV2][Board_pattern(board, PATTERN_HV2_1)];
    result += eval->values[PATTERN_HV2][Board_pattern(board, PATTERN_HV2_2)];
    result += eval->values[PATTERN_HV2][Board_pattern(board, PATTERN_HV2_3)];
    result += eval->values[PATTERN_HV2][Board_pattern(board, PATTERN_HV2_4)];

    result += eval->values[PATTERN_DIAG8][Board_pattern(board, PATTERN_DIAG8_1)];
    result += eval->values[PATTERN_DIAG8][Board_pattern(board, PATTERN_DIAG8_2)];

    result += eval->values[PATTERN_DIAG7][Board_pattern(board, PATTERN_DIAG7_1)];
    result += eval->values[PATTERN_DIAG7][Board_pattern(board, PATTERN_DIAG7_2)];
    result += eval->values[PATTERN_DIAG7][Board_pattern(board, PATTERN_DIAG7_3)];
    result += eval->values[PATTERN_DIAG7][Board_pattern(board, PATTERN_DIAG7_4)];

    result += eval->values[PATTERN_DIAG6][Board_pattern(board, PATTERN_DIAG6_1)];
    result += eval->values[PATTERN_DIAG6][Board_pattern(board, PATTERN_DIAG6_2)];
    result += eval->values[PATTERN_DIAG6][Board_pattern(board, PATTERN_DIAG6_3)];
    result += eval->values[PATTERN_DIAG6][Board_pattern(board, PATTERN_DIAG6_4)];

    result += eval->values[PATTERN_DIAG5][Board_pattern(board, PATTERN_DIAG5_1)];
    result += eval->values[PATTERN_DIAG5][Board_pattern(board, PATTERN_DIAG5_2)];
    result += eval->values[PATTERN_DIAG5][Board_pattern(board, PATTERN_DIAG5_3)];
    result += eval->values[PATTERN_DIAG5][Board_pattern(board, PATTERN_DIAG5_4)];

    result += eval->values[PATTERN_DIAG4][Board_pattern(board, PATTERN_DIAG4_1)];
    result += eval->values[PATTERN_DIAG4][Board_pattern(board, PATTERN_DIAG4_2)];
    result += eval->values[PATTERN_DIAG4][Board_pattern(board, PATTERN_DIAG4_3)];
    result += eval->values[PATTERN_DIAG4][Board_pattern(board, PATTERN_DIAG4_4)];

    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_1)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_2)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_3)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_4)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_5)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_6)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_7)];
    result += eval->values[PATTERN_EDGE8][Board_pattern(board, PATTERN_EDGE8_8)];

    result += eval->values[PATTERN_CORNER8][Board_pattern(board, PATTERN_CORNER8_1)];
    result += eval->values[PATTERN_CORNER8][Board_pattern(board, PATTERN_CORNER8_2)];
    result += eval->values[PATTERN_CORNER8][Board_pattern(board, PATTERN_CORNER8_3)];
    result += eval->values[PATTERN_CORNER8][Board_pattern(board, PATTERN_CORNER8_4)];

    result += eval->values[PATTERN_PARITY][Board_count_disks(board, EMPTY) & 1];

    return result;
}

static void add_pattern(Evaluator* eval, int pattern, int id, int mirror, double diff)
{
    eval->pattern_num[pattern][id]++;
    eval->pattern_sum[pattern][id] += diff;

    if (mirror >= 0) {
        eval->pattern_num[pattern][mirror] = eval->pattern_num[pattern][id];
        eval->pattern_sum[pattern][mirror] = eval->pattern_sum[pattern][id];
    }
}

void Evaluator_add(Evaluator *eval, const Board *board, int value)
{
    int index;
    double diff;

    diff = (double)(value - Evaluator_evaluate(eval, board));

    index = Board_pattern(board, PATTERN_HV4_1);
    add_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV4_2);
    add_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV4_3);
    add_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV4_4);
    add_pattern(eval, PATTERN_HV4, eval->mirror_line[index], index, diff);

    index = Board_pattern(board, PATTERN_HV3_1);
    add_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV3_2);
    add_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV3_3);
    add_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV3_4);
    add_pattern(eval, PATTERN_HV3, eval->mirror_line[index], index, diff);

    index = Board_pattern(board, PATTERN_HV2_1);
    add_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV2_2);
    add_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV2_3);
    add_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_HV2_4);
    add_pattern(eval, PATTERN_HV2, eval->mirror_line[index], index, diff);

    index = Board_pattern(board, PATTERN_DIAG8_1);
    add_pattern(eval, PATTERN_DIAG8, eval->mirror_line[index], index, diff);
    index = Board_pattern(board, PATTERN_DIAG8_2);
    add_pattern(eval, PATTERN_DIAG8, eval->mirror_line[index], index, diff);

    index = Board_pattern(board, PATTERN_DIAG7_1);
    add_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = Board_pattern(board, PATTERN_DIAG7_2);
    add_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = Board_pattern(board, PATTERN_DIAG7_3);
    add_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);
    index = Board_pattern(board, PATTERN_DIAG7_4);
    add_pattern(eval, PATTERN_DIAG7, eval->mirror_line[index * POW3_1], index, diff);

    index = Board_pattern(board, PATTERN_DIAG6_1);
    add_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = Board_pattern(board, PATTERN_DIAG6_2);
    add_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = Board_pattern(board, PATTERN_DIAG6_3);
    add_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);
    index = Board_pattern(board, PATTERN_DIAG6_4);
    add_pattern(eval, PATTERN_DIAG6, eval->mirror_line[index * POW3_2], index, diff);

    index = Board_pattern(board, PATTERN_DIAG5_1);
    add_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = Board_pattern(board, PATTERN_DIAG5_2);
    add_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = Board_pattern(board, PATTERN_DIAG5_3);
    add_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);
    index = Board_pattern(board, PATTERN_DIAG5_4);
    add_pattern(eval, PATTERN_DIAG5, eval->mirror_line[index * POW3_3], index, diff);

    index = Board_pattern(board, PATTERN_DIAG4_1);
    add_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = Board_pattern(board, PATTERN_DIAG4_2);
    add_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = Board_pattern(board, PATTERN_DIAG4_3);
    add_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);
    index = Board_pattern(board, PATTERN_DIAG4_4);
    add_pattern(eval, PATTERN_DIAG4, eval->mirror_line[index * POW3_4], index, diff);

    index = Board_pattern(board, PATTERN_EDGE8_1);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_2);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_3);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_4);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_5);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_6);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_7);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);
    index = Board_pattern(board, PATTERN_EDGE8_8);
    add_pattern(eval, PATTERN_EDGE8, index, -1, diff);

    index = Board_pattern(board, PATTERN_CORNER8_1);
    add_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = Board_pattern(board, PATTERN_CORNER8_2);
    add_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = Board_pattern(board, PATTERN_CORNER8_3);
    add_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);
    index = Board_pattern(board, PATTERN_CORNER8_4);
    add_pattern(eval, PATTERN_CORNER8, eval->mirror_corner[index], index, diff);

    add_pattern(eval, PATTERN_PARITY, (Board_count_disks(board, EMPTY) & 1), -1, diff);
}

static void update_pattern(Evaluator *eval, int pattern, int id)
{
    int diff;

    if (eval->pattern_num[pattern][id] > MIN_FREQUNECY) {
        diff = (int)(eval->pattern_sum[pattern][id] / eval->pattern_num[pattern][id] * UPDATE_RATIO);

        if ((MAX_PATTERN_VALUE - diff) < eval->values[pattern][id]) {
            eval->values[pattern][id] = MAX_PATTERN_VALUE;
        } else if ((-MAX_PATTERN_VALUE - diff) > eval->values[pattern][id]) {
            eval->values[pattern][id] = -MAX_PATTERN_VALUE;
        } else {
            eval->values[pattern][id] += diff;
        }

        eval->pattern_num[pattern][id] = 0;
        eval->pattern_sum[pattern][id] = 0;
    }
}

void Evaluator_update(Evaluator *eval)
{
    for (int i = 0; i < PATTERN_NUM; i++) {
        for (int j = 0; j < pattern_size[i]; j++) {
            update_pattern(eval, i, j);
        }
    }
}
