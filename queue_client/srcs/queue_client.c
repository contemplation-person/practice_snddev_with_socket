#include "create_snd_dev.h"
#include "libari.h"

#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

int is_poilcy_string(char *token, int max_len, char *msgType)
{
    int token_len = strlen(token);

    if (token_len > max_len)
    {
        fprintf(stderr, "%s%s is too long\n%s", \
                ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

int is_poilcy_int(int num, char *msgType)
{
    if (num < 0)
    {
        fprintf(stderr, "%s%s is wrong\n%s", \
                ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

int make_create_snd_dev_policy(Msg_queue *msg_q)
{ 
    int     num;
    int     read_len;
    char    csdp_msg[BUFSIZ] = {0,};
    char    *save_msg;
    char    *token;

    read_len = read(STDIN_FILENO, csdp_msg, BUFSIZ);
    if (read_len < 1)
    {
        if (read_len == 0)
            exit(EXIT_SUCCESS);
        printf("read error\n");
        return false;
    }

    token = strtok_r(csdp_msg, ", \t\n\r\f\v", &save_msg);
    if (token == NULL)
    {
        ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
        return false;
    }

    if (is_poilcy_string(token, MAX_LTE_ID, "LTE_ID") == false)
    {
        return false;
    }

    strcpy(msg_q->create_snd_dev_policy.lte_id, token);

    for(int idx = 1; idx < MAX_CREATE_SND_DEV_POLICY; idx++)
    {
        token = strtok_r(NULL, ", \t\n\r\f\v", &save_msg);
        if (token == NULL)
        {
            ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
            return false;
        }

        switch (idx)
        {
        case SLICE_ID:
            if (is_poilcy_string(token, MAX_SLICE_ID, "SLICE_ID") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.slice_id, token);
            break;
        case AUTH_TYPE:
            num = atoi(token);
            if (is_poilcy_int(num, "AUTH_TYPE") == false)
            {
                return false;
            }
            msg_q->create_snd_dev_policy.auth_type = num;
            break;
        case TWO_FA_USE:
            num = atoi(token);
            if (is_poilcy_int(num, "TWO_FA_USE") == false)
            {
                return false;
            }
            msg_q->create_snd_dev_policy.two_fa_use = num;
            break;
        case DEVICE_SUSPEND:
            num = atoi(token);
            if (is_poilcy_int(num, "DEVICE_SUSPEND") == false)
            {
                return false;
            }
            msg_q->create_snd_dev_policy.device_suspend = num;
            break;
        case ID_TYPE:
            num = atoi(token);
            if (is_poilcy_int(num, "ID_TYPE") == false)
            {
                return false;
            }
            msg_q->create_snd_dev_policy.id_type = num;
            break;
        case IP_POOL_INDEX:
            num = atoi(token);
            if (is_poilcy_int(num, "IP_POOL_INDEX") == false)
            {
                return false;
            }
            msg_q->create_snd_dev_policy.ip_pool_index = num;
            break;
        case DEVICE_ID:
            if (is_poilcy_string(token, MAX_DEVICE_ID, "DEVICE_ID") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.device_id, token);
            break;
        case MDN:
            if (is_poilcy_string(token, MAX_MDN, "MDN") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.mdn, token);
            break;
        case IP:
            if (is_poilcy_string(token, MAX_IP, "IP") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.ip, token);
            break;
        case USER_ID:
            if (is_poilcy_string(token, MAX_USER_ID, "USER_ID") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.user_id, token); 
            break;
        case DEVICE_TYPE:
            if (is_poilcy_string(token, MAX_DEVICE_TYPE, "DEVICE_TYPE") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.device_type, token);
            break;
        case DEVICE_NAME:
            if (is_poilcy_string(token, MAX_DEVICE_NAME, "DEVICE_NAME") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.device_name, token);
            break;
        case SERIAL_NUMBER:
            if(is_poilcy_string(token, MAX_SERIAL_NUMBER, "SERIAL_NUMBER") == false)
            {
                return false;
            }
            strcpy(msg_q->create_snd_dev_policy.serial_number, token);
            break;
        default:
            break;
        }
    }

    ari_title_print_fd(STDOUT_FILENO, "Create_snd_dev_policy\n\n", COLOR_GREEN_CODE);
    return true;
}

int main()
{
    Msg_queue   msg_q;
    key_t       key;
    int         msgid;
    char        *alert = "Usage : LTE_ID, SLICE_ID, AUTH_TYPE, TWO_FA_USE, DEVICE_SUSPEND, ID_TYPE, IP_POOL_INDEX, DEVICE_ID, MDN, IP, USER_ID, DEVICE_TYPE, DEVICE_NAME, SERIAL_NUMBER\n";

    key = ftok("create_snd_dev_policy", 42);
    msgid = msgget(key, 0666| IPC_CREAT);
    msg_q.msg_type = CREATE_SND_DEV_POLICY_TYPE_ENUM;

    while (42)
    {
        ari_title_print(alert, COLOR_GREEN_CODE);

        if (make_create_snd_dev_policy(&msg_q))
        {
            if (msgsnd(msgid, &msg_q, sizeof(msg_q), 0) == -1)
            {
                perror("msgsnd");
                return 1;
            }
        }
    }

    return 0;
}