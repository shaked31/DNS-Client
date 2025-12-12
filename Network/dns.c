//
// Created by Shaked Pollak on 11/7/2025.
//

#include "dns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NetworkManager.h"
#include "../Utils/Serializor.h"


struct dnsHeader buildHeader() {
    /* I'm building a dns query packet
     * So the flags are:
     *    QR = 0
     *    OpCode = 0
     *    AA = 0
     *    TC = 0
     *    RD = 1
     *    RA = 0
     *    Z = 0
     *    RCODE = 0
     */
    const uint16_t randomID = 0x031; //(uint16_t)rand();

    const uint16_t flags = htons(0x0100);
    // flags |= (1 << 8); // sets the 8th bit to 1, the rest are 0
    const struct dnsHeader header = {.id = htons(randomID), .flags = flags, .QDcount = htons(1), .ANcount = htons(0), .NScount = htons(0), .ARcount = htons(0)};
    printf("ID is: %hu\n", header.id);
    printf("flags are: %u\n", header.flags);
    printf("QD count is: %u\n", header.QDcount);
    printf("AN count is: %u\n", header.ANcount);
    printf("NS count is: %u\n", header.NScount);
    printf("AR count is: %u\n", header.ARcount);
    return header;
}

struct Qname buildQname(const char* dnsName) {
    char* dnsNameCpy = malloc(strlen(dnsName)+1); // plus 1 for the null terminator
    strcpy_s(dnsNameCpy, strlen(dnsName)+1, dnsName);

    size_t totalSize = 1; // init with null terminator
    size_t offset = 0;
    char* qname = malloc(totalSize);
    qname[0] = 0x00;

    char* currentLabel = strtok(dnsNameCpy, ".");

    while (currentLabel != NULL) {
        printf("token is: %s\n", currentLabel);
        size_t currentLabelLen = strlen(currentLabel);
        totalSize += (currentLabelLen + 1);
        char* temp = realloc(qname, totalSize);

        if (temp == NULL) {
            printf("Couldn't reallocate memory, exiting...\n");
            free(qname);
            free(dnsNameCpy);
            exit(EXIT_FAILURE);
        }
        qname = temp;

        qname[offset] = (uint8_t)currentLabelLen;
        memcpy(qname + offset + 1, currentLabel, currentLabelLen);
        offset += (currentLabelLen + 1);

        currentLabel = strtok(NULL, "."); // strtok replaces the delim of the original string to the null terminator
    }
    qname[offset] = 0x00;
    totalSize = offset + 1;

    free(dnsNameCpy);

    printf("final dns name (query mode) is: \n");
    for (size_t i=0 ; i< totalSize ; i++) {
        printf("%02X", (uint8_t)qname[i]);
    }

    printf("\n");
    struct Qname q = {qname, totalSize};
    return q;
}

struct dnsQuery buildQuery(const char* dnsName) {
    /*
     *  Qname - Query Name
     *  Qtype - Type Of Query
     *  Qclass - Protocol Class of dns query
     *
     *  for domain name, Qtype = CNAME = 0x0005
     *  the standard Qclass for A (and AAAA) is the Internet Class - IN. Qclass = IN = 0x0001
     */
    const uint16_t CNAME = 0x05;
    const uint16_t INClass = 0x01;


    const struct Qname qname = buildQname(dnsName);
    struct dnsQuery query = {.Qname = qname, .Qtype = htons(CNAME), .Qclass = htons(INClass)};
    return query;
}