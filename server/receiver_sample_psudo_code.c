/**
 * @file receiver.c
 * @author jbb1004 (jbb1004@arielnetwork.co.kr)
 * @brief 
 * @version 0.1.0
 * @date 2024-08-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include "medif_api.h"
#include "rest_api_message.h"
#include "json-c/json.h"
 #include <arpa/inet.h>

char b[40960];
int b_len;

typedef struct Conn {
    int fd;
    char b[40960];
    int b_len;
} Conn;

Conn conns[BUFSIZ];

typedef struct CreateSnddevPolicy {
    int auth_type;
    ;
} CreateSnddevPolicy;

FILE *fd;

void write_file(CreateSnddevPolicy *p)
{
    fprintf(fd, "AUTH_TYPE     : %d", p->auth_type);
// ***
}

void parse_body(CreateSnddevPolicy *policy, const char *json)
{
    struct json_object *root = json_tokener_parse(json);
    struct json_object *obj;

    obj = json_object_object_get(root, "AUTH_TYPE");
    policy->auth_type = json_object_get_int(obj);
    // ***
}
void make_body()
{}

int parse_msg(char *buf, int len)
{
    SocketHeader *sockh = (void *)buf;
    CreateSnddevPolicy policy;

    if (len < sizeof(SocketHeader)) {
        goto buffer_move;
    }
    if (ntohl(sockh->bodyLen) < len + b_len) {
        goto buffer_move;
    }
    memcpy(b + b_len, buf, len);
    parse_resthead();
    parse_body(&policy, &buf[sizeof(SocketHeader) + sizeof(RestLibHeadType)]);
    write_file(&policy);
    send_response("Success");

    return 1;
buffer_move:
    memcpy(b + b_len, buf, len);
    b_len += len;
    return 0;
}

char *allowd_ip = "127.0.0.1";

int main()
{
    char buf[BUFSIZ];
    int connfd, listen_fd;
    char str_addr[32];
    socklen_t addrlen;
    struct sockaddr_in addr;

    init_socket(); // server socket
    while (true) {
        connfd = accept(listen_fd, (struct sockaddr *)&addr, &addrlen);
        inet_ntop(AF_INET, &addr, str_addr, addrlen);
        if (strcmp(str_addr, allowd_ip) != 0) {
            close(connfd);
        }
        int n = read(connfd, buf, sizeof(buf));
        parse_msg(buf, n);
    }
}
