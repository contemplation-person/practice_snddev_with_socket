#include "snd.h"

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

int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
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