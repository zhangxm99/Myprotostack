//
// Created by 张晰铭 on 2023/2/3.
//

#ifndef MYTCPIP_NETDEVICE_H
#define MYTCPIP_NETDEVICE_H

#include <cstdint>
#include <array>


class netdevice {
public:
    uint32_t inet4;
//    std::array<unsigned char,6> hwaddr;
    unsigned char hwaddr[6];
    netdevice();
    void netdevice_transmit();
};


#endif //MYTCPIP_NETDEVICE_H
