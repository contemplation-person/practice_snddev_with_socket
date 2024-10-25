#include "snd_client.h"

int main(int argc, char **argv) {
    int sock;
    int msgid;
    struct sockaddr_in server_addr;
    char msg[BUFSIZ];
    int recv_len;
    Snd_dev_policy *shared_msg;


    vaild_argc(argc);

    bzero(&server_addr, sizeof(server_addr));

    init_socket(&sock, &server_addr, argv);
    set_non_blocking(sock);

    ari_title_print_fd(STDIN_FILENO, "connect server", COLOR_GREEN_CODE);

    create_file("/tmp/emg");
    msgid = init_msg_queue();
    shared_msg = init_shared_memory();

    while (42) {
        send_socket(sock, msg, msgid, shared_msg, make_msg);

        recv_len = recv(sock, msg, BUFSIZ, 0);
        if (recv_len > 0) {
            ari_title_print("---------recv data------------", COLOR_GREEN_CODE);
            ari_title_print("Rest code", COLOR_YELLOW_CODE);
            printf("%s\n", ((RestMsgType *)(msg + sizeof(SocketHeader)))->header.resCode);
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
