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
