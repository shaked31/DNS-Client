//
// Created by shaked on 11/10/2025.
//

#ifndef DNS_CLIENT_SERIALIZE_H
#define DNS_CLIENT_SERIALIZE_H

#include <stdint.h>
#include "../Network/dns.h"
#include <stdint.h>


#pragma pack(push, 1)
struct hexPacket {
    char* packetData;
    size_t packetSize;
};
#pragma pack(pop)


struct hexPacket serializeRequest(const struct dnsHeader, const struct dnsQuery);
struct packet deserializeResponse(char* hexResponse);

#endif //DNS_CLIENT_SERIALIZE_H