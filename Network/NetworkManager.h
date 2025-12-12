//
// Created by Shaked Pollak on 11/7/2025.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "dns.h"

#define MAX_CMD_OUTPUT_SIZE 256
#define DNS_PORT 53

int networkInit();
void networkFin();
int sendPacket(const struct packet packet, const char* ipDst);
char* recvPacket();
char* findDnsServerIP();



#endif //NETWORKMANAGER_H
