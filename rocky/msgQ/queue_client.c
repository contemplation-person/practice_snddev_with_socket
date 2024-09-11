#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/shm.h>

#include "snd_dev.h"
#include "libari.h"

int is_poilcy_string(char *token, int max_len, char *msgType) {
    int token_len = strlen(token);

    if (token_len > max_len) {
        fprintf(stderr, "%s%s is too long\n%s", ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

int is_poilcy_int(int num, char *msgType) {
    if (num < 0) {
        fprintf(stderr, "%s%s is wrong\n%s", ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

char* make_create_snd_dev_policy(char *sdp_msg) {
    int     num;
    char*   save_msg;
    char*   token;
    char    copy_msg[BUFSIZ];

    memmove(copy_msg, sdp_msg, sizeof(Snd_dev_policy) + 1);
    Snd_dev_policy *sdp = (Snd_dev_policy*)sdp_msg;


    token = strtok_r(copy_msg, ", \t\n\r\f\v", &save_msg);
    if (token == NULL) {
        ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
        return NULL;
    }

    if (is_poilcy_string(token, MAX_LTE_ID, "LTE_ID") == false) {
        return NULL;
    }
    strcpy(sdp->lte_id, token);

    // TODO : 현재는 1개 리스트, 추후 가변적으로 리스트를 생성할 것.
    sdp->max_list_idx = 1;

    for (int item = SLICE_ID; item < SND_DEV_POLICY_MAX; item++) {
        token = strtok_r(NULL, ", \t\n\r\f\v", &save_msg);

        if (token == NULL) {
            ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
            return NULL;
        }

        switch (item) {
            case SLICE_ID:
                if (is_poilcy_string(token, MAX_SLICE_ID, "SLICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->slice_id, token);
                break;
            case AUTH_TYPE:
                num = atoi(token);
                if (is_poilcy_int(num, "AUTH_TYPE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->auth_type = num;
                break;
            case TWO_FA_USE:
                num = atoi(token);
                if (is_poilcy_int(num, "TWO_FA_USE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->two_fa_use = num;
                break;
            case DEVICE_SUSPEND:
                num = atoi(token);
                if (is_poilcy_int(num, "DEVICE_SUSPEND") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->device_suspend = num;
                break;
            case ID_TYPE:
                num = atoi(token);
                if (is_poilcy_int(num, "ID_TYPE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->id_type = num;
                break;
            case IP_POOL_INDEX:
                num = atoi(token);
                if (is_poilcy_int(num, "IP_POOL_INDEX") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->ip_pool_index = num;
                break;
            case DEVICE_ID:
                if (is_poilcy_string(token, MAX_DEVICE_ID, "DEVICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_id, token);
                break;
            case MDN:
                if (is_poilcy_string(token, MAX_MDN, "MDN") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->mdn, token);
                break;
            case IP:
                if (is_poilcy_string(token, MAX_IP, "IP") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->ip, token);
                break;
            case USER_ID:
                if (is_poilcy_string(token, MAX_USER_ID, "USER_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->user_id, token);
                break;
            case DEVICE_TYPE:
                if (is_poilcy_string(token, MAX_DEVICE_TYPE, "DEVICE_TYPE") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_type, token);
                break;
            case DEVICE_NAME:
                if (is_poilcy_string(token, MAX_DEVICE_NAME, "DEVICE_NAME") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_name, token);
                break;
            case SERIAL_NUMBER:
                if (is_poilcy_string(token, MAX_SERIAL_NUMBER, "SERIAL_NUMBER") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->serial_number, token);
                break;
            default:
                break;
        }
    }
    ari_title_print_fd(STDOUT_FILENO, "Snd_dev_policy\n\n", COLOR_GREEN_CODE);
    return (char*)sdp;
}

char* make_modify_snd_dev_policy(char *sdp_msg) {
    int     num;
    char*   save_msg;
    char*   token;
    char    copy_msg[BUFSIZ];

    memmove(copy_msg, sdp_msg, sizeof(Snd_dev_policy) + 1);

    Snd_dev_policy *sdp = (Snd_dev_policy*)sdp_msg;

    token = strtok_r(copy_msg, ", \t\n\r\f\v", &save_msg);
    if (token == NULL) {
        ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
        return NULL;
    }

    if (is_poilcy_string(token, MAX_LTE_ID, "LTE_ID") == false) {
        return NULL;
    }

    strcpy(sdp->lte_id, token);

    // TODO : 현재는 1개 리스트, 추후 가변적으로 리스트를 생성할 것.
    sdp->max_list_idx = 1;

    for (int item = SLICE_ID; item < SND_DEV_POLICY_MAX; item++) {
        token = strtok_r(NULL, ", \t\n\r\f\v", &save_msg);

        if (token == NULL) {
            ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
            return NULL;
        }

        switch (item) {
            case SLICE_ID:
                if (is_poilcy_string(token, MAX_SLICE_ID, "SLICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->slice_id, token);
                break;
            case AUTH_TYPE:
                num = atoi(token);
                if (is_poilcy_int(num, "AUTH_TYPE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->auth_type = num;
                break;
            case TWO_FA_USE:
                num = atoi(token);
                if (is_poilcy_int(num, "TWO_FA_USE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->two_fa_use = num;
                break;
            case DEVICE_SUSPEND:
                num = atoi(token);
                if (is_poilcy_int(num, "DEVICE_SUSPEND") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->device_suspend = num;
                break;
            case ID_TYPE:
                num = atoi(token);
                if (is_poilcy_int(num, "ID_TYPE") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->id_type = num;
                break;
            case IP_POOL_INDEX:
                num = atoi(token);
                if (is_poilcy_int(num, "IP_POOL_INDEX") == false) {
                    return NULL;
                }
                sdp->snd_dev_policy->ip_pool_index = num;
                break;
            case DEVICE_ID:
                if (is_poilcy_string(token, MAX_DEVICE_ID, "DEVICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_id, token);
                break;
            case MDN:
                if (is_poilcy_string(token, MAX_MDN, "MDN") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->mdn, token);
                break;
            case IP:
                if (is_poilcy_string(token, MAX_IP, "IP") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->ip, token);
                break;
            case USER_ID:
                if (is_poilcy_string(token, MAX_USER_ID, "USER_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->user_id, token);
                break;
            case DEVICE_TYPE:
                if (is_poilcy_string(token, MAX_DEVICE_TYPE, "DEVICE_TYPE") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_type, token);
                break;
            case DEVICE_NAME:
                if (is_poilcy_string(token, MAX_DEVICE_NAME, "DEVICE_NAME") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_name, token);
                break;
            case SERIAL_NUMBER:
                if (is_poilcy_string(token, MAX_SERIAL_NUMBER, "SERIAL_NUMBER") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->serial_number, token);
                break;
            default:
                break;
        }
    }

    ari_title_print_fd(STDOUT_FILENO, "Modify_snd_dev_policy\n\n", COLOR_GREEN_CODE);
    return (char*)sdp;
}

char* make_delete_snd_dev_policy(char *sdp_msg) {
    int     item;
    char*   save_msg;
    char*   token;
    char    copy_msg[BUFSIZ];

    memmove(copy_msg, sdp_msg, sizeof(Snd_dev_policy) + 1);

    Snd_dev_policy *sdp = (Snd_dev_policy*)sdp_msg;

    token = strtok_r(copy_msg, ", \t\n\r\f\v", &save_msg);
    if (token == NULL) {
        ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
        return NULL;
    }

    if (is_poilcy_string(token, MAX_LTE_ID, "LTE_ID") == false) {
        return NULL;
    }

    strcpy(sdp->lte_id, token);

    // TODO : 현재는 1개 리스트, 추후 가변적으로 리스트를 생성할 것.
    sdp->max_list_idx = 1;
    item = SLICE_ID;
    while (item) {
        token = strtok_r(NULL, ", \t\n\r\f\v", &save_msg);

        if (token == NULL) {
            ari_title_print_fd(STDOUT_FILENO, "Input argument count must match the format.\n", COLOR_RED_CODE);
            return NULL;
        }

        switch (item) {
            case SLICE_ID:
                if (is_poilcy_string(token, MAX_SLICE_ID, "SLICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->slice_id, token);
                item = DEVICE_ID;
                break;
            case DEVICE_ID:
                if (is_poilcy_string(token, MAX_DEVICE_ID, "DEVICE_ID") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_id, token);
                item = DEVICE_TYPE;
                break;
            case DEVICE_TYPE:
                if (is_poilcy_string(token, MAX_DEVICE_TYPE, "DEVICE_TYPE") == false) {
                    return NULL;
                }
                strcpy(sdp->snd_dev_policy->device_type, token);
                item = 0;
                break;
        }
    }

    ari_title_print_fd(STDOUT_FILENO, "Delete_snd_dev_policy\n\n", COLOR_GREEN_CODE);
    return (char*)sdp;
}

int read_type(char* msg) {
    int read_len;
    int type;

    read_len = read(STDIN_FILENO, msg, BUFSIZ);
    if (read_len < 0) {
        fprintf(stderr, "read error\n");
        return -1;
    }

    msg[read_len] = '\0';

    type = atoi(msg);

    if (0 < type && type < EMG_MSG_EMUM_MAX) {
        return type;
    }

    return 0;
}

bool display_data_list(int type) {
    char* alert;

    switch (type) {
        case CREATE_SND_DEV_POLICY_ENUM:
            alert = "Usage : LTE_ID, SLICE_ID, AUTH_TYPE, TWO_FA_USE, DEVICE_SUSPEND, \
                    ID_TYPE, IP_POOL_INDEX, DEVICE_ID, MDN, IP, USER_ID, DEVICE_TYPE, \
                    DEVICE_NAME, SERIAL_NUMBER\n";
            break;
        case MODIFY_SND_DEV_POLICY_EMUM:
            alert = "Usage : LTE_ID, SLICE_ID, DEVICE_ID, ID_TYPE, MDN, IP_POOL_INDEX, \
                    IP, AUTH_TYPE, USER_ID, TWO_FA_USE, DEVICE_SUSPEND, DEVICE_TYPE, \
                    MODEL_NAME, SERIAL_NUM\n";
            break;
        case DELETE_SND_DEV_POLICY_ENUM:
            alert = "Usage : LTEID, SLICE_ID, DEVICE_ID, ID_TYPE\n";
            break;
        default:
            fprintf(stderr, "Invalid type\n");
            return false;
    }
    ari_title_print(alert, COLOR_GREEN_CODE);
    return true;
}

bool display_type_list() {
    char *alert = "Usage\n\
                CREATE_SND_DEV_POLICY = 1\n\
                MODIFY_SND_DEV_POLICY = 2\n\
                DELETE_SND_DEV_POLICY = 3\n";
    return ari_putstr_fd(alert, STDOUT_FILENO);
}

bool save_shared_memory_data(int type, Snd_dev_policy* shared_msg) {
    char msg[BUFSIZ] = {0};
    char *mtype_data;

    if (read(STDIN_FILENO, msg, BUFSIZ) < 0) {
        perror("read");
        return false;
    }
    switch (type) {
        case CREATE_SND_DEV_POLICY_ENUM:
            mtype_data = make_create_snd_dev_policy(msg);
            break;
        case MODIFY_SND_DEV_POLICY_EMUM:
            mtype_data = make_modify_snd_dev_policy(msg);
            break;
        case DELETE_SND_DEV_POLICY_ENUM:
            mtype_data = make_delete_snd_dev_policy(msg);
            break;
        default:
            return false;
    }

    if (mtype_data == NULL) {
        return false;
    }

    memmove(shared_msg, mtype_data, sizeof(Snd_dev_policy));
    return true;
}

int main() {
    Msg_queue msg_q;

    key_t msg_key;
    int msgid;

    key_t shared_key;
    int shared_id;
    Snd_dev_policy *shared_msg;

    char msg[512] = {0};

    msg_key = ftok("/tmp/emg", 42);
    msgid = msgget(msg_key, 0666 | IPC_CREAT);

    shared_key = ftok("/tmp/emg", 24);
    if (shared_key == -1) {
        perror("ftok");
        return false;
    }
    shared_id = shmget(shared_key, sizeof(Snd_dev_policy), 0666 | IPC_CREAT);
    if (shared_id == -1) {
        perror("shmget");
        printf("errno : %d\n", errno);
        return false;
    }
    shared_msg = shmat(shared_id, NULL, 0);
    if (shared_msg == (Snd_dev_policy*)(-1)) {
        perror("shmat");
        return false;
    }

    while (42) {
        display_type_list();
        msg_q.msg_type = read_type(msg);
        if (display_data_list(msg_q.msg_type) == false) {
            ari_print_error("display_data_list error", __FILE__, __LINE__);
            continue;
        }

        if (save_shared_memory_data(msg_q.msg_type, shared_msg)) {
            if (msgsnd(msgid, &msg_q, sizeof(msg_q), 0) == -1) {
                perror("msgsnd");
                fprintf(stderr, "plz retry\n");
            }
        }
    }

    return 0;
}