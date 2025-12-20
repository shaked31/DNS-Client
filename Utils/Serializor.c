//
// Created by shaked on 11/10/2025.
//

#include "Serializor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hexPacket serializeRequest(const struct dnsHeader header, const struct dnsQuery query) {
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

    char* buffer = malloc(packetSize);
    char* start = buffer;

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

    struct hexPacket p = {start, packetSize};
    return p;
}

struct packet deserializeResponse(char* hexResponse) {
    struct dnsHeader header = {0};
    struct dnsQuery query = {0};
    struct dnsAnswer answer = {0};

    memcpy(&header.id, hexResponse, sizeof(header.id));
    hexResponse += sizeof(header.id);

    memcpy(&header.flags, hexResponse, sizeof(header.flags));
    hexResponse += sizeof(header.flags);

    memcpy(&header.QDcount, hexResponse, sizeof(header.QDcount));
    hexResponse += sizeof(header.QDcount);

    memcpy(&header.ANcount, hexResponse, sizeof(header.ANcount));
    hexResponse += sizeof(header.ANcount);

    memcpy(&header.NScount, hexResponse, sizeof(header.NScount));
    hexResponse += sizeof(header.NScount);

    memcpy(&header.ARcount, hexResponse, sizeof(header.ARcount));
    hexResponse += sizeof(header.ARcount);

    query.Qname.qnameLength = strlen(query.Qname.qname)+1;

    memcpy(query.Qname.qname, hexResponse, query.Qname.qnameLength);
    hexResponse += query.Qname.qnameLength;

    memcpy(&query.Qtype, hexResponse, sizeof(query.Qtype));
    hexResponse += sizeof(query.Qtype);

    memcpy(&query.Qclass, hexResponse, sizeof(query.Qclass));
    hexResponse += sizeof(query.Qclass);


}