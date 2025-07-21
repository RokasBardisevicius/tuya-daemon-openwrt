#ifndef UBUS_IFAINFO_H
#define UBUS_IFAINFO_H
#include "data.h"

int get_uci_ifa(struct uci_context *ctx, struct Data *data, int capacity);
int get_ubus_ifa(struct Data *data);

#endif