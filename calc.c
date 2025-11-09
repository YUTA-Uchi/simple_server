#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "calc.h"

// URLデコード関数
char* url_decode(const char *src) {
    if (!src) return NULL;
    
    size_t len = strlen(src);
    char *decoded = malloc(len + 1);
    if (!decoded) {
        perror("malloc");
        return NULL;
    }
    
    size_t i = 0, j = 0;
    while (i < len) {
        if (src[i] == '+') {
            decoded[j++] = ' ';
            i++;
        } else if (src[i] == '%' && i + 2 < len && 
                   isxdigit(src[i+1]) && isxdigit(src[i+2])) {
            char hex[3] = {src[i+1], src[i+2], '\0'};
            decoded[j++] = (char)strtol(hex, NULL, 16);
            i += 3;
        } else {
            decoded[j++] = src[i++];
        }
    }
    decoded[j] = '\0';
    return decoded;
}

// 計算実行
int calculate(int num1, char op, int num2, int *result) {
    switch(op) {
        case '+': *result = num1 + num2; return 1;
        case '-': *result = num1 - num2; return 1;
        case '*': *result = num1 * num2; return 1;
        case '/': 
            if (num2 == 0) return 0;
            *result = num1 / num2;
            return 1;
        default: return 0;
    }
}

// クエリパラメータから計算式を抽出して計算
int parse_and_calculate(const char *request, int *result) {
    if (!request || !result) return 0;
    
    // "query="を探す
    const char *query_start = strstr(request, "query=");
    if (!query_start) return 0;
    
    query_start += 6; // "query="の長さ分スキップ
    
    // クエリ値を抽出（スペース、改行、&で終了）
    char *query_value = malloc(strlen(query_start) + 1);
    if (!query_value) {
        perror("malloc");
        return 0;
    }
    
    size_t i = 0;
    while (query_start[i] && query_start[i] != ' ' && 
           query_start[i] != '\r' && query_start[i] != '\n' &&
           query_start[i] != '&') {
        query_value[i] = query_start[i];
        i++;
    }
    query_value[i] = '\0';
    
    // URLデコード
    char *decoded = url_decode(query_value);
    free(query_value);
    if (!decoded) return 0;
    
    // 計算式をパース
    int num1, num2;
    char op;
    int parsed = sscanf(decoded, "%d %c %d", &num1, &op, &num2);
    free(decoded);
    
    if (parsed != 3) return 0;
    
    // 計算実行
    return calculate(num1, op, num2, result);
}
