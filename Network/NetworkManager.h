//
// Created by Shaked Pollak on 11/7/2025.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <winsock.h>
#include <wininet.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include "dns.h"

#define MAX_CMD_OUTPUT_SIZE 256
#define DNS_PORT 53


int sendPacket(const uint8_t* packet, char* ipDst);
char* findDnsServerIP();



#endif //NETWORKMANAGER_H
