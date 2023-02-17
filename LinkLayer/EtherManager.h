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
    queue<DataView<ip_hdr,sizeof(eth_hdr)>> ipQ;

public:
    EtherManager(char *,char *);
    DataView<struct ip_hdr, sizeof(struct eth_hdr)> ip_read();
    int ip_write(uint32_t nxthop, ip_hdr* ipData);
};


#endif //MYTCPIP_ETHERMANAGER_H
