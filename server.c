
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "http_handler.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define QUEUE_SIZE 10


int main(void) {
    int s, b, l, sa, on = 1;
    char *buffer = NULL;
    struct sockaddr_in server_addr;
    
    // バッファの動的確保
    buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    // サーバーアドレス初期化
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    // ソケット作成
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        perror("socket");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    
    // ソケットオプション設定
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
        perror("setsockopt");
        close(s);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    
    // バインド
    b = bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (b < 0) {
        perror("bind");
        close(s);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    
    // リッスン
    l = listen(s, QUEUE_SIZE);
    if (l < 0) {
        perror("listen");
        close(s);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // メインループ
    while (1) {
        sa = accept(s, NULL, NULL);
        if (sa < 0) {
            perror("accept");
            continue;
        }
        
        // リクエスト受信
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = read(sa, buffer, BUFFER_SIZE - 1);
        if (bytes < 0) {
            perror("read");
            close(sa);
            continue;
        }
        buffer[bytes] = '\0';
        
        printf("Received:\n%s\n", buffer);
        
        // HTTPレスポンス生成
        size_t response_len = 0;
        char *http_response = make_response(buffer, &response_len);
        
        if (http_response) {
            // レスポンス送信
            ssize_t written = write(sa, http_response, response_len);
            if (written < 0) {
                perror("write");
            }
            free(http_response);
        } else {
            fprintf(stderr, "Failed to generate response\n");
        }
        
        close(sa);
    }
    
    close(s);
    free(buffer);
    return 0;
}
