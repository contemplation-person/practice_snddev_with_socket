#include "msgQ.h"

int main() {
    Msg_queue msg_q;

    key_t msg_key;
    int msgid;

    key_t shared_key;
    int shared_id;
    Snd_dev_policy *shared_msg;

    char msg[512] = {0};

    const char *filepath = "/tmp/emg";

    if (access(filepath, F_OK) == -1) {
        FILE *file = fopen(filepath, "w");
        if (file == NULL) {
            perror("Failed to create file");
            return EXIT_FAILURE; 
        }
        fclose(file);
    }

    msg_key = ftok("/tmp/emg", 42);
    if (msg_key == -1) {
        perror("ftok");
        return false;
    }

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
