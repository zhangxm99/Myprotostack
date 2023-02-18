//
// Created by 张晰铭 on 2023/2/3.
//

#include "Netdevice.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "if_ether.h"


Netdevice::Netdevice(char *addr, char *mac): iface(),writeLock() {
    if(inet_pton(AF_INET,addr,&inet4) != 1){
        fputs("failed to bind the ip address",stderr);
        exit(1);
    }
    sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &hwaddr[0],
           &hwaddr[1],
           &hwaddr[2],
           &hwaddr[3],
           &hwaddr[4],
           &hwaddr[5]);
}

DataView<eth_hdr,0> Netdevice::receive() {
    auto pData = new uint8_t[ETHERMTU];
    iface.tun_read(pData, ETHERMTU);
    DataView<eth_hdr,0> ret(pData);
    return ret;
}

void Netdevice::transmit(std::array<uint8_t, 6> dst, uint16_t ethertype, uint8_t *payload, int payloadlen) {
    writeLock.lock();
    ((eth_hdr*)&writeBuf)->ethertype = htons(ethertype);
    ((eth_hdr*)&writeBuf)->smac = hwaddr;
    ((eth_hdr*)&writeBuf)->dmac = dst;

    memcpy(((eth_hdr*)&writeBuf)->payload,payload,payloadlen);

    int len = payloadlen + sizeof(eth_hdr);

    iface.tun_write(writeBuf, len);
    writeLock.unlock();
}

