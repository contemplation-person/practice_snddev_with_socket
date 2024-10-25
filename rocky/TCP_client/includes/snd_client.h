#ifndef SND_CLIENT_H
#define SND_CLIENT_H

#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"
#include "snd_dev.h"

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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <sys/shm.h>

Snd_dev_policy* init_shared_memory();
int             init_msg_queue();
int             init_socket(int *sock, struct sockaddr_in *server_addr, char **argv);
int             set_non_blocking(int sockfd);
int             send_socket(int sockfd, char *msg, int msgid, Snd_dev_policy *shared_msg, int (*make_msg)(char *, int, Snd_dev_policy*));
int             make_socket_header(char *msg, int bodyLen);
Msg_queue       get_msg_queue(int msgid);
int             make_msg(char* msg, int msgid, Snd_dev_policy* shared_msg);

int             is_valid_json(const char *json_string);
json_object*    make_object(Snd_dev_policy_list item, int mType);
void            ari_json_object_print(json_object *json);
int             make_json_body(char *msg, Snd_dev_policy *sdp, int mType);


int             make_rest_header(int mType, char *msg, int json_len);
void            vaild_argc(int argc);
void            create_file(char *filepath);

#endif