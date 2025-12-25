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

HANDLE printMutex;

unsigned __stdcall threadFunc(char* line) {
    struct dnsHeader header = {0};
    struct dnsQuery query = {0};

    // removes the '\n' from the string
    char* newlinePosition = strchr(line, '\n');
    if (newlinePosition != NULL) {
        *newlinePosition = 0x00;
    }

    WaitForSingleObject(printMutex, INFINITE); // Synchronize printing

    printf("[INFO] -- building a DNS query for %s\n", line);
    header = buildHeader();
    query = buildQuery(line);
    const struct packet packet = serializeRequest(header, query);
    struct packet response = handlePacket(packet);

    struct response resPack = deserializeResponse(response);
    if (resPack.answer.type == A_TYPE && resPack.answer.RDLength == IPv4_BYTES_SIZE) {
        uint8_t* ip = (uint8_t*)resPack.answer.RData;

        printf("[SUCCESS] -- A record of %s: %u.%u.%u.%u\n\n",line ,ip[0], ip[1], ip[2], ip[3]);
    }

    ReleaseMutex(printMutex);

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
    const char* filename = "C:\\Users\\Shaked Pollak\\OneDrive\\Desktop\\DNS-Client-Master\\dns.txt";
    // const char* filename = "C:\\Users\\Shaked\\OneDrive\\Desktop\\DNS-Client\\dns.txt";
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

        unsigned int threadID;

        char* lineCpy = malloc(strlen(line));
        strcpy(lineCpy, line);
        hThreads[countThreads] = (HANDLE)_beginthreadex(NULL, 0, threadFunc, lineCpy, 0, &threadID);
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
