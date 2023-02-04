//
// Created by 张晰铭 on 2023/2/2.
//

#ifndef MYTCPIP_ARP_H
#define MYTCPIP_ARP_H

#include <cstdint>
#include "if_ether.h"
#include "netdevice.h"

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
    unsigned char hwsize;
    unsigned char prosize;
    uint16_t opcode;
    unsigned char data[];
} __attribute__((packed));

struct arp_ipv4
{
    unsigned char smac[6];
    uint32_t sip;
    unsigned char dmac[6];
    uint32_t dip;
} __attribute__((packed));

struct arp_cache_entry{
    uint16_t hwtype;
    uint32_t sip;
    unsigned char smac[6];
    unsigned int state;
};


class ArpManager {
private:
    arp_cache_entry arp_cache_table[ARP_CACHE_LEN];

    void arpIncoming(eth_hdr *framehdr, netdevice &dev);
    void mergeOrInsert(arp_ipv4 *data,uint16_t hwtype);
    void arpReply(netdevice *netdev, eth_hdr *hdr, arp_hdr *arphdr);

public:
    ArpManager();
};


#endif //MYTCPIP_ARP_H
