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
        sum += * (uint8_t *) ptr;

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
        ipHdr->id  = ntohs(ipHdr->id);

        /* check if it's a split IP packet */
        if((*((uint8_t*)ipHdr+6)&0x40) == 0){
            /* split */
            switch (splitMp[ipHdr->id].insert(v)) {
                case 0:
                    v = splitMp[ipHdr->id].get();
                    splitMp.erase(ipHdr->id);
                    ipHdr = v.hdr;
                    break;
                case -2:
                    splitMp.erase(ipHdr->id);
                default:
                    continue;

            }
        }

        switch (ipHdr->proto) {
            case ICMPV4:
                icmpMgr.icmpIncoming(ipHdr);
                break;
            case UDP:
                insertQ(UDPq,DataView<ip_hdr,sizeof(eth_hdr)>(v),UDP);
                break;
            case TCP:
                insertQ(TCPq,DataView<ip_hdr,sizeof(eth_hdr)>(v),TCP);
                break;
            default:
                perror("Unknown IP header proto\n");
                continue;
        }
    }


}

void IPManager::insertQ(queue<DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)>> &q,
                           DataView<ip_hdr,sizeof(eth_hdr)> v,uint32_t protoType) {
    uint32_t src = v.hdr->saddr;
    uint32_t dst = v.hdr->daddr;
    int dataofipLen = htons(v.hdr->len - v.hdr->ihl*4);
    auto res = DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)>(v);
    (res.hdr)->dst = dst;
    (res.hdr)->src = src;
    (res.hdr)->ZERO = 0;
    (res.hdr)->proto = protoType;
    (res.hdr)->len = dataofipLen;
    q.emplace(res);
}

//! \return fake header
DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)> IPManager::read(uint32_t protoType) {
    auto &q = protoType == UDP?UDPq:TCPq;
    while(q.empty()) usleep(100);
    auto res(q.front());
    q.pop();
    return res;
}

int IPManager::write(uint32_t ip,uint8_t proto, uint8_t *ipData, int len) {
    return dev.transmit(ip,proto,0,(char*)ipData,len);
}