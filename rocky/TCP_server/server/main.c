#include "snd.h"

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

    if (alti_connect("EMG", "emg123", "Server=172.17.0.2;CONNTYPE=1") == false) {
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
