#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// URLデコード
static char* url_decode(const char *src) {
    size_t len = strlen(src);
    char *decoded = malloc(len + 1);
    if (!decoded) return NULL;
    
    size_t i = 0, j = 0;
    while (i < len) {
        if (src[i] == '+') {
            decoded[j++] = ' ';
            i++;
        } else if (src[i] == '%' && i + 2 < len) {
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

CalcResult calculate_from_query(const char *query) {
    CalcResult result = {0, 0};
    
    if (!query) return result;
    
    // "query="を探す
    const char *query_param = strstr(query, "query=");
    if (!query_param) return result;
    
    query_param += 6; // "query="の長さ分スキップ
    
    // スペースまたは改行で終了
    char *query_value = malloc(strlen(query_param) + 1);
    if (!query_value) return result;
    
    size_t i = 0;
    while (query_param[i] && query_param[i] != ' ' && 
           query_param[i] != '\r' && query_param[i] != '\n' &&
           query_param[i] != '&') {
        query_value[i] = query_param[i];
        i++;
    }
    query_value[i] = '\0';
    
    // URLデコード
    char *decoded = url_decode(query_value);
    free(query_value);
    if (!decoded) return result;
    
    // 計算式をパース
    int num1, num2;
    char op;
    if (sscanf(decoded, "%d %c %d", &num1, &op, &num2) == 3) {
        switch(op) {
            case '+': result.result = num1 + num2; result.success = 1; break;
            case '-': result.result = num1 - num2; result.success = 1; break;
            case '*': result.result = num1 * num2; result.success = 1; break;
            case '/': 
                if (num2 != 0) {
                    result.result = num1 / num2;
                    result.success = 1;
                }
                break;
            default: break;
        }
    }
    
    free(decoded);
    return result;
}
