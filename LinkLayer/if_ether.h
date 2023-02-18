//
// Created by 张晰铭 on 2023/1/30.
//

#ifndef MYTCPIP_IF_ETHER_H
#define MYTCPIP_IF_ETHER_H
#include <cstdint>
#include <array>

#define ETH_P_ARP 0x0806
#define ETH_P_IP 0x0800

using MAC_t = std::array<uint8_t,6>;

//frame header
struct eth_hdr{
    MAC_t dmac;
    MAC_t smac;
    uint16_t ethertype;
    uint8_t payload[];
} __attribute__((packed));

#endif //MYTCPIP_IF_ETHER_H
