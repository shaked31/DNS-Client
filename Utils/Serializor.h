//
// Created by shaked on 11/10/2025.
//

#ifndef DNS_CLIENT_SERIALIZE_H
#define DNS_CLIENT_SERIALIZE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "../include/packet.h"
#include "../include/dns.h"

struct packet serializeRequest(const struct dnsHeader, const struct dnsQuery);
struct response deserializeResponse(struct packet packet);
void freeResponse(struct response *res);

#endif //DNS_CLIENT_SERIALIZE_H