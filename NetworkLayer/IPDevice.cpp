//
// Created by 张晰铭 on 2023/2/8.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include "IPDevice.h"
uint16_t IPDevice::checksum(void *addr, int count)
{
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

IPDevice::IPDevice(char *_ip, char *MAC): ethMgr(_ip, MAC) {
    inet_pton(AF_INET,_ip,&ip);
}

int IPDevice::transmit(uint32_t addr, uint8_t proto, uint8_t tos, char *payload, int len) {
    write_lock.lock();
    auto ipHdr = (ip_hdr*)&writeBuf[0];

    ipHdr->tos = tos;
    ipHdr->proto = proto;
    ipHdr->daddr = addr;
    ipHdr->version = 4;
    ipHdr->ihl = 5;
    ipHdr->saddr = ip;
    ipHdr->ttl = 64;
    ipHdr->id = 0;
    /*
     * because of the different implement of bit field order by compiler,
     * here using pointer method set flag = 0b010, frag_offset = 0,
     * which means no slicing the IP packet
     */
    *(uint16_t*)((uint8_t *)ipHdr+5) = 0x4000;

    ipHdr->len = htons(len + ipHdr->ihl*4);
    ipHdr->csum = 0;
    ipHdr->csum = checksum(ipHdr,ipHdr->ihl*4);

    memcpy(((ip_hdr*)&writeBuf[0])->data,payload,len);
    int res = ethMgr.ip_write(addr,(ip_hdr*)&writeBuf[0]);

    write_lock.unlock();
    return res;
}

DataView<ip_hdr,sizeof(eth_hdr)> IPDevice::receive() {
    return ethMgr.ip_read();
}
