#ifndef CALC_H
#define CALC_H

// 計算結果の構造体
typedef struct {
    int result;
    int success;
} CalcResult;

// クエリから計算式を抽出して計算
CalcResult calculate_from_query(const char *query);

#endif
