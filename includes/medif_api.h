#ifndef MEDIF_API_H
#define MEDIF_API_H

#define REST_API_FIRST	1
#define FOREACH_REST_API(_REST_API_)                           \
    _REST_API_(REST_API_UNKNOWN)                               \
        _REST_API_(CREATE_FIRST_NODE)                       \
        _REST_API_(DELETE_FIRST_NODE)                       \
        _REST_API_(QUERY_FIRSTDEVSTATUS)                       \
        _REST_API_(CREATE_SNDDEV_POLICY)                       \
        _REST_API_(DELETE_SNDDEV_POLICY)                       \
        _REST_API_(QUERY_SNDDEVSTATUS)                         \
        _REST_API_(CREATE_USERINFO)                       \
        _REST_API_(DELETE_USERINFO)                       \
        _REST_API_(SDP_AUTH_RESULT)                       \
        _REST_API_(MODIFY_CORP_POLICY)                         \
        _REST_API_(MODIFY_FIRSTDEV_POLICY)                     \
        _REST_API_(MODIFY_SNDDEV_POLICY)                       \
        _REST_API_(QUERY_SNDDEVNUM)                            \
        _REST_API_(QUERY_FIRSTDEV_HISTORY)                     \
        _REST_API_(QUERY_SNDDEVFORFIRSTDEV_HISTORY)            \
        _REST_API_(QUERY_SNDDEV_HISTORY)                       \
        _REST_API_(INIT_USER_PWD_POLICY)                       \
        _REST_API_(SET_USER_PWD_POLICY)                        \
        _REST_API_(HTTP_CLIENT_REQUEST)                        \
        _REST_API_(CLI_CREATE_USER_POLICY)                     \
        _REST_API_(CLI_MODIFY_USER_POLICY)                     \
        _REST_API_(CLI_DELETE_USER_POLICY)                     \
        _REST_API_(CLI_EXPIRE_NOTI_USER_POLICY)                \
        _REST_API_(CLI_CREATE_IPPOOL_POLICY)                   \
        _REST_API_(CLI_DELETE_IPPOOL_POLICY)                   \
        _REST_API_(CLI_CREATE_CORP_POLICY)                     \
        _REST_API_(CLI_HEALTH_CHECK)                           \
        _REST_API_(VERIFY_PASSWORD)                            \
        _REST_API_(GET_MDN)                                    \
        _REST_API_(GET_USERID)                                 \
        _REST_API_(CLI_REQUEST_2FA)                            \
        _REST_API_(RESPONSE_2FA)                               \
        _REST_API_(MODIFY_IPPOOL_POLICY)                       \
        _REST_API_(CLI_ERS_HEALTH_CHECK)                       \
        _REST_API_(MODIFY_USERINFO)                       \

#define GENERATE_REST_API_ENUM(_REST_API_ENUM_) _REST_API_ENUM_,
#define GENERATE_REST_API_STRING(_REST_API_STRING_) #_REST_API_STRING_,

typedef enum {
    FOREACH_REST_API(GENERATE_REST_API_ENUM)
    REST_API_MAX
} RestApi;

#define DEF_MTYPE_CLI_REQ	3
#define DEF_MTYPE_CLI_RES	4
#define DEF_MTYPE_SVR_REQ	5
#define DEF_MTYPE_SVR_RES	6
#define DEF_MTYPE_CLI_INIT  7
#define DEF_MTYPE_RELOAD_CFG 8
typedef struct _SocketHeader_ {
    int	bodyLen;
    int	mapType;
    int	mType;
} SocketHeader;

#endif

