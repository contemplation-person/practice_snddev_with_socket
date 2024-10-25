#include "snd_client.h"

// msg type을 저장
int make_rest_header(int mType, char *msg, int json_len) {
    char *rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    RestLibHeadType *rest_header = (RestLibHeadType *)(msg + sizeof(SocketHeader));
    static int tid = 0;

    switch (mType) {
        case CREATE_SND_DEV_POLICY_ENUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[CREATE_SNDDEV_POLICY]);
            break;
        case MODIFY_SND_DEV_POLICY_EMUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[MODIFY_SNDDEV_POLICY]);
            break;
        case DELETE_SND_DEV_POLICY_ENUM:
            sprintf(rest_header->mtype, "%s", rest_str_arr[DELETE_SNDDEV_POLICY]);
            break;
    }

    sprintf(rest_header->method, "%s", "POST");
    sprintf(rest_header->tid, "%d", tid++);
    sprintf(rest_header->param2Id, "%s", "127.0.0.1");
    sprintf(rest_header->param1Id, "%s", "EMG@an#2345&12980!");
    sprintf(rest_header->uri, "%s/%s", "127.0.0.1:3000", "api/emg/policy/SndDev/create");
    sprintf(rest_header->bodyLen, "%d", json_len);

    if (tid > 99999999) tid = 0;

    return sizeof(RestLibHeadType);
}

void create_file(char *filepath) {
    if (access(filepath, F_OK) == -1) {
        FILE *file = fopen(filepath, "w");
        if (file == NULL) {
            perror("Failed to create file");
            exit(EXIT_FAILURE);
        }
        fclose(file);
    }
}

void vaild_argc(int argc) {
    if (argc != 3) {
        ari_title_print_fd(STDERR_FILENO, "인자가 다릅니다. ./snd_client [IP] [PORT]", COLOR_RED_CODE);
        exit(-1);
    }
}
