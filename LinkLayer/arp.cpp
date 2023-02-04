//
// Created by 张晰铭 on 2023/2/2.
//

#include "arp.h"
#include <arpa/inet.h>
#include <cstdio>
#include <algorithm>
#include <cstring>

ArpManager::ArpManager():arp_cache_table{} {}

void ArpManager::mergeOrInsert(arp_ipv4 *data, uint16_t hwtype) {
    auto item = std::find_if(arp_cache_table,arp_cache_table+ARP_CACHE_LEN,[hwtype,data](arp_cache_entry &item){
        return item.hwtype == hwtype && item.sip == data->sip;
    });
    if(item != arp_cache_table+ARP_CACHE_LEN){
        memcpy(item->smac,data->smac,6);
    } else{
        auto insert = std::find_if(arp_cache_table,arp_cache_table+ARP_CACHE_LEN,[](arp_cache_entry &item){
            return item.state == ARP_FREE;
        });
        if(insert != arp_cache_table+ARP_CACHE_LEN){
            insert->state  = ARP_RESOLVED;
            insert->sip    = data->sip;
            insert->hwtype = hwtype;
            memcpy(insert->smac,data->smac,6);

            return;
        }
        printf("No place for ARP Table\n");
    }
}

void ArpManager::arpReply(netdevice *netdev, eth_hdr *hdr, arp_hdr *arphdr) {

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
void ArpManager::arpIncoming(eth_hdr *framehdr, netdevice &dev) {
    auto arpHdr = (arp_hdr*)framehdr->payload;
    // big endian to little endian
    arpHdr->hwtype  =  ntohs(arpHdr->hwtype);
    arpHdr->opcode  =  ntohs(arpHdr->opcode);
    arpHdr->protype =  ntohs(arpHdr->protype);

    if(arpHdr->hwtype != ARP_ETHERNET){
        printf("unsupported hardware\n");
        return;
    }
    if(arpHdr->protype != ARP_IPV4){
        printf("unsupported protocol\n");
        return;
    }

    auto arpIpv4 = (arp_ipv4 *)arpHdr->data;
    mergeOrInsert(arpIpv4,arpHdr->hwtype);

    if(arpIpv4->dip != dev.inet4){
        printf("Not a package for us\n");
    }

//    switch (arpHdr->opcode) {
//        case ARP_REQUEST:
////            arpReply()
//
//    }




}