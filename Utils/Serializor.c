//
// Created by shaked on 11/10/2025.
//

#include "Serializor.h"




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


int read_name(const char* buf, size_t len, size_t *offset, char **out) {
    size_t pos = *offset;
    size_t start = pos;
    size_t out_len = 0;
    char temp[256];
    int jumped = 0;

    while (pos < len) {
        uint8_t len_byte = buf[pos];

        if ((len_byte & 0xC0) == 0xC0) {
            // Name compression pointer
            if (pos + 1 >= len) return -1; // invalid
            uint16_t pointer = ((len_byte & 0x3F) << 8) | buf[pos + 1];
            pos = pointer; // jump to the offset
            jumped = 1;
            continue;
        }

        if (len_byte == 0) {
            pos++; // end of name
            break;
        }

        pos++; // skip length byte
        if (out_len + len_byte + 1 > sizeof(temp)) return -1;
        memcpy(temp + out_len, buf + pos, len_byte);
        out_len += len_byte;
        temp[out_len++] = '.';
        pos += len_byte;
    }

    if (out_len > 0) temp[out_len - 1] = '\0'; // remove last dot
    else temp[0] = '\0';

    *out = malloc(out_len);
    if (!*out) return -1;
    memcpy(*out, temp, out_len);

    if (!jumped) *offset = pos;
    else *offset += 2; // pointer consumes 2 bytes

    return 0;
}



struct response deserializeResponse(struct packet hexResponse) {
    struct dnsHeader header = {0};
    struct dnsQuery query = {0};

    size_t offset = 0;

    char* start = hexResponse.packetData;
    memcpy(&header.id, start + offset, sizeof(header.id));
    offset += sizeof(header.id);

    memcpy(&header.flags, start + offset, sizeof(header.flags));
    offset += sizeof(header.flags);

    memcpy(&header.QDcount, start + offset, sizeof(header.QDcount));
    offset += sizeof(header.QDcount);

    memcpy(&header.ANcount, start + offset, sizeof(header.ANcount));
    offset += sizeof(header.ANcount);

    memcpy(&header.NScount, start + offset, sizeof(header.NScount));
    offset += sizeof(header.NScount);

    memcpy(&header.ARcount, start + offset, sizeof(header.ARcount));
    offset += sizeof(header.ARcount);

    // query.Qname.qnameLength = strlen(query.Qname.qname)+1;
    read_name(start, hexResponse.packetSize, &offset, &query.Qname.qname);

    memcpy(query.Qname.qname, start + offset, query.Qname.qnameLength);
    offset += query.Qname.qnameLength;

    memcpy(&query.Qtype, start + offset, sizeof(query.Qtype));
    offset += sizeof(query.Qtype);

    memcpy(&query.Qclass, start + offset, sizeof(query.Qclass));
    offset += sizeof(query.Qclass);



    const uint16_t ancount = ntohs(header.ANcount);
    // for (size_t i = 0 ; i < ancount ; i++) {
    struct dnsAnswer answer = {0};
    read_name(start, hexResponse.packetSize, &offset, &answer.name);
    // need to free name

    if (offset + 10 > hexResponse.packetSize) {
        perror("Sorry, ran into trouble while deserializing the response");
    }
    memcpy(&answer.type, start + offset, sizeof(answer.type));
    offset += sizeof(answer.type);

    memcpy(&answer.class, start + offset, sizeof(answer.type));
    offset += sizeof(answer.class);

    memcpy(&answer.ttl, start + offset, sizeof(answer.type));
    offset += sizeof(answer.ttl);

    memcpy(&answer.RDLength, start + offset, sizeof(answer.type));
    offset += sizeof(answer.RDLength);

    answer.RData = malloc(answer.RDLength);
    memcpy(answer.RData, start + offset, answer.RDLength);

    struct response resPack = {header, query, answer};
    return resPack;
    // }
}

