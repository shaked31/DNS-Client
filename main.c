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
    const char* filename = "C:\\Users\\Shaked Pollak\\OneDrive\\Desktop\\DNS-Client\\dns.txt";
    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("Sorry, couldn't open file location\n");
        return EXIT_FAILURE;
    }
    char buffer[MAX_LINE_LENGTH];
    const struct dnsHeader header = buildHeader();
    struct dnsQuery query;

    char* dnsServerIP = findDnsServerIP();
    if (strlen(dnsServerIP) == 0) {
        perror("Couldn't find the DNS Server's IP");
        return EXIT_FAILURE;
    }

    printf("Domain Names excepted are:\n");

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) { // reads each line
        printf("%s", buffer);
        char* newlinePosition = strchr(buffer, '\n');
        if (newlinePosition != NULL) {
            *newlinePosition = 0x00;
        }
        query = buildQuery(buffer);

        const struct packet packet = serializeRequest(header, query);
        sendPacket(packet, dnsServerIP);
        free(packet.packetData);
        free((void*)query.Qname.qname);

        char* buffer = recvPacket();

    }

    fclose(fptr);
    networkFin();
    return EXIT_SUCCESS;
}