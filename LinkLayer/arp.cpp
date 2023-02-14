//
// Created by 张晰铭 on 2023/2/2.
//

#include "arp.h"
#include <arpa/inet.h>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <csignal>
#include "if_ether.h"


ArpManager::ArpManager(Netdevice& _dev): arp_cache_table{} ,dev(_dev) {}

void ArpManager::mergeOrInsert(const arp_ipv4 *data, uint16_t hwtype) {
    auto item = std::find_if(arp_cache_table,arp_cache_table+ARP_CACHE_LEN,[hwtype,data](arp_cache_entry &item){
        return item.hwtype == hwtype && item.sip == data->sip;
    });
    if(item != arp_cache_table+ARP_CACHE_LEN){
        item->smac = data->smac;
    } else{
        auto insert = std::find_if(arp_cache_table,arp_cache_table+ARP_CACHE_LEN,[](arp_cache_entry &item){
            return item.state == ARP_FREE;
        });
        if(insert != arp_cache_table+ARP_CACHE_LEN){
            insert->state  = ARP_RESOLVED;
            insert->sip    = data->sip;
            insert->hwtype = hwtype;
            insert->smac   = data->smac;

            return;
        }
        printf("No place for ARP Table\n");
    }
}

void ArpManager::arpReply() {
    auto arpHdr = (arp_hdr*)buf;
    auto arpData = (arp_ipv4*)arpHdr->data;
    arpData->dip = arpData->sip;
    arpData->sip = dev.inet4;
    arpData->dmac = arpData->smac;
    arpData->smac = dev.hwaddr;

    arpHdr->opcode = ARP_REPLY;
    arpHdr->opcode = htons(arpHdr->opcode);
    arpHdr->hwtype = htons(arpHdr->hwtype);
    arpHdr->protype = htons(arpHdr->protype);

    dev.transmit(arpData->dmac,ETH_P_ARP,buf,sizeof(arp_ipv4)+sizeof(arp_hdr));

}

//
void ArpManager::arpRequest(uint32_t ip) {
    buf_lock.lock();

    auto arpHdr = (arp_hdr*) buf;
    auto arpData = (arp_ipv4*)arpHdr->data;

    arpData->dip = ip;
    arpData->sip = dev.inet4;
    arpData->dmac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    arpData->smac = dev.hwaddr;

    arpHdr->opcode = ARP_REQUEST;
    arpHdr->opcode = htons(arpHdr->opcode);
    arpHdr->hwtype = htons(ARP_ETHERNET);
    arpHdr->protype = htons(ARP_IPV4);
    arpHdr->hwsize = 6;
    arpHdr->prosize = 4;

    dev.transmit(arpData->dmac,ETH_P_ARP,buf,sizeof(arp_ipv4)+sizeof(arp_hdr));

    buf_lock.unlock();
}

/* search corresponding MAC addr
 * if not cached ,then send ARPREQUEST
*/
MAC_t ArpManager::searchMAC(uint32_t ip) {
    int retry = 3;
    SEARCH:
    auto searched = find_if(arp_cache_table,arp_cache_table+ARP_CACHE_LEN,[&ip](const arp_cache_entry &item){
        return item.state == ARP_RESOLVED && item.sip == ip;
    });
    if(searched == arp_cache_table+ARP_CACHE_LEN){
        // try to get corresponding MAC three times
        while (retry--){
            arpRequest(ip);
            usleep(500);
            goto SEARCH;
        }
        // no response
        return {0,0,0,0,0,0};
    } else{
        return searched->smac;
    }

}

/* processing incoming arp package,
 * update arp cache table first,if
 * this is for us,then reply it.

          +---------+
          |ether_hdr|
          +---------+
          | arp_hdr |
          +---------+
          | payload |
          | arp_ipv4|
          +---------+
*/
void ArpManager::arpIncoming(arp_hdr *arpHdr) {
    buf_lock.lock();
    memcpy(buf,(char*)arpHdr,ETHERMTU - sizeof(eth_hdr));
    arpHdr = (arp_hdr*) buf;
    // big endian to little endian
    arpHdr->hwtype  =  ntohs(arpHdr->hwtype);
    arpHdr->opcode  =  ntohs(arpHdr->opcode);
    arpHdr->protype =  ntohs(arpHdr->protype);

    if(arpHdr->hwtype != ARP_ETHERNET){
        printf("unsupported hardware\n");
        buf_lock.unlock();
        return;
    }
    if(arpHdr->protype != ARP_IPV4){
        printf("unsupported protocol\n");
        buf_lock.unlock();
        return;
    }

    auto arpIpv4 = (arp_ipv4 *)arpHdr->data;

    mergeOrInsert(arpIpv4,arpHdr->hwtype);

    if(arpIpv4->dip != dev.inet4){
        printf("Not a package for us\n");
        buf_lock.unlock();
        return;
    }

    switch (arpHdr->opcode) {
        case ARP_REQUEST:
            arpReply();
            break;
        case ARP_REPLY:
            break;
        default:
            printf("Unknown ARP opcode\n");
    }
    buf_lock.unlock();




}