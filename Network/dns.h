//
// Created by Shaked Pollak on 11/7/2025.
//

#include <stdint.h>

#ifndef DNS_H
#define DNS_H

struct dnsHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t QDcount;
    uint16_t ANcount;
    uint16_t NScount;
    uint16_t ARcount;
};

int buildPayload();
int buildHeader();

#endif //DNS_H
