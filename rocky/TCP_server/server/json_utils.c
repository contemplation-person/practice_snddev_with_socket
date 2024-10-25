#include "snd.h"

int new_create_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
    if (!new_list) {
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        return 0;
    }

    new_list->auth_type = json_object_get_int(json_object_object_get(csp_list_obj, "AUTH_TYPE"));
    new_list->two_fa_use = json_object_get_int(json_object_object_get(csp_list_obj, "TWO_FA_USE"));
    new_list->device_suspend = json_object_get_int(json_object_object_get(csp_list_obj, "ID_TYPE"));
    new_list->ip_pool_index = json_object_get_int(json_object_object_get(csp_list_obj, "IP_POOL_INDEX"));
    strcpy(new_list->device_id, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_ID")));
    strcpy(new_list->mdn, json_object_get_string(json_object_object_get(csp_list_obj, "MDN")));
    strcpy(new_list->ip, json_object_get_string(json_object_object_get(csp_list_obj, "IP")));
    strcpy(new_list->user_id, json_object_get_string(json_object_object_get(csp_list_obj, "USER_ID")));
    strcpy(new_list->device_type, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_TYPE")));
    strcpy(new_list->device_name, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_NAME")));
    strcpy(new_list->serial_number, json_object_get_string(json_object_object_get(csp_list_obj, "SERIAL_NUMBER")));
    new_list->next = NULL;

    if (snddev_policy_header->create_snddev_policy_start_list == NULL) {
        snddev_policy_header->create_snddev_policy_start_list = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    } else {
        snddev_policy_header->create_snddev_policy_end_list->next = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    }

    return 1;
}

int new_modify_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
    if (!new_list) {
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        return 0;
    }

    new_list->auth_type = json_object_get_int(json_object_object_get(csp_list_obj, "AUTH_TYPE"));
    new_list->two_fa_use = json_object_get_int(json_object_object_get(csp_list_obj, "TWO_FA_USE"));
    new_list->device_suspend = json_object_get_int(json_object_object_get(csp_list_obj, "ID_TYPE"));
    new_list->ip_pool_index = json_object_get_int(json_object_object_get(csp_list_obj, "IP_POOL_INDEX"));
    strcpy(new_list->device_id, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_ID")));
    strcpy(new_list->mdn, json_object_get_string(json_object_object_get(csp_list_obj, "MDN")));
    strcpy(new_list->ip, json_object_get_string(json_object_object_get(csp_list_obj, "IP")));
    strcpy(new_list->user_id, json_object_get_string(json_object_object_get(csp_list_obj, "USER_ID")));
    strcpy(new_list->device_type, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_TYPE")));
    strcpy(new_list->device_name, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_NAME")));
    strcpy(new_list->serial_number, json_object_get_string(json_object_object_get(csp_list_obj, "SERIAL_NUMBER")));
    new_list->next = NULL;

    if (snddev_policy_header->create_snddev_policy_start_list == NULL) {
        snddev_policy_header->create_snddev_policy_start_list = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    } else {
        snddev_policy_header->create_snddev_policy_end_list->next = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    }

    return 1;
}

int new_delete_snddev_policy_list(Snddev_policy_header *snddev_policy_header, struct json_object *csp_list_obj) {
    Snddev_policy_list *new_list = (Snddev_policy_list *)malloc(sizeof(Snddev_policy_list));
    if (!new_list) {
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        return 0;
    }

    strcpy(new_list->device_id, json_object_get_string(json_object_object_get(csp_list_obj, "DEVICE_ID")));
    new_list->device_suspend = json_object_get_int(json_object_object_get(csp_list_obj, "ID_TYPE"));

    new_list->next = NULL;

    if (snddev_policy_header->create_snddev_policy_start_list == NULL) {
        snddev_policy_header->create_snddev_policy_start_list = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    } else {
        snddev_policy_header->create_snddev_policy_end_list->next = new_list;
        snddev_policy_header->create_snddev_policy_end_list = new_list;
    }

    return 1;
}

int parse_json_list(int mType, struct json_object *parsed_json, Snddev_policy_header *snddev_policy_header) {
    struct json_object *csp_list;
    struct json_object *csp_list_obj;
    int list_len;

    if (!json_object_object_get_ex(parsed_json, "list", &csp_list)) {
        printf("list not found\n");
        return 0;
    }

    list_len = json_object_array_length(csp_list);
    for (int idx = 0; idx < list_len; idx++) {
        csp_list_obj = json_object_array_get_idx(csp_list, idx);
        switch (mType)
        {
            case CREATE_SNDDEV_POLICY:
                if (!new_create_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_create_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
            case MODIFY_SNDDEV_POLICY:
                if (!new_modify_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_modify_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
            case DELETE_SNDDEV_POLICY:
                if (!new_delete_snddev_policy_list(snddev_policy_header, csp_list_obj)) {
                    fprintf(stderr, "new_delete_snddev_policy_list error: %s\n", strerror(errno));
                    return 0;
                }
                break;
        }
    }

    return 1;
}

void ari_json_object_print(json_object *json) {
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

void parse_json_header(struct json_object *parsed_json, Snddev_policy_header *snddev_policy) {
    struct json_object *lte_id;
    struct json_object *slice_id;

    if (json_object_object_get_ex(parsed_json, "LTEID", &lte_id)) {
        strcpy(snddev_policy->lte, json_object_get_string(lte_id));
    }

    if (json_object_object_get_ex(parsed_json, "SLICE_ID", &slice_id)) {
        strcpy(snddev_policy->slice_id, json_object_get_string(slice_id));
    }
}

int parse_rest_msg(RestMsgType *rest_msg, Snddev_policy_header *snddev_policy) {
    struct json_object *parsed_json;
    char* rest_str_arr[] = {FOREACH_REST_API(GENERATE_REST_API_STRING)};
    int mType = REST_API_UNKNOWN;

    while (mType < REST_API_MAX) {
        if (!strcmp(rest_msg->header.mtype, rest_str_arr[mType])) {
            break;
        }
        mType++;
    }

    parsed_json = json_tokener_parse(rest_msg->jsonBody);
    if (parsed_json == NULL) {
        return 0;
    }
    
    parse_json_header(parsed_json, snddev_policy);
        if (!parse_json_list(mType, parsed_json, snddev_policy)) {
            json_object_put(parsed_json);
            return 0;
        }

    ari_title_print("parsed_json", COLOR_GREEN_CODE);
    ari_json_object_print(parsed_json);

    json_object_put(parsed_json);
    return 1;
}