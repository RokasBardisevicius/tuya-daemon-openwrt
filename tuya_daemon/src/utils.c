#include <cjson/cJSON.h>
#include <stdlib.h>
#include "tuya_cacert.h"
#include "utils.h"
#include <tuya_log.h>
#include <tuya_error_code.h>
#include <tuyalink_core.h>
#include <syslog.h>
#include "ubus_meminfo.h"
#include "data.h"


tuya_mqtt_context_t client_instance;

char *create_json_payload(struct Data *data){
    cJSON *final_json = cJSON_CreateObject();
    cJSON *interfaces_array = cJSON_CreateArray();
    for (int i = 0; i < data->count_ifa; i++) {
        cJSON *iface_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(iface_obj, "interface", data->interfaces[i].name);
        cJSON_AddStringToObject(iface_obj, "ip", data->interfaces[i].ip);
        cJSON_AddNumberToObject(iface_obj, "mask", data->interfaces[i].mask);


        char *iface_str = cJSON_PrintUnformatted(iface_obj);
        replace_quotes(iface_str);
        cJSON *iface_str_obj = cJSON_CreateString(iface_str);
        cJSON_AddItemToArray(interfaces_array, iface_str_obj);
        cJSON_Delete(iface_obj);
        free(iface_str);
    }

    cJSON_AddItemToObject(final_json, "interfaces", interfaces_array);
    cJSON_AddNumberToObject(final_json, "ram_total", data->total_ram);
    cJSON_AddNumberToObject(final_json, "ram_free", data->free_ram);

    char *json_str = cJSON_PrintUnformatted(final_json);
    cJSON_Delete(final_json);
    return json_str;
}

void on_connected(tuya_mqtt_context_t* context, void* user_data){
    TY_LOGI("on connected");
    const char *dev_id = (const char *)user_data;
    tuyalink_thing_data_model_get(&client_instance, dev_id);
}

void on_disconnect(tuya_mqtt_context_t* context, void* user_data){
    syslog(LOG_WARNING, "MQTT disconnected");
}

void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg){
   
    FILE *log_file = fopen("/tmp/tuya_action.log", "a");
    if (log_file == NULL) {
        printf("Error: Unable to open the file.\n");
        return;
    }
    
    fprintf(log_file, "Model data:%s\n", msg->data_string);
    fclose(log_file);

    TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    switch (msg->type) {
        case THING_TYPE_ACTION_EXECUTE:
            TY_LOGI("Action execute: %s", msg->data_string);
            tuyalink_thing_property_report(context, (const char *)user_data, "{\"status\":true}");
            break;

        case THING_TYPE_MODEL_RSP:
            TY_LOGI("Model data:%s", msg->data_string);
            break;

        case THING_TYPE_PROPERTY_SET:
            TY_LOGI("property set:%s", msg->data_string);
            tuyalink_thing_property_report(context, (const char *)user_data, "{\"status\":true}");
            break;

        case THING_TYPE_PROPERTY_REPORT_RSP:
            break;

        default:
            break;
    }
    printf("\r\n");
}

tuya_mqtt_context_t* create_mqtt(char* dev_id, char* dev_secret){
    int ret = OPRT_OK;
    tuya_mqtt_context_t* client = &client_instance;

    ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = (const uint8_t *)tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = dev_id,
        .device_secret = dev_secret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });

    if (ret != OPRT_OK) {
        printf("MQTT init failed: %d\n", ret);
        return NULL;
    }

    ret = tuya_mqtt_connect(client);
    if (ret != OPRT_OK) {
        printf("MQTT connect failed: %d\n", ret);
        return NULL;
    }
    return client;
}

char *replace_quotes(char *str) {
    for (char *p = str; *p; ++p) {
        if (*p == '"') {
            *p = '\'';
        }
    }
    return str;
}
