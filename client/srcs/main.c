#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"
#include "json-c/json.h"

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

size_t str_delim_len(const char *str, const char delim)
{
    size_t len = 0;

    while (str[len] && delim != str[len])
    {
        len++;
    }

    return (len);
}

const char *pass_char(const char *msg, char *delim) 
{
    while (*msg != '\0') 
    {
        if (strchr(delim, *msg) == NULL) 
        {
            return msg;
        }
        msg++;
    }
    return NULL;
}

int insert_api_method(RestMsgType *type, const char *msg)
{
    char        *method[] = {"POST", "PUT", "GET", "DELETE"};
    int         in_api = 0;
    int         method_index = -1;
    const char  *cpy_msg = pass_char(msg, " \f\n\r\t\v");
    const char  *uri;
    int         uri_len;


    while (in_api == 0 && ++method_index < 4)
    {
        in_api |= !strncmp(method[method_index], cpy_msg, strlen(method[method_index]));
    }
    
    if (!in_api || !isspace(cpy_msg[strlen(method[method_index])]))
    {
        return 0;
    }

    strcpy(type->header.method, method[method_index]);
    uri = pass_char(cpy_msg + strlen(type->header.method), " \f\n\r\t\v");
    uri_len =  str_delim_len(uri, ' ');
    //strncpy(type->header.uri, uri, uri_len);
    type->header.uri[uri_len] = '/';
    
    printf("print type method : %s\n", type->header.method);
    // TODO : 이거 하기 전에 호스트  
    printf("print type uri: %s\n", type->header.uri);

    return 1;
}

int insert_uri(RestMsgType *type, const char *msg)
{
    const char *cpy_msg = pass_char(msg, " \f\n\r\t\v");

    if (strncmp("Host", cpy_msg, 4))
    {
        return 0;
    }

    cpy_msg = pass_char(cpy_msg + 4, " \f\n\r\t\v:");
    printf("cpy_msg: %s\n", cpy_msg);
    //   memmove(type->header.)
    // TODO : 여기부터 url save && 
    
    

    (void) type;
    (void) msg;
    return 2;
}

int insert_akey(RestMsgType *type, const char *msg)
{
    const char *cpy_msg = pass_char(msg, " \f\n\r\t\v");

    if (strncmp("akey", cpy_msg, 4))
    {
        return 0;
    }

    cpy_msg = pass_char(cpy_msg + 4, " \f\n\r\t\v:");
    strncpy(type->header.param1Id, cpy_msg, str_delim_len(cpy_msg, '\n'));

    // printf("type->header.param1Id : %s\n", type->header.param1Id);
    return 4;
}

void insert_tid(RestMsgType *type)
{
    static int   tid;

    tid++;
    if (tid == 2146435072)
        tid = 0;

    memcpy(type->header.tid, &tid, MAX_REST_TID_LEN);
}

void insert_mtype(RestMsgType *type, int type_num)
{
    const char   *rest_api[] = {
        FOREACH_REST_API(GENERATE_REST_API_STRING)
    };

    memcpy(type->header.mtype, rest_api[type_num], strlen(rest_api[type_num]));
}

int insert_body(RestMsgType *type, const char *msg, int bracket_pos, int msg_len)
{
    int      body_len = 0;

    body_len = msg_len - bracket_pos;

    if (!is_valid_json(msg + bracket_pos))
    {
        ari_print_error("wrong json body", __FILE__, __LINE__ );
        return 0;
    }
    memcpy(type->jsonBody, msg + bracket_pos, msg_len);
    memcpy(type->header.bodyLen, &body_len, MAX_REST_BODY_LEN_LEN);

    printf("type->jsonBody : %s\n", type->jsonBody); // TODO : 지워

    return 1;
}

void insert_param2Id(RestMsgType *type, char *ip) 
{
    strcpy(type->header.param2Id, ip);
    printf("res_msg_type : %s \n", type->header.param2Id);
}

/*
 * @brief 
 * 1 = insert_api_method
 * 2 = insert_full_url
 * 4 = insert_akey
 * */
void parsing_msg(RestMsgType *type, const char *msg, int msg_len)
{
    // TODO : 나중에 지워
    printf("읽은 파일 길이 : %ld \n읽은 파일  \n%s \n", strlen(msg), msg);

    const size_t header_len = str_delim_len(msg, '{');
    int          rest_api_num = 5;
    int          line_start_pos = 0;
    int          line_end_pos = str_delim_len(msg, '\n');
    int          checking_num = 0;
    char         *cpy_msg = malloc(header_len + 1);

    if (!insert_body(type, msg, header_len, msg_len))
    {
        return (free(cpy_msg));
    }

    insert_param2Id(type, ip);
    insert_tid(type);
    insert_mtype(type, rest_api_num);

    memmove(cpy_msg, msg, header_len);
    cpy_msg[header_len] = '\0';

    // 아 연결리스트 만들걸..... 
    while (line_start_pos < line_end_pos)
    {
        //ari_title_print_fd(2, msg + line_end_pos, COLOR_GREEN_CODE);
        checking_num |= insert_api_method(type, msg + line_start_pos);
        checking_num |= insert_uri(type, msg + line_start_pos);
        // TODO : host : host + 파싱
        checking_num |= insert_akey(type, msg + line_start_pos);

        line_start_pos = line_end_pos + 1;
        line_end_pos += str_delim_len(msg + line_start_pos, '\n') + 1;
        // printf("str_delim_len: %d\n", (int)*(msg + line_end_pos));
        // printf("line_start_pos %d, line_end_pos : %d\n", line_start_pos, line_end_pos);
    }


    ari_print_error(cpy_msg, __FILE__, __LINE__);
    exit(1);

    return (free(cpy_msg));

    
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
    
}

int main(int argc, char **argv) 
{
    int sock = 0;
    char msg[BUF_SIZE] = {0,};
    int msg_len = 0;

    struct sockaddr_in server_addr;

    SocketHeader sock_header;
    RestMsgType res_msg_type;

    bzero(&sock_header, sizeof(sock_header));
    bzero(&server_addr, sizeof(server_addr));
    // TODO : 보내고 난 뒤 초기화 할 것
    bzero(&res_msg_type, sizeof(res_msg_type));

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

    // TODO: 나중에 지워
    int fd = open("test/reqSet/mcp_req.json", O_RDONLY);
    msg_len = read(fd, msg, BUF_SIZE - 1);


    parsing_msg(&res_msg_type, msg, msg_len);
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
