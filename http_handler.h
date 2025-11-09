#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <stdio.h>
#include <stddef.h>
// HTTPリクエストを処理してレスポンスを生成
char* make_response(const char *request, size_t *response_len);

#endif
