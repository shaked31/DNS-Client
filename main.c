#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Network/dns.h"
#include "Utils/Serializor.h"
#include "Network/NetworkManager.h"

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 16


int main(void) {
    networkInit();
    // printf("Hello!\nEnter File Location\n");
    // char filename[256];
    // fgets(filename, sizeof(filename), stdin);
    // filename[strcspn(filename, "\n")] = '\0';
    // const char* filename = "C:\\Users\\Shaked Pollak\\OneDrive\\Desktop\\DNS-Client\\dns.txt";
    const char* filename = "C:\\Users\\Shaked\\OneDrive\\Desktop\\DNS-Client\\dns.txt";
    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("Sorry, couldn't open file location\n");
        exit(EXIT_FAILURE);
    }
    char buffer[MAX_LINE_LENGTH];
    struct dnsHeader header;
    struct dnsQuery query;

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) { // reads each line
        printf("%s", buffer);
        char* newlinePosition = strchr(buffer, '\n');
        if (newlinePosition != NULL) {
            *newlinePosition = 0x00;
        }

        header = buildHeader(); // in while loop to regenerate random ID
        query = buildQuery(buffer);
        const struct packet packet = serializeRequest(header, query);
        struct packet response = handlePacket(packet);

        struct response resPack = deserializeResponse(response);
        printf("%02X", resPack.answer.RData);
        free(packet.packetData);
        free((void*)query.Qname.qname);
    }

    fclose(fptr);
    networkFin();
    return EXIT_SUCCESS;
}