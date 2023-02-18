//
// Created by 张晰铭 on 2023/2/3.
//

#ifndef MYTCPIP_NETDEVICE_H
#define MYTCPIP_NETDEVICE_H

#include <cstdint>
#include <array>
#include "if_ether.h"
#include "../IF/tuntap.h"
#include <mutex>

#define ETHERMTU 1514



class Netdevice {
private:
    Tuntap iface;
    uint8_t writeBuf[ETHERMTU];
    mutex writeLock;

public:
    uint32_t inet4;
    array<uint8_t, 6> hwaddr;

    Netdevice(char *addr, char *hwaddr);
    void transmit(MAC_t dst, uint16_t ethertype, uint8_t *payload,int payloadlen);
    DataView<eth_hdr,0> receive();


};


#endif //MYTCPIP_NETDEVICE_H
