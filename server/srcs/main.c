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
#include <fcntl.h>
#include <arpa/inet.h>

#define CLIENT_NUM  1025
#define LEAVE_CLIENT 0

typedef struct
{
    char buf[BUFSIZ];
} t_client;

int recv_binary(int target_fd, char *binary, int binary_len)
{
	int recv_len = 0;
	int goal_len = binary_len;
	int len = 0;

	while (recv_len < goal_len)
	{
		len = recv(target_fd, binary + recv_len, goal_len - recv_len, 0);
		if (len == 0)
		{
			return 0;
		}
		recv_len += len;
	}
	// TODO: len이 -1인 상황 생각해볼 것,
	return 1;
}

int check_akey(char *rest_msg)
{
	static char *akey = "EMG@an#2345&12980!";

	if (strncmp(akey, rest_msg, strlen(akey) + 1))
	{
		return 0;
	}
	return 1;
}

int check_json(char *json)
{
	struct json_object *root = json_tokener_parse(json);
    if (root == NULL) 
    {
        return 0;
    }

    json_object_put(root);
	return 1;
}

// TODO: 여기부터 다시 시작
int recv_msg(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set)
{
	SocketHeader *sockh = (SocketHeader *)(client + target_fd); 
	RestMsgType *rest_msg = (RestMsgType *)(sockh + sizeof(SocketHeader));

	if (recv_binary(target_fd, (char *)sockh, sizeof(SocketHeader)) == LEAVE_CLIENT || \
		recv_binary(target_fd, (char *)rest_msg, ntohl(sockh->bodyLen)) == LEAVE_CLIENT)
	{
		bzero(client + target_fd, sizeof(t_client));
		close(target_fd);
		FD_CLR(target_fd, w_fd_set);
		FD_CLR(target_fd, r_fd_set);
		return 1;
	}

	FD_SET(target_fd, w_fd_set);

	if (!check_akey(rest_msg->header.param1Id))
	{
		// akey 값이 일치하지 않음 response
		return 1;
	}
	if (!check_json(rest_msg->jsonBody))
	{
		//json 값이 유효하지 않음 response
		return 1;
	}

	printf("-jsonBody- %s\n", rest_msg->jsonBody);
	exit(1);
	// create file
	// success response msg


}

void send_msg(t_client *client, int target_fd, fd_set *w_fd_set)
{
    int send_len = 0;
    int start_pos = 0;

    do 
    {
        send_len = send(target_fd , client + target_fd + start_pos, BUFSIZ - start_pos, 0);
		start_pos += send_len;
    } while (send_len == -1);

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

int set_non_blocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);

	if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int init_socket(char *port, struct sockaddr_in *servaddr)
{
    int sockfd;
    int true = 1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		ari_putstr_fd("socket creation failed...\n", 2); 
		exit(1); 
	} 

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&true, sizeof(socklen_t));

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
	if (set_non_blocking(sockfd) == -1)
	{
		fprintf(stderr, "nonblock Error: %s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
    return sockfd;
}

char *allowed_ip = "127.0.0.1";

int main(int argc, char **argv) {
	struct sockaddr_in servaddr;
	struct sockaddr_in cli; 

	int sockfd;
    int connfd;
	int select_cnt;
    int fd_max;

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
		select_cnt = select(fd_max + 1, &r_copy_fd_set, &w_copy_fd_set, NULL, NULL);
		for(int target_fd = 3; target_fd <= fd_max && select_cnt > 0; target_fd++)
		{
			if (FD_ISSET(target_fd, &r_copy_fd_set))
			{
				if (target_fd != sockfd)
				{
					recv_msg(client, target_fd, &r_fd_set, &w_fd_set);
				}
				else
				{
					connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
					if (strncmp(inet_ntoa(cli.sin_addr), allowed_ip, strlen(allowed_ip)))
					{
						ari_title_print_fd(STDERR_FILENO, "Not allowed ip", COLOR_RED_CODE);
						close(connfd);
						continue;
					}
					if (connfd < 0)
					{
						continue;
					}
                    FD_SET(connfd, &r_fd_set);
					set_non_blocking(connfd);
					//fprintf(stderr, "connfd : %d\n", connfd);
                    if (fd_max < connfd)
						fd_max = connfd;
				}
                select_cnt--;
            }
            else if (FD_ISSET(target_fd, &w_copy_fd_set))
			{
				send_msg(client, target_fd, &w_fd_set);
                select_cnt--;
            }
        }
	}
}
