//
// Created by shaked on 11/10/2025.
//

#include "Serializor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct packet serializeRequest(const struct dnsHeader header, const struct dnsQuery query) {
    size_t packetSize =
        sizeof(header.id) +
        sizeof(header.flags) +
        sizeof(header.QDcount) +
        sizeof(header.ANcount) +
        sizeof(header.NScount) +
        sizeof(header.ARcount) +
        query.Qname.qnameLength +
        sizeof(query.Qtype) +
        sizeof(query.Qclass);

    uint8_t* buffer = (uint8_t*)malloc(packetSize);
    uint8_t* start = buffer;


    memcpy(buffer, &header.id, sizeof(header.id));
    buffer += sizeof(header.id);

    memcpy(buffer, &header.flags, sizeof(header.flags));
    buffer += sizeof(header.flags);

    memcpy(buffer, &header.QDcount, sizeof(header.QDcount));
    buffer += sizeof(header.QDcount);

    memcpy(buffer, &header.ANcount, sizeof(header.ANcount));
    buffer += sizeof(header.ANcount);

    memcpy(buffer, &header.NScount, sizeof(header.NScount));
    buffer += sizeof(header.NScount);

    memcpy(buffer, &header.ARcount, sizeof(header.ARcount));
    buffer += sizeof(header.ARcount);

    memcpy(buffer, query.Qname.qname, query.Qname.qnameLength);
    buffer += query.Qname.qnameLength;

    memcpy(buffer, &query.Qtype, sizeof(query.Qtype));
    buffer += sizeof(query.Qtype);

    memcpy(buffer, &query.Qclass, sizeof(query.Qclass));
    buffer += sizeof(query.Qclass);

    struct packet p = {start, packetSize};
    return p;
}
