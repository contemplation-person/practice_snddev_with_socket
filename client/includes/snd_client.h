#ifndef SND_CLIENT_H
#define SND_CLIENT_H
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

#endif