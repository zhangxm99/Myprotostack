//
// Created by 张晰铭 on 2023/2/7.
//

#include <cstring>
#include <csignal>
#include "EtherManager.h"
#include "thread"
#include "arpa/inet.h"
#include "if_ether.h"
#include "../NetworkLayer/ipv4_hdr.h"

void EtherManager::insertipQ(char *ip) {
    array<unsigned char,ETHERMTU> arr{};
    memcpy(&arr[0],ip,ETHERMTU - sizeof(eth_hdr));
    ipQ.emplace(arr);
}

[[noreturn]] void EtherManager::readLoop() {
    while(true){
        eth_hdr *ethHdr = dev.receive();
        switch (ntohs(ethHdr->ethertype)) {
            case ETH_P_ARP:
                arpMgr.arpIncoming((arp_hdr*)(ethHdr->payload));
                break;
            case ETH_P_IP:
                insertipQ((char*)ethHdr->payload);
                break;
            default:
                printf("%x not implement\n",ntohs(ethHdr->ethertype));
        }

    }
}

array<unsigned char,ETHERMTU> EtherManager::ip_read() {
    while(ipQ.empty()) usleep(100); //spinlock
    array<unsigned char,ETHERMTU> res{};
    res = ipQ.front();
    ipQ.pop();
    return res;
}

int EtherManager::ip_write(uint32_t nxthop,array<unsigned char,ETHERMTU>& ipData) {
    auto dmac = arpMgr.searchMAC(nxthop);
    //cant find such ip addr
    if(dmac == MAC_t{0,0,0,0,0,0}){
        return -1;
    }

    dev.transmit(dmac,ETH_P_IP,&ipData[0],ETHERMTU - sizeof(eth_hdr));
}

EtherManager::EtherManager(char *_ip, char *MAC_): dev(_ip,MAC_), arpMgr(dev) {
    std::thread l(&EtherManager::readLoop, this);
    l.detach();
}
