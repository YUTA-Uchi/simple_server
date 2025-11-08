#include "http_handler.h"
#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* handle_http_request(const char *request, size_t *response_len) {
    char *response = NULL;
    
    // GET /calcリクエストをチェック
    if (strncmp(request, "GET /calc", 9) == 0) {
        CalcResult calc_result = calculate_from_query(request);
        
        if (calc_result.success) {
            // 計算結果をボディに変換
            char body[64];
            int body_len = snprintf(body, sizeof(body), "%d", calc_result.result);
            
            // レスポンス全体のサイズを計算
            char header[256];
            int header_len = snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %d\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n", body_len);
            
            // メモリ動的確保
            *response_len = header_len + body_len;
            response = malloc(*response_len + 1);
            if (response) {
                memcpy(response, header, header_len);
                memcpy(response + header_len, body, body_len);
                response[*response_len] = '\0';
            }
        } else {
            // エラーレスポンス
            const char *error_response = 
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Length: 11\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Bad Request";
            *response_len = strlen(error_response);
            response = malloc(*response_len + 1);
            if (response) {
                strcpy(response, error_response);
            }
        }
    } else {
        // 404レスポンス
        const char *not_found = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 9\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Not Found";
        *response_len = strlen(not_found);
        response = malloc(*response_len + 1);
        if (response) {
            strcpy(response, not_found);
        }
    }
    
    return response;
}
