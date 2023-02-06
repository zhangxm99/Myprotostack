//
// Created by 张晰铭 on 2023/2/3.
//

#ifndef MYTCPIP_NETDEVICE_H
#define MYTCPIP_NETDEVICE_H

#include <cstdint>
#include <array>
#include "if_ether.h"
#include "../IF/tuntap.h"
#include <vector>

#define ETHERMTU 1500



class Netdevice {
private:
    Tuntap iface;
    char buf[ETHERMTU];
public:
    uint32_t inet4;
    std::array<unsigned char,6> hwaddr;

    Netdevice(char *addr, char *hwaddr);
    void transmit(std::array<unsigned char,6> dst, uint16_t ethertype, int payloadlen);
    eth_hdr* receive();
};


#endif //MYTCPIP_NETDEVICE_H
