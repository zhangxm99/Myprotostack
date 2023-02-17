//
// Created by 张晰铭 on 2023/2/7.
//

#include <csignal>
#include "EtherManager.h"
#include "thread"
#include "arpa/inet.h"
#include "if_ether.h"

[[noreturn]] void EtherManager::readLoop() {
    while(true){
        auto view = dev.receive();
        auto ethHdr = view.hdr;
        switch (ntohs(ethHdr->ethertype)) {
            case ETH_P_ARP:
                arpMgr.arpIncoming((arp_hdr*)(ethHdr->payload));
                break;
            case ETH_P_IP:{
                ipQ.emplace(DataView<ip_hdr,sizeof(eth_hdr)>(view));
                break;
            }
            default:
                printf("%x not implement\n",ntohs(ethHdr->ethertype));
        }

    }
}

DataView<ip_hdr,sizeof(eth_hdr)> EtherManager::ip_read() {
    while(ipQ.empty()) usleep(100); //spinlock
    auto res(ipQ.front());
    ipQ.pop();
    return res;
}

int EtherManager::ip_write(uint32_t nxthop,ip_hdr* ipData) {
    auto dmac = arpMgr.searchMAC(nxthop);
    //cant find such ip addr
    if(dmac == MAC_t{0,0,0,0,0,0}){
        return -1;
    }
    int len = ntohs(ipData->len);

    dev.transmit(dmac,ETH_P_IP,(unsigned char *)ipData,len);
}

EtherManager::EtherManager(char *_ip, char *MAC_): dev(_ip,MAC_), arpMgr(dev) {
    std::thread l(&EtherManager::readLoop, this);
    l.detach();
}
