#include "msgQ.h"

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
