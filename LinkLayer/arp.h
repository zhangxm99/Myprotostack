//
// Created by 张晰铭 on 2023/2/2.
//

#ifndef MYTCPIP_ARP_H
#define MYTCPIP_ARP_H

#include <cstdint>
#include "if_ether.h"
#include "Netdevice.h"

#define ARP_ETHERNET    0x0001
#define ARP_IPV4        0x0800
#define ARP_REQUEST     0x0001
#define ARP_REPLY       0x0002

#define ARP_FREE        0
#define ARP_WAITING     1
#define ARP_RESOLVED    2

#define ARP_CACHE_LEN   64

struct arp_hdr
{
    uint16_t hwtype;
    uint16_t protype;
    uint8_t hwsize;
    uint8_t prosize;
    uint16_t opcode;
    uint8_t data[];
} __attribute__((packed));

struct arp_ipv4
{
    MAC_t smac;
    uint32_t sip;
    MAC_t dmac;
    uint32_t dip;
} __attribute__((packed));

struct arp_cache_entry{
    uint16_t hwtype;
    uint32_t sip;
    MAC_t smac;
    unsigned int state;
};


class ArpManager {
private:
    arp_cache_entry arp_cache_table[ARP_CACHE_LEN];
    Netdevice &dev;

    void arpRequest(uint32_t);
    void mergeOrInsert(const arp_ipv4 *,uint16_t);
    void arpReply(arp_hdr*);


public:
    ArpManager(Netdevice&);
    void arpIncoming(arp_hdr *framehdr);
    MAC_t searchMAC(uint32_t ip);

};


#endif //MYTCPIP_ARP_H
