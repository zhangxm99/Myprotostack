//
// Created by 张晰铭 on 2023/2/14.
//

#ifndef MYTCPIP_IPMANAGER_H
#define MYTCPIP_IPMANAGER_H


#include "IPDevice.h"
#include "ICMPManager.h"

struct UDPFakeheader{
    uint32_t src;
    uint32_t dst;
    uint8_t ZERO;
    uint8_t UDPproto;
    uint16_t UDPlen;
    uint8_t data[];
} __attribute__((packed));

struct udp_hdr
{
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
};

class IPManager {
private:
    IPDevice dev;
    ICMPManager icmpMgr;

    queue<DataView<UDPFakeheader,sizeof(eth_hdr)+sizeof(ip_hdr)-sizeof(UDPFakeheader)>> UDPq;
//    queue<array<unsigned char,ETHERMTU>> TCPq;

    [[noreturn]] void readLoop();
    uint16_t checksum(void *addr, int count);
    void insertudpQ(queue<DataView<UDPFakeheader,sizeof(eth_hdr)+sizeof(ip_hdr)-sizeof(UDPFakeheader)>>&,
                    DataView<ip_hdr,sizeof(eth_hdr)> );
public:
    IPManager(char *,char *);

    auto udp_read();
    int udp_write(uint32_t ip, array<unsigned char, ETHERMTU> &ipData,int len);

};


#endif //MYTCPIP_IPMANAGER_H
