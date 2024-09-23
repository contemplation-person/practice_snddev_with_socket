#include "snd.h"

static short _esqlopts[10] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0};

int alti_connect(char *usr, char *pwd, char *conn_opt) {
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
        ulpSqlstmt.esqlopts = _esqlopts;
        ulpSqlstmt.sqlcaerr = &sqlca;
        ulpSqlstmt.sqlcodeerr = &SQLCODE;
        ulpSqlstmt.sqlstateerr = ulpGetSqlstate();
        ulpSqlstmt.hostvalue[0].mHostVar = (void *)usr;
        ulpSqlstmt.hostvalue[1].mHostVar = (void *)pwd;
        ulpSqlstmt.hostvalue[2].mHostVar = (void *)conn_opt;
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
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
        ulpSqlstmt.esqlopts = _esqlopts;
        ulpSqlstmt.sqlcaerr = &sqlca;
        ulpSqlstmt.sqlcodeerr = &SQLCODE;
        ulpSqlstmt.sqlstateerr = ulpGetSqlstate();
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
    }

    printf("------------------------------------------------------------------\n");
    printf("[Disconnect]\n");
    printf("------------------------------------------------------------------\n");

    return (sqlca.sqlcode == SQL_SUCCESS);
}

bool init_indicator(Emg_ind_type *emg_ind) {
    emg_ind->LTEID_ind = 0;
    emg_ind->SLICE_ID_ind = 0;
    emg_ind->MDN_ind = 0;
    emg_ind->IP_POOL_INDEX_ind = 0;
    emg_ind->IP_ind = 0;
    emg_ind->AUTH_TYPE_ind = 0;
    emg_ind->USER_ID_ind = 0;
    emg_ind->TWOFA_USE_ind = 0;
    emg_ind->DEVICE_SUSPEND_ind = 0;
    emg_ind->DEVICE_TYPE_ind = 0;
    emg_ind->MODEL_NAME_ind = 0;
    emg_ind->SERIAL_NUM_ind = 0;
    emg_ind->DEVICE_ID_ind = 0;
    emg_ind->ID_TYPE_ind = 0;

    return true;
}

bool set_indicator(Emg_ind_type *emg_ind, Emg_type emg) {
    if (emg.LTEID == 0 || emg.SLICE_ID == 0) {
        return false;
    }
    emg_ind->LTEID_ind = strlen(emg.LTEID);
    emg_ind->SLICE_ID_ind = strlen(emg.SLICE_ID);

    emg_ind->IP_POOL_INDEX_ind = 0;
    emg_ind->AUTH_TYPE_ind = 0;
    emg_ind->TWOFA_USE_ind = 0;
    emg_ind->DEVICE_SUSPEND_ind = 0;
    emg_ind->ID_TYPE_ind = 0;

    if (emg.MDN == NULL) {
        emg_ind->MDN_ind = SQL_NULL_DATA;
    } else {
        emg_ind->MDN_ind = strlen(emg.MDN);
    }
    if (emg.IP == NULL) {
        emg_ind->IP_ind = SQL_NULL_DATA;
    } else {
        emg_ind->IP_ind = strlen(emg.IP);
    }
    if (emg.USER_ID == NULL) {
        emg_ind->USER_ID_ind = SQL_NULL_DATA;
    } else {
        emg_ind->USER_ID_ind = strlen(emg.USER_ID);
    }
    if (emg.DEVICE_TYPE == NULL) {
        emg_ind->DEVICE_TYPE_ind = SQL_NULL_DATA;
    } else {
        emg_ind->DEVICE_TYPE_ind = strlen(emg.DEVICE_TYPE);
    }
    if (emg.MODEL_NAME == NULL) {
        emg_ind->MODEL_NAME_ind = SQL_NULL_DATA;
    } else {
        emg_ind->MODEL_NAME_ind = strlen(emg.MODEL_NAME);
    }
    if (emg.SERIAL_NUM == NULL) {
        emg_ind->SERIAL_NUM_ind = SQL_NULL_DATA;
    } else {
        emg_ind->SERIAL_NUM_ind = strlen(emg.SERIAL_NUM);
    }
    if (emg.DEVICE_ID == NULL) {
        emg_ind->DEVICE_ID_ind = SQL_NULL_DATA;
    } else {
        emg_ind->DEVICE_ID_ind = strlen(emg.DEVICE_ID);
    }

    return true;
}

void print_alti_error() {
    printf("Error : [%d] %s\n\n", SQLCODE, sqlca.sqlerrm.sqlerrmc);
}

int insert_sql(Emg_type emg, Emg_ind_type emg_ind) {
    /*  INSERT INTO SUBUE_TBL VALUES (:emg :emg_ind); */
    {
        struct ulpSqlstmt ulpSqlstmt;
        memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
        ulpHostVar ulpHostVar[14];
        ulpSqlstmt.hostvalue = ulpHostVar;
        ulpSqlstmt.stmttype = 3;
        ulpSqlstmt.stmtname = NULL;
        ulpSqlstmt.ismt = 0;
        ulpSqlstmt.numofhostvar = 14;
        ulpSqlstmt.statusptr = NULL;
        ulpSqlstmt.errcodeptr = NULL;
        ulpSqlstmt.isatomic = 0;
        ulpSqlstmt.stmt =
            (char *)"INSERT INTO SUBUE_TBL VALUES (? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?          )";
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
        ulpSqlstmt.hostvalue[0].mHostInd = (SQLLEN *)&(emg_ind.LTEID_ind);
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
        ulpSqlstmt.hostvalue[1].mHostInd = (SQLLEN *)&(emg_ind.SLICE_ID_ind);
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
        ulpSqlstmt.hostvalue[2].mHostInd = (SQLLEN *)&(emg_ind.MDN_ind);
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
        ulpSqlstmt.hostvalue[3].mHostInd = (SQLLEN *)&(emg_ind.IP_POOL_INDEX_ind);
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
        ulpSqlstmt.hostvalue[4].mHostInd = (SQLLEN *)&(emg_ind.IP_ind);
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
        ulpSqlstmt.hostvalue[5].mHostInd = (SQLLEN *)&(emg_ind.AUTH_TYPE_ind);
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
        ulpSqlstmt.hostvalue[6].mHostInd = (SQLLEN *)&(emg_ind.USER_ID_ind);
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
        ulpSqlstmt.hostvalue[7].mHostInd = (SQLLEN *)&(emg_ind.TWOFA_USE_ind);
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
        ulpSqlstmt.hostvalue[8].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_SUSPEND_ind);
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
        ulpSqlstmt.hostvalue[9].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_TYPE_ind);
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
        ulpSqlstmt.hostvalue[10].mHostInd = (SQLLEN *)&(emg_ind.MODEL_NAME_ind);
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
        ulpSqlstmt.hostvalue[11].mHostInd = (SQLLEN *)&(emg_ind.SERIAL_NUM_ind);
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
        ulpSqlstmt.hostvalue[12].mHostVar = (void *)(emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[12].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_ID_ind);
        ulpSqlstmt.hostvalue[12].isstruct = 1;
        ulpSqlstmt.hostvalue[12].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[12].isarr = 0;
        ulpSqlstmt.hostvalue[12].mType = 6;
        ulpSqlstmt.hostvalue[12].mSizeof = 0;
        ulpSqlstmt.hostvalue[12].mLen = 18;
        ulpSqlstmt.hostvalue[12].mMaxlen = sizeof(emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[12].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[12].mInOut = (short)0;
        ulpSqlstmt.hostvalue[12].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[13].mHostVar = (void *)&(emg.ID_TYPE);
        ulpSqlstmt.hostvalue[13].mHostInd = (SQLLEN *)&(emg_ind.ID_TYPE_ind);
        ulpSqlstmt.hostvalue[13].isstruct = 1;
        ulpSqlstmt.hostvalue[13].structsize = sizeof(emg);
        ulpSqlstmt.hostvalue[13].isarr = 0;
        ulpSqlstmt.hostvalue[13].mType = 2;
        ulpSqlstmt.hostvalue[13].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[13].mLen = 0;
        ulpSqlstmt.hostvalue[13].mMaxlen = 0;
        ulpSqlstmt.hostvalue[13].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[13].mInOut = (short)0;
        ulpSqlstmt.hostvalue[13].mIsDynAlloc = (short)0;
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
    }

    return (sqlca.sqlcode == SQL_SUCCESS);
}

int update_sql(Emg_type emg, Emg_ind_type emg_ind) {
    Emg_type select_emg;

    /*  SELECT *                           FROM subue_tbl WHERE DEVICE_ID=:emg.DEVICE_ID AND LTEID=:emg.LTEID; */
    {
        struct ulpSqlstmt ulpSqlstmt;
        memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
        ulpHostVar ulpHostVar[16];
        ulpSqlstmt.hostvalue = ulpHostVar;
        ulpSqlstmt.stmttype = 4;
        ulpSqlstmt.stmtname = NULL;
        ulpSqlstmt.ismt = 0;
        ulpSqlstmt.numofhostvar = 16;
        ulpSqlstmt.statusptr = NULL;
        ulpSqlstmt.errcodeptr = NULL;
        ulpSqlstmt.isatomic = 0;
        ulpSqlstmt.stmt = (char *)"SELECT *                           FROM subue_tbl WHERE DEVICE_ID=?  AND LTEID=? ";
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
        ulpSqlstmt.hostvalue[0].mHostVar = (void *)(select_emg.LTEID);
        ulpSqlstmt.hostvalue[0].mHostInd = (SQLLEN *)&(emg_ind.LTEID_ind);
        ulpSqlstmt.hostvalue[0].isstruct = 1;
        ulpSqlstmt.hostvalue[0].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[0].isarr = 0;
        ulpSqlstmt.hostvalue[0].mType = 6;
        ulpSqlstmt.hostvalue[0].mSizeof = 0;
        ulpSqlstmt.hostvalue[0].mLen = 22;
        ulpSqlstmt.hostvalue[0].mMaxlen = sizeof(select_emg.LTEID);
        ulpSqlstmt.hostvalue[0].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[0].mInOut = (short)1;
        ulpSqlstmt.hostvalue[0].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[1].mHostVar = (void *)(select_emg.SLICE_ID);
        ulpSqlstmt.hostvalue[1].mHostInd = (SQLLEN *)&(emg_ind.SLICE_ID_ind);
        ulpSqlstmt.hostvalue[1].isstruct = 1;
        ulpSqlstmt.hostvalue[1].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[1].isarr = 0;
        ulpSqlstmt.hostvalue[1].mType = 6;
        ulpSqlstmt.hostvalue[1].mSizeof = 0;
        ulpSqlstmt.hostvalue[1].mLen = 6;
        ulpSqlstmt.hostvalue[1].mMaxlen = sizeof(select_emg.SLICE_ID);
        ulpSqlstmt.hostvalue[1].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[1].mInOut = (short)1;
        ulpSqlstmt.hostvalue[1].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[2].mHostVar = (void *)(select_emg.MDN);
        ulpSqlstmt.hostvalue[2].mHostInd = (SQLLEN *)&(emg_ind.MDN_ind);
        ulpSqlstmt.hostvalue[2].isstruct = 1;
        ulpSqlstmt.hostvalue[2].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[2].isarr = 0;
        ulpSqlstmt.hostvalue[2].mType = 6;
        ulpSqlstmt.hostvalue[2].mSizeof = 0;
        ulpSqlstmt.hostvalue[2].mLen = 13;
        ulpSqlstmt.hostvalue[2].mMaxlen = sizeof(select_emg.MDN);
        ulpSqlstmt.hostvalue[2].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[2].mInOut = (short)1;
        ulpSqlstmt.hostvalue[2].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[3].mHostVar = (void *)&(select_emg.IP_POOL_INDEX);
        ulpSqlstmt.hostvalue[3].mHostInd = (SQLLEN *)&(emg_ind.IP_POOL_INDEX_ind);
        ulpSqlstmt.hostvalue[3].isstruct = 1;
        ulpSqlstmt.hostvalue[3].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[3].isarr = 0;
        ulpSqlstmt.hostvalue[3].mType = 2;
        ulpSqlstmt.hostvalue[3].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[3].mLen = 0;
        ulpSqlstmt.hostvalue[3].mMaxlen = 0;
        ulpSqlstmt.hostvalue[3].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[3].mInOut = (short)1;
        ulpSqlstmt.hostvalue[3].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[4].mHostVar = (void *)(select_emg.IP);
        ulpSqlstmt.hostvalue[4].mHostInd = (SQLLEN *)&(emg_ind.IP_ind);
        ulpSqlstmt.hostvalue[4].isstruct = 1;
        ulpSqlstmt.hostvalue[4].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[4].isarr = 0;
        ulpSqlstmt.hostvalue[4].mType = 6;
        ulpSqlstmt.hostvalue[4].mSizeof = 0;
        ulpSqlstmt.hostvalue[4].mLen = 41;
        ulpSqlstmt.hostvalue[4].mMaxlen = sizeof(select_emg.IP);
        ulpSqlstmt.hostvalue[4].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[4].mInOut = (short)1;
        ulpSqlstmt.hostvalue[4].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[5].mHostVar = (void *)&(select_emg.AUTH_TYPE);
        ulpSqlstmt.hostvalue[5].mHostInd = (SQLLEN *)&(emg_ind.AUTH_TYPE_ind);
        ulpSqlstmt.hostvalue[5].isstruct = 1;
        ulpSqlstmt.hostvalue[5].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[5].isarr = 0;
        ulpSqlstmt.hostvalue[5].mType = 2;
        ulpSqlstmt.hostvalue[5].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[5].mLen = 0;
        ulpSqlstmt.hostvalue[5].mMaxlen = 0;
        ulpSqlstmt.hostvalue[5].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[5].mInOut = (short)1;
        ulpSqlstmt.hostvalue[5].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[6].mHostVar = (void *)(select_emg.USER_ID);
        ulpSqlstmt.hostvalue[6].mHostInd = (SQLLEN *)&(emg_ind.USER_ID_ind);
        ulpSqlstmt.hostvalue[6].isstruct = 1;
        ulpSqlstmt.hostvalue[6].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[6].isarr = 0;
        ulpSqlstmt.hostvalue[6].mType = 6;
        ulpSqlstmt.hostvalue[6].mSizeof = 0;
        ulpSqlstmt.hostvalue[6].mLen = 24;
        ulpSqlstmt.hostvalue[6].mMaxlen = sizeof(select_emg.USER_ID);
        ulpSqlstmt.hostvalue[6].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[6].mInOut = (short)1;
        ulpSqlstmt.hostvalue[6].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[7].mHostVar = (void *)&(select_emg.TWOFA_USE);
        ulpSqlstmt.hostvalue[7].mHostInd = (SQLLEN *)&(emg_ind.TWOFA_USE_ind);
        ulpSqlstmt.hostvalue[7].isstruct = 1;
        ulpSqlstmt.hostvalue[7].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[7].isarr = 0;
        ulpSqlstmt.hostvalue[7].mType = 2;
        ulpSqlstmt.hostvalue[7].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[7].mLen = 0;
        ulpSqlstmt.hostvalue[7].mMaxlen = 0;
        ulpSqlstmt.hostvalue[7].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[7].mInOut = (short)1;
        ulpSqlstmt.hostvalue[7].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[8].mHostVar = (void *)&(select_emg.DEVICE_SUSPEND);
        ulpSqlstmt.hostvalue[8].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_SUSPEND_ind);
        ulpSqlstmt.hostvalue[8].isstruct = 1;
        ulpSqlstmt.hostvalue[8].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[8].isarr = 0;
        ulpSqlstmt.hostvalue[8].mType = 2;
        ulpSqlstmt.hostvalue[8].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[8].mLen = 0;
        ulpSqlstmt.hostvalue[8].mMaxlen = 0;
        ulpSqlstmt.hostvalue[8].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[8].mInOut = (short)1;
        ulpSqlstmt.hostvalue[8].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[9].mHostVar = (void *)(select_emg.DEVICE_TYPE);
        ulpSqlstmt.hostvalue[9].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_TYPE_ind);
        ulpSqlstmt.hostvalue[9].isstruct = 1;
        ulpSqlstmt.hostvalue[9].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[9].isarr = 0;
        ulpSqlstmt.hostvalue[9].mType = 6;
        ulpSqlstmt.hostvalue[9].mSizeof = 0;
        ulpSqlstmt.hostvalue[9].mLen = 12;
        ulpSqlstmt.hostvalue[9].mMaxlen = sizeof(select_emg.DEVICE_TYPE);
        ulpSqlstmt.hostvalue[9].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[9].mInOut = (short)1;
        ulpSqlstmt.hostvalue[9].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[10].mHostVar = (void *)(select_emg.MODEL_NAME);
        ulpSqlstmt.hostvalue[10].mHostInd = (SQLLEN *)&(emg_ind.MODEL_NAME_ind);
        ulpSqlstmt.hostvalue[10].isstruct = 1;
        ulpSqlstmt.hostvalue[10].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[10].isarr = 0;
        ulpSqlstmt.hostvalue[10].mType = 6;
        ulpSqlstmt.hostvalue[10].mSizeof = 0;
        ulpSqlstmt.hostvalue[10].mLen = 32;
        ulpSqlstmt.hostvalue[10].mMaxlen = sizeof(select_emg.MODEL_NAME);
        ulpSqlstmt.hostvalue[10].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[10].mInOut = (short)1;
        ulpSqlstmt.hostvalue[10].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[11].mHostVar = (void *)(select_emg.SERIAL_NUM);
        ulpSqlstmt.hostvalue[11].mHostInd = (SQLLEN *)&(emg_ind.SERIAL_NUM_ind);
        ulpSqlstmt.hostvalue[11].isstruct = 1;
        ulpSqlstmt.hostvalue[11].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[11].isarr = 0;
        ulpSqlstmt.hostvalue[11].mType = 6;
        ulpSqlstmt.hostvalue[11].mSizeof = 0;
        ulpSqlstmt.hostvalue[11].mLen = 32;
        ulpSqlstmt.hostvalue[11].mMaxlen = sizeof(select_emg.SERIAL_NUM);
        ulpSqlstmt.hostvalue[11].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[11].mInOut = (short)1;
        ulpSqlstmt.hostvalue[11].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[12].mHostVar = (void *)(select_emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[12].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_ID_ind);
        ulpSqlstmt.hostvalue[12].isstruct = 1;
        ulpSqlstmt.hostvalue[12].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[12].isarr = 0;
        ulpSqlstmt.hostvalue[12].mType = 6;
        ulpSqlstmt.hostvalue[12].mSizeof = 0;
        ulpSqlstmt.hostvalue[12].mLen = 18;
        ulpSqlstmt.hostvalue[12].mMaxlen = sizeof(select_emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[12].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[12].mInOut = (short)1;
        ulpSqlstmt.hostvalue[12].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[13].mHostVar = (void *)&(select_emg.ID_TYPE);
        ulpSqlstmt.hostvalue[13].mHostInd = (SQLLEN *)&(emg_ind.ID_TYPE_ind);
        ulpSqlstmt.hostvalue[13].isstruct = 1;
        ulpSqlstmt.hostvalue[13].structsize = sizeof(select_emg);
        ulpSqlstmt.hostvalue[13].isarr = 0;
        ulpSqlstmt.hostvalue[13].mType = 2;
        ulpSqlstmt.hostvalue[13].mSizeof = sizeof(int);
        ulpSqlstmt.hostvalue[13].mLen = 0;
        ulpSqlstmt.hostvalue[13].mMaxlen = 0;
        ulpSqlstmt.hostvalue[13].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[13].mInOut = (short)1;
        ulpSqlstmt.hostvalue[13].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[14].mHostVar = (void *)(emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[14].mHostInd = NULL;
        ulpSqlstmt.hostvalue[14].isarr = 0;
        ulpSqlstmt.hostvalue[14].mType = 6;
        ulpSqlstmt.hostvalue[14].isstruct = 0;
        ulpSqlstmt.hostvalue[14].mSizeof = 0;
        ulpSqlstmt.hostvalue[14].mLen = 18;
        ulpSqlstmt.hostvalue[14].mMaxlen = sizeof(emg.DEVICE_ID);
        ulpSqlstmt.hostvalue[14].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[14].mInOut = (short)0;
        ulpSqlstmt.hostvalue[14].mIsDynAlloc = (short)0;
        ulpSqlstmt.hostvalue[15].mHostVar = (void *)(emg.LTEID);
        ulpSqlstmt.hostvalue[15].mHostInd = NULL;
        ulpSqlstmt.hostvalue[15].isarr = 0;
        ulpSqlstmt.hostvalue[15].mType = 6;
        ulpSqlstmt.hostvalue[15].isstruct = 0;
        ulpSqlstmt.hostvalue[15].mSizeof = 0;
        ulpSqlstmt.hostvalue[15].mLen = 22;
        ulpSqlstmt.hostvalue[15].mMaxlen = sizeof(emg.LTEID);
        ulpSqlstmt.hostvalue[15].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[15].mInOut = (short)0;
        ulpSqlstmt.hostvalue[15].mIsDynAlloc = (short)0;
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
    }

    if (sqlca.sqlcode == SQL_SUCCESS) {
        /*  UPDATE SUBUE_TBL SET (LTEID, SLICE_ID, MDN, IP_POOL_INDEX, IP, AUTH_TYPE, USER_ID, TWOFA_USE,
         * DEVICE_SUSPEND, DEVICE_TYPE, MODEL_NAME, SERIAL_NUM, DEVICE_ID, ID_TYPE)=(:emg         ) WHERE
         * DEVICE_ID=:emg.DEVICE_ID AND LTEID=:emg.LTEID; */
        {
            struct ulpSqlstmt ulpSqlstmt;
            memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
            ulpHostVar ulpHostVar[16];
            ulpSqlstmt.hostvalue = ulpHostVar;
            ulpSqlstmt.stmttype = 3;
            ulpSqlstmt.stmtname = NULL;
            ulpSqlstmt.ismt = 0;
            ulpSqlstmt.numofhostvar = 16;
            ulpSqlstmt.statusptr = NULL;
            ulpSqlstmt.errcodeptr = NULL;
            ulpSqlstmt.isatomic = 0;
            ulpSqlstmt.stmt = (char *)"UPDATE SUBUE_TBL SET (LTEID, SLICE_ID, MDN, IP_POOL_INDEX, IP, AUTH_TYPE, USER_ID, TWOFA_USE, DEVICE_SUSPEND, DEVICE_TYPE, MODEL_NAME, SERIAL_NUM, DEVICE_ID, ID_TYPE)=(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?          ) WHERE DEVICE_ID=?  AND LTEID=? ";
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
            ulpSqlstmt.hostvalue[0].mHostInd = (SQLLEN *)&(emg_ind.LTEID_ind);
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
            ulpSqlstmt.hostvalue[1].mHostInd = (SQLLEN *)&(emg_ind.SLICE_ID_ind);
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
            ulpSqlstmt.hostvalue[2].mHostInd = (SQLLEN *)&(emg_ind.MDN_ind);
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
            ulpSqlstmt.hostvalue[3].mHostInd = (SQLLEN *)&(emg_ind.IP_POOL_INDEX_ind);
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
            ulpSqlstmt.hostvalue[4].mHostInd = (SQLLEN *)&(emg_ind.IP_ind);
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
            ulpSqlstmt.hostvalue[5].mHostInd = (SQLLEN *)&(emg_ind.AUTH_TYPE_ind);
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
            ulpSqlstmt.hostvalue[6].mHostInd = (SQLLEN *)&(emg_ind.USER_ID_ind);
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
            ulpSqlstmt.hostvalue[7].mHostInd = (SQLLEN *)&(emg_ind.TWOFA_USE_ind);
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
            ulpSqlstmt.hostvalue[8].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_SUSPEND_ind);
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
            ulpSqlstmt.hostvalue[9].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_TYPE_ind);
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
            ulpSqlstmt.hostvalue[10].mHostInd = (SQLLEN *)&(emg_ind.MODEL_NAME_ind);
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
            ulpSqlstmt.hostvalue[11].mHostInd = (SQLLEN *)&(emg_ind.SERIAL_NUM_ind);
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
            ulpSqlstmt.hostvalue[12].mHostVar = (void *)(emg.DEVICE_ID);
            ulpSqlstmt.hostvalue[12].mHostInd = (SQLLEN *)&(emg_ind.DEVICE_ID_ind);
            ulpSqlstmt.hostvalue[12].isstruct = 1;
            ulpSqlstmt.hostvalue[12].structsize = sizeof(emg);
            ulpSqlstmt.hostvalue[12].isarr = 0;
            ulpSqlstmt.hostvalue[12].mType = 6;
            ulpSqlstmt.hostvalue[12].mSizeof = 0;
            ulpSqlstmt.hostvalue[12].mLen = 18;
            ulpSqlstmt.hostvalue[12].mMaxlen = sizeof(emg.DEVICE_ID);
            ulpSqlstmt.hostvalue[12].mUnsign = (short)0;
            ulpSqlstmt.hostvalue[12].mInOut = (short)0;
            ulpSqlstmt.hostvalue[12].mIsDynAlloc = (short)0;
            ulpSqlstmt.hostvalue[13].mHostVar = (void *)&(emg.ID_TYPE);
            ulpSqlstmt.hostvalue[13].mHostInd = (SQLLEN *)&(emg_ind.ID_TYPE_ind);
            ulpSqlstmt.hostvalue[13].isstruct = 1;
            ulpSqlstmt.hostvalue[13].structsize = sizeof(emg);
            ulpSqlstmt.hostvalue[13].isarr = 0;
            ulpSqlstmt.hostvalue[13].mType = 2;
            ulpSqlstmt.hostvalue[13].mSizeof = sizeof(int);
            ulpSqlstmt.hostvalue[13].mLen = 0;
            ulpSqlstmt.hostvalue[13].mMaxlen = 0;
            ulpSqlstmt.hostvalue[13].mUnsign = (short)0;
            ulpSqlstmt.hostvalue[13].mInOut = (short)0;
            ulpSqlstmt.hostvalue[13].mIsDynAlloc = (short)0;
            ulpSqlstmt.hostvalue[14].mHostVar = (void *)(emg.DEVICE_ID);
            ulpSqlstmt.hostvalue[14].mHostInd = NULL;
            ulpSqlstmt.hostvalue[14].isarr = 0;
            ulpSqlstmt.hostvalue[14].mType = 6;
            ulpSqlstmt.hostvalue[14].isstruct = 0;
            ulpSqlstmt.hostvalue[14].mSizeof = 0;
            ulpSqlstmt.hostvalue[14].mLen = 18;
            ulpSqlstmt.hostvalue[14].mMaxlen = sizeof(emg.DEVICE_ID);
            ulpSqlstmt.hostvalue[14].mUnsign = (short)0;
            ulpSqlstmt.hostvalue[14].mInOut = (short)0;
            ulpSqlstmt.hostvalue[14].mIsDynAlloc = (short)0;
            ulpSqlstmt.hostvalue[15].mHostVar = (void *)(emg.LTEID);
            ulpSqlstmt.hostvalue[15].mHostInd = NULL;
            ulpSqlstmt.hostvalue[15].isarr = 0;
            ulpSqlstmt.hostvalue[15].mType = 6;
            ulpSqlstmt.hostvalue[15].isstruct = 0;
            ulpSqlstmt.hostvalue[15].mSizeof = 0;
            ulpSqlstmt.hostvalue[15].mLen = 22;
            ulpSqlstmt.hostvalue[15].mMaxlen = sizeof(emg.LTEID);
            ulpSqlstmt.hostvalue[15].mUnsign = (short)0;
            ulpSqlstmt.hostvalue[15].mInOut = (short)0;
            ulpSqlstmt.hostvalue[15].mIsDynAlloc = (short)0;
            ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
        }

    } else {
        printf("SELECT Error : [%d] %s\n\n", SQLCODE, sqlca.sqlerrm.sqlerrmc);
    }

    return (sqlca.sqlcode == SQL_SUCCESS);
}

int delete_sql(Emg_type emg) {
    /*  DELETE FROM SUBUE_TBL WHERE LTEID=:emg.LTEID; */
    {
        struct ulpSqlstmt ulpSqlstmt;
        memset(&ulpSqlstmt, 0, sizeof(ulpSqlstmt));
        ulpHostVar ulpHostVar[1];
        ulpSqlstmt.hostvalue = ulpHostVar;
        ulpSqlstmt.stmttype = 3;
        ulpSqlstmt.stmtname = NULL;
        ulpSqlstmt.ismt = 0;
        ulpSqlstmt.numofhostvar = 1;
        ulpSqlstmt.statusptr = NULL;
        ulpSqlstmt.errcodeptr = NULL;
        ulpSqlstmt.isatomic = 0;
        ulpSqlstmt.stmt = (char *)"DELETE FROM SUBUE_TBL WHERE LTEID=? ";
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
        ulpSqlstmt.hostvalue[0].isarr = 0;
        ulpSqlstmt.hostvalue[0].mType = 6;
        ulpSqlstmt.hostvalue[0].isstruct = 0;
        ulpSqlstmt.hostvalue[0].mSizeof = 0;
        ulpSqlstmt.hostvalue[0].mLen = 22;
        ulpSqlstmt.hostvalue[0].mMaxlen = sizeof(emg.LTEID);
        ulpSqlstmt.hostvalue[0].mUnsign = (short)0;
        ulpSqlstmt.hostvalue[0].mInOut = (short)0;
        ulpSqlstmt.hostvalue[0].mIsDynAlloc = (short)0;
        ulpDoEmsql(NULL, &ulpSqlstmt, NULL);
    }

    return (sqlca.sqlcode == SQL_SUCCESS);
}
