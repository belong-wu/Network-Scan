#pragma once

struct icmp_packet_t {
    unsigned char type_;
    unsigned char code_;
    unsigned short int checksum_;

    uint16_t ident;
    uint16_t seq;

    // data
    double sending_ts;
    char magic[10];
};