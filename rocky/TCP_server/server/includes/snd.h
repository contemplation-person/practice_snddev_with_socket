#ifndef SND_H
# define SND_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <json-c/json.h>

#include "libari.h"
#include "medif_api.h"
#include "rest_api_message.h"
#include "host_var_emg.h"

#define CLIENT_NUM 1025
#define MAX_DEVICE_ID_LEN 18
#define MAX_MDN_LEN 12
#define MAX_IP_LEN 40
#define MAX_USER_ID_LEN 23
#define MAX_DEVICE_TYPE_LEN 11
#define MAX_DEVICE_NAME_LEN 31
#define MAX_SERIAL_NUMBER_LEN 31

typedef enum {
    RESPONSE_UNKNOWN,
    RESPONSE_SUCCESS,
    RESPONSE_ERROR_AKEY,
    RESPONSE_ERROR_JSON,
    RESPONSE_ERROR_SQL,
} Res_code_type;

typedef enum { 
	TRY_RECV_SOCKET_HEADER, 
	TRY_RECV_BODY, 
	TRY_WRITE, 
	LEAVE_CLIENT, 
	SND_FATAL_ERROR 
} t_buf_state;

typedef enum
{
    CREATE_SND_DEV_POLICY_ENUM = 1,
    MODIFY_SND_DEV_POLICY_EMUM,
    DELETE_SND_DEV_POLICY_ENUM,
    EMG_MSG_EMUM_MAX
} Emg_msg_type;


typedef struct Snddev_policy{
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
    struct Snddev_policy *next;
} Snddev_policy_list;

typedef struct {
    char lte[20];
    char slice_id[4];
    char real_ip[39];

    Snddev_policy_list *create_snddev_policy_start_list;
    Snddev_policy_list *create_snddev_policy_end_list;
} Snddev_policy_header;

typedef struct {
    t_buf_state buf_state;

    int curent_len;

    char *buf_ptr;
    char buf[BUFSIZ];
} t_client;

int     init_socket(char *port, struct sockaddr_in *servaddr);
int     set_non_blocking(int sockfd);
int     validate_client_connection(int sockfd, int *connfd, struct sockaddr_in *cli, char *allowed_ip, int *len, Snddev_policy_header *sph);

int     alti_connect(char *usr, char *pwd, char *conn_opt);
int     alti_disconnect();
bool    init_indicator(Emg_ind_type *emg_ind);
bool    set_indicator(Emg_ind_type *emg_ind, Emg_type emg);
void    print_alti_error();

int     insert_sql(Emg_type emg, Emg_ind_type emg_ind);
int     update_sql(Emg_type emg, Emg_ind_type emg_ind);
int     delete_sql(Emg_type emg);
int     exec_query(RestLibHeadType* rest_msg,  Snddev_policy_header snddev_policy);

int new_create_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj);
int new_modify_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj);
int new_delete_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj);
int parse_json_list(int mType, struct json_object *parsed_json, Snddev_policy_header *snddev_policy_header);
void ari_json_object_print(json_object *json);
void parse_json_header(struct json_object *parsed_json, Snddev_policy_header *snddev_policy);
int parse_rest_msg(RestMsgType *rest_msg, Snddev_policy_header *snddev_policy);

void init_client_one(t_client *client);
void init_client(t_client *client);
t_buf_state recv_msg(t_client *client, int target_fd);

int same_as_akey(RestLibHeadType *rest_msg);
void check_argc(int argc);
void validate_rules(t_client *client, int target_fd, Snddev_policy_header *snddev_policy_header);

int create_snd_file(Snddev_policy_header snddev_policy);
void clear_csp_list(Snddev_policy_header *snddev_policy_header);
int make_server_response_code(SocketHeader *socketHeader, RestMsgType *restMsgType, Res_code_type res_code);
int send_msg(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set);
#endif 
