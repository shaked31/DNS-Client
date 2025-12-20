//
// Created by Shaked Pollak on 11/7/2025.
//

#include <stdint.h>

#ifndef DNS_H
#define DNS_H

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 16



#pragma pack(push, 1)
struct dnsHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t QDcount; // number of entries in the question section
    uint16_t ANcount; // number of resource records (set in the response by the server)
    uint16_t NScount; // number of authority resource records in the authority section of the response (set in the response by the server)
    uint16_t ARcount; // number of resource records in the additional records section (set in the response by the server)
};
#pragma pack(pop)


#pragma pack(push, 1)
struct Qname {
    const char* qname;
    size_t qnameLength;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct dnsQuery {
    struct Qname Qname;
    uint16_t Qtype;
    uint16_t Qclass;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct packet {
    struct dnsHeader header;
    struct dnsQuery query;
};
#pragma pack(pop)

struct dnsHeader buildHeader();
struct dnsQuery buildQuery(char* dnsName);
size_t getQnameLength(char* qname);

#endif //DNS_H
