//
// Created by 张晰铭 on 2023/2/3.
//

#include "Netdevice.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include "if_ether.h"

Netdevice::Netdevice(char *addr, char *mac):iface(),inet4(), hwaddr(),buf{} {
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

eth_hdr* Netdevice::receive() {
    iface.tun_read(buf,ETHERMTU);
    return (eth_hdr *)(buf);
}

void Netdevice::transmit(std::array<unsigned char, 6> dst, uint16_t ethertype, int payloadlen) {
    ((eth_hdr*)&buf)->ethertype = htons(ethertype);
    ((eth_hdr*)&buf)->smac = hwaddr;
    ((eth_hdr*)&buf)->dmac = dst;

    int len = payloadlen + sizeof(eth_hdr);

    iface.tun_write(buf,len);
}

