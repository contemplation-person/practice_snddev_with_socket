#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"
#include <json-c/json.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 8192

int is_valid_json(const char *json_string) 
{
    struct json_object *parsed_json = json_tokener_parse(json_string);
    
    if (parsed_json == NULL) 
    {
        return 0;
    }

    json_object_put(parsed_json);
    return 1;
}

/*
    헤더에서 해당 키 값을 찾아서 뒷 내용 분석후 저장
    그리고 그걸 서버에 날린다.
 */
/*
    바디에서 들어온 문자열을 모두 찍어주고,
    그 데이터에서 akey를 확인, 틀리면 실패 rest api 날리고
    파싱 데이터에 있는 것으로 파일을 생성 후
    success 메시지 날리기.
 */
/*
    edge case는 뭐가 있을까???????
 */
typedef struct 
{
    SocketHeader    header;
    RestLibHeadType rest_header;
} SendMsgType;

int make_json_body(SendMsgType *msg)
{
    json_object *json_root = json_object_new_object();

    json_object_object_add(json_root, "LTEID", json_object_new_string("HANHWA"));
    json_object_object_add(json_root, "SLICE_ID", json_object_new_string("1"));

    // json array
    json_object *json_array = json_object_new_array();

    json_object_array_add(json_array, json_object_new_string("MAC F2-10-29-21-62-51")); // divice id
    json_object_array_add(json_array, json_object_new_int(1));                          // id_type 
    json_object_array_add(json_array, json_object_new_string("01029216251"));           // MDN
    json_object_array_add(json_array, json_object_new_int(1));                          // IP PULL INDEX
    json_object_array_add(json_array, json_object_new_string("10.5.0.142"));            // IP
    json_object_array_add(json_array, json_object_new_int(1));                          // 추후 AUTH_TYPE이 AUTH_METHOD 인지 확인
    json_object_array_add(json_array, json_object_new_string("SNDHELLOWORLD"));         // USER_ID
    json_object_array_add(json_array, json_object_new_int(1));                          // 2FA_USE
    json_object_array_add(json_array, json_object_new_int(0));                          // DEVICE_SUSPEND
    json_object_array_add(json_array, json_object_new_string("PC"));                    // DEVICE_TYPE
    json_object_array_add(json_array, json_object_new_string("MODEL GOOD"));            // DEVICE_NAME
    json_object_array_add(json_array, json_object_new_string("207NZCG009587"));         // serial number

    json_object_object_add(json_root, "SLICE_ID", json_object_new_string("1"));


    return (0);
}

void create_message(SendMsgType *msg)
{
    int json_len = 0;

    make_json_body(msg);
    make_rest_lib_header_type(msg);
    make_socket_header(msg);
}

int main(int argc, char **argv) 
{
    int                     sock = 0;
    int                     msg_len = 0;
    char                    msg[BUF_SIZE] = {0,};
    struct sockaddr_in      server_addr;
    SendMsgType             msg;

    bzero(&server_addr, sizeof(server_addr));

    if (argc != 3)
    {
        ari_title_print_fd(STDERR_FILENO, "인자가 너무 적습니다. ./snd_client [IP] [PORT]", COLOR_RED_CODE);
        return (-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        ari_print_error("socket error", __FILE__, __LINE__);
        return (-1);
    }

    bzero(&msg, sizeof(SendMsgType));

    // TODO: 나중에 지워
    create_message(msg);
    exit(0);

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        ari_print_error("connect error", __FILE__, __LINE__);
        return (-1);
    }

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    

    while (1)
    {
        // bzero(&res_msg_type, sizeof(res_msg_type))
        //read(STDIN_FILENO, msg, BUF_SIZE);
        // 파싱
        parsing_msg(&res_msg_type, msg, msg_len, argv[1], argv[2]);
        // 저장

        // 전송
        write(STDOUT_FILENO, msg, BUF_SIZE);
    }
    
    return (0);

}
