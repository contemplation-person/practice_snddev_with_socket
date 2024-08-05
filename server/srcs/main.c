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

typedef struct
{
	int id;
	char *rb;
	char *wb;
} t_client;

typedef enum
{
	CLIENT_NUM = 3000,
	BUF_LEN = 300000,
} e_all;

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void init(t_client *client)
{
	for (int fd = 0; fd < CLIENT_NUM; fd++)
	{
		client[fd].id = -1;
		client[fd].wb = NULL;
		client[fd].rb = NULL;
	}
}


int recv_(t_client *client, int target_fd, int fd_max, fd_set *r_fd_set, fd_set *w_fd_set)
{
	char buf[BUF_LEN]= {0};
	char *save_msg = NULL;
	char *tmp = NULL;
	int len = recv(target_fd, buf, BUF_LEN, 0);

	if (0 > len) return 0;
	else if (len > 0)
	{
		client[target_fd].rb = str_join(client[target_fd].rb, buf);
		sprintf(buf, "client %d : ", client[target_fd].id);
		len = extract_message(&client[target_fd].rb, &tmp);
		while (len)
		{
			if (len == -1)
			{
				free(tmp);
				return 0;
			}
			save_msg = str_join(save_msg, buf);
			save_msg = str_join(save_msg, tmp);
			len = extract_message(&client[target_fd].rb, &tmp);
		}
		fprintf(stderr, "save: %s", save_msg);
	} 
	else 
	{
		sprintf(buf, "client %d : ", client[target_fd].id);
		save_msg = str_join(save_msg, buf);

		client[target_fd].id = -1;
		free(client[target_fd].wb);
		free(client[target_fd].rb);
		client[target_fd].wb = NULL;
		client[target_fd].rb = NULL;
		close(target_fd);
		FD_CLR(target_fd, w_fd_set);
		FD_CLR(target_fd, r_fd_set);

	}
	if (!save_msg) return 1;
	for (int fd = 3; fd <= fd_max; fd++)
	{
		if (client[fd].id != -1 && target_fd != fd)
		{
			client[fd].wb = str_join(client[fd].wb, save_msg);
			FD_SET(fd, w_fd_set);
		}
	}	
	return 1;
}


void register_(t_client *client, int target_fd, int id, int fd_max, fd_set *r_fd_set, fd_set *w_fd_set)
{
	char prifix_msg[50] = {0};

	sprintf(prifix_msg, "serv: client %d arrived\n", id);
	client[target_fd].id = id;
	FD_SET(target_fd, r_fd_set);
	for (int fd = 3; fd <= fd_max; fd++)
	{
		if (client[fd].id != -1 && target_fd != fd)
		{
			client[fd].wb = str_join(client[fd].wb, prifix_msg);
			FD_SET(fd, w_fd_set);
		}
	}
}

void send_(t_client *client, int target_fd, fd_set *w_fd_set)
{
	int send_len = ari_putstr_fd(client[target_fd].wb, target_fd);
	int wb_len = strlen(client[target_fd].wb);

	if (send_len == wb_len)
	{
		free(client[target_fd].wb);
		client[target_fd].wb = 0;
		FD_CLR(target_fd, w_fd_set);
	}
	else
	{
		strcpy(client[target_fd].wb, &client[target_fd].wb[send_len]);
	}
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

int main(int argc, char **argv) {
	struct sockaddr_in servaddr;
	struct sockaddr_in cli; 
	int sockfd;
    int connfd;
	int id = 0;
    int len = sizeof(cli);

	t_client client[CLIENT_NUM];

    check_argc(argc);

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		ari_putstr_fd("socket creation failed...\n", 2); 
		exit(1); 
	} 

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		ari_putstr_fd("socket bind failed...\n", 2); 
		close(sockfd);
		exit(1); 
	} 
	if (listen(sockfd, 10) != 0) {
		ari_putstr_fd("cannot listen\n", 2); 
		close(sockfd);
		exit(1); 
	}

	int fd_max = sockfd;
	int s_cnt;

	fd_set w_fd_set;
	fd_set r_fd_set;
	fd_set w_copy_fd_set;
	fd_set r_copy_fd_set;

	FD_ZERO(&r_fd_set);
	FD_ZERO(&w_fd_set);
	FD_SET(sockfd, &r_fd_set);

	init(client);
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
					if (!recv_(client, target_fd, fd_max, &r_fd_set, &w_fd_set))
					{
						for (int i = 0; i <= fd_max; i++)
						{
							if (client[target_fd].id != -1)
								close(target_fd), close(sockfd);
						}
					}
					fprintf(stderr, "target fd : %d\n", target_fd);
				}
				else
				{
					connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
					if (connfd < 0) 
						continue;
					register_(client, connfd, id, fd_max, &r_fd_set, &w_fd_set);
					if (fd_max < connfd)
						fd_max = connfd;
					id++;
					fprintf(stderr, "connfd : %d\n", connfd);
				}
                s_cnt--;
			}
		}
	}
}
