#include "snd.h"

static short _esqlopts[10] = {0,0,1,0,0,0,0,0,0,0};

int alti_connect() {
    char* usr="EMG";
    char* pwd="emg123";
    char* conn_opt="Server=172.17.0.2;CONNTYPE=1";
    
{
    struct ulpSqlstmt ulpSqlstmt;
    memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
    ulpHostVar ulpHostVar[3];
    ulpSqlstmt.hostvalue = ulpHostVar;
    ulpSqlstmt.stmttype = 1;
    ulpSqlstmt.stmtname = NULL;
    ulpSqlstmt.ismt = 0;
    ulpSqlstmt.sqlinfo = 0;
    ulpSqlstmt.numofhostvar = 3;
    ulpSqlstmt.esqlopts    = _esqlopts;
    ulpSqlstmt.sqlcaerr    = &sqlca;
    ulpSqlstmt.sqlcodeerr  = &SQLCODE;
    ulpSqlstmt.sqlstateerr = ulpGetSqlstate();
    ulpSqlstmt.hostvalue[0].mHostVar = (void*)usr;
    ulpSqlstmt.hostvalue[1].mHostVar = (void*)pwd;
    ulpSqlstmt.hostvalue[2].mHostVar = (void*)conn_opt;
    ulpDoEmsql( NULL, &ulpSqlstmt, NULL );
}

    printf("------------------------------------------------------------------\n");
    printf("[Connect]\n");
    printf("------------------------------------------------------------------\n");

    return (sqlca.sqlcode == SQL_SUCCESS);
}

int alti_disconnect() {
{
    struct ulpSqlstmt ulpSqlstmt;
    memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
    ulpSqlstmt.stmttype = 2;
    ulpSqlstmt.stmtname = NULL;
    ulpSqlstmt.ismt = 0;
    ulpSqlstmt.numofhostvar = 0;
    ulpSqlstmt.esqlopts    = _esqlopts;
    ulpSqlstmt.sqlcaerr    = &sqlca;
    ulpSqlstmt.sqlcodeerr  = &SQLCODE;
    ulpSqlstmt.sqlstateerr = ulpGetSqlstate();
    ulpDoEmsql( NULL, &ulpSqlstmt, NULL );
}

    printf("------------------------------------------------------------------\n");
    printf("[Disconnect]\n");
    printf("------------------------------------------------------------------\n");

    return (sqlca.sqlcode == SQL_SUCCESS);
}

// LTEID가 UNIQUE CODE
int insert_sql(Emg_type struct_code) {
    Emg_type emg = struct_code;

    /*  INSERT INTO SUBUE_TBL VALUES (:emg); */
    {
        struct ulpSqlstmt ulpSqlstmt;
        memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
        ulpHostVar ulpHostVar[12];
        ulpSqlstmt.hostvalue = ulpHostVar;
        ulpSqlstmt.stmttype = 3;
        ulpSqlstmt.stmtname = NULL;
        ulpSqlstmt.ismt = 0;
        ulpSqlstmt.numofhostvar = 12;
        ulpSqlstmt.statusptr = NULL;
        ulpSqlstmt.errcodeptr = NULL;
        ulpSqlstmt.isatomic = 0;
        ulpSqlstmt.stmt = (char *)"INSERT INTO SUBUE_TBL VALUES (? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )";
        ulpSqlstmt.iters = 0;
        ulpSqlstmt.sqlinfo = 0;
        ulpSqlstmt.scrollcur = 0;
        ulpSqlstmt.cursorscrollable = 0;
        ulpSqlstmt.cursorsensitivity = 1;
        ulpSqlstmt.cursorwithhold = 0;
        ulpSqlstmt.esqlopts = _esqlopts;
        ulpSqlstmt.sqlcaerr = &sqlca;
        ulpSqlstmt.sqlcodeerr = &SQLCODE;
        ulpSqlstmt.sqlstateerr = ulpGetSqlstate();
        ulpSqlstmt.hostvalue[0].mHostVar = (void *)(emg.LTEID);
        ulpSqlstmt.hostvalue[0].mHostInd = NULL;
        ulpSqlstmt.hostvalue[0].isstruct = 1;
        ulpSqlstmt.hostvalue[0].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[0].isarr = 0;
        ulpSqlstmt.hostvalue[0].mType = 6;
        ulpSqlstmt.hostvalue[0].mSizeof = 0;
        ulpSqlstmt.hostvalue[0].mLen = 22;
        ulpSqlstmt.hostvalue[0].mMaxlen = sizeof(emg.LTEID);
        ulpSqlstmt.hostvalue[0].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[0].mInOut = (short)0;
        ulpSqlstmt.hostvalue[0].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[1].mHostVar = (void *)(emg.SLICE_ID);
        ulpSqlstmt.hostvalue[1].mHostInd = NULL;
        ulpSqlstmt.hostvalue[1].isstruct = 1;
        ulpSqlstmt.hostvalue[1].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[1].isarr = 0;
        ulpSqlstmt.hostvalue[1].mType = 6;
        ulpSqlstmt.hostvalue[1].mSizeof = 0;
        ulpSqlstmt.hostvalue[1].mLen = 6;
        ulpSqlstmt.hostvalue[1].mMaxlen = sizeof(emg.SLICE_ID);
        ulpSqlstmt.hostvalue[1].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[1].mInOut = (short)0;
        ulpSqlstmt.hostvalue[1].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[2].mHostVar = (void *)(emg.MDN);
        ulpSqlstmt.hostvalue[2].mHostInd = NULL;
        ulpSqlstmt.hostvalue[2].isstruct = 1;
        ulpSqlstmt.hostvalue[2].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[2].isarr = 0;
        ulpSqlstmt.hostvalue[2].mType = 6;
        ulpSqlstmt.hostvalue[2].mSizeof = 0;
        ulpSqlstmt.hostvalue[2].mLen = 13;
        ulpSqlstmt.hostvalue[2].mMaxlen = sizeof(emg.MDN);
        ulpSqlstmt.hostvalue[2].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[2].mInOut = (short)0;
        ulpSqlstmt.hostvalue[2].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[3].mHostVar = (void *)&(emg.IP_POOL_INDEX);
        ulpSqlstmt.hostvalue[3].mHostInd = NULL;
        ulpSqlstmt.hostvalue[3].isstruct = 1;
        ulpSqlstmt.hostvalue[3].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[3].isarr = 0;
        ulpSqlstmt.hostvalue[3].mType = 2;
        ulpSqlstmt.hostvalue[3].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[3].mLen = 0;
        ulpSqlstmt.hostvalue[3].mMaxlen = 0;
        ulpSqlstmt.hostvalue[3].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[3].mInOut = (short)0;
        ulpSqlstmt.hostvalue[3].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[4].mHostVar = (void *)(emg.IP);
        ulpSqlstmt.hostvalue[4].mHostInd = NULL;
        ulpSqlstmt.hostvalue[4].isstruct = 1;
        ulpSqlstmt.hostvalue[4].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[4].isarr = 0;
        ulpSqlstmt.hostvalue[4].mType = 6;
        ulpSqlstmt.hostvalue[4].mSizeof = 0;
        ulpSqlstmt.hostvalue[4].mLen = 41;
        ulpSqlstmt.hostvalue[4].mMaxlen = sizeof(emg.IP);
        ulpSqlstmt.hostvalue[4].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[4].mInOut = (short)0;
        ulpSqlstmt.hostvalue[4].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[5].mHostVar = (void *)&(emg.AUTH_TYPE);
        ulpSqlstmt.hostvalue[5].mHostInd = NULL;
        ulpSqlstmt.hostvalue[5].isstruct = 1;
        ulpSqlstmt.hostvalue[5].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[5].isarr = 0;
        ulpSqlstmt.hostvalue[5].mType = 2;
        ulpSqlstmt.hostvalue[5].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[5].mLen = 0;
        ulpSqlstmt.hostvalue[5].mMaxlen = 0;
        ulpSqlstmt.hostvalue[5].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[5].mInOut = (short)0;
        ulpSqlstmt.hostvalue[5].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[6].mHostVar = (void *)(emg.USER_ID);
        ulpSqlstmt.hostvalue[6].mHostInd = NULL;
        ulpSqlstmt.hostvalue[6].isstruct = 1;
        ulpSqlstmt.hostvalue[6].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[6].isarr = 0;
        ulpSqlstmt.hostvalue[6].mType = 6;
        ulpSqlstmt.hostvalue[6].mSizeof = 0;
        ulpSqlstmt.hostvalue[6].mLen = 24;
        ulpSqlstmt.hostvalue[6].mMaxlen = sizeof(emg.USER_ID);
        ulpSqlstmt.hostvalue[6].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[6].mInOut = (short)0;
        ulpSqlstmt.hostvalue[6].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[7].mHostVar = (void *)&(emg.TWOFA_USE);
        ulpSqlstmt.hostvalue[7].mHostInd = NULL;
        ulpSqlstmt.hostvalue[7].isstruct = 1;
        ulpSqlstmt.hostvalue[7].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[7].isarr = 0;
        ulpSqlstmt.hostvalue[7].mType = 2;
        ulpSqlstmt.hostvalue[7].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[7].mLen = 0;
        ulpSqlstmt.hostvalue[7].mMaxlen = 0;
        ulpSqlstmt.hostvalue[7].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[7].mInOut = (short)0;
        ulpSqlstmt.hostvalue[7].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[8].mHostVar = (void *)&(emg.DEVICE_SUSPEND);
        ulpSqlstmt.hostvalue[8].mHostInd = NULL;
        ulpSqlstmt.hostvalue[8].isstruct = 1;
        ulpSqlstmt.hostvalue[8].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[8].isarr = 0;
        ulpSqlstmt.hostvalue[8].mType = 2;
        ulpSqlstmt.hostvalue[8].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[8].mLen = 0;
        ulpSqlstmt.hostvalue[8].mMaxlen = 0;
        ulpSqlstmt.hostvalue[8].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[8].mInOut = (short)0;
        ulpSqlstmt.hostvalue[8].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[9].mHostVar = (void *)(emg.DEVICE_TYPE);
        ulpSqlstmt.hostvalue[9].mHostInd = NULL;
        ulpSqlstmt.hostvalue[9].isstruct = 1;
        ulpSqlstmt.hostvalue[9].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[9].isarr = 0;
        ulpSqlstmt.hostvalue[9].mType = 6;
        ulpSqlstmt.hostvalue[9].mSizeof = 0;
        ulpSqlstmt.hostvalue[9].mLen = 12;
        ulpSqlstmt.hostvalue[9].mMaxlen = sizeof(emg.DEVICE_TYPE);
        ulpSqlstmt.hostvalue[9].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[9].mInOut = (short)0;
        ulpSqlstmt.hostvalue[9].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[10].mHostVar = (void *)(emg.MODEL_NAME);
        ulpSqlstmt.hostvalue[10].mHostInd = NULL;
        ulpSqlstmt.hostvalue[10].isstruct = 1;
        ulpSqlstmt.hostvalue[10].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[10].isarr = 0;
        ulpSqlstmt.hostvalue[10].mType = 6;
        ulpSqlstmt.hostvalue[10].mSizeof = 0;
        ulpSqlstmt.hostvalue[10].mLen = 32;
        ulpSqlstmt.hostvalue[10].mMaxlen = sizeof(emg.MODEL_NAME);
        ulpSqlstmt.hostvalue[10].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[10].mInOut = (short)0;
        ulpSqlstmt.hostvalue[10].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[11].mHostVar = (void *)(emg.SERIAL_NUM);
        ulpSqlstmt.hostvalue[11].mHostInd = NULL;
        ulpSqlstmt.hostvalue[11].isstruct = 1;
        ulpSqlstmt.hostvalue[11].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[11].isarr = 0;
        ulpSqlstmt.hostvalue[11].mType = 6;
        ulpSqlstmt.hostvalue[11].mSizeof = 0;
        ulpSqlstmt.hostvalue[11].mLen = 32;
        ulpSqlstmt.hostvalue[11].mMaxlen = sizeof(emg.SERIAL_NUM);
        ulpSqlstmt.hostvalue[11].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[11].mInOut = (short)0;
        ulpSqlstmt.hostvalue[11].mIsDynAlloc = (short)0;
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
    }

    return (sqlca.sqlcode == SQL_SUCCESS);
}