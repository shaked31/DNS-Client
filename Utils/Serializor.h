//
// Created by shaked on 11/10/2025.
//

#ifndef DNS_CLIENT_SERIALIZE_H
#define DNS_CLIENT_SERIALIZE_H

#include <stdint.h>
#include "../Network/dns.h"

uint8_t* serializeRequest(struct dnsHeader, struct dnsQuery);

#endif //DNS_CLIENT_SERIALIZE_H