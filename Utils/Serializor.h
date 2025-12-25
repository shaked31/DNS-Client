//
// Created by shaked on 11/10/2025.
//

#ifndef DNS_CLIENT_SERIALIZE_H
#define DNS_CLIENT_SERIALIZE_H

#include "../Network/dns.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma pack(push, 1)
struct packet {
    char* packetData;
    size_t packetSize;
};
#pragma pack(pop)


struct packet serializeRequest(const struct dnsHeader, const struct dnsQuery);
struct response deserializeResponse(struct packet hexResponse);

#endif //DNS_CLIENT_SERIALIZE_H