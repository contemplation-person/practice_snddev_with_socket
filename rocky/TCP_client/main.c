#include "snd_client.h"

int is_valid_json(const char *json_string) {
    struct json_object *parsed_json = json_tokener_parse(json_string);

    if (parsed_json == NULL) {
        return 0;
    }

    json_object_put(parsed_json);
    return 1;
}

// msg type을 저장
int make_rest_header(int mType, char *msg, int json_len) {
    char *rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    RestLibHeadType *rest_header = (RestLibHeadType *)(msg + sizeof(SocketHeader));
    static int tid = 0;

    switch (mType) {
        case CREATE_SND_DEV_POLICY_ENUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[CREATE_SNDDEV_POLICY]);
            break;
        case MODIFY_SND_DEV_POLICY_EMUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[MODIFY_SNDDEV_POLICY]);
            break;
        case DELETE_SND_DEV_POLICY_ENUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[DELETE_SNDDEV_POLICY]);
            break;
    }

    sprintf(rest_header->method, "%s", "POST");
    sprintf(rest_header->tid, "%d", tid++);
    sprintf(rest_header->param2Id, "%s", "127.0.0.1");
    sprintf(rest_header->param1Id, "%s", "EMG@an#2345&12980!");
    sprintf(rest_header->uri, "%s/%s", "127.0.0.1:3000", "api/emg/policy/SndDev/create");
    sprintf(rest_header->bodyLen, "%d", json_len);

    if (tid > 99999999) tid = 0;

    return sizeof(RestLibHeadType);
}

json_object *make_object(Snd_dev_policy_list item, int mType) {
    json_object *obj_item = json_object_new_object();
    char *item_str_arr[] = {FOREACH_ELEMENT(GENERATE_ELEMENT_STRING)};

    if (item.device_id)
        json_object_object_add(obj_item, item_str_arr[DEVICE_ID], json_object_new_string(item.device_id));
    if (item.device_type)
        json_object_object_add(obj_item, item_str_arr[DEVICE_TYPE], json_object_new_string(item.device_type));

    if (mType == DELETE_SND_DEV_POLICY_ENUM) 
        return obj_item;

    json_object_object_add(obj_item, item_str_arr[AUTH_TYPE], json_object_new_int(item.auth_type));
    json_object_object_add(obj_item, item_str_arr[TWO_FA_USE], json_object_new_int(item.two_fa_use));
    json_object_object_add(obj_item, item_str_arr[DEVICE_SUSPEND], json_object_new_int(item.device_suspend));
    json_object_object_add(obj_item, item_str_arr[ID_TYPE], json_object_new_int(item.id_type));
    json_object_object_add(obj_item, item_str_arr[IP_POOL_INDEX], json_object_new_int(item.ip_pool_index));

    if (item.mdn) json_object_object_add(obj_item, item_str_arr[MDN], json_object_new_string(item.mdn));
    if (item.ip) json_object_object_add(obj_item, item_str_arr[IP], json_object_new_string(item.ip));
    if (item.user_id) json_object_object_add(obj_item, item_str_arr[USER_ID], json_object_new_string(item.user_id));
    if (item.device_name)
        json_object_object_add(obj_item, item_str_arr[DEVICE_NAME], json_object_new_string(item.device_name));
    if (item.serial_number)
        json_object_object_add(obj_item, item_str_arr[SERIAL_NUMBER], json_object_new_string(item.serial_number));

    return obj_item;
}

void ari_json_object_print(json_object *json) {
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

/*
*@param sdp : snd_dev_policy 구조체
*/
int make_json_body(char *msg, Snd_dev_policy *sdp, int mType) {
    json_object *json_root = json_object_new_object();
    int json_len;
    char *target_buf;

    json_object_object_add(json_root, "LTEID", json_object_new_string(sdp->lte_id));
    json_object_object_add(json_root, "SLICE_ID", json_object_new_string(sdp->slice_id));

    json_object *json_array = json_object_new_array();

    for (int idx = 0; idx < sdp->max_list_idx; idx++) {
        json_object_array_add(json_array, make_object(sdp->snd_dev_policy[idx], mType));
    }

    json_object_object_add(json_root, "list", json_array);

    ari_title_print_fd(STDOUT_FILENO, "client json object", COLOR_GREEN_CODE);
    ari_json_object_print(json_root);

    bzero(msg, BUFSIZ);

    target_buf = msg + sizeof(SocketHeader) + sizeof(RestLibHeadType);
    json_len = sprintf(target_buf, "%s", json_object_to_json_string(json_root));
    json_object_put(json_root);

    return json_len;
}

int make_socket_header(char *msg, int bodyLen) {
    SocketHeader *socket_header = (SocketHeader *)msg;

    socket_header->bodyLen = htonl(bodyLen);
    socket_header->mType = htonl(DEF_MTYPE_CLI_REQ);

    return sizeof(SocketHeader);
}

Msg_queue get_msg_queue(int msgid) {
    Msg_queue msg_queue = {0};

    if (msgrcv(msgid, &msg_queue, sizeof(Msg_queue), 0, IPC_NOWAIT) == -1) {
        msg_queue.msg_type = -1;
    }

    return msg_queue;
}

int make_msg(char* msg, int msgid, Snd_dev_policy* shared_msg) {
    int json_len;
    Msg_queue msg_q = get_msg_queue(msgid);

    if (CREATE_SND_DEV_POLICY_ENUM > msg_q.msg_type || msg_q.msg_type > DELETE_SND_DEV_POLICY_ENUM) {
        return false;
    }

    json_len = make_json_body(msg, (Snd_dev_policy *)shared_msg, msg_q.msg_type);

    return (json_len + make_rest_header(msg_q.msg_type, msg, json_len) + make_socket_header(msg, json_len + sizeof(RestLibHeadType)));
}

int send_socket(int sockfd, char *msg, int msgid, Snd_dev_policy *shared_msg, int (*make_msg)(char *, int, Snd_dev_policy*)) {
    int total_len = make_msg(msg, msgid, shared_msg);
    int send_len = 0;

    if (total_len == false) {
        return false;
    }

    send_len = send(sockfd, msg, total_len, 0);
    if (send_len == -1) {
        perror("send");
        return false;
    }

    return true;
}

int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

void vaild_argc(int argc) {
    if (argc != 3) {
        ari_title_print_fd(STDERR_FILENO, "인자가 다릅니다. ./snd_client [IP] [PORT]", COLOR_RED_CODE);
        exit(-1);
    }
}

int init_socket(int *sock, struct sockaddr_in *server_addr, char **argv) {
    server_addr->sin_family = AF_INET;
    // server_addr->sin_addr.s_addr = inet_addr(argv[1]);
    inet_pton(AF_INET, argv[1], &(server_addr->sin_addr));
    server_addr->sin_port = htons(atoi(argv[2]));

    *sock = socket(server_addr->sin_family, SOCK_STREAM, 0);
    if (*sock == -1) {
        ari_print_error("socket error", __FILE__, __LINE__);
        exit(-1);
    }

    if (connect(*sock, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)) != 0) {
        ari_print_error("connect error", __FILE__, __LINE__);
        perror("connect");
        exit(-1);
    }

    return true;
}

int main(int argc, char **argv) {
    key_t msg_q_key;
    struct sockaddr_in server_addr;

    char msg[BUFSIZ];

    int sock;
    int msgid;
    int recv_len;

    int shared_id;
    key_t shared_key;
    Snd_dev_policy *shared_msg;
    RestMsgType *rest_msg;

    vaild_argc(argc);

    bzero(&server_addr, sizeof(server_addr));

    init_socket(&sock, &server_addr, argv);
    set_non_blocking(sock);

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    rest_msg = (RestMsgType *)(msg + sizeof(SocketHeader));

    msg_q_key = ftok("/tmp/emg", 42);
    if (msg_q_key == -1) {
        perror("ftok");
        return false;
    }

    msgid = msgget(msg_q_key, 0666 | IPC_CREAT);

    shared_key = ftok("/tmp/emg", 24);
    if (shared_key == -1) {
        perror("ftok");
        return false;
    }

    shared_id = shmget(shared_key, sizeof(Snd_dev_policy), 0666);
    if  (shared_id == -1) {
        perror("shmget");
        return false;
    }
    shared_msg = shmat(shared_id, NULL, 0);
    if (shared_msg == (Snd_dev_policy *)(-1)) {
        perror("shmat");
    
        return false;
    }

    while (42) {
        send_socket(sock, msg, msgid, shared_msg, make_msg);

        recv_len = recv(sock, msg, BUFSIZ, 0);
        if (recv_len > 0) {
            ari_title_print("---------recv data------------", COLOR_GREEN_CODE);
            ari_title_print("Rest code", COLOR_YELLOW_CODE);
            printf("%s\n", rest_msg->header.resCode);
            ari_title_print("Rest header", COLOR_YELLOW_CODE);
            printf("%s\n", msg + sizeof(SocketHeader));
            ari_title_print("recv json body", COLOR_MAGENTA_CODE);
            ari_json_object_print(json_tokener_parse(msg + sizeof(SocketHeader) + sizeof(RestLibHeadType)));
            ari_title_print("------------------------------", COLOR_GREEN_CODE);
        }
    }

    msgctl(msgid, IPC_RMID, NULL);
    close(sock);

    return (0);
}
