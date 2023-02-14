//
// Created by 张晰铭 on 2023/2/14.
//

#ifndef MYTCPIP_IPMANAGER_H
#define MYTCPIP_IPMANAGER_H


#include "IPDevice.h"
#include "ICMPManager.h"

class IPManager {
private:
    IPDevice dev;
    ICMPManager icmpMgr;

    queue<array<unsigned char,ETHERMTU>> UDPq;
    queue<array<unsigned char,ETHERMTU>> TCPq;

    [[noreturn]] void readLoop();
    uint16_t checksum(void *addr, int count);
    void insertQ(queue<array<unsigned char, 1514>> &queue1, uint8_t data[]);
public:
    IPManager(char *,char *);


};


#endif //MYTCPIP_IPMANAGER_H
