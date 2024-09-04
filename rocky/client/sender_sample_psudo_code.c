/**
 * @file sender.c
 * @author jbb1004 (jbb1004@arielnetwork.co.kr)
 * @brief 
 * @version 0.1.0
 * @date 2024-08-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include "medif_api.h"
#include "rest_api_message.h"
#include "json-c/json.h"

int tid;

int issue_tid()
{
    int id = tid++;
    if (tid > 99999999)
        tid = 0;
    return id++;
}

int make_body(char *json)
{
    struct json_object *root = json_object_new_object();

    json_object_object_add(root, "AUTH_TYPE", json_object_new_int(10));
    // **** -> array , int, string, object
    return sprintf(json, json_object_get_string(root));
}

void make_resthead(RestLibHeadType *h)
{
    strcpy(h->mtype, "SNDDEV_POLICY");
    sprintf(h->tid, "%d", issue_tid());
    strcpy(h->method, "POST");
}

int make_msg(char *buf)
{
    SocketHeader *sockh = (SocketHeader *)buf;
    int body_len = make_body((char *)(buf + sizeof(SocketHeader) + sizeof(RestLibHeadType)));
    make_resthead((RestLibHeadType *)(buf + sizeof(SocketHeader)));
    sockh->mType = DEF_MTYPE_CLI_REQ;
    sockh->bodyLen = htonl(sizeof(RestLibHeadType) + body_len);
    return sizeof(SocketHeader) + sizeof(RestLibHeadType) + body_len;
}

int main()
{
    char buf[BUFSIZ];
    int len;

    init_socket();
    memset(buf, 0, sizeof(buf));
    len = make_msg(buf);
    send_socket(buf, len);
    while (true) {
        read();
        parse(head + resthead + json);
        printf(json);
        break;
    }
    return 0;
}
