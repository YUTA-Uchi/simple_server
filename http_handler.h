#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <stdlib.h>
// HTTPリクエストを処理してレスポンスを生成
char* handle_http_request(const char *request, size_t *response_len);

#endif
