#ifndef UTILS_H
#define UTILS_H

#include <tuyalink_core.h>
#include "data.h"

char *create_json_payload(struct Data *data);
void on_disconnect(tuya_mqtt_context_t* context, void* user_data);
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);
tuya_mqtt_context_t* create_mqtt(char* dev_id, char* dev_secret);
char *replace_quotes(char *str);

#endif