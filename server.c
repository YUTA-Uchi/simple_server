
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include "http_handler.h"

#define PORT 8080
#define BACKLOG 5
#define BUFFER_SIZE 4096

// クライアント処理（子プロセスで実行）
void handle_client(int client_sock) {
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc failed");
        close(client_sock);
        exit(1);
    }
    
    // リクエスト受信
    ssize_t bytes_read = read(client_sock, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("read failed");
        free(buffer);
        close(client_sock);
        exit(1);
    }
    buffer[bytes_read] = '\0';
    
    printf("Received request from client:\n%s\n", buffer);
    
    // HTTPレスポンス生成
    size_t response_len = 0;
    char *response = handle_http_request(buffer, &response_len);
    free(buffer);
    
    if (!response) {
        fprintf(stderr, "Failed to generate response\n");
        close(client_sock);
        exit(1);
    }
    
    // レスポンス送信
    ssize_t bytes_written = write(client_sock, response, response_len);
    if (bytes_written < 0) {
        perror("write failed");
    }
    
    free(response);
    close(client_sock);
    exit(0);
}

// SIGCHLDハンドラ（ゾンビプロセス回収）
void sigchld_handler(int signo) {
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // SIGCHLDハンドラ設定
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }
    
    // ソケット作成
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket failed");
        exit(1);
    }
    
    // ソケットオプション設定
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_sock);
        exit(1);
    }
    
    // アドレス設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // バインド
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_sock);
        exit(1);
    }
    
    // リッスン
    if (listen(server_sock, BACKLOG) < 0) {
        perror("listen failed");
        close(server_sock);
        exit(1);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // メインループ
    while (1) {
        // 接続受付
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            if (errno == EINTR) {
                continue; // シグナルで中断された場合は継続
            }
            perror("accept failed");
            continue;
        }
        
        printf("Client connected: %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));
        
        // 子プロセスでクライアント処理
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(client_sock);
            continue;
        }
        
        if (pid == 0) {
            // 子プロセス
            close(server_sock); // 子プロセスではサーバーソケット不要
            handle_client(client_sock);
            // handle_client内でexit()するのでここには到達しない
        } else {
            // 親プロセス
            close(client_sock); // 親プロセスではクライアントソケット不要
        }
    }
    
    close(server_sock);
    return 0;
}
