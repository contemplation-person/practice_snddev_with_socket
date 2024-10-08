#include "snd.h"

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

int new_create_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
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

int new_modify_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
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

int new_delete_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
    if (!new_list) {
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        return 0;
    }

    strcpy(new_list->device_id, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_ID")));
    new_list->device_suspend = json_object_get_int(json_object_object_get(csp_list_obj, "ID_TYPE"));

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

int parse_json_list(int mType, struct json_object *parsed_json, Snddev_policy_header *snddev_policy_header) {
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
        switch (mType)
        {
            case CREATE_SNDDEV_POLICY:
                if (!new_create_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_create_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
            case MODIFY_SNDDEV_POLICY:
                if (!new_modify_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_modify_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
            case DELETE_SNDDEV_POLICY:
                if (!new_delete_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_delete_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
        }
    }

    return 1;
}

void ari_json_object_print(json_object *json) {
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

void parse_json_header(struct json_object *parsed_json, Snddev_policy_header *snddev_policy) {
    struct json_object *lte_id;
    struct json_object *slice_id;

    if (json_object_object_get_ex(parsed_json, "LTEID", &lte_id)) {
        strcpy(snddev_policy->lte, json_object_get_string(lte_id));
    }

    if (json_object_object_get_ex(parsed_json, "SLICE_ID", &slice_id)) {
        strcpy(snddev_policy->slice_id, json_object_get_string(slice_id));
    }
}

int parse_rest_msg(RestMsgType *rest_msg, Snddev_policy_header *snddev_policy) {
    struct json_object *parsed_json;
    char* rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    int mType = REST_API_UNKNOWN;

    while (mType < REST_API_MAX) {
        if (!strcmp(rest_msg->header.mtype, rest_str_arr[mType])) {
            break;
        }
        mType++;
    }

    parsed_json = json_tokener_parse(rest_msg->jsonBody);
    if (parsed_json == NULL) {
        return 0;
    }
    
    parse_json_header(parsed_json, snddev_policy);
        if (!parse_json_list(mType, parsed_json, snddev_policy)) {
            json_object_put(parsed_json);
            return 0;
        }

    ari_title_print("parsed_json", COLOR_GREEN_CODE);
    ari_json_object_print(parsed_json);

    json_object_put(parsed_json);
    return 1;
}

int create_snd_file(Snddev_policy_header snddev_policy) {
    char file_name[100] = {0};
    int file_num = 0;
    Snddev_policy_list *csp_list = snddev_policy.create_snddev_policy_start_list;
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

void clear_csp_list(Snddev_policy_header *snddev_policy_header) {
    Snddev_policy_list *csp_list = snddev_policy_header->create_snddev_policy_start_list;
    Snddev_policy_list *next;

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
        
        case RESPONSE_ERROR_SQL:
            memmove(restMsgType->header.resCode, "400", 4);
            break;
        
        case RESPONSE_UNKNOWN:
            memmove(restMsgType->header.resCode, "404", 4);
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

int exec_query(RestLibHeadType* rest_msg,  Snddev_policy_header snddev_policy) {
    char *rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    Emg_ind_type emg_ind = {0};
    Emg_type emg = {0};

    strcpy(emg.LTEID, snddev_policy.lte);
    strcpy(emg.SLICE_ID, snddev_policy.slice_id);
    strcpy(emg.MDN, snddev_policy.create_snddev_policy_start_list->mdn);

    emg.IP_POOL_INDEX = snddev_policy.create_snddev_policy_start_list->ip_pool_index;
    strcpy(emg.IP, snddev_policy.create_snddev_policy_start_list->ip);
    emg.AUTH_TYPE = snddev_policy.create_snddev_policy_start_list->auth_type;
    strcpy(emg.USER_ID, snddev_policy.create_snddev_policy_start_list->user_id);
    emg.TWOFA_USE = snddev_policy.create_snddev_policy_start_list->two_fa_use;
    emg.DEVICE_SUSPEND = snddev_policy.create_snddev_policy_start_list->device_suspend;

    strcpy(emg.DEVICE_TYPE, snddev_policy.create_snddev_policy_start_list->device_type);
    strcpy(emg.MODEL_NAME, snddev_policy.create_snddev_policy_start_list->device_name);
    strcpy(emg.SERIAL_NUM, snddev_policy.create_snddev_policy_start_list->serial_number);
    strcpy(emg.DEVICE_ID, snddev_policy.create_snddev_policy_start_list->device_id); 
    emg.ID_TYPE = snddev_policy.create_snddev_policy_start_list->id_type;

    set_indicator(&emg_ind, emg);

    for (int idx = REST_API_UNKNOWN; idx < REST_API_MAX; idx++) {
        if (!strcmp(rest_msg->mtype, rest_str_arr[idx])) {
            switch (idx)
            {
            case CREATE_SNDDEV_POLICY:
                if (insert_sql(emg, emg_ind) == false)
                    return REST_API_UNKNOWN;
                break;
            case MODIFY_SNDDEV_POLICY:
                if (update_sql(emg, emg_ind) == false)
                    return REST_API_UNKNOWN;
                break;
            case DELETE_SNDDEV_POLICY:
                if (delete_sql(emg) == false)
                    return REST_API_UNKNOWN;
                break;
            }

            return idx;
        }
    }

    return REST_API_UNKNOWN;
}

void validate_rules(t_client *client, int target_fd, Snddev_policy_header *snddev_policy_header) {
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
        
        if (exec_query((RestLibHeadType *)(client[target_fd].buf + sizeof(SocketHeader)), snddev_policy_header[target_fd])) {
            make_server_response((SocketHeader *)(client[target_fd].buf),
                                 (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_SUCCESS);
        } else if (sqlca.sqlcode) {
            make_server_response((SocketHeader *)(client[target_fd].buf),
                                 (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_ERROR_SQL);
        }
        else {
            make_server_response((SocketHeader *)(client[target_fd].buf),
                                 (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader)), RESPONSE_UNKNOWN);
        }

        // create_snd_file(snddev_policy_header[target_fd]);
    }

}
/**
 * @param sph : Snddev_policy_header
 * @return boolean 
 */
int validate_client_connection(int sockfd, int *connfd, struct sockaddr_in *cli, char *allowed_ip, int *len,
                               Snddev_policy_header *sph) {
    *connfd = accept(sockfd, (struct sockaddr *)cli, (socklen_t *)len);

    strcpy(sph[*connfd].real_ip, inet_ntoa(cli->sin_addr));
    ari_title_print_fd(STDOUT_FILENO, "connect ip", COLOR_GREEN_CODE);
    ari_title_print_fd(STDOUT_FILENO, sph[*connfd].real_ip, COLOR_YELLOW_CODE);

    if (strncmp(sph[*connfd].real_ip, allowed_ip, strlen(allowed_ip)) || *connfd < 0) {
        ari_title_print_fd(STDERR_FILENO, "Not allowed ip", COLOR_RED_CODE);
        close(*connfd);
        return false;
    }

    return true;
}


int send_msg(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set) {
    SocketHeader *sockh = (SocketHeader *)(client[target_fd].buf);
    int *current_len = &client[target_fd].curent_len;
    char **buf_ptr = &client[target_fd].buf_ptr;

    int send_len;
    int goal_len = sizeof(SocketHeader) + ntohl(sockh->bodyLen);

    send_len = send(target_fd, client[target_fd].buf, goal_len - *current_len, 0);
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        *current_len += send_len;
        *buf_ptr += send_len;
        return false;
    } else if (send_len == -1 || send_len == 0) {
        close(target_fd);
        FD_CLR(target_fd, r_fd_set);
        FD_CLR(target_fd, w_fd_set);
        return false;
    } 

    init_client_one((client + target_fd));
    FD_CLR(target_fd, w_fd_set);

    return true;
}

int main(int argc, char **argv) {
    struct sockaddr_in servaddr;
    struct sockaddr_in cli;

    int sockfd;
    int connfd;
    int select_cnt;
    int fd_max;
    int len = sizeof(cli);

    char *allowed_ip = "172.17.0.1";

    t_client client[CLIENT_NUM] = {0};
    Snddev_policy_header snddev_policy_header[CLIENT_NUM] = {0};
    t_buf_state buf_state;

    check_argc(argc);

    sockfd = init_socket(argv[1], &servaddr);

    if (alti_connect("EMG", "emg123", "Server=172.17.0.3;CONNTYPE=1") == false) {
        print_alti_error();
        exit(1);
    }

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

        for (int target_fd = 3; target_fd <= fd_max ; target_fd++) {
            if (FD_ISSET(target_fd, &r_copy_fd_set)) {
                select_cnt--;

                if (target_fd != sockfd) {
                    buf_state = recv_msg(client, target_fd);
                    if (buf_state == LEAVE_CLIENT) {
                        close(target_fd);
                        FD_CLR(target_fd, &w_fd_set);
                        FD_CLR(target_fd, &r_fd_set);
                    }
                    if (buf_state == TRY_WRITE) {
                        printf("location");
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
                if (send_msg(client, target_fd, &r_fd_set, &w_fd_set)) {
                    clear_csp_list(snddev_policy_header + target_fd);
                }
            }
        }
    }

    alti_disconnect();
}
