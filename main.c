#include "include/dns.h"
#include "Utils/Serializor.h"
#include "Network/NetworkManager.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>


#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 16

#define A_TYPE 1
#define IPv4_BYTES_SIZE 4
#define MAX_THREADS 256

#define IPv4 1
#define IPv6 2

struct threadParams {
    char* line;
    int protocol;
};

HANDLE printMutex;

unsigned __stdcall threadFunc(void* args) {

    struct threadParams* params = (struct threadParams*)args;

    struct dnsHeader header = {0};
    struct dnsQuery query = {0};




    // printf("[INFO] -- building a DNS query for %s\n", line);
    header = buildHeader();
    query = buildQuery(params->line);
    const struct packet packet = serializeRequest(header, query);
    struct packet response = handlePacket(packet);

    struct response resPack = deserializeResponse(response);
    WaitForSingleObject(printMutex, INFINITE); // Synchronize printing

    printf("[SUCCESS] -- A records of %s:\n", params->line);
    for (size_t i = 0 ; i < resPack.header.ANcount ; i++) {
        if (resPack.answers[i].type == A_TYPE && resPack.answers[i].RDLength == IPv4_BYTES_SIZE) {
            uint8_t* ip = (uint8_t*)resPack.answers[i].RData;
            printf("%u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
        }
    }
    ReleaseMutex(printMutex);



    free(params->line);
    free(packet.packetData);
    free((void*)query.Qname.qname);
    freeResponse(&resPack);

    _endthreadex(0); // ends the thread (optional because it happens automatically when the function returns)
    return 0;
}

int main() {
    networkInit();
    // printf("Hello!\nEnter File Location\n");
    // char filename[256];
    // fgets(filename, sizeof(filename), stdin);
    // filename[strcspn(filename, "\n")] = '\0';

    printf("Enter the number of internet protocol that you want to resolve: \n(1) IPv4\n(2) IPv6\n");
    char protocol[2];
    fgets(protocol, sizeof(protocol), stdin);
    int protocolChoise = atoi(protocol);

    // const char* filename = "C:\\Users\\Shaked Pollak\\OneDrive\\Desktop\\DNS-Client-Master\\dns.txt";
    const char* filename = "C:\\Users\\Shaked\\OneDrive\\Desktop\\DNS-Client\\dns.txt";
    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("[ERROR] -- Sorry, couldn't open file location\n");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    printMutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE hThreads[MAX_THREADS];
    int countThreads = 0;
    while (fgets(line, sizeof(line), fptr) != NULL) { // reads each line

        // removes the '\n' from the string
        line[strcspn(line, "\n")] = '\0';
        // char* newlinePosition = strchr(line, '\n');
        // if (newlinePosition != NULL) {
        //     *newlinePosition = 0x00;
        // }

        unsigned int threadID;

        struct threadParams* params = malloc(sizeof(struct threadParams));
        params->line = malloc(strlen(line) + 1);
        strcpy(params->line, line);
        params->protocol = protocolChoise;

        hThreads[countThreads] = (HANDLE)_beginthreadex(NULL, 0, threadFunc, (void*)params, 0, &threadID);
        if (hThreads == NULL) {
            perror("[ERROR] -- Couldn't create thread\n");
            exit(EXIT_FAILURE);
        }
        else {
            countThreads++;
        }
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
