//
// Created by shaked on 11/10/2025.
//

#ifndef DNS_CLIENT_SERIALIZE_H
#define DNS_CLIENT_SERIALIZE_H

#include <stdint.h>
#include "../Network/dns.h"

#pragma pack(push, 1)
struct packet {
    uint8_t* packetData;
    size_t packetSize;
};
#pragma pack(pop)

struct packet serializeRequest(const struct dnsHeader, const struct dnsQuery);

#endif //DNS_CLIENT_SERIALIZE_H