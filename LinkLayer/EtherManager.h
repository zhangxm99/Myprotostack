//
// Created by 张晰铭 on 2023/2/7.
//

#ifndef MYTCPIP_ETHERMANAGER_H
#define MYTCPIP_ETHERMANAGER_H

#include <thread>
#include "Netdevice.h"
#include "arp.h"
#include "queue"
#include "../NetworkLayer/ipv4_hdr.h"
#include <mutex>

class EtherManager {
private:
    Netdevice dev;
    ArpManager arpMgr;

    [[noreturn]] void readLoop();
    void insertipQ(char *);

    queue<array<unsigned char,ETHERMTU>> ipQ;

public:
    EtherManager(char *,char *);
    array<unsigned char,ETHERMTU> ip_read();
    int ip_write(uint32_t nxthop, array<unsigned char, ETHERMTU> &ipData);
};


#endif //MYTCPIP_ETHERMANAGER_H
