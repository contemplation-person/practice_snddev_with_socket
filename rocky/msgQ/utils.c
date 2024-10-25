#include "msgQ.h"

bool is_poilcy_string(char *token, int max_len, char *msgType) {
    int token_len = strlen(token);

    if (token_len > max_len) {
        fprintf(stderr, "%s%s is too long\n%s", ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

bool is_poilcy_int(int num, char *msgType) {
    if (num < 0) {
        fprintf(stderr, "%s%s is wrong\n%s", ASCII_COLOR_RED, msgType, ASCII_COLOR_RESET);
        return false;
    }
    return true;
}

bool display_data_list(int type) {
    char* alert;

    switch (type) {
        case CREATE_SND_DEV_POLICY_ENUM:
            alert = "Usage : LTE_ID, SLICE_ID, AUTH_TYPE, TWO_FA_USE, DEVICE_SUSPEND, ID_TYPE, IP_POOL_INDEX, DEVICE_ID, MDN, IP, USER_ID, DEVICE_TYPE, DEVICE_NAME, SERIAL_NUMBER\n";
            break;
        case MODIFY_SND_DEV_POLICY_EMUM:
            alert = "Usage : LTE_ID, SLICE_ID, DEVICE_ID, ID_TYPE, MDN, IP_POOL_INDEX, IP, AUTH_TYPE, USER_ID, TWO_FA_USE, DEVICE_SUSPEND, DEVICE_TYPE, MODEL_NAME, SERIAL_NUM\n";
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
    char *alert = "Usage\nCREATE_SND_DEV_POLICY = 1\nMODIFY_SND_DEV_POLICY = 2\nDELETE_SND_DEV_POLICY = 3\n";
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

