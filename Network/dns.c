//
// Created by Shaked Pollak on 11/7/2025.
//

#include "dns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NetworkManager.h"
#include "../Utils/Serializor.h"

#define CNAME 0x05
#define INClass 0x01

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
    const uint16_t randomID = (uint16_t)rand();

    const uint16_t flags = 0;
    // flags |= (1 << 8); // sets the 8th bit to 1, the rest are 0
    const struct dnsHeader header = {.id = randomID, .flags = flags, .QDcount = 1, .ANcount = 0, .NScount = 0, .ARcount = 0};
    printf("ID is: %hu\n", header.id);
    printf("flags are: %u\n", header.flags);
    printf("QD count is: %u\n", header.QDcount);
    printf("AN count is: %u\n", header.ANcount);
    printf("NS count is: %u\n", header.NScount);
    printf("AR count is: %u\n", header.ARcount);
    return header;
}


struct dnsQuery buildQuery(char* dnsName) {
    /*
     *  Qname - Query Name
     *  Qtype - Type Of Query
     *  Qclass - Protocol Class of dns query
     *
     *  for domain name, Qtype = CNAME = 0x0005
     *  the standard Qclass for A (and AAAA) is the Internet Class - IN. Qclass = IN = 0x0001
     */

    char* dnsNameCpy = NULL;
    strcpy(dnsNameCpy, dnsName);

    char* str = malloc(sizeof(uint8_t));
    str[0] = 0x00;

    char* temp = NULL;
    char* currentLabel = strtok(dnsNameCpy, ".");

    size_t offset = 0;
    size_t strSize;

    while (currentLabel != NULL) {
        printf("token is: %s\n", currentLabel);
        size_t currentLabelLen = strlen(currentLabel);
        strSize = strlen(str) + currentLabelLen + 1;
        temp = realloc(str, strSize); // including the 1 in line 40 for the null terminator
        if (temp == NULL) {
            printf("Couldn't reallocate memory, exiting...\n");
            free(str);
            exit(EXIT_FAILURE);
        }

        str = temp;

        str[offset] = (uint8_t)currentLabelLen;
        memcpy(str + offset + 1, currentLabel, currentLabelLen);
        offset += currentLabelLen + 1;

        currentLabel = strtok(NULL, "."); // strtok replaces the delim of the original string to the null terminator
    }
    str[offset] = 0x00;

    printf("final dns name (query mode) is: \n");
    // free(temp);
    for (size_t i=0 ; i<= strlen(dnsNameCpy) ; i++) {
        printf("%X", (uint8_t)dnsNameCpy[i]);
    }

    struct dnsQuery query = {.Qname = str, .Qtype = CNAME, .Qclass = INClass};
    return query;
}