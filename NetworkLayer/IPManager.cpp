//
// Created by 张晰铭 on 2023/2/14.
//

#include <netinet/in.h>
#include <csignal>
#include <arpa/inet.h>
#include "IPManager.h"

uint16_t IPManager::checksum(void *addr, int count) {
    /* Compute Internet Checksum for "count" bytes
 *         beginning at location "addr".
 * Taken from https://tools.ietf.org/html/rfc1071
 */
    uint32_t sum = 0;
    auto * ptr = (uint16_t*)addr;

    while( count > 1 )  {
        /*  This is the inner loop */
        sum += * ptr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += * (uint8_t *) addr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

IPManager::IPManager(char *_ip, char *MAC_): dev(_ip,MAC_), icmpMgr(_ip,dev) {
    inet_pton(AF_INET,_ip,&myIP);
    std::thread l(&IPManager::readLoop, this);
    l.detach();
}

[[noreturn]] void IPManager::readLoop() {
    while (true){
        auto v = dev.receive();
        auto ipHdr = v.hdr;
        if (ipHdr->version != IPV4) {
            perror("Datagram version was not IPv4\n");
            continue;
        }

        if (ipHdr->ihl < 5) {
            perror("IPv4 header length must be at least 5\n");
            continue;
        }

        if (ipHdr->ttl == 0) {
            perror("Time to live of datagram reached 0\n");
            continue;
        }

        uint16_t csum = checksum(ipHdr, ipHdr->ihl * 4);
        if(csum != 0){
            continue;
        }

        ipHdr->len = ntohs(ipHdr->len);

        switch (ipHdr->proto) {
            case ICMPV4:
                icmpMgr.icmpIncoming(ipHdr);
                break;
            case UDP:
                printf("%ld\n",v.pData.use_count());
                insertudpQ(UDPq,DataView<ip_hdr,sizeof(eth_hdr)>(v));
                break;
            case TCP:
//                inserttcpQ(TCPq,ipHdr->data);
                break;
            default:
                perror("Unknown IP header proto\n");
                continue;
        }

    }
}

void IPManager::insertudpQ(queue<DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)>> &q,
                           DataView<ip_hdr,sizeof(eth_hdr)> v) {
    uint32_t src = v.hdr->saddr;
    uint32_t dst = v.hdr->daddr;
    auto res = DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)>(v);
    (res.hdr)->dst = dst;
    (res.hdr)->src = src;
    (res.hdr)->ZERO = 0;
    (res.hdr)->UDPproto = UDP;
    (res.hdr)->UDPlen = ((udp_hdr *)(res.hdr->data))->len;
    q.emplace(res);
}

//! \return udp fake header
DataView<udp_fakehdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(udp_fakehdr)> IPManager::udp_read() {
    while(UDPq.empty()) usleep(100);
    auto res(UDPq.front());
    UDPq.pop();
    return res;
}

int IPManager::udp_write(uint32_t ip, uint8_t *ipData, int len) {
    return dev.transmit(ip,UDP,0,(char*)ipData,len);
}