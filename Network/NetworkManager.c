//
// Created by Shaked Pollak on 11/7/2025.
//

#include "NetworkManager.h"

#include <inttypes.h>
#include <stdint.h>
#include <time.h>

char* dnsServerIP = NULL;

// This function allocates memory to IPstr, and frees it in NetworkFin()
char* findDnsServerIP() {
    FILE* fptr;
    char result[MAX_CMD_OUTPUT_SIZE];
    char command[] = "ipconfig /all | find \"DNS Servers\"";
    fptr = _popen(command, "r");
    if (fgets(result, sizeof(result), fptr) != NULL) {
        char* ptrToIP = strchr(result, ':');
        ptrToIP += 2; // there is ' ' after ':'
        char* newlinePosition = strchr(ptrToIP, '\n');
        *newlinePosition = 0x00;
        if (ptrToIP != NULL) {
            int subStrLen = strlen(ptrToIP);
            char* IPstr = (char*)malloc(subStrLen + 1);
            strcpy(IPstr, ptrToIP);
            printf("[INFO] -- The local DNS server IP is: %s\n\n", IPstr);
            _pclose(fptr);
            return IPstr;
        }
    }
    _pclose(fptr);
    return "";
}

int networkInit() {
    struct WSAData wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        /*
         *  WSAStartup starts the winsock dll. without this, we can't use functions like socket()
         *  MAKEWORD(2,2) creates version number (2.2, the most common socket version)
         *  &wsaData is a pointer to a WSA struct. WSAStartup fills this struct
         */
        perror("[ERROR] -- Couldn't init winsock");
        return EXIT_FAILURE;
    }
    dnsServerIP = findDnsServerIP();
    return EXIT_SUCCESS;
}

void networkFin() {
    WSACleanup();
    free(dnsServerIP);
}

void checkPtonResult(int ptonResult) {
    /*  inet_pton converts printable to network order and copies ipDst to addr.sin_addr
     *  success return value = 1
     *  invalid IP address = 0
     *  address family is unsupported = -1
    */
    if (ptonResult == 0) {
        perror("[ERROR] -- Something is wrong with the ip address given!\n");
        exit(EXIT_FAILURE);
    }
    if (ptonResult == -1) {
        perror("[ERROR] -- Something is wrong with the address family\n");
        exit(EXIT_FAILURE);
    }
}

int sendPacket(const SOCKET udpSock, const struct packet packet, struct sockaddr_in addr) {
    if (send(udpSock, packet.packetData, packet.packetSize, 0) < 0) {
        perror("[ERROR] -- Sorry, couldn't send socket\n");
        closesocket(udpSock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // printf("[SUCCESS] -- The socket is sent successfully!\n");
    return EXIT_SUCCESS;
}

// This function allocates memory to buffer, must free it as packet.packetData in main.c
struct packet recvPacket(const SOCKET udpSock) {
    const int BUFFER_SIZE = 4096;
    char* buffer = malloc(BUFFER_SIZE);

    int bytesRecv = recv(udpSock, buffer, BUFFER_SIZE, 0);
    if (bytesRecv == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            struct packet packet = {.packetData = NULL, .packetSize = bytesRecv};
            return packet;
        }
        perror("[ERROR] -- Sorry, couldn't receive packet\n");
        exit(EXIT_FAILURE);
    }
    struct packet packet = {.packetData = buffer, .packetSize = bytesRecv};
    return packet;
}

struct packet handlePacket(const struct packet packet, uint32_t threadID) {
    if (strlen(dnsServerIP) == 0) {
        printf("[ERROR] -- Couldn't find the DNS Server's IP -- Thread ID %"PRIu32"\n", threadID);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(DNS_PORT)};
    int ptonResult = inet_pton(AF_INET, dnsServerIP, &(addr.sin_addr));
    checkPtonResult(ptonResult);

    size_t countTimeouts = 0;
    uint32_t timeouts[5] = {1000, 2000, 4000, 8000, 10000};
    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSock == INVALID_SOCKET) {
        printf("[ERROR] -- Sorry, couldn't create socket\n -- Thread ID %"PRIu32"\n", threadID);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (connect(udpSock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        printf("[ERROR] -- Sorry, couldn't connect to local DNS server -- Thread ID %"PRIu32"\n", threadID);
        exit(EXIT_FAILURE);
    }
    struct packet response = {0};
    while (countTimeouts < sizeof(timeouts) / sizeof(timeouts[0])) {
        setsockopt(udpSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeouts[countTimeouts], sizeof(timeouts[countTimeouts]));
        sendPacket(udpSock, packet, addr);
        response = recvPacket(udpSock);
        if (response.packetData != NULL) {
            closesocket(udpSock);
            return response;
        }
        printf("[ERROR] -- DNS response didnt arrive after %" PRIu32" seconds, sending another query -- Thread ID %"PRIu32"\n", timeouts[countTimeouts]/1000, threadID);
        countTimeouts++;
    }
    printf("[ERROR] -- After multipule retries, didn't receive any response -- Thread ID %"PRIu32"\n", threadID);
    closesocket(udpSock);
    return response; // when response.packetData == NULL
}



