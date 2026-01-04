//
// Created by shaked on 11/10/2025.
//

#include "Serializor.h"

#include <stdbool.h>


struct packet serializeRequest(const struct dnsHeader header, const struct dnsQuery query) {
    size_t packetSize =
        sizeof(header.id) +
        sizeof(header.flags) +
        sizeof(header.QDcount) +
        sizeof(header.ANcount) +
        sizeof(header.NScount) +
        sizeof(header.ARcount) +
        query.Qname.qnameLength +
        sizeof(query.Qtype) +
        sizeof(query.Qclass);

    char* buffer = malloc(packetSize);
    char* start = buffer;

    memcpy(buffer, &header.id, sizeof(header.id));
    buffer += sizeof(header.id);

    memcpy(buffer, &header.flags, sizeof(header.flags));
    buffer += sizeof(header.flags);

    memcpy(buffer, &header.QDcount, sizeof(header.QDcount));
    buffer += sizeof(header.QDcount);

    memcpy(buffer, &header.ANcount, sizeof(header.ANcount));
    buffer += sizeof(header.ANcount);

    memcpy(buffer, &header.NScount, sizeof(header.NScount));
    buffer += sizeof(header.NScount);

    memcpy(buffer, &header.ARcount, sizeof(header.ARcount));
    buffer += sizeof(header.ARcount);

    memcpy(buffer, query.Qname.qname, query.Qname.qnameLength);
    buffer += query.Qname.qnameLength;

    memcpy(buffer, &query.Qtype, sizeof(query.Qtype));
    buffer += sizeof(query.Qtype);

    memcpy(buffer, &query.Qclass, sizeof(query.Qclass));
    buffer += sizeof(query.Qclass);

    struct packet p = {start, packetSize};
    return p;
}


int read_name(const char* buf, size_t len, size_t *offset, char** name) {
    size_t pos = *offset;
    size_t start = pos;
    size_t nameLen = 0;
    char temp[256];
    bool jumped = FALSE;

    while (pos < len) {
        uint8_t labelLen = buf[pos];

        // Check if this is a compression pointer (2 MSB are 11)
        if ((labelLen & 0xC0) == 0xC0) {
            if (pos + 1 >= len)
                return EXIT_FAILURE; // invalid
            uint16_t pointer = ((labelLen & 0x3F) << 8) | buf[pos + 1]; // Calculate 14 LSB - the offset from the start of the packet
            pos = pointer; // jump to the offset
            jumped = TRUE; // set jumped
            continue;
        }

        if (labelLen == 0) {
            // end of name
            pos++;
            break;
        }

        pos++; // skip length byte
        if (nameLen + labelLen + 1 > sizeof(temp))
            return EXIT_FAILURE; // buffer is to small
        memcpy(temp + nameLen, buf + pos, labelLen); // copy the name to temp
        nameLen += labelLen;
        temp[nameLen] = '.';
        nameLen++;
        pos += labelLen;
    }

    if (nameLen > 0)
        temp[nameLen - 1] = '\0'; // remove last dot
    else
        temp[0] = '\0';

    *name = malloc(nameLen + 1);
    memcpy(*name, temp, nameLen + 1);

    if (!jumped)
        *offset = pos;
    else
        *offset += 2; // pointer consumes 2 bytes
    return 0;
}


/* This function allocates memory to answer.RData
 * Also it allocates memory to query.Qname.qname and answer.name via read_name
* Must free it in main.c (as response.answer.RData)
*/

struct response deserializeResponse(struct packet packet) {
    struct dnsHeader header = {0};
    struct dnsQuery query = {0};

    size_t offset = 0;

    char* start = packet.packetData;
    memcpy(&header.id, start + offset, sizeof(header.id));
    offset += sizeof(header.id);
    header.id = ntohs(header.id);

    memcpy(&header.flags, start + offset, sizeof(header.flags));
    offset += sizeof(header.flags);
    header.flags = ntohs(header.flags);

    memcpy(&header.QDcount, start + offset, sizeof(header.QDcount));
    offset += sizeof(header.QDcount);
    header.QDcount = ntohs(header.QDcount);

    memcpy(&header.ANcount, start + offset, sizeof(header.ANcount));
    offset += sizeof(header.ANcount);
    header.ANcount = ntohs(header.ANcount);

    memcpy(&header.NScount, start + offset, sizeof(header.NScount));
    offset += sizeof(header.NScount);
    header.NScount = ntohs(header.NScount);

    memcpy(&header.ARcount, start + offset, sizeof(header.ARcount));
    offset += sizeof(header.ARcount);
    header.ARcount = ntohs(header.ARcount);

    // query.Qname.qnameLength = strlen(query.Qname.qname)+1;
    read_name(start, packet.packetSize, &offset, &query.Qname.qname);

    memcpy(&query.Qtype, start + offset, sizeof(query.Qtype));
    offset += sizeof(query.Qtype);
    query.Qtype = ntohs(query.Qtype);

    memcpy(&query.Qclass, start + offset, sizeof(query.Qclass));
    offset += sizeof(query.Qclass);
    query.Qclass = ntohs(query.Qclass);


    struct dnsAnswer* answers = malloc(sizeof(struct dnsAnswer) * header.ANcount);

    // release answers[i].name and answers[i].RData in freeResponse
    for (size_t i = 0 ; i < header.ANcount ; i++) {
        read_name(start, packet.packetSize, &offset, &answers[i].name);

        memcpy(&answers[i].type, start + offset, sizeof(answers[i].type));
        offset += sizeof(answers[i].type);
        answers[i].type = ntohs(answers[i].type);

        memcpy(&answers[i].class, start + offset, sizeof(answers[i].class));
        offset += sizeof(answers[i].class);
        answers[i].class = ntohs(answers[i].class);

        memcpy(&answers[i].ttl, start + offset, sizeof(answers[i].ttl));
        offset += sizeof(answers[i].ttl);
        answers[i].ttl = ntohl(answers[i].ttl);

        memcpy(&answers[i].RDLength, start + offset, sizeof(answers[i].RDLength));
        offset += sizeof(answers[i].RDLength);
        answers[i].RDLength = ntohs(answers[i].RDLength);

        answers[i].RData = malloc(answers[i].RDLength);
        memcpy(answers[i].RData, start + offset, answers[i].RDLength);
        offset += answers[i].RDLength;
    }

    struct response resPack = {header, query, answers};
    return resPack;
}

void freeResponse(struct response *res) {
    free((void*)res->query.Qname.qname);
    for (size_t i = 0 ; i < res->header.ANcount ; i++) {
        free(res->answers[i].name);
        free(res->answers[i].RData);
    }
    free(res->answers);
}