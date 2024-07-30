#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 8192

void parsing_json_string(RestMsgType *type, const char *message)
{
    
    
}

int main(int argc, char **argv) 
{
    int sock;
    char message[BUF_SIZE] = {0,};
    struct sockaddr_in server_addr;

    RestMsgType res_msg_type;
    SocketHeader sock_header;


    if (argc != 3)
    {
        ari_title_print_fd(STDERR_FILENO, "인자가 너무 적습니다. ./snd_client [IP] [PORT]", COLOR_RED_CODE);
        return (-1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        ari_print_error("socket error", __FILE__, __LINE__);
        return (-1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        ari_print_error("connect error", __FILE__, __LINE__);
        return (-1);
    }

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    

    while (1)
    {
        bzero(&t, sizeof(t));
        read(STDIN_FILENO, message, BUF_SIZE);
        parsing_json_string(&res_msg_type, message);
        // 파싱
        // 저장

        // 전송
        write(STDOUT_FILENO, message, BUF_SIZE);
    }
    
    return (0);

}
