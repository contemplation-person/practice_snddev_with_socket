#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"
#include "json-c/json.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define CLIENT_NUM  1025

typedef struct
{
    char buf[BUFSIZ];
} t_client;

int recv_(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set)
{
    int goal_len = BUFSIZ;
    int start_pos = 0;
	int recv_len = recv(target_fd, client + target_fd, goal_len, 0);

	if (0 > recv_len) 
        return 0;
	else if (recv_len == 0)
	{
        // client leave
        bzero(client + target_fd, sizeof(t_client));
		close(target_fd);
		FD_CLR(target_fd, w_fd_set);
		FD_CLR(target_fd, r_fd_set);
	} 
	else 
	{
		printf("goal_len : %d, recv_len : %d, start_pos : %d\n", goal_len, recv_len, start_pos);
		goal_len -= recv_len;
		while (0 < goal_len)
		{
			recv_len = recv(target_fd, client[target_fd].buf + start_pos, goal_len, 0);
			goal_len -= recv_len;
			start_pos += recv_len;
		}
		FD_SET(target_fd, w_fd_set);
	}
	return 1;
}

void send_(t_client *client, int target_fd, fd_set *w_fd_set)
{
    int goal_len = BUFSIZ;
    int send_len = 0;
    int start_pos = 0;

    do 
    {
        send_len = send(target_fd , client + target_fd + start_pos, goal_len, 0);
		if (send_len == -1)
		{
			fprintf(stderr, "send Error: %s\n", strerror(errno));
			break;
		}
		start_pos += send_len;
        goal_len -= send_len;
    } while (goal_len > 0);

    // 해당 클라이언트 초기화
    bzero(client + target_fd, sizeof(t_client));
    FD_CLR(target_fd, w_fd_set);
}

void check_argc(int argc)
{
    if (argc != 2)
    {
        ari_title_print_fd(STDERR_FILENO, "ARG ERROR [PORT]", COLOR_RED_CODE);
        ari_title_print_fd(STDERR_FILENO, "ex: [4242]", COLOR_RED_CODE);
        exit(EXIT_FAILURE);
    }
}

int init_socket(char *port, struct sockaddr_in *servaddr)
{
    int sockfd;
    int true = 1;

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		ari_putstr_fd("socket creation failed...\n", 2); 
		exit(1); 
	} 

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&true, sizeof(socklen_t));

	bzero(servaddr, sizeof(*servaddr)); 

	// assign IP, PORT 
	servaddr->sin_family = AF_INET; 
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY); //127.0.0.1
	servaddr->sin_port = htons(atoi(port)); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)servaddr, sizeof(*servaddr))) != 0) { 
		ari_putstr_fd("socket bind failed...\n", 2); 
		close(sockfd);
		exit(1); 
	} 
	if (listen(sockfd, 10) != 0) {
		ari_putstr_fd("cannot listen\n", 2); 
		close(sockfd);
		exit(1); 
	}
    return sockfd;
}

int main(int argc, char **argv) {
	struct sockaddr_in servaddr;
	struct sockaddr_in cli; 

	int sockfd;
    int connfd;
	int s_cnt;
    int fd_max;

	int id = 0;
    int len = sizeof(cli);

	t_client client[CLIENT_NUM] = {0,};

    check_argc(argc);

    sockfd = init_socket(argv[1], &servaddr);

	fd_set w_fd_set;
	fd_set r_fd_set;
	fd_set w_copy_fd_set;
	fd_set r_copy_fd_set;

	FD_ZERO(&r_fd_set);
	FD_ZERO(&w_fd_set);
	FD_SET(sockfd, &r_fd_set);

	fd_max = sockfd;
	while (42)
	{
		w_copy_fd_set = w_fd_set;
		r_copy_fd_set = r_fd_set;
		s_cnt = select(fd_max + 1, &r_copy_fd_set, &w_copy_fd_set, NULL, NULL);
		for(int target_fd = 3; target_fd <= fd_max && s_cnt > 0; target_fd++)
		{
            if (FD_ISSET(target_fd, &w_copy_fd_set))
			{
				send_(client, target_fd, &w_fd_set);
                s_cnt--;
            }
			if (FD_ISSET(target_fd, &r_copy_fd_set))
			{
				if (target_fd != sockfd)
				{
					printf("in target_fd : %d\n", target_fd);
					if (!recv_(client, target_fd, &r_fd_set, &w_fd_set))
					{
						for (int i = 3; i <= fd_max; i++)
						{
							if (client[i].buf)
								close(target_fd);
						}
                        close(sockfd);
                        fprintf(stderr, "recv Error: %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
					}
					fprintf(stderr, "target fd : %d\n", target_fd);
				}
				else
				{
					connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
					if (connfd < 0) 
						continue;
                    FD_SET(connfd, &r_fd_set);
					fprintf(stderr, "connfd : %d\n", connfd);
                    if (fd_max < connfd)
						fd_max = connfd;
					id++;
				}
                s_cnt--;
            }
        }
	}
}
