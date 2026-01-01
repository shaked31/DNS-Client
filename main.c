#include <inttypes.h>

#include "include/dns.h"
#include "Utils/Serializor.h"
#include "Network/NetworkManager.h"
#include "include/packet.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>


#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 64

#define A_TYPE 1
#define AAAA_TYPE 28
#define IPv4_BYTES_SIZE 4
#define IPv6_BYTES_SIZE 16

#define MAX_THREADS 256


struct threadParams {
    char* line;
    int protocol;
};

HANDLE printMutex;

unsigned __stdcall threadFunc(void* args) {

    struct threadParams* params = (struct threadParams*)args;
    uint32_t threadID = GetCurrentThreadId();
    struct dnsHeader header = {0};
    struct dnsQuery query = {0};

    header = buildHeader();
    printf("[INFO] -- Working on thread ID %"PRIu32" for %s\n", threadID, params->line);
    query = buildQuery(params->line, params->protocol);
    const struct packet packet = serializeRequest(header, query);

    struct packet response = handlePacket(packet, threadID);

    struct response resPack = deserializeResponse(response);
    WaitForSingleObject(printMutex, INFINITE); // Synchronize printing

    if (ntohs(header.id) != resPack.header.id) {
        printf("[ERROR] -- Wrong packet received\n");
        _endthreadex(EXIT_FAILURE);
    }

    if (params->protocol == IPv4OPTION && resPack.header.ANcount > 0)
        printf("[SUCCESS] -- A records of %s:\n", params->line);
    else if (params->protocol == IPv6OPTION && resPack.header.ANcount > 0)
        printf("[SUCCESS] -- AAAA records of %s:\n", params->line);
    else if (params->protocol == IPv4OPTION && resPack.header.ANcount == 0)
        printf("[ERROR] -- There is No A records of %s\n", params->line);
    else if (params->protocol == IPv6OPTION && resPack.header.ANcount == 0)
        printf("[ERROR] -- There is No AAAA records of %s\n", params->line);

    for (size_t i = 0 ; i < resPack.header.ANcount ; i++) {
        if (resPack.answers[i].type == A_TYPE && resPack.answers[i].RDLength == IPv4_BYTES_SIZE) {
            uint8_t* ip = (uint8_t*)resPack.answers[i].RData;
            printf("%u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
        }
        if (resPack.answers[i].type == AAAA_TYPE && resPack.answers[i].RDLength == IPv6_BYTES_SIZE) {
            uint8_t* ip = (uint8_t*)resPack.answers[i].RData;
            for (size_t j = 0 ; j < IPv6_BYTES_SIZE ; j += 2) {
                printf("%02X%02X", ip[j], ip[j+1]);
                if (j < IPv6_BYTES_SIZE - 2) {
                    printf(":");
                }
            }
            printf("\n");
        }
    }
    ReleaseMutex(printMutex);

    free(params->line);
    free(params);
    free(packet.packetData);
    free((void*)query.Qname.qname);
    freeResponse(&resPack);
    _endthreadex(EXIT_SUCCESS); // ends the thread (optional because it happens automatically when the function returns)
}

int main() {
    networkInit();

    printf("Hello!\nEnter TXT File Location\n");
    char filename[256];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    printf("Enter the number of internet protocol that you want to resolve: \n(1) IPv4\n(2) IPv6\n");
    char protocol[2];
    fgets(protocol, sizeof(protocol), stdin);
    int protocolChoise = atoi(protocol);
    if (protocolChoise != IPv4OPTION && protocolChoise != IPv6OPTION) {
        printf("[ERROR] -- Must choose a valid option!");
        exit(EXIT_FAILURE);
    }

    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("[ERROR] -- Sorry, couldn't open file\n");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    printMutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE hThreads[MAX_THREADS];
    int countThreads = 0;
    while (fgets(line, sizeof(line), fptr) != NULL) { // reads each line
        line[strcspn(line, "\n")] = '\0';

        unsigned int threadID;
        struct threadParams* params = malloc(sizeof(struct threadParams));
        params->line = malloc(strlen(line) + 1);
        strcpy(params->line, line);
        params->protocol = protocolChoise;

        hThreads[countThreads] = (HANDLE)_beginthreadex(NULL, 0, threadFunc, (void*)params, 0, &threadID);
        if (hThreads[countThreads] == 0) {
            printf("[ERROR] -- Couldn't create thread\n");
            free(params->line);
            free(params);
            exit(EXIT_FAILURE);
        }
        countThreads++;
    }

    WaitForMultipleObjects(countThreads,hThreads, TRUE, INFINITE);

    for (size_t i = 0 ; i < countThreads ; i++) {
        CloseHandle(hThreads[i]);
    }

    CloseHandle(printMutex);
    fclose(fptr);
    networkFin();
    return EXIT_SUCCESS;
}
