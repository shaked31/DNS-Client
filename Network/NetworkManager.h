//
// Created by Shaked Pollak on 11/7/2025.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/packet.h"

#define MAX_CMD_OUTPUT_SIZE 256
#define DNS_PORT 53

int networkInit();
void networkFin();
struct packet handlePacket(const struct packet packet, uint32_t threadID);

#endif //NETWORKMANAGER_H
