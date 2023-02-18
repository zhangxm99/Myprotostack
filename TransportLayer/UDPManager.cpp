//
// Created by 张晰铭 on 2023/2/15.
//

#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "UDPManager.h"


UDPStruct UDPManager::getData(uint16_t localport) {
    while(portMp[localport].empty()) usleep(100);
    auto res = portMp[localport].front();
    portMp[localport].pop();
    return res;
}

UDPManager::UDPManager(char *_ip, char *MAC_): ipmgr(_ip,MAC_) {
    inet_pton(AF_INET,_ip,&ip);
    thread l(&UDPManager::readLoop, this);
    l.detach();
}

uint16_t UDPManager::checksum(void *addr, int count) {
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

void UDPManager::readLoop() {
    while (true){
        auto UDPcontent = ipmgr.udp_read();
        auto source = UDPcontent.hdr->src;
        auto srcport = ntohs(((udp_hdr*)(UDPcontent.hdr->data))->source);
        auto destport = ntohs(((udp_hdr*)(UDPcontent.hdr->data))->dest);
        auto len = ntohs(UDPcontent.hdr->UDPlen);

        //check
        if(checksum(UDPcontent.hdr,len + sizeof(udp_fakehdr)) != 0) continue;

        portMp[destport].emplace(UDPStruct {
            .ip = source,
            .conterport = srcport,
            .datagram = DataView<uint8_t,sizeof(eth_hdr)+sizeof(ip_hdr)+sizeof(udp_hdr)>(UDPcontent),
            .sz = (uint16_t)(len - sizeof(udp_hdr)),
        });
        printf("%lu\n",portMp[100].size());


    }
}
int UDPManager::writeData(uint32_t dst, uint16_t selfport,uint16_t desport,uint8_t *data,int len) {
    uint8_t buf[len+sizeof(udp_hdr)+sizeof(udp_fakehdr)];
    ((udp_fakehdr*)(buf))->src = ip;
    ((udp_fakehdr*)(buf))->dst = dst;
    ((udp_fakehdr*)(buf))->UDPlen = htons(len + sizeof(udp_hdr));
    ((udp_fakehdr*)(buf))->UDPproto = UDP;
    ((udp_fakehdr*)(buf))->ZERO = 0;
    ((udp_hdr*)(buf+sizeof(udp_fakehdr)))->dest = htons(desport);
    ((udp_hdr*)(buf+sizeof(udp_fakehdr)))->source = htons(selfport);
    ((udp_hdr*)(buf+sizeof(udp_fakehdr)))->len = htons(len + sizeof(udp_hdr));
    ((udp_hdr*)(buf+sizeof(udp_fakehdr)))->check = 0;
    memcpy(buf + sizeof(udp_fakehdr) + sizeof(udp_hdr), data, len);
    ((udp_hdr*)(buf+sizeof(udp_fakehdr)))->check = checksum(buf, len + sizeof(udp_hdr) + sizeof(udp_fakehdr));

    ipmgr.udp_write(dst,buf+sizeof(udp_fakehdr), len + sizeof(udp_hdr));
}
