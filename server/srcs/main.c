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

#include "json-c/json.h"
#include "libari.h"
#include "medif_api.h"
#include "rest_api_message.h"

#define CLIENT_NUM 1025
#define MAX_DEVICE_ID_LEN 18
#define MAX_MDN_LEN 12
#define MAX_IP_LEN 40
#define MAX_USER_ID_LEN 23
#define MAX_DEVICE_TYPE_LEN 11
#define MAX_DEVICE_NAME_LEN 31
#define MAX_SERIAL_NUMBER_LEN 31

typedef enum {
    RESPONSE_SUCCESS = 1,
    RESPONSE_ERROR_AKEY,
    RESPONSE_ERROR_JSON,
} Res_code_type;

typedef enum { 
	TRY_RECV_SOCKET_HEADER, 
	TRY_RECV_BODY, 
	TRY_WRITE, 
	LEAVE_CLIENT, 
	SND_FATAL_ERROR 
} t_buf_state;

typedef struct s_create_snddev_policy_list {
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
    struct s_create_snddev_policy_list *next;
} Create_snddev_policy_list;

typedef struct {
    char lte[20];
    char slice_id[4];
    char real_ip[39];

    Create_snddev_policy_list *create_snddev_policy_start_list;
    Create_snddev_policy_list *create_snddev_policy_end_list;
} Create_snddev_policy_header;

typedef struct {
    t_buf_state buf_state;

    int curent_len;

    char *buf_ptr;
    char buf[BUFSIZ];
} t_client;

int same_as_akey(RestLibHeadType *rest_msg) {
    static char *akey = "EMG@an#2345&12980!";

    if (strncmp(akey, rest_msg->param1Id, strlen(akey) + 1)) {
        return 0;
    }

    return 1;
}

t_buf_state recv_msg(t_client *client, int target_fd) {
    int recv_len;
    int len;
    int total_len;
    int *current_len = &client[target_fd].curent_len;

    char **buf_ptr = &client[target_fd].buf_ptr;
    char **buf = (char **)&client[target_fd].buf;

    t_buf_state *buf_state = &client[target_fd].buf_state;
    t_buf_state state = client[target_fd].buf_state;

    switch (*buf_state) {
        case TRY_RECV_SOCKET_HEADER:
            len = sizeof(SocketHeader) - *current_len;
            break;
        case TRY_RECV_BODY:
            len = ntohl(((SocketHeader *)(buf))->bodyLen) - *current_len;
            break;
        default:
            *buf_state = TRY_RECV_SOCKET_HEADER;
            return LEAVE_CLIENT;
    }

    recv_len = recv(target_fd, *buf_ptr, len, 0);
    total_len = len + *current_len;

    if (recv_len == -1 || recv_len == 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return state;
        }
        return LEAVE_CLIENT;
    }

    if (recv_len + *current_len == total_len) {
        *current_len = 0;
        (*buf_state)++;
    } else {
        *current_len += recv_len;
    }
    *buf_ptr += recv_len;

    return *buf_state;
}

int send_msg(t_client *client, int target_fd) {
    SocketHeader *sockh = (SocketHeader *)(client[target_fd].buf);

    int send_len = 0;
    int goal_len = sizeof(SocketHeader) + ntohl(sockh->bodyLen);
    int total_len;

    // FIXME: 만약 문자가 partial write라면 cnt를 기억하고 다시 보내야함.
    while (send_len < goal_len) {
        total_len = send(target_fd, client[target_fd].buf, goal_len - send_len, 0);
        if (total_len < 1) {
            return total_len;
        }
        send_len += total_len;
    }

    return 1;
}

void check_argc(int argc) {
    if (argc != 2) {
        ari_title_print_fd(STDERR_FILENO, "ARG ERROR [PORT]", COLOR_RED_CODE);
        ari_title_print_fd(STDERR_FILENO, "ex: [4242]", COLOR_RED_CODE);
        exit(EXIT_FAILURE);
    }
}

int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int init_socket(char *port, struct sockaddr_in *servaddr) {
    int sockfd;
    int yes = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        ari_putstr_fd("socket creation failed...\n", 2);
        exit(1);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(socklen_t));

    bzero(servaddr, sizeof(*servaddr));

    servaddr->sin_family = AF_INET;
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port = htons(atoi(port));

    if ((bind(sockfd, (const struct sockaddr *)servaddr, sizeof(*servaddr))) != 0) {
        fprintf(stderr, "bind Error: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 10) != 0) {
        fprintf(stderr, "listen Error: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }

    if (set_non_blocking(sockfd) == -1) {
        fprintf(stderr, "nonblock Error: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }
    return sockfd;
}

int new_create_snddev_policy_list(Create_snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Create_snddev_policy_list *new_list = (Create_snddev_policy_list *)malloc(sizeof(Create_snddev_policy_list));

    if (!new_list) {
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        return 0;
    }

    new_list->auth_type = json_object_get_int(json_object_object_get(csp_list_obj, "AUTH_TYPE"));
    new_list->two_fa_use = json_object_get_int(json_object_object_get(csp_list_obj, "TWO_FA_USE"));
    new_list->device_suspend = json_object_get_int(json_object_object_get(csp_list_obj, "ID_TYPE"));
    new_list->ip_pool_index = json_object_get_int(json_object_object_get(csp_list_obj, "IP_POOL_INDEX"));

    strcpy(new_list->device_id, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_ID")));
    strcpy(new_list->mdn, json_object_get_string(json_object_object_get(csp_list_obj, "MDN")));
    strcpy(new_list->ip, json_object_get_string(json_object_object_get(csp_list_obj, "IP")));
    strcpy(new_list->user_id, json_object_get_string(json_object_object_get(csp_list_obj, "USER_ID")));
    strcpy(new_list->device_type, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_TYPE")));

    strcpy(new_list->device_name, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_NAME")));
    strcpy(new_list->serial_number, json_object_get_string(json_object_object_get(csp_list_obj, "SERIAL_NUMBER")));
    new_list->next = NULL;

    if (snddev_policy_header->create_snddev_policy_start_list == NULL) {
        snddev_policy_header->create_snddev_policy_start_list = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    } else {
        snddev_policy_header->create_snddev_policy_end_list->next = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    }

    return 1;
}

int parse_json_list(struct json_object *parsed_json, Create_snddev_policy_header *snddev_policy_header) {
    struct json_object *csp_list;
    struct json_object *csp_list_obj;
    int list_len;

    if (!json_object_object_get_ex(parsed_json, "list", &csp_list)) {
        printf("list not found\n");
        return 0;
    }

    list_len = json_object_array_length(csp_list);
    for (int idx = 0; idx < list_len; idx++) {
        csp_list_obj = json_object_array_get_idx(csp_list, idx);
        if (!new_create_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
            fprintf(stderr, "new_create_snddev_policy_list error: %s\n", strerror(errno));
            return 0;
        }
    }

    return 1;
}

void ari_json_object_print(json_object *json) {
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

void parse_json_header(struct json_object *parsed_json, Create_snddev_policy_header *snddev_policy) {
    struct json_object *lte_id;
    struct json_object *slice_id;

    if (json_object_object_get_ex(parsed_json, "LTEID", &lte_id)) {
        strcpy(snddev_policy->lte, json_object_get_string(lte_id));
    }

    if (json_object_object_get_ex(parsed_json, "SLICE_ID", &slice_id)) {
        strcpy(snddev_policy->slice_id, json_object_get_string(slice_id));
    }
}

int parse_rest_msg(RestMsgType *rest_msg, Create_snddev_policy_header *snddev_policy) {
    struct json_object *parsed_json;

    parsed_json = json_tokener_parse(rest_msg->jsonBody);
    if (parsed_json == NULL) {
        return 0;
    }

    parse_json_header(parsed_json, snddev_policy);
    if (!parse_json_list(parsed_json, snddev_policy)) {
        json_object_put(parsed_json);
        return 0;
    }

    ari_title_print("parsed_json", COLOR_GREEN_CODE);
    ari_json_object_print(parsed_json);

    json_object_put(parsed_json);
    return 1;
}

int create_snd_file(Create_snddev_policy_header snddev_policy) {
    char file_name[100] = {0};
    int file_num = 0;
    Create_snddev_policy_list *csp_list = snddev_policy.create_snddev_policy_start_list;
    FILE *fp;
    struct tm t;

    system("mkdir -p file_test");
    while (csp_list) {
        sprintf(file_name, "file_test/%s_num_%d", csp_list->device_id, file_num++);
        if (!access(file_name, F_OK)) {
            continue;
        }

        fp = fopen(file_name, "w");
        if (!fp) {
            return 0;
        } else {
            file_num = 0;
        }

        t = *localtime(&(time_t){time(NULL)});

        fprintf(fp, "%-20s: %d-%d-%d %d:%d:%d\n", "생성 시각", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour,
                t.tm_min, t.tm_sec);
        fprintf(fp, "%-20s: %d-%d-%d %d:%d:%d\n", "변경 시각", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour,
                t.tm_min, t.tm_sec);
        fprintf(fp, "%-15s: %s\n", "DEVICE_ID", csp_list->device_id);
        fprintf(fp, "%-15s: %s\n", "LTE_ID", snddev_policy.lte);
        fprintf(fp, "%-15s: %s\n", "SLICE_ID", snddev_policy.slice_id);

        fprintf(fp, "%-15s: %d\n", "AUTH_TYPE", csp_list->auth_type);
        fprintf(fp, "%-15s: %d\n", "TWO_FA_USE", csp_list->two_fa_use);
        fprintf(fp, "%-15s: %d\n", "DEVICE_SUSPEND", csp_list->device_suspend);
        fprintf(fp, "%-15s: %d\n", "ID_TYPE", csp_list->id_type);
        fprintf(fp, "%-15s: %d\n", "IP_POOL_INDEX", csp_list->ip_pool_index);

        fprintf(fp, "%-15s: %s\n", "MDN", csp_list->mdn);
        fprintf(fp, "%-15s: %s\n", "IP", csp_list->ip);
        fprintf(fp, "%-15s: %s\n", "USER_ID", csp_list->user_id);
        fprintf(fp, "%-15s: %s\n", "DEVICE_TYPE", csp_list->device_type);
        fprintf(fp, "%-15s: %s\n", "DEVICE_NAME", csp_list->device_name);
        fprintf(fp, "%-15s: %s\n", "SERIAL_NUMBER", csp_list->serial_number);
        fclose(fp);
        csp_list = csp_list->next;
    }

    return 1;
}

void clear_csp_list(Create_snddev_policy_header *snddev_policy_header) {
    Create_snddev_policy_list *csp_list = snddev_policy_header->create_snddev_policy_start_list;
    Create_snddev_policy_list *next;

    while (csp_list) {
        next = csp_list->next;
        free(csp_list);
        csp_list = next;
    }
    snddev_policy_header->lte[0] = '\0';
    snddev_policy_header->slice_id[0] = '\0';
    snddev_policy_header->real_ip[0] = '\0';
    snddev_policy_header->create_snddev_policy_start_list = NULL;
    snddev_policy_header->create_snddev_policy_end_list = NULL;
}

int make_server_response(SocketHeader *socketHeader, RestMsgType *restMsgType, Res_code_type res_code) {
    socketHeader->mType = htonl(DEF_MTYPE_SVR_RES);

    switch (res_code) {
        case RESPONSE_SUCCESS:
            memmove(restMsgType->header.resCode, "200", 4);
            break;

        case RESPONSE_ERROR_AKEY:
            memmove(restMsgType->header.resCode, "401", 4);
            break;

        case RESPONSE_ERROR_JSON:
            memmove(restMsgType->header.resCode, "400", 4);
            break;
    }

    return res_code;
}

void init_client_one(t_client *client) {
    client->buf_state = TRY_RECV_SOCKET_HEADER;
    client->curent_len = 0;
    client->buf_ptr = client->buf;
    client->buf[0] = '\0';
}

void init_client(t_client *client) {
    for (int i = 0; i < CLIENT_NUM; i++) {
        init_client_one(client + i);
    }
}

void validate_rules(t_client *client, int target_fd, Create_snddev_policy_header *snddev_policy_header) {
    if (!same_as_akey((RestLibHeadType *)(client[target_fd].buf + sizeof(SocketHeader)))) {
        make_server_response((SocketHeader *)(client[target_fd].buf),
                             (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_ERROR_AKEY);
        ari_title_print_fd(STDERR_FILENO, "akey error", COLOR_RED_CODE);
    } else if (!parse_rest_msg((RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)),
                               &(snddev_policy_header[target_fd]))) {
        make_server_response((SocketHeader *)(client[target_fd].buf),
                             (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_ERROR_JSON);
        ari_title_print_fd(STDERR_FILENO, "json parse error", COLOR_RED_CODE);
    } else {
        create_snd_file(snddev_policy_header[target_fd]);
        make_server_response((SocketHeader *)(client[target_fd].buf),
                             (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_SUCCESS);
    }
    ari_title_print_fd(STDOUT_FILENO, "send msg", COLOR_YELLOW_CODE);

}
/**
 * @param sph : Create_snddev_policy_header
 * @return boolean 
 */
int validate_client_connection(int sockfd, int *connfd, struct sockaddr_in *cli, char *allowed_ip, int *len,
                               Create_snddev_policy_header *sph) {
    *connfd = accept(sockfd, (struct sockaddr *)cli, (socklen_t *)len);

    strcpy(sph[*connfd].real_ip, inet_ntoa(cli->sin_addr));
    ari_title_print_fd(STDOUT_FILENO, "connect ip", COLOR_GREEN_CODE);
    ari_title_print_fd(STDOUT_FILENO, sph[*connfd].real_ip, COLOR_GREEN_CODE);

    if (strncmp(sph[*connfd].real_ip, allowed_ip, strlen(allowed_ip)) || *connfd < 0) {
        ari_title_print_fd(STDERR_FILENO, "Not allowed ip", COLOR_RED_CODE);
        close(*connfd);
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    system("ulimit -s 10000");

    struct sockaddr_in servaddr;
    struct sockaddr_in cli;

    int sockfd;
    int connfd;
    int select_cnt;
    int fd_max;
    int len = sizeof(cli);

    char *allowed_ip = "127.0.0.1";

    t_client client[CLIENT_NUM] = {0};
    Create_snddev_policy_header snddev_policy_header[CLIENT_NUM] = {0};
    t_buf_state buf_state;

    check_argc(argc);

    sockfd = init_socket(argv[1], &servaddr);

    fd_set w_fd_set;
    fd_set r_fd_set;
    fd_set r_copy_fd_set;
    fd_set w_copy_fd_set;

    FD_ZERO(&r_fd_set);
    FD_ZERO(&w_fd_set);
    FD_SET(sockfd, &r_fd_set);

    fd_max = sockfd;
    init_client((t_client *)&client);

    while (42) {
        r_copy_fd_set = r_fd_set;
        w_copy_fd_set = w_fd_set;
        select_cnt = select(fd_max + 1, &r_copy_fd_set, &w_copy_fd_set, NULL, NULL);

        for (int target_fd = 3; target_fd <= fd_max && select_cnt > 0; target_fd++) {
            if (FD_ISSET(target_fd, &r_copy_fd_set)) {
                select_cnt--;

                if (target_fd != sockfd) {
                    ari_title_print_fd(STDOUT_FILENO, "recv msg", COLOR_YELLOW_CODE);
                    buf_state = recv_msg(client, target_fd);
                    if (buf_state == LEAVE_CLIENT) {
                        close(target_fd);
                        FD_CLR(target_fd, &w_fd_set);
                        FD_CLR(target_fd, &r_fd_set);
                    }
                    if (buf_state == TRY_WRITE) {
						validate_rules(client, target_fd, snddev_policy_header);
                        FD_SET(target_fd, &w_fd_set);
                    } else {
                        continue;
                    }
                } else {
					if (validate_client_connection(sockfd, &connfd, &cli, allowed_ip, &len, snddev_policy_header)) {
						FD_SET(connfd, &r_fd_set);
						set_non_blocking(connfd);
						if (fd_max < connfd) {
							fd_max = connfd;
						}
					} else {
						continue;
					}
                }
            }
            if (FD_ISSET(target_fd, &w_copy_fd_set)) {
                // 아니면 재시도
                send_msg(client, target_fd);
                if (errno == ECONNRESET || errno == EPIPE) {
                    close(target_fd);
                    FD_CLR(target_fd, &r_fd_set);
                }

                init_client_one(client + target_fd);
                FD_CLR(target_fd, &w_fd_set);

                clear_csp_list(snddev_policy_header + target_fd);
            }
        }
    }
}
