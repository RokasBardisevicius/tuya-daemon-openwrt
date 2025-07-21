#include <syslog.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include "data.h"

enum {
	TOTAL_MEMORY,
	FREE_MEMORY,
	__MEMORY_MAX,
};

enum {
	MEMORY_DATA,
	__INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY]	  = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY]	  = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

static void ubus_mem_info_cb(struct ubus_request *req, int type, struct blob_attr *msg){
	struct Data *memoryData = (struct Data *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) {
		syslog(LOG_ERR, "No memory data received\n");
		return;
	}

	blobmsg_parse(memory_policy, __MEMORY_MAX, memory, blobmsg_data(tb[MEMORY_DATA]),
		      blobmsg_data_len(tb[MEMORY_DATA]));

	memoryData->total_ram    = blobmsg_get_u64(memory[TOTAL_MEMORY]);
	memoryData->free_ram     = blobmsg_get_u64(memory[FREE_MEMORY]);
}

int get_ubus_meminfo(struct Data *memoryData){
    struct ubus_context *ctx;
	uint32_t id;

    ctx = ubus_connect(NULL);
	if (!ctx) {
		syslog(LOG_ERR, "Failed to connect to ubus\n");
		return -1;
	}

    if (ubus_lookup_id(ctx, "system", &id) || ubus_invoke(ctx, id, "info", NULL, ubus_mem_info_cb, memoryData, 3000)) {
            syslog(LOG_ERR, "cannot request memory info from procd\n");
            return -1;
        }
    ubus_free(ctx);
    return 0;
}