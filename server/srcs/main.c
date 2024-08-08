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

#define CLIENT_NUM 1025
#define LEAVE_CLIENT 0
#define MAX_DEVICE_ID_LEN 18
#define MAX_MDN_LEN 12
#define MAX_IP_LEN 40
#define MAX_USER_ID_LEN 23
#define MAX_DEVICE_TYPE_LEN 11
#define MAX_DEVICE_NAME_LEN 31
#define MAX_SERIAL_NUMBER_LEN 31

typedef struct s_create_snddev_policy_list
{
	int auth_type;
	int two_fa_use;
	int device_suspend;
	int id_type;
	int ip_pool_index;

	char device_id[MAX_DEVICE_ID_LEN];
	char mdn[MAX_MDN_LEN];
	char ip[MAX_IP_LEN];
	char user_id[MAX_USER_ID_LEN];
	char device_type[MAX_DEVICE_TYPE_LEN];

	char device_name[MAX_DEVICE_NAME_LEN];
	char serial_number[MAX_SERIAL_NUMBER_LEN];
	struct s_create_snddev_policy_list *next;
} create_snddev_policy_list;

typedef struct
{
	char lte[20];
	char slice_id[4];
	create_snddev_policy_list *create_snddev_policy_start_list;
	create_snddev_policy_list *create_snddev_policy_end_list;
} create_snddev_policy_header;

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
		printf("\nrecv len: %d\n", len);
		if (len < 1)
		{
			return len;
		}
		recv_len += len;
	}

	return 1;
}

int same_as_akey(RestMsgType *rest_msg)
{
	static char *akey = "EMG@an#2345&12980!";

	if (strncmp(akey, rest_msg->header.param1Id, strlen(akey) + 1))
	{
		return 0;
	}
	return 1;
}

int recv_msg(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set)
{
	SocketHeader *sockh = (SocketHeader *)(client + target_fd);
	RestMsgType *rest_msg = (RestMsgType *)(sockh + sizeof(SocketHeader));

	// TODO : rest msg 를 보고 json을 recv를 해야함.
	if (recv_binary(target_fd, (char *)sockh, sizeof(SocketHeader)) == LEAVE_CLIENT ||
		recv_binary(target_fd, (char *)rest_msg, htonl(sockh->bodyLen)) == LEAVE_CLIENT)
	{
		bzero(client + target_fd, sizeof(t_client));
		close(target_fd);
		FD_CLR(target_fd, w_fd_set);
		FD_CLR(target_fd, r_fd_set);
		return 0;
	}

	memmove(client + target_fd, sockh, sizeof(SocketHeader));
	memmove(client + target_fd + sizeof(SocketHeader), rest_msg, sizeof(RestLibHeadType) + htonl(sockh->bodyLen));

	return 1;
}

int send_msg(t_client *client, int target_fd)
{
	// TODO : error response
	int send_len = 0;
	int start_pos = 0;

	do
	{
		send_len = send(target_fd, client + target_fd + start_pos, BUFSIZ - start_pos, 0);
		if (send_len == LEAVE_CLIENT)
		{
			return LEAVE_CLIENT;
		}
		printf("\nsend_len: %d\n", send_len);
		start_pos += send_len;
	} while (send_len > 0);

	return 1;
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

	if (flags == -1)
	{
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
	if (sockfd == -1)
	{
		ari_putstr_fd("socket creation failed...\n", 2);
		exit(1);
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&true, sizeof(socklen_t));

	bzero(servaddr, sizeof(*servaddr));

	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr->sin_port = htons(atoi(port));

	if ((bind(sockfd, (const struct sockaddr *)servaddr, sizeof(*servaddr))) != 0)
	{
		fprintf(stderr, "bind Error: %s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
	if (listen(sockfd, 10) != 0)
	{
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

int new_create_snddev_policy_list(create_snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj)
{
	create_snddev_policy_list *new_list = (create_snddev_policy_list *)malloc(sizeof(create_snddev_policy_list));
	if (!new_list)
	{
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

	if (snddev_policy_header->create_snddev_policy_start_list == NULL)
	{
		snddev_policy_header->create_snddev_policy_start_list = new_list;
		snddev_policy_header->create_snddev_policy_end_list = new_list;
	}
	else
	{
		snddev_policy_header->create_snddev_policy_end_list->next = new_list;
		snddev_policy_header->create_snddev_policy_end_list = new_list;
	}

	return 1;
}

int parse_json_list(struct json_object *parsed_json, create_snddev_policy_header *snddev_policy_header)
{
	struct json_object *csp_list;
	struct json_object *csp_list_obj;
	int list_len;

	if (!json_object_object_get_ex(parsed_json, "list", &csp_list))
	{
		printf("list not found\n");
		return 0;
	}

	list_len = json_object_array_length(csp_list);
	for (int idx = 0; idx < list_len; idx++)
	{
		csp_list_obj = json_object_array_get_idx(csp_list, idx);
		if (!new_create_snddev_policy_list(snddev_policy_header, csp_list_obj))
		{
			fprintf(stderr, "new_create_snddev_policy_list error: %s\n", strerror(errno));
			return 0;
		}
	}

	return 1;
}

void ari_json_object_print(json_object *json)
{
	const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

	ari_putendl_fd(print_string, 1);
}

void parse_json_header(struct json_object *parsed_json, create_snddev_policy_header *snddev_policy)
{
	struct json_object *lte_id;
	struct json_object *slice_id;

	if (json_object_object_get_ex(parsed_json, "LTEID", &lte_id))
	{
		strcpy(snddev_policy->lte, json_object_get_string(lte_id));
	}
	if (json_object_object_get_ex(parsed_json, "SLICE_ID", &slice_id))
	{
		strcpy(snddev_policy->slice_id, json_object_get_string(slice_id));
	}
}

int parse_rest_msg(RestMsgType *rest_msg, create_snddev_policy_header *snddev_policy)
{
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(rest_msg->jsonBody);
	if (parsed_json == NULL)
	{
		return 0;
	}

	parse_json_header(parsed_json, snddev_policy);
	if (!parse_json_list(parsed_json, snddev_policy))
	{
		json_object_put(parsed_json);
		return 0;
	}

	ari_title_print("parsed_json", COLOR_GREEN_CODE);
	ari_json_object_print(parsed_json);

	json_object_put(parsed_json);
	return 1;
}

int create_snd_file(create_snddev_policy_header snddev_policy)
{
	char file_name[100] = {0};
	int file_num = 0;
	create_snddev_policy_list *csp_list = snddev_policy.create_snddev_policy_start_list;
	FILE *fp;

	system("mkdir -p file_test");
	while (csp_list)
	{
		sprintf(file_name, "file_test/%s_num_%d", csp_list->device_id, file_num++);
		if (!access(file_name, F_OK))
		{
			continue;
		}
		fp = fopen(file_name, "w");
		if (!fp)
		{
			return 0;
		}
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

void clear_csp_list(create_snddev_policy_header *snddev_policy_header)
{
	create_snddev_policy_list *csp_list = snddev_policy_header->create_snddev_policy_start_list;
	create_snddev_policy_list *next;

	while (csp_list)
	{
		next = csp_list->next;
		free(csp_list);
		csp_list = next;
	}
	snddev_policy_header->create_snddev_policy_start_list = NULL;
	snddev_policy_header->create_snddev_policy_end_list = NULL;

	snddev_policy_header->lte[0] = 0;
	snddev_policy_header->slice_id[0] = 0;
}

int main(int argc, char **argv)
{
	struct sockaddr_in servaddr;
	struct sockaddr_in cli;

	int sockfd;
	int connfd;
	int select_cnt;
	int fd_max;
	int len = sizeof(cli);

	char *allowed_ip = "127.0.0.1";

	t_client client[CLIENT_NUM] = { 0, };
	create_snddev_policy_header snddev_policy_header[CLIENT_NUM] = { 0, };

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
		for (int target_fd = 3; target_fd <= fd_max && select_cnt > 0; target_fd++)
		{
			if (FD_ISSET(target_fd, &r_copy_fd_set))
			{
				if (target_fd != sockfd)
				{
					if (recv_msg(client, target_fd, &r_fd_set, &w_fd_set) == -1)
					{
						// TODO: error recv response
					}
					if (!same_as_akey((RestMsgType *)(client + target_fd + sizeof(SocketHeader))))
					{
						// TODO :akey 값이 일치하지 않음 response
					}
					if (!parse_rest_msg((RestMsgType *)(client + target_fd + sizeof(SocketHeader)), &(snddev_policy_header[target_fd])))
					{
						// TODO : error response
					}
					create_snd_file(snddev_policy_header[target_fd]);
					// TODO : success response msg
					FD_SET(target_fd, &w_fd_set);
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
					ari_title_print_fd(STDOUT_FILENO, "connect ip", COLOR_GREEN_CODE);
					ari_title_print_fd(STDOUT_FILENO, inet_ntoa(cli.sin_addr), COLOR_GREEN_CODE);
					if (fd_max < connfd)
						fd_max = connfd;
				}
				select_cnt--;
			}
			else if (FD_ISSET(target_fd, &w_copy_fd_set))
			{
				if (send_msg(client, target_fd) == LEAVE_CLIENT)
				{
					close(target_fd);
					FD_CLR(target_fd, &r_fd_set);
				}
				bzero(client + target_fd, sizeof(t_client));
				FD_CLR(target_fd, &w_fd_set);
				
				select_cnt--;
			}
			clear_csp_list(snddev_policy_header + target_fd);
		}
	}
}
