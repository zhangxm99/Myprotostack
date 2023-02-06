//
// Created by 张晰铭 on 2023/1/30.
//

#ifndef MYTCPIP_IF_ETHER_H
#define MYTCPIP_IF_ETHER_H
#include <cstdint>
#include <array>

#define ETH_P_ARP 0x0806

//frame header
struct eth_hdr{
    std::array<unsigned char,6> dmac;
    std::array<unsigned char,6> smac;
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

#endif //MYTCPIP_IF_ETHER_H
