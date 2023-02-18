//
// Created by 张晰铭 on 2023/2/15.
//

#ifndef MYTCPIP_UDPMANAGER_H
#define MYTCPIP_UDPMANAGER_H
#include "../NetworkLayer/IPManager.h"
#include <unordered_map>


struct UDPStruct{
    uint32_t ip{};
    uint16_t conterport{};
    DataView<uint8_t,sizeof(eth_hdr)+sizeof(ip_hdr)+sizeof(udp_hdr)> datagram;
    uint16_t sz{};
};

class UDPManager {
private:
    uint32_t ip;
    IPManager ipmgr;
    unordered_map<uint16_t,queue<UDPStruct>> portMp;
    [[noreturn]]void readLoop();
    uint16_t checksum(void *addr, int count);


public:
    UDPManager(char*,char*);
    UDPStruct getData(uint16_t);
    int writeData(uint32_t,uint16_t,uint16_t ,uint8_t *,int );



};


#endif //MYTCPIP_UDPMANAGER_H
