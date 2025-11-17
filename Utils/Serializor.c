//
// Created by shaked on 11/10/2025.
//

#include "Serializor.h"

#include <stdlib.h>
#include <string.h>

uint8_t* serializeRequest(struct dnsHeader header, struct dnsQuery query) {
    uint8_t* buffer = (uint8_t*)malloc(sizeof(struct dnsHeader));
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

    memcpy(buffer, &query.Qname, sizeof(query.Qname));
    buffer += sizeof(query.Qname);

    memcpy(buffer, &query.Qtype, sizeof(query.Qtype));
    buffer += sizeof(query.Qtype);

    memcpy(buffer, &query.Qclass, sizeof(query.Qclass));
    buffer += sizeof(query.Qclass);

    return buffer;
}
