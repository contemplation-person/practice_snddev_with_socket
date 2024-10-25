#ifndef MSG_Q_H
#define MSG_Q_H

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/shm.h>

#include "snd_dev.h"
#include "libari.h"

bool    is_poilcy_string(char *token, int max_len, char *msgType);
bool    is_poilcy_int(int num, char *msgType);

bool    display_data_list(int type);
bool    display_type_list();

bool    save_shared_memory_data(int type, Snd_dev_policy* shared_msg);

int     read_type(char* msg);

char*   make_create_snd_dev_policy(char *sdp_msg);
char*   make_modify_snd_dev_policy(char *sdp_msg);
char*   make_delete_snd_dev_policy(char *sdp_msg);

#endif
