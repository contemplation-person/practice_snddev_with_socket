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
#include <time.h>

#define MAX_DEVICE_ID_LEN 18
#define MAX_MDN_LEN 12
#define MAX_IP_LEN 40
#define MAX_USER_ID_LEN 23
#define MAX_DEVICE_TYPE_LEN 11
#define MAX_DEVICE_NAME_LEN 31
#define MAX_SERIAL_NUMBER_LEN 31

typedef struct 
{
    int auth_type;
    int two_fa_use;
    int device_suspend;
    int id_type; 
    int ip_pool_index;

    char device_id[MAX_DEVICE_ID_LEN];
    char mdn[MAX_MDN_LEN];
    char ip[MAX_IP_LEN];
    char user_id[MAX_USER_ID_LEN];
    char device_type[MAX_DEVICE_TYPE_LEN];

    char device_name[MAX_DEVICE_NAME_LEN];
    char serial_number[MAX_SERIAL_NUMBER_LEN];
} t_element;

#define FORECH_ELEMENT(GENERATE_ELEMENT) \
    GENERATE_ELEMENT(AUTH_TYPE) \
    GENERATE_ELEMENT(TWO_FA_USE) \
    GENERATE_ELEMENT(DEVICE_SUSPEND) \
    GENERATE_ELEMENT(ID_TYPE) \
    GENERATE_ELEMENT(IP_POOL_INDEX) \
    GENERATE_ELEMENT(DEVICE_ID) \
    GENERATE_ELEMENT(MDN) \
    GENERATE_ELEMENT(IP) \
    GENERATE_ELEMENT(USER_ID) \
    GENERATE_ELEMENT(DEVICE_TYPE) \
    GENERATE_ELEMENT(DEVICE_NAME) \
    GENERATE_ELEMENT(SERIAL_NUMBER)


#define GENERATE_ELEMENT_STRING(ELEMENT) #ELEMENT,
#define GENERATE_ELEMENT_ENUM(ELEMENT) ELEMENT,

typedef enum
{
    FORECH_ELEMENT(GENERATE_ELEMENT_ENUM)
    MAX_ELEMENT,
} t_element_str;

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

int make_rest_header(char *msg, int json_len)
{
    char *rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    RestLibHeadType *rest_header = (RestLibHeadType *)(msg + sizeof(SocketHeader));
    static int tid = 0;

    sprintf(rest_header->method, "%s", "POST");
    sprintf(rest_header->tid, "%d", tid++);
    sprintf(rest_header->param2Id, "%s", "127.0.0.1");
    sprintf(rest_header->param1Id, "%s", "EMG@an#2345&12980!");
    sprintf(rest_header->mtype, "%s", rest_str_arr[CREATE_SNDDEV_POLICY]);
    sprintf(rest_header->uri, "%s/%s", "127.0.0.1:3000", "api/emg/policy/SndDev/create");
    sprintf(rest_header->bodyLen, "%d", json_len);

    if (tid > 99999999)
        tid = 0;

    return sizeof(RestLibHeadType);
}

json_object *make_object(t_element element)
{
    json_object *object_element = json_object_new_object();
    char *element_str_arr[] = {FORECH_ELEMENT(GENERATE_ELEMENT_STRING)};

    if (element.auth_type)
        json_object_object_add(object_element, element_str_arr[AUTH_TYPE], json_object_new_int(element.auth_type));
    if (element.two_fa_use)
        json_object_object_add(object_element, element_str_arr[TWO_FA_USE], json_object_new_int(element.two_fa_use));
    if (element.device_suspend)
        json_object_object_add(object_element, element_str_arr[DEVICE_SUSPEND], json_object_new_int(element.device_suspend));
    if (element.id_type)
        json_object_object_add(object_element, element_str_arr[ID_TYPE], json_object_new_int(element.id_type));
    if (element.ip_pool_index)
        json_object_object_add(object_element, element_str_arr[IP_POOL_INDEX], json_object_new_int(element.ip_pool_index));
    if (element.device_id)
        json_object_object_add(object_element, element_str_arr[DEVICE_ID], json_object_new_string(element.device_id));
    if (element.mdn)
        json_object_object_add(object_element, element_str_arr[MDN], json_object_new_string(element.mdn));
    if (element.ip)
        json_object_object_add(object_element, element_str_arr[IP], json_object_new_string(element.ip));
    if (element.user_id)
        json_object_object_add(object_element, element_str_arr[USER_ID], json_object_new_string(element.user_id));
    if (element.device_type)
        json_object_object_add(object_element, element_str_arr[DEVICE_TYPE], json_object_new_string(element.device_type));
    if (element.device_name)
        json_object_object_add(object_element, element_str_arr[DEVICE_NAME], json_object_new_string(element.device_name));
    if (element.serial_number)
        json_object_object_add(object_element, element_str_arr[SERIAL_NUMBER], json_object_new_string(element.serial_number));

    return object_element;
}

void ari_json_object_print(json_object *json)
{
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

int make_json_body(char *msg)
{
    json_object *json_root = json_object_new_object();
    t_element element;
    int json_len;

    json_object_object_add(json_root, "LTEID", json_object_new_string("HANHWA"));
    json_object_object_add(json_root, "SLICE_ID", json_object_new_string("1"));

    json_object *json_array = json_object_new_array();

    bzero(&element, sizeof(t_element));
    element.auth_type = 1;
    element.two_fa_use = 1;
    element.device_suspend = 0;
    element.id_type = 1;
    element.ip_pool_index = 1;

    strcpy(element.device_id, "F2-10-29-21-62-51");
    strcpy(element.mdn, "01029216251");
    strcpy(element.ip, "10.5.0.142");
    strcpy(element.user_id, "SNDHELLOWORLD");
    strcpy(element.device_type, "PC");
    strcpy(element.device_name, "MODEL GOOD");
    strcpy(element.serial_number, "207NZCG009587");

    json_object_array_add(json_array, make_object(element));

    bzero(&element, sizeof(t_element));
    element.auth_type = 0;
    element.two_fa_use = 0;
    element.device_suspend = 1;
    element.id_type = 1;
    element.ip_pool_index = 1;

    strcpy(element.device_id, "F2-11-11-11-11-11");
    strcpy(element.mdn, "22222222222");
    strcpy(element.ip, "11.15.0.111");
    strcpy(element.user_id, "HIHI");
    strcpy(element.device_type, "PC");
    strcpy(element.device_name, "YEAH");
    strcpy(element.serial_number, "333NZ3G003333");

    json_object_array_add(json_array, make_object(element));
    
    json_object_object_add(json_root, "list", json_array);

    ari_title_print_fd(STDOUT_FILENO, "client json object", COLOR_GREEN_CODE);
    ari_json_object_print(json_root);

    json_len = sprintf(msg + sizeof(SocketHeader) + sizeof(RestLibHeadType), "%s", json_object_to_json_string(json_root));
    json_object_put(json_root);

    return json_len;
}

int make_socket_header(char *msg, int bodyLen)
{
    SocketHeader *socket_header = (SocketHeader *)msg;

    socket_header->bodyLen = htonl(bodyLen);
    socket_header->mType = htonl(DEF_MTYPE_CLI_REQ);


    return sizeof(SocketHeader);
}

int make_msg(char *msg) 
{
    int json_len = make_json_body(msg);

    return (json_len + make_rest_header(msg, json_len) + make_socket_header(msg, json_len + sizeof(RestLibHeadType)));
}

void send_socket(int sockfd, char *msg, int (*make_msg)(char *))
{
    int total_len = make_msg(msg);
    int send_len = 0;
    
    // TODO : 인자가 -1일 때 처리
    do 
    {
        send_len += send(sockfd, msg, total_len, 0);
    }while (send_len < total_len);
}

int main(int argc, char **argv) 
{
    int                     sock = 0;
    struct sockaddr_in      server_addr;
    char                    msg[BUFSIZ];
    SocketHeader            *socket_header;
    RestMsgType             *rest_msg;

    bzero(&server_addr, sizeof(server_addr));

    if (argc != 3)
    {
        ari_title_print_fd(STDERR_FILENO, "인자가 다릅니다. ./snd_client [IP] [PORT]", COLOR_RED_CODE);
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

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0)
    {
        ari_print_error("connect error", __FILE__, __LINE__);
        return (-1);
    }

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    for (int i = 0; i < 5; i++)
    {
        clock_t start = clock();

        bzero(&msg, sizeof(BUFSIZ));

        socket_header = (SocketHeader *)msg;
        rest_msg = (RestMsgType *)(msg + sizeof(SocketHeader));

        send_socket(sock, msg, make_msg);

        bzero(&msg, BUFSIZ);
        // TODO :read 받은 데이터를 socket header, rest header, json body로 나눠 담기
        // TODO : 받은 데이터가 ...... 원래 보낸 데이터 보다 커서.... 문제가 생기는 것,,,,,
        // 그렇다면 무작정 보내면 안됨. 보낼 때, 보낼 데이터의 길이를 먼저 보내고, 그 길이만큼 받아야함.
        // 받을 때도.... 그렇게 받아야함.... 
        int recv_len = recv(sock, msg, BUFSIZ, 0);

        printf("recv_len : %d\n", recv_len);

        printf("recv time : %ld\n", clock() - start);

        ari_title_print("recv data", COLOR_GREEN_CODE);
        printf("socket mtype->%d\n", ntohl(socket_header->mType));
        printf("socket header->%d\n", ntohl(socket_header->bodyLen));
        printf("rest code %s\n", rest_msg->header.resCode);
        printf("rest header->%s\n", msg + sizeof(SocketHeader));

        ari_title_print("recv json body", COLOR_MAGENTA_CODE);
        ari_json_object_print(json_tokener_parse(msg + sizeof(SocketHeader) + sizeof(RestLibHeadType)));
    }


    close(sock);
    
    return (0);

}
