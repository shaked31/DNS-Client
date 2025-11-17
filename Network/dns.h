//
// Created by Shaked Pollak on 11/7/2025.
//

#include <stdint.h>

#ifndef DNS_H
#define DNS_H

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 16

struct flags {
    unsigned int QR : 1; // Query/Response flags. 0 for query and 1 for response.
    unsigned int OpCode : 4; // 0 for standard query
    unsigned int AA : 1; // Authoritative Answer (set in the response by the server)
    unsigned int TC : 1; // Truncation (set in the response by the server)
    unsigned int RD : 1; // Recursion Desired (set in the query by the client) - tells dns server to continue to the next one if it doesn't have the answer
    unsigned int RA : 1; // Recursion Available (set in the response by the server)
    unsigned int Z : 3; // Reserved, set to 0 (set in the request by the client)
    unsigned int RCODE : 4; // Response code (set in the response by the server)
};

struct dnsHeader {
    uint16_t id;
    // struct flags flags;
    uint16_t flags;
    uint16_t QDcount; // number of entries in the question section
    uint16_t ANcount; // number of resource records (set in the response by the server)
    uint16_t NScount; // number of authority resource records in the authority section of the response (set in the response by the server)
    uint16_t ARcount; // number of resource records in the additional records section (set in the response by the server)
};

struct dnsQuery {
    char* Qname;
    uint16_t Qtype;
    uint16_t Qclass;
};

struct dnsHeader buildHeader();
struct dnsQuery buildQuery(char* dnsName);

#endif //DNS_H
