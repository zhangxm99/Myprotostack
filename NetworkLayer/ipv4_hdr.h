//
// Created by 张晰铭 on 2023/2/8.
//

#ifndef MYTCPIP_IPV4_HDR_H
#define MYTCPIP_IPV4_HDR_H


#include <cstdint>

#define IPV4   0x04

#define ICMPV4 0x01
#define UDP    0x11
#define TCP    0x06

struct ip_hdr {
#if __BYTE_ORDER == LITTLE_ENDIAN
    uint8_t ihl : 4;
    uint8_t version : 4;
#elif __BYTE_ORDER == BIG_ENDIAN
    uint8_t version : 4;
    uint8_t ihl : 4;
#else
#error "Unknown Byte Order"
#endif
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_offset : 13;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
    uint8_t data[];
} __attribute__((packed));

#endif //MYTCPIP_IPV4_HDR_H
