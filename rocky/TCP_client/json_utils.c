#include "snd_client.h"

int is_valid_json(const char *json_string) {
    struct json_object *parsed_json = json_tokener_parse(json_string);

    if (parsed_json == NULL) {
        return 0;
    }

    json_object_put(parsed_json);
    return 1;
}

json_object *make_object(Snd_dev_policy_list item, int mType) {
    json_object *obj_item = json_object_new_object();
    char *item_str_arr[] = {FOREACH_ELEMENT(GENERATE_ELEMENT_STRING)};

    if (item.device_id)
        json_object_object_add(obj_item, item_str_arr[DEVICE_ID], json_object_new_string(item.device_id));
    if (item.device_type)
        json_object_object_add(obj_item, item_str_arr[DEVICE_TYPE], json_object_new_string(item.device_type));

    if (mType == DELETE_SND_DEV_POLICY_ENUM) 
        return obj_item;

    json_object_object_add(obj_item, item_str_arr[AUTH_TYPE], json_object_new_int(item.auth_type));
    json_object_object_add(obj_item, item_str_arr[TWO_FA_USE], json_object_new_int(item.two_fa_use));
    json_object_object_add(obj_item, item_str_arr[DEVICE_SUSPEND], json_object_new_int(item.device_suspend));
    json_object_object_add(obj_item, item_str_arr[ID_TYPE], json_object_new_int(item.id_type));
    json_object_object_add(obj_item, item_str_arr[IP_POOL_INDEX], json_object_new_int(item.ip_pool_index));

    if (item.mdn) json_object_object_add(obj_item, item_str_arr[MDN], json_object_new_string(item.mdn));
    if (item.ip) json_object_object_add(obj_item, item_str_arr[IP], json_object_new_string(item.ip));
    if (item.user_id) json_object_object_add(obj_item, item_str_arr[USER_ID], json_object_new_string(item.user_id));
    if (item.device_name)
        json_object_object_add(obj_item, item_str_arr[DEVICE_NAME], json_object_new_string(item.device_name));
    if (item.serial_number)
        json_object_object_add(obj_item, item_str_arr[SERIAL_NUMBER], json_object_new_string(item.serial_number));

    return obj_item;
}

void ari_json_object_print(json_object *json) {
    const char *print_string = json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);

    ari_putendl_fd(print_string, 1);
}

/*
*@param sdp : snd_dev_policy 구조체
*/
int make_json_body(char *msg, Snd_dev_policy *sdp, int mType) {
    json_object *json_root = json_object_new_object();
    int json_len;
    char *target_buf;

    json_object_object_add(json_root, "LTEID", json_object_new_string(sdp->lte_id));
    json_object_object_add(json_root, "SLICE_ID", json_object_new_string(sdp->slice_id));

    json_object *json_array = json_object_new_array();

    for (int idx = 0; idx < sdp->max_list_idx; idx++) {
        json_object_array_add(json_array, make_object(sdp->snd_dev_policy[idx], mType));
    }

    json_object_object_add(json_root, "list", json_array);

    ari_title_print_fd(STDOUT_FILENO, "client json object", COLOR_GREEN_CODE);
    ari_json_object_print(json_root);

    bzero(msg, BUFSIZ);

    target_buf = msg + sizeof(SocketHeader) + sizeof(RestLibHeadType);
    json_len = sprintf(target_buf, "%s", json_object_to_json_string(json_root));
    json_object_put(json_root);

    return json_len;
}
