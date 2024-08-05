#include "libari.h"
#include "rest_api_message.h"
#include "medif_api.h"
#include "json-c/json.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    SocketHeader header;
    RestMsgType msg;
} t_client;

typedef enum
{
    CLIENT_NUM = 1024,
    BUF_LEN = 8193,
} e_all;

int extract_message(char **buf, char **msg)
{
    char *newbuf;
    int i;

    *msg = 0;
    if (*buf == 0)
    {
        return (0);
    }
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
    char *newbuf;
    int len;

    if (buf == 0)
    {
        len = 0;
    }
    else
    {
        len = strlen(buf);
    }
    newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
    if (newbuf == 0)
    {
        return (0);
    }
    newbuf[0] = 0;
    if (buf != 0)
    {
        strcat(newbuf, buf);
    }
    free(buf);
    strcat(newbuf, add);
    return (newbuf);
}

void init_client(t_client *client)
{
    bzero(client, sizeof(t_client) * CLIENT_NUM);
}

void set_client_send_msg(t_client *client, int target_fd)
{
    char *str_rest_api[] = {
        FOREACH_REST_API(GENERATE_REST_API_STRING)
        };

    bzero(&(client[target_fd]), sizeof(t_client));

    client[target_fd].header.bodyLen = sizeof(client[target_fd].msg.jsonBody);
    client[target_fd].header.mType = CREATE_SNDDEV_POLICY;
    strcpy(client[target_fd].msg.header.mtype, str_rest_api[CREATE_SNDDEV_POLICY]);
}

void create_file(char *file_name, FILE **fp, FILE **fp2)
{
    int i = 0;

    while (1)
    {
        sprintf(file_name, "%s_%d", file_name, i);
        if ((*fp = fopen(file_name, "r")) == NULL)
        {
            *fp = fopen(file_name, "w");
            break;
        }
        fclose(*fp);
        i++;
    }

    while (1)
    {
        sprintf(file_name, "%s_%d", file_name, i);
        if ((*fp2 = fopen(file_name, "r")) == NULL)
        {
            *fp2 = fopen(file_name, "w");
            break;
        }
        fclose(*fp2);
        i++;
    }
}

void set_client_send_error_msg(t_client *client, int target_fd)
{
    bzero(&(client[target_fd]), sizeof(t_client));

    client[target_fd].header.bodyLen = 0;
    client[target_fd].header.mType = CREATE_SNDDEV_POLICY;
}

void write_file(FILE *fp, FILE *fp2, t_client *client)
{
    char buf[1000] = {0,};

    int line = sprintf(buf, "MAC\t\t\t\t:%s\n", client->msg.header.param2Id);
    // TODO : json에서 데이터를 뽑아서 저장
    line = sprintf(buf + line, "helloworld\t\t\t\t:\n");


    fprintf(fp, "%s", client->msg.jsonBody);
    fprintf(fp2, "%s", client->msg.jsonBody);
}

int recv_client(t_client *client, int target_fd, fd_set *r_fd_set, fd_set *w_fd_set)
{
    int len = recv(target_fd, &(client[target_fd]), sizeof(t_client), 0);

    if (len > 0)
    {
        // client message
        fprintf(stderr, "client message : %s\n", (char *)client[target_fd]);
        FD_SET(target_fd, w_fd_set);
    }
    else if (len == 0)
    {
        // leave client
        bzero(&(client[target_fd]), sizeof(t_client)); 
        close(target_fd);
        FD_CLR(target_fd, w_fd_set);
        FD_CLR(target_fd, r_fd_set);
    }
    else
    {
        return 0;
    }

    return 1;
}


void send_client(t_client *client, int target_fd, fd_set *w_fd_set)
{
    int send_len = send(target_fd, &(client[target_fd]), sizeof(t_client), 0);
    int msg_len = sizeof(RestMsgType);
    // send message 작으면 같아질 때까지 다시 보내기

    if (send_len < 0)
    {
        ari_print_error("send error", __FILE__, __LINE__);
        return;
    }
    while (send_len < msg_len)
    {
        send_len += send(target_fd, &(client[target_fd]) + send_len, sizeof(t_client) - send_len, 0);
    }
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

void init_socket(int *sockfd, const int port, struct sockaddr_in *serv_addr)
{
    if (!port)
    {
        ari_print_error("port error", __FILE__, __LINE__);
        exit(-1);
    }

    *sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*sockfd == -1)
    {
        ari_print_error("socket error", __FILE__, __LINE__);
        exit(-1);
    }

    bzero(serv_addr, sizeof(*serv_addr));

    // assign PORT
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr->sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(*sockfd, (const struct sockaddr *)serv_addr, sizeof(*serv_addr))) != 0)
    {
        ari_print_error("bind error", __FILE__, __LINE__);
        close(*sockfd);
        exit(-1);
    }
    if (listen(*sockfd, 10) != 0)
    {
        ari_print_error("listen error", __FILE__, __LINE__);
        close(*sockfd);
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    int connfd;
    int len;
    int fd_max = 0;
    int select_cnt;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    t_client client[CLIENT_NUM + 1];

    fd_set w_fd_set;
    fd_set r_fd_set;
    fd_set w_copy_fd_set;
    fd_set r_copy_fd_set;


    len = sizeof(cli_addr);

    check_argc(argc);
    init_socket(&sockfd, atoi(argv[1]), &serv_addr);
    init_client(client);

    fd_max = sockfd;

    FD_ZERO(&r_fd_set);
    FD_ZERO(&w_fd_set);
    FD_SET(sockfd, &r_fd_set);

    while (42)
    {
        w_copy_fd_set = w_fd_set;
        r_copy_fd_set = r_fd_set;
        select_cnt = select(fd_max + 1, &r_copy_fd_set, &w_copy_fd_set, NULL, NULL);
        for (int target_fd = 3; target_fd <= fd_max && 0 < select_cnt; target_fd++)
        {
            if (FD_ISSET(target_fd, &r_copy_fd_set))
            {
                if (target_fd != sockfd)
                {
                    // 해당 fd 클라이언트로부터 요청
                    recv_client(client, target_fd, &r_fd_set, &w_fd_set);
                    fprintf(stderr, "target fd : %d\n", target_fd);
                }
                else
                {
                    // client 접속 시도
                    connfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&len);
                    if (connfd < 0)
                        continue;
                    FD_SET(target_fd, &r_fd_set);
                    if (fd_max < connfd)
                        fd_max = connfd;
                    fprintf(stderr, "클라이언트 접속 fd = %d\n", connfd);
                }
                select_cnt--;
            }
            if (FD_ISSET(target_fd, &w_copy_fd_set))
            {
                // target 메세지 보내기
                send_client(client, target_fd, &w_fd_set);
                select_cnt--;
            }
        }
    }
}
