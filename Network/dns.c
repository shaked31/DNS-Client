//
// Created by Shaked Pollak on 11/7/2025.
//

#include "dns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    uint16_t randomID = rand() & 0xFFFF;

    uint16_t flags = 0;
    flags |= (1 << 8); // sets the 8th bit to 1, the rest are 0
    struct dnsHeader header = {.id = randomID, .flags = flags, .QDcount = 1, .ANcount = 0, .NScount = 0, .ARcount = 0};

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
     *
     */
    int labelCounter = 0;
    int labelCharCounter = 0;
    uint8_t* labelLengthCounter = (uint8_t*)malloc(sizeof(uint8_t));
    labelLengthCounter[0] = 0;

    char* ptrToDnsName = malloc(sizeof(char) * strlen(dnsName));
    strcpy(ptrToDnsName, dnsName);

    while (*ptrToDnsName != 0x00) {
        if (*ptrToDnsName == '.') {
            labelLengthCounter[labelCounter] = labelCharCounter;
            labelCounter++;
            if (*(ptrToDnsName+1) != 0x00) {
                // means there is another label
                labelLengthCounter = realloc(labelLengthCounter, sizeof(uint8_t)*(labelCounter + 1));
                labelLengthCounter[labelCounter] = 0;
            }
            labelCharCounter = 0; // resets to 0
        }
        else {
            labelCharCounter++;
        }
        ptrToDnsName++;
    }

    labelLengthCounter[labelCounter] = labelCharCounter; // add last label count to length array
    labelCounter++;
    labelLengthCounter = realloc(labelLengthCounter, sizeof(labelLengthCounter) - sizeof(uint8_t));


    size_t QnameLength = sizeof(uint8_t)*sizeof(labelLengthCounter) + labelCounter;
    uint8_t Qname[QnameLength];
    // = (uint8_t*)malloc(sizeof(uint8_t)*sizeof(labelLengthCounter) + labelCounter); // includes the null operator bit
    int bitCounter = 0;
    printf("Label Count %d:\n", labelCounter);
    for (size_t i=0 ; i < labelCounter ; i++) {
        printf("Length of label %zu is: %d\n", i, labelLengthCounter[i]);
        for (size_t j=0 ; j < labelLengthCounter[i] ; j++, bitCounter++) {
            if (j == 0) {
                Qname[bitCounter] = labelLengthCounter[i];
            }
            if (bitCounter == QnameLength) {
                Qname[bitCounter] = 0x00;
            }
            // else {
            //     Qname[bitCounter] =
            // }
        }
    }

    for (size_t i=0 ; i<=bitCounter ; i++) {
        printf("%u", Qname[i]);
    }

    struct dnsQuery query = {.Qname = dnsName, .Qtype = CNAME, .Qclass = INClass};
    return query;
}