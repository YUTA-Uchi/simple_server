#include <stdlib.h>
#include <string.h>
#include "http_handler.h"
#include "calc.h"

// HTTPレスポンスを生成
char* make_response(const char *request, size_t *response_len) {
    if (!request || !response_len) return NULL;
    
    char *response = NULL;
    int calc_result;
    
    // GET /calcリクエストかチェック
    if (strncmp(request, "GET /calc", 9) == 0) {
        if (parse_and_calculate(request, &calc_result)) {
            // 計算結果を文字列に変換
            char body[64];
            int body_len = snprintf(body, sizeof(body), "%d", calc_result);
            
            // HTTPレスポンスを構築
            char header[256];
            int header_len = snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %d\r\n"
                "\r\n", body_len);
            
            // メモリ動的確保
            *response_len = header_len + body_len;
            response = malloc(*response_len + 1);
            if (!response) {
                perror("malloc");
                return NULL;
            }
            
            memcpy(response, header, header_len);
            memcpy(response + header_len, body, body_len);
            response[*response_len] = '\0';
        } else {
            // 計算失敗
            const char *error_response = 
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Length: 11\r\n"
                "\r\n"
                "Bad Request";
            *response_len = strlen(error_response);
            response = malloc(*response_len + 1);
            if (response) {
                strcpy(response, error_response);
            }
        }
    } else {
        // 404 Not Found
        const char *not_found = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 9\r\n"
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
