#ifndef HOST_VAR_EMG_H
#define HOST_VAR_EMG_H

#include <ulpLibInterface.h>
// EXEC SQL BEGIN DECLARE SECTION;

typedef struct {
    char LTEID[22];         // VARCHAR(21) + 1
    char SLICE_ID[6];       // VARCHAR(5) + 1
    char MDN[13];           // VARCHAR(12) + 1
    int  IP_POOL_INDEX;     // INTEGER
    char IP[41];            // VARCHAR(40) + 1
    int  AUTH_TYPE;         // INTEGER
    char USER_ID[24];       // VARCHAR(23) + 1
    int  TWOFA_USE;         // INTEGER
    int  DEVICE_SUSPEND;    // INTEGER
    char DEVICE_TYPE[12];   // VARCHAR(11) + 1
    char MODEL_NAME[32];    // VARCHAR(31) + 1
    char SERIAL_NUM[32];    // VARCHAR(31) + 1
    char DEVICE_ID[18];     // VARCHAR(17) + 1
    int  ID_TYPE;           // INTEGER
} Emg_type;

// EXEC SQL END DECLARE SECTION;

int alti_connect(char *usr, char *pwd, char *conn_opt);
int alti_disconnect();

int insert_sql(Emg_type emg);
int update_sql(Emg_type emg);
int delete_sql(Emg_type emg);

#endif