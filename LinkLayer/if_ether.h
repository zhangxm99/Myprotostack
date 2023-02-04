//
// Created by 张晰铭 on 2023/1/30.
//

#ifndef MYTCPIP_IF_ETHER_H
#define MYTCPIP_IF_ETHER_H
#include <cstdint>

//frame header
struct eth_hdr{
    unsigned char dmac[6];
    unsigned char smac[6];
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

#endif //MYTCPIP_IF_ETHER_H
