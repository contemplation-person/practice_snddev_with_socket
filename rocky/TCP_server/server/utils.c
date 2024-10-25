#include "snd.h"

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

int same_as_akey(RestLibHeadType *rest_msg) {
    static char *akey = "EMG@an#2345&12980!";

    if (strncmp(akey, rest_msg->param1Id, strlen(akey) + 1)) {
        return 0;
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

void validate_rules(t_client *client, int target_fd, Snddev_policy_header *snddev_policy_header) {
    RestLibHeadType* restLibHead = (RestLibHeadType *)(client[target_fd].buf + sizeof(SocketHeader));
    SocketHeader* sockh = (SocketHeader *)(client[target_fd].buf);
    RestMsgType* restMsg = (RestMsgType *)(client[target_fd].buf + sizeof(SocketHeader));

    if (!same_as_akey(restLibHead)) {
        make_server_response_code(sockh, restMsg, RESPONSE_ERROR_AKEY);
        ari_title_print_fd(STDERR_FILENO, "akey error", COLOR_RED_CODE);
    } else if (!parse_rest_msg(restMsg, &(snddev_policy_header[target_fd]))) {
        make_server_response_code(sockh, restMsg, RESPONSE_ERROR_JSON);
        ari_title_print_fd(STDERR_FILENO, "json parse error", COLOR_RED_CODE);
    } else {
        if (exec_query(restLibHead, snddev_policy_header[target_fd])) {
            make_server_response_code(sockh, restMsg, RESPONSE_SUCCESS);
        } else if (sqlca.sqlcode) {
            make_server_response_code(sockh, restMsg, RESPONSE_ERROR_SQL);
        } else {
            make_server_response_code(sockh, restMsg, RESPONSE_UNKNOWN);
        }

        // create_snd_file(snddev_policy_header[target_fd]);
    }

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

int make_server_response_code(SocketHeader *socketHeader, RestMsgType *restMsgType, Res_code_type res_code) {
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