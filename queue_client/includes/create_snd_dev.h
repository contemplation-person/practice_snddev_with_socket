#ifndef CREATE_SND_DEV_H
#define CREATE_SND_DEV_H

#define MAX_DEVICE_ID 18
#define MAX_MDN 12
#define MAX_IP 40
#define MAX_USER_ID 23
#define MAX_DEVICE_TYPE 11
#define MAX_DEVICE_NAME 31
#define MAX_SERIAL_NUMBER 31
#define MAX_LTE_ID 20
#define MAX_SLICE_ID 4

/*
struct snd {

};
struct snd snd[1];
*/
typedef struct 
{
    char lte_id[MAX_LTE_ID];
    char slice_id[MAX_SLICE_ID];

    int auth_type;
    int two_fa_use;
    int device_suspend;
    int id_type; 
    int ip_pool_index;

    char device_id[MAX_DEVICE_ID];
    char mdn[MAX_MDN];
    char ip[MAX_IP];
    char user_id[MAX_USER_ID];
    char device_type[MAX_DEVICE_TYPE];

    char device_name[MAX_DEVICE_NAME];
    char serial_number[MAX_SERIAL_NUMBER];
    } Create_snd_dev_policy;

#define FORECH_ELEMENT(GENERATE_ELEMENT) \
    GENERATE_ELEMENT(LTE_ID) \
    GENERATE_ELEMENT(SLICE_ID) \
    GENERATE_ELEMENT(AUTH_TYPE) \
    GENERATE_ELEMENT(TWO_FA_USE) \
    GENERATE_ELEMENT(DEVICE_SUSPEND) \
    GENERATE_ELEMENT(ID_TYPE) \
    GENERATE_ELEMENT(IP_POOL_INDEX) \
    GENERATE_ELEMENT(DEVICE_ID) \
    GENERATE_ELEMENT(MDN) \
    GENERATE_ELEMENT(IP) \
    GENERATE_ELEMENT(USER_ID) \
    GENERATE_ELEMENT(DEVICE_TYPE) \
    GENERATE_ELEMENT(DEVICE_NAME) \
    GENERATE_ELEMENT(SERIAL_NUMBER)

#define GENERATE_ELEMENT_STRING(ELEMENT) #ELEMENT,
#define GENERATE_ELEMENT_ENUM(ELEMENT) ELEMENT,

typedef enum
{
    FORECH_ELEMENT(GENERATE_ELEMENT_ENUM)
    MAX_CREATE_SND_DEV_POLICY,
} Create_snd_dev_policy_enum;

typedef struct 
{
    long msg_type;
    Create_snd_dev_policy create_snd_dev_policy;
}Msg_queue;

typedef enum
{
    CREATE_SND_DEV_POLICY_TYPE_ENUM = 1,
} Emg_msg_type;

#endif