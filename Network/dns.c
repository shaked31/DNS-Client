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
    const uint16_t randomID = rand() & 0xFFFF;

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
     *
     */
    // int labelCounter = 0;
    // uint8_t labelCharCounter = 0;
    // // uint8_t* labelLengthCounter = (uint8_t*)malloc(sizeof(uint8_t));
    // // labelLengthCounter[0] = 0;
    //
    // char* ptrToDnsName = malloc(sizeof(char) * strlen(dnsName));
    // char* dnsNameCpy = malloc(sizeof(char) * strlen(dnsName));
    // strcpy(ptrToDnsName, dnsName);
    // strcpy(dnsNameCpy, dnsName);
    //
    // uint8_t* newDnsQueryName = malloc(sizeof(char) * strlen(dnsName));
    // uint8_t* newDnsQueryNamePtr = newDnsQueryName; // saves ptr to start of byte array
    // // char* ptrToLastLabel = malloc(sizeof(char) * strlen(dnsName));
    //
    // char* currentLabel;// = malloc(sizeof(char) * strlen(dnsName));;
    // // strcpy(currentLabel, dnsName);
    //
    // currentLabel = strtok(dnsNameCpy, ".");
    // while (*ptrToDnsName != 0x00) {
    //     if (*ptrToDnsName == '.') {
    //         *newDnsQueryName = labelCharCounter;
    //         newDnsQueryName = realloc(newDnsQueryName, labelCharCounter);
    //
    //         if (currentLabel != NULL) {
    //             printf("token is: %s\n", currentLabel);
    //             currentLabel = strtok(NULL, "."); // strtok replaces the delim of the original string to the null terminator
    //         }
    //
    //         *newDnsQueryName = (uint8_t)*currentLabel;
    //         labelCharCounter = 0;
    //     }
    //     else {
    //         labelCharCounter++;
    //     }
    //     ptrToDnsName++;
    // }
    //
    // // labelLengthCounter[labelCounter] = labelCharCounter; // add last label count to length array
    // labelCounter++;
    // // labelLengthCounter = realloc(labelLengthCounter, sizeof(labelLengthCounter) - sizeof(uint8_t));
    //
    //
    // // size_t QnameLength = sizeof(uint8_t)*sizeof(labelLengthCounter) + labelCounter;
    // // uint8_t Qname[QnameLength];
    // // = (uint8_t*)malloc(sizeof(uint8_t)*sizeof(labelLengthCounter) + labelCounter); // includes the null operator bit
    // // int bitCounter = 0;
    // // printf("Label Count %d:\n", labelCounter);
    // // for (size_t i=0 ; i < labelCounter ; i++) {
    // //     printf("Length of label %zu is: %d\n", i, labelLengthCounter[i]);
    // //     for (size_t j=0 ; j < labelLengthCounter[i] ; j++, bitCounter++) {
    // //         if (j == 0) {
    // //             Qname[bitCounter] = labelLengthCounter[i];
    // //         }
    // //         if (bitCounter == QnameLength) {
    // //             Qname[bitCounter] = 0x00;
    // //         }
    //         // else {
    //         //     Qname[bitCounter] =
    //         // }
    // //     }
    // // }
    //
    // for (size_t i=0 ; i<=strlen(newDnsQueryName) ; i++) {
    //     printf("%u/n", newDnsQueryName[i]);
    // }
    // //
    // // struct dnsQuery query = {.Qname = dnsName, .Qtype = CNAME, .Qclass = INClass};

    char* str = malloc(sizeof(uint8_t));
    str[0] = 0x00;
    char* temp;
    char* currentLabel;
    size_t strSize;
    currentLabel = strtok(dnsName, ".");

    while (currentLabel != NULL) {
        printf("token is: %s\n", currentLabel);
        strSize = strlen(str) + strlen(currentLabel) + 1;
        temp = realloc(str, strSize); // including the 1 in line 40 for the null terminator
        if (temp == NULL) {
            printf("Couldn't reallocate memory, exiting...\n");
            exit(EXIT_FAILURE);
        }

        str = temp;

        printf("current dns query name is %s\n", str);
        char strCurrentLabelLength[16];
        sprintf(strCurrentLabelLength, "%llu", strlen(currentLabel));

        // strcat adds null terminator to str
        strcat(str, strCurrentLabelLength);
        strcat(str, currentLabel);

        currentLabel = strtok(NULL, "."); // strtok replaces the delim of the original string to the null terminator
    }

    printf("final dns name (query mode) is: %s\n", str);

    struct dnsQuery query = {.Qname = str, .Qtype = CNAME, .Qclass = INClass};
    return query;
}