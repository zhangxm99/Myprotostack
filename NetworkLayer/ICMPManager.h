//
// Created by 张晰铭 on 2023/2/10.
//

#ifndef MYTCPIP_ICMPMANAGER_H
#define MYTCPIP_ICMPMANAGER_H
#include <cstdint>
#include "ipv4_hdr.h"
#include "IPDevice.h"

#define ICMP_V4_REPLY           0x00
#define ICMP_V4_DST_UNREACHABLE 0x03
#define ICMP_V4_SRC_QUENCH      0x04
#define ICMP_V4_REDIRECT        0x05
#define ICMP_V4_ECHO            0x08
#define ICMP_V4_ROUTER_ADV      0x09
#define ICMP_V4_ROUTER_SOL      0x0a
#define ICMP_V4_TIMEOUT         0x0b
#define ICMP_V4_MALFORMED       0x0c

struct icmp_v4 {
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint8_t data[];
} __attribute__((packed));

struct icmp_v4_echo {
    uint16_t id;
    uint16_t seq;
    uint8_t data[];
} __attribute__((packed));

struct icmp_v4_dst_unreachable {
    uint8_t unused;
    uint8_t len;
    uint16_t var;
    uint8_t data[];
} __attribute__((packed));
class ICMPManager {
private:
    uint32_t ip;
    IPDevice &dev;



public:
    ICMPManager(char* ,IPDevice&);
    void icmpIncoming(ip_hdr *ipHdr);


    uint16_t checksum(icmp_v4 *pV4, uint16_t len);

};


#endif //MYTCPIP_ICMPMANAGER_H
