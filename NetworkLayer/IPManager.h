//
// Created by 张晰铭 on 2023/2/14.
//

#ifndef MYTCPIP_IPMANAGER_H
#define MYTCPIP_IPMANAGER_H


#include "IPDevice.h"
#include "ICMPManager.h"

struct udp_fakehdr{
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
    uint8_t data[];
}__attribute__((packed));

class IPManager {
private:
    IPDevice dev;
    ICMPManager icmpMgr;

    queue<DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)>> UDPq;
//    queue<array<uint8_t,ETHERMTU>> TCPq;

    [[noreturn]] void readLoop();
    uint16_t checksum(void *addr, int count);
    void insertudpQ(queue<DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)>>&,
                    DataView<ip_hdr,sizeof(eth_hdr)> );
public:
    uint32_t myIP;
    IPManager(char *,char *);

    DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)> udp_read();
    int udp_write(uint32_t ip, uint8_t *ipData, int len);

};


#endif //MYTCPIP_IPMANAGER_H
