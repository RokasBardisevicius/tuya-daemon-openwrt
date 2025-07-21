#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include "data.h"

enum {
    IPV4_ADDRESS,
    __INFO_MAX
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
    [IPV4_ADDRESS] = { .name = "ipv4-address", .type = BLOBMSG_TYPE_ARRAY },
};

enum {
    IFA_ADDR,
    IFA_MASK,
    __IP_ENTRY_MAX
};

static const struct blobmsg_policy ip_entry_policy[__IP_ENTRY_MAX] = {
    [IFA_ADDR] = { .name = "address", .type = BLOBMSG_TYPE_STRING },
    [IFA_MASK] = { .name = "mask", .type = BLOBMSG_TYPE_INT32 },
};

static void ubus_callback(struct ubus_request *req, int type, struct blob_attr *msg){
	struct InterfacesData *iface = req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__IP_ENTRY_MAX];
	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[IPV4_ADDRESS]) {
		syslog(LOG_INFO,  "No addr data received from: %s\n", iface->name);
		return;
	}
    struct blob_attr *cur;
    int rem;
    blobmsg_for_each_attr(cur, tb[IPV4_ADDRESS], rem){
        blobmsg_parse(ip_entry_policy, __IP_ENTRY_MAX, memory,blobmsg_data(cur),blobmsg_data_len(cur));
        strncpy(iface->ip, blobmsg_get_string(memory[IFA_ADDR]), sizeof(iface->ip) - 1);
        iface->mask = blobmsg_get_u32(memory[IFA_MASK]);
    }
}

int get_ubus_ifa(struct Data *data){
    struct ubus_context *ubus_ctx = ubus_connect(NULL);
    int count = 0;

    if (!ubus_ctx) {
        syslog(LOG_ERR, "Error connecting with ubus\n");
        return 1;
    }
    for (int i=0; i<data->count_ifa; i++){
        char ubus_name[64];
        const char *ifname = data->interfaces[i].name;
        snprintf(ubus_name, sizeof(ubus_name), "network.interface.%s", ifname);

        uint32_t id;
        if (ubus_lookup_id(ubus_ctx, ubus_name, &id) != 0) {
            syslog(LOG_ERR, "Failed to found %s\n", ubus_name);
            continue;
        }

        if (ubus_invoke(ubus_ctx, id, "status", NULL, ubus_callback, &data->interfaces[i], 3000) != 0) {
            syslog(LOG_ERR, "Ubus invoke failed\n");
        }
    }
    ubus_free(ubus_ctx);
    return 0;
    }

int get_uci_ifa(struct uci_context *ctx, struct Data *data, int capacity){
    struct uci_package *pkg = NULL;
    struct uci_element *e;
    int count = 0;

    if (uci_load(ctx, "network", &pkg) != UCI_OK) {
        syslog(LOG_ERR, "Failed to load uci 'network'\n");
        return 1;
    }

    uci_foreach_element(&pkg->sections, e) {
        if (count >= capacity)
            break;

        struct uci_section *s = uci_to_section(e);

        if (strcmp(s->type, "interface") == 0) {
            strncpy(data->interfaces[count].name, s->e.name, sizeof(data->interfaces[count].name) - 1);
            count++;
        }
    }

    data->count_ifa = count;
    uci_unload(ctx, pkg);
    return 0;
}
