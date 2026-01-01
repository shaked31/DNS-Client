//
// Created by Shaked Pollak on 12/25/2025.
//

#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>

#define IPv4OPTION 1
#define IPv6OPTION 2


#pragma pack(push, 1)
struct packet {
    char* packetData;
    size_t packetSize;
};
#pragma pack(pop)


#endif //PACKET_H
