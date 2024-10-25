#include "snd_client.h"

Snd_dev_policy* init_shared_memory() {
    int shared_id;
    key_t shared_key;
    Snd_dev_policy *shared_msg;

    shared_key = ftok("/tmp/emg", 24);
    if (shared_key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    shared_id = shmget(shared_key, sizeof(Snd_dev_policy), 0666);
    if  (shared_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shared_msg = shmat(shared_id, NULL, 0);
    if (shared_msg == (Snd_dev_policy *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    return shared_msg;
}

int init_msg_queue() {
    key_t msg_q_key;
    int msgid;

    msg_q_key = ftok("/tmp/emg", 42);
    if (msg_q_key == -1) {
        perror("ftok");
        return false;
    }

    msgid = msgget(msg_q_key, 0666 | IPC_CREAT);
    return msgid;
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

int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
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