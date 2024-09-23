#ifndef HOST_VAR_EMG_H
#define HOST_VAR_EMG_H

#include <ulpLibInterface.h>

typedef struct {
    char LTEID[22];        // VARCHAR(21) + 1
    char SLICE_ID[6];      // VARCHAR(5) + 1
    char MDN[13];          // VARCHAR(12) + 1
    int IP_POOL_INDEX;     // INTEGER
    char IP[41];           // VARCHAR(40) + 1
    int AUTH_TYPE;         // INTEGER
    char USER_ID[24];      // VARCHAR(23) + 1
    int TWOFA_USE;         // INTEGER
    int DEVICE_SUSPEND;    // INTEGER
    char DEVICE_TYPE[12];  // VARCHAR(11) + 1
    char MODEL_NAME[32];   // VARCHAR(31) + 1
    char SERIAL_NUM[32];   // VARCHAR(31) + 1
    char DEVICE_ID[18];    // VARCHAR(17) + 1
    int ID_TYPE;           // INTEGER
} Emg_type;

typedef struct {
    SQLLEN LTEID_ind;
    SQLLEN SLICE_ID_ind;
    SQLLEN MDN_ind;
    SQLLEN IP_POOL_INDEX_ind;  // 정수형 인디케이터
    SQLLEN IP_ind;
    SQLLEN AUTH_TYPE_ind;      // 정수형 인디케이터
    SQLLEN USER_ID_ind;
    SQLLEN TWOFA_USE_ind;      // 정수형 인디케이터
    SQLLEN DEVICE_SUSPEND_ind; // 정수형 인디케이터
    SQLLEN DEVICE_TYPE_ind;
    SQLLEN MODEL_NAME_ind;
    SQLLEN SERIAL_NUM_ind;
    SQLLEN DEVICE_ID_ind;
    SQLLEN ID_TYPE_ind;        // 정수형 인디케이터
} Emg_ind_type;

int alti_connect(char *usr, char *pwd, char *conn_opt);
int alti_disconnect();

bool set_indicator(Emg_ind_type *emg_ind, Emg_type emg);
bool init_indicator(Emg_ind_type *emg_ind);

int insert_sql(Emg_type emg, Emg_ind_type emg_ind);
int update_sql(Emg_type emg, Emg_ind_type emg_ind);
int delete_sql(Emg_type emg);

void print_alti_error();

#endif
