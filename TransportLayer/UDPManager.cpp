//
// Created by 张晰铭 on 2023/2/15.
//

#include <csignal>
#include <netinet/in.h>
#include "UDPManager.h"


UDPStruct UDPManager::getData(uint16_t localport) {
    while(portMp[localport].empty()) usleep(100);
    auto res = portMp[localport].front();
    portMp[localport].pop();
    return res;
}

UDPManager::UDPManager(IPManager &_ipmgr) : ipmgr(_ipmgr),ip(_ipmgr.myIP) {
    for(int i = 1;i < 50;i++) unusedPort.emplace(i);
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
        sum += * (uint8_t *) ptr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

void UDPManager::readLoop() {
    while (true){
        auto UDPcontent = ipmgr.read(UDP);
        auto source = UDPcontent.hdr->src;
        auto srcport = ntohs(((udp_hdr*)(UDPcontent.hdr->data))->source);
        auto destport = ntohs(((udp_hdr*)(UDPcontent.hdr->data))->dest);
        auto len = ntohs(UDPcontent.hdr->len);

        //check
        if(checksum(UDPcontent.hdr,len + sizeof(pseudohdr)) != 0) continue;

        portMp[destport].emplace(UDPStruct {
            .ip = source,
            .conterport = srcport,
            .datagram = DataView<uint8_t,sizeof(eth_hdr)+sizeof(ip_hdr)+sizeof(udp_hdr)>(UDPcontent),
            .sz = (uint16_t)(len - sizeof(udp_hdr)),
        });


    }
}

uint16_t UDPManager::applyPort() {
    auto res = *unusedPort.begin();
    unusedPort.erase(unusedPort.begin());
    return res;
}

int16_t UDPManager::usePort(uint16_t port) {
    if(unusedPort.count(port) == 1){
        unusedPort.erase(unusedPort.find(port));
        return 1;
    } else{
        return -1;
    }
}

void UDPManager::freePort(uint16_t port) {
    unusedPort.emplace(port);
}

int UDPManager::writeData(uint32_t dst, uint16_t selfport,uint16_t desport,uint8_t *data,int len) {
    uint8_t buf[len+sizeof(udp_hdr)+sizeof(pseudohdr)];
    ((pseudohdr*)(buf))->src = ip;
    ((pseudohdr*)(buf))->dst = dst;
    ((pseudohdr*)(buf))->len = htons(len + sizeof(udp_hdr));
    ((pseudohdr*)(buf))->proto = UDP;
    ((pseudohdr*)(buf))->ZERO = 0;
    ((udp_hdr*)(buf+sizeof(pseudohdr)))->dest = htons(desport);
    ((udp_hdr*)(buf+sizeof(pseudohdr)))->source = htons(selfport);
    ((udp_hdr*)(buf+sizeof(pseudohdr)))->len = htons(len + sizeof(udp_hdr));
    ((udp_hdr*)(buf+sizeof(pseudohdr)))->check = 0;
    memcpy(buf + sizeof(pseudohdr) + sizeof(udp_hdr), data, len);
    ((udp_hdr*)(buf+sizeof(pseudohdr)))->check = checksum(buf, len + sizeof(udp_hdr) + sizeof(pseudohdr));

    ipmgr.write(dst,UDP, buf + sizeof(pseudohdr), len + sizeof(udp_hdr));
}
