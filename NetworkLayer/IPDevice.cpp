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
        sum += * (uint8_t *) ptr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

IPDevice::IPDevice(char *_ip, char *MAC): ethMgr(_ip, MAC) {
    inet_pton(AF_INET,_ip,&ip);
}

int IPDevice::transmit(uint32_t addr, uint8_t proto, uint8_t tos, char *payload, uint16_t len) {
    write_lock.lock();

    auto ipHdr = (ip_hdr*)&writeBuf[0];
    uint16_t accumulateLen = 0;
    /* In ip split scene, frag_offset field is multiples of 8, so need round up*/
    auto origMaxDataLen = ETHERMTU - sizeof(eth_hdr) - sizeof(ip_hdr);
    auto roundMaxLen = (origMaxDataLen/8) * 8;

    while(len){
        ipHdr->tos = tos;
        ipHdr->proto = proto;
        ipHdr->daddr = addr;
        ipHdr->version = 4;
        ipHdr->ihl = 5;
        ipHdr->saddr = ip;
        ipHdr->ttl = 64;
        ipHdr->id = htons(id);

        if(len <= origMaxDataLen){
            ipHdr->len = htons(len + ipHdr->ihl*4);
            *((uint16_t*)ipHdr+3) = 0;
            if(accumulateLen == 0){
                *((uint8_t*)ipHdr+6)  = 0x40;
            } else{
                *((uint16_t*)ipHdr+3) = htons(accumulateLen/8);
            }
            len = 0;
        } else{
            ipHdr->len = htons(roundMaxLen + ipHdr->ihl*4);
            *((uint16_t*)ipHdr+3) = htons(accumulateLen/8);
            *((uint8_t*)ipHdr+6) |= 0x20;
            len -= roundMaxLen;
        }
        ipHdr->csum = 0;
        ipHdr->csum = checksum(ipHdr,ipHdr->ihl*4);
        memcpy(ipHdr->data,payload+accumulateLen,ntohs(ipHdr->len)-ipHdr->ihl*4);
        ethMgr.ip_write(addr,(ip_hdr*)&writeBuf[0]);
        accumulateLen += roundMaxLen;
    }

    id = (id+1) % UINT16_MAX;
    write_lock.unlock();
}

DataView<ip_hdr,sizeof(eth_hdr)> IPDevice::receive() {
    return ethMgr.ip_read();
}
