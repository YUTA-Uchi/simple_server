#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define LOCAL_HOST "localhost"
#define SERVER_PORT "8080"
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int sockfd;
    char *send_buffer = NULL;
    char *recv_buffer = NULL;
    struct addrinfo hints, *res;
    const char *query = "2%2B10"; // デフォルト: 2+10
    
    // コマンドライン引数から数式を取得
    if (argc > 1) {
        query = argv[1];
    }
    
    // バッファの動的確保
    send_buffer = malloc(BUFFER_SIZE);
    recv_buffer = malloc(BUFFER_SIZE);
    if (!send_buffer || !recv_buffer) {
        perror("malloc");
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    // サーバーをホスト名で取得

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(LOCAL_HOST, SERVER_PORT, &hints, &res) != 0) {
        fprintf(stderr, "ERROR, no such host\n");
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    // ソケット作成
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("socket");
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    // サーバーに接続
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(sockfd);
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    // HTTP GETリクエスト作成
    int len = snprintf(send_buffer, BUFFER_SIZE,
                      "GET /calc?query=%s HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n", query);
    
    if (len >= BUFFER_SIZE) {
        fprintf(stderr, "Request too long\n");
        close(sockfd);
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    printf("Sending request:\n%s\n", send_buffer);
    
    // リクエスト送信
    ssize_t bytes = write(sockfd, send_buffer, strlen(send_buffer));
    if (bytes < 0) {
        perror("write");
        close(sockfd);
        free(send_buffer);
        free(recv_buffer);
        return EXIT_FAILURE;
    }
    
    // レスポンス受信
    printf("Response:\n");
    memset(recv_buffer, 0, BUFFER_SIZE);
    while ((bytes = read(sockfd, recv_buffer, BUFFER_SIZE - 1)) > 0) {
        recv_buffer[bytes] = '\0';
        printf("%s", recv_buffer);
        memset(recv_buffer, 0, BUFFER_SIZE);
    }
    if (bytes < 0) {
        perror("read");
    }
    printf("\n");
    
    // クリーンアップ
    close(sockfd);
    freeaddrinfo(res);
    free(send_buffer);
    free(recv_buffer);
    return EXIT_SUCCESS;
}
