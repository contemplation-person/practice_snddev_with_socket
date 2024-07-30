#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 8192

void parsing_json_string(RestMsgType *type, const char *message)
{
    // TODO : 나중에 지워
    (void)type;
    printf("읽은 파일 길이 : %ld \n읽은 파일  \n%s \n", strlen(message), message);

/*
    문자열을 한바퀴 돌면서 {랑 }로 바디랑 헤더 분리
    분리된 바디는 type에 저장.

    { = NULL로 변환,
    그러면 헤더만 깔끔히 분리 가능,
    헤더에서 해당 키 값을 찾아서 뒷 내용 분석후 저장
    그리고 그걸 서버에 날린다.
 */
/*
    바디에서는 들어온 문자열을 모두 찍어주고,
    그 데이터에서 akey를 확인, 틀리면 실패 rest api 날리고
    파싱 데이터에 있는 것으로 파일을 생성 후
    success 메시지 날리기.
 */
/*
    edge case는 뭐가 있을까???????
    혹시 데이터 문자열에 {게 있으면?}
    bracket을 확인한다. 하나인지.짝이 맞는지?
 */
    
}

int main(int argc, char **argv) 
{
    int sock;
    char message[BUF_SIZE] = {0,};
    struct sockaddr_in server_addr;

    SocketHeader sock_header;
    RestMsgType res_msg_type;

    bzero(&sock_header, sizeof(sock_header));
    bzero(&res_msg_type, sizeof(res_msg_type));
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

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

    // TODO: 나중에 지워
    int fd = open("test/reqSet/mcp_req.json", O_RDONLY);
    read(fd, message, BUF_SIZE);
    parsing_json_string(&res_msg_type, message);
    exit(0);

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        ari_print_error("connect error", __FILE__, __LINE__);
        return (-1);
    }

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    

    while (1)
    {
        //read(STDIN_FILENO, message, BUF_SIZE);
        parsing_json_string(&res_msg_type, message);
        // 파싱
        // 저장

        // 전송
        write(STDOUT_FILENO, message, BUF_SIZE);
    }
    
    return (0);

}
