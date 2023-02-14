//
// Created by 张晰铭 on 2023/2/14.
//

#include <netinet/in.h>
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
    std::thread l(&IPManager::readLoop, this);
    l.detach();
}

[[noreturn]] void IPManager::readLoop() {
    while (true){
        auto ipHdr = dev.receive();
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
                insertQ(UDPq,ipHdr->data);
                break;
            case TCP:
                insertQ(TCPq,ipHdr->data);
                break;
            default:
                perror("Unknown IP header proto\n");
                continue;
        }

    }
}

void IPManager::insertQ(queue<array<unsigned char, 1514>> &q, uint8_t *data) {
    array<unsigned char,ETHERMTU> arr{};
    memcpy(&arr[0],data,ETHERMTU - sizeof(eth_hdr) - sizeof(ip_hdr));
    q.emplace(arr);
}
