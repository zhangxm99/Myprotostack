//
// Created by 张晰铭 on 2023/2/10.
//

#include "ICMPManager.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

ICMPManager::ICMPManager(char* _ip,IPDevice &_dev):dev(_dev) {
    inet_pton(AF_INET,_ip,&ip);
}


void ICMPManager::icmpIncoming(ip_hdr *ipHdr) {
    auto icmpHdr = (icmp_v4 *)ipHdr->data;
    int len{};
    switch (icmpHdr->type) {
        case ICMP_V4_ECHO:
            icmpHdr->type = ICMP_V4_REPLY;
            icmpHdr->csum = 0;
            len = ipHdr->len - (ipHdr->ihl * 4);
            icmpHdr->csum = checksum(icmpHdr, len);
            if(dev.transmit(ipHdr->saddr,ICMPV4,0,(char*)icmpHdr,len) == -1) perror("ICMP reply failed\n");
            break;
        default:
            printf("Unimplemented ICMP request\n");
    }
}

uint16_t ICMPManager::checksum(icmp_v4 *addr, uint16_t count) {
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
