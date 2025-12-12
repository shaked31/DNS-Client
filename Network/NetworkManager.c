//
// Created by Shaked Pollak on 11/7/2025.
//

#include "NetworkManager.h"
#include "../Utils/Serializor.h"


int networkInit() {
    struct WSAData wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        /*
         *  WSAStartup starts the winsock dll. without this, we can't use functions like socket()
         *  MAKEWORD(2,2) creates version number (2.2, the most common socket version)
         *  &wsaData is a pointer to a WSA struct. WSAStartup fills this struct
         */
        perror("Couldn't init winsock");
        return EXIT_FAILURE;
    }
}

void networkFin() {
    WSACleanup();
}


char* recvPacket() {
    struct sockaddr_in peerAddr;
    struct sockaddr_in myAddr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(DNS_PORT)};
    // INADDR_ANY means the socket will accept traffic from every interface available
    const int BUFFER_SIZE = 4096;
    char* buffer = {0};

    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSock == INVALID_SOCKET) {
        perror("Sorry, couldn't create socket\n");
    }

    int res = bind(udpSock, (struct sockaddr*)&myAddr, sizeof(myAddr));
    if (res == SOCKET_ERROR) {
        perror("Sorry, couldn't bind socket to address\n");
    }

    int addressLength = sizeof(peerAddr);
    int bytesRecv = recvfrom(udpSock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&peerAddr, &addressLength);
    if (bytesRecv == SOCKET_ERROR) {
        perror("Sorry, couldn't receive packet\n");
    }

    printf("Received a packet from %s:%d -- Message:\n%s\n", inet_ntoa(peerAddr.sin_addr), ntohs(peerAddr.sin_port), buffer);
    return buffer;
}

int sendPacket(const struct packet packet, const char* ipDst) {
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(DNS_PORT)};
    int inet_result = inet_pton(AF_INET, ipDst, &(addr.sin_addr));

    /*  inet_pton converts printable to network order and copies ipDst to addr.sin_addr
     *  success return value = 1
     *  invalid IP address = 0
     *  address family is unsupported = -1
     */
    if (inet_result == 0) {
        perror("Something wrong with the ip address given!\n");
        return EXIT_FAILURE;
    }
    if (inet_result == -1) {
        perror("Somethings wrong with the address family\n");
        return EXIT_FAILURE;
    }

    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpSock == INVALID_SOCKET) {
        perror("Sorry, couldn't create socket\n");
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (sendto(udpSock, packet.packetData, packet.packetSize + 1, 0,
        (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Sorry, couldn't send socket\n");
            closesocket(udpSock);
            WSACleanup();
            return EXIT_FAILURE;
    }

    printf("The socket is sent successfully!\n");
    closesocket(udpSock);
    return EXIT_SUCCESS;
}

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
            char* IPstr = (char*)malloc(sizeof(char) * (subStrLen));
            strcpy(IPstr, ptrToIP);
            printf("The DNS Server IP is: %s\n", IPstr);
            _pclose(fptr);
            return IPstr;
        }
    }
    _pclose(fptr);
    return "";
}
