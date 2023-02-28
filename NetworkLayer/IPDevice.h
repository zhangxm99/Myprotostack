//
// Created by 张晰铭 on 2023/2/8.
//

#ifndef MYTCPIP_IPDEVICE_H
#define MYTCPIP_IPDEVICE_H
#include <queue>
#include <cstring>
#include "../LinkLayer/EtherManager.h"


class IPDevice {
private:
    EtherManager ethMgr;
    array<uint8_t,ETHERMTU> writeBuf{};
    mutex write_lock;
    uint16_t id{};


    uint32_t ip{};

    uint16_t checksum(void *addr, int count);
public:
    IPDevice(char *, char*);

    int transmit(uint32_t addr,uint8_t proto,uint8_t tos,char *payload,uint16_t len);
    DataView<ip_hdr,sizeof(eth_hdr)> receive();



};


#endif //MYTCPIP_IPDEVICE_H
