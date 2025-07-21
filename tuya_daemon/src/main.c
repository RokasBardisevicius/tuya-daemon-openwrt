#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <argp.h> 
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <tuya_error_code.h>
#include <tuyalink_core.h>
#include <uci.h>
#include "utils.h"
#include "ubus_ifainfo.h"
#include "ubus_meminfo.h"
#include "data.h"

int running = 1;

struct AppConfig {
    char productId[64];
    char deviceId[64];
    char deviceSecret[64];
    int daemon_on;
};

void sig_handler(int signum) {
    running = 0;
}

int parse_opt (int key, char *arg, struct argp_state *state){
    struct AppConfig *config = state->input;
        switch (key){
            case 'p':
                    strncpy(config->productId, arg, sizeof(config->productId)-1);
                break;
            case 'e':
                    strncpy(config->deviceId, arg, sizeof(config->deviceId)-1);
                break;
            case 's':
                    strncpy(config->deviceSecret, arg, sizeof(config->deviceSecret)-1);
                break;
            case 'd':
                    config->daemon_on = 1;
                break;
            default:
                return ARGP_ERR_UNKNOWN;
            }
            return 0;
} 

void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    if (chdir("/") < 0)
        exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    
    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started");
}

int main(int argc, char *argv[]) {

    int struct_size = 2;

    struct AppConfig config = {0};
    struct uci_context *uci_ctx = uci_alloc_context();

    signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
    signal(SIGQUIT, sig_handler);

    struct argp_option options[] = {
        { "product", 'p', "PRODUCT_ID", 0, "Tuya Product ID" },
        { "device", 'e', "DEVICE_ID", 0, "Tuya Device ID" },
        { "secret", 's', "Device_Secret", 0, "Tuya Device Secret Code" },
        { "daemon", 'd' ,0 , 0, "Create daemon"},
        { 0 }
    };
    struct argp argp = { options, parse_opt, 0, 0 };

    if (argp_parse(&argp, argc, argv, 0, 0, &config) != 0) {
        syslog(LOG_ERR, "Failed to parse arguments.\n");
        return 1;
    } else {
        syslog(LOG_INFO, "Arguments parsed correctly");
    }

    if (config.daemon_on){
        daemonize();
    }

    tuya_mqtt_context_t *client = create_mqtt(config.deviceId, config.deviceSecret);
    if (!client) {
        syslog(LOG_ERR, "MQTT initialization failed.\n");
        return 1;
    } else {
        syslog(LOG_INFO, "MQTT connected.\n");
    }

    struct Data data = {0};
    data.interfaces = malloc(struct_size * sizeof(struct InterfacesData));
    if (!data.interfaces) {
        syslog(LOG_ERR, "Failed to allocate memory for interfaces");
        return 1;
    }
    while (running) {
        tuya_mqtt_loop(client); 
        get_ubus_meminfo(&data);
        get_uci_ifa(uci_ctx, &data, struct_size);
        get_ubus_ifa(&data);

        char *json_str = create_json_payload(&data);
        if (json_str) {
            tuyalink_thing_property_report(client, NULL, json_str);
            free(json_str);
        }
        if(struct_size == data.count_ifa){
            struct_size++;
            struct Data *tmp = realloc(data.interfaces, sizeof(struct InterfacesData)* struct_size);
            if (tmp == NULL){
                syslog(LOG_ERR, "Realloc failed.");
                free(data.interfaces);
                break;
            } else {
                data.interfaces = tmp;
            }
        }
        usleep(100 * 1000);
    }
    free(data.interfaces);
    uci_free_context(uci_ctx);

    tuya_mqtt_disconnect(client);
    tuya_mqtt_deinit(client);
    return 0;
}

