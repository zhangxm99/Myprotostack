#include <iostream>
#include "IF/tuntap.h"
#include "LinkLayer/Netdevice.h"
#include <unistd.h>
#include <arpa/inet.h>
#include "LinkLayer/arp.h"
#include "LinkLayer/EtherManager.h"
#include "NetworkLayer/IPManager.h"

#include <queue>

#include "thread"
using namespace std;

struct T{
    uint16_t a:3;
    uint16_t b:13;
}__attribute__((packed));;


int main() {
    char *ip = "10.0.0.4";
    char *MAC = "7e:1a:6d:c7:c8:f9";

//    Netdevice dev(ip,MAC);
//    ArpManager arpmgr(dev);
//
//    eth_hdr *recv = nullptr;
//
//    thread read = thread([&](){while(1) {recv = dev.receive();
//        printf("%x\n",ntohs(recv->ethertype));
//        }});
//
//    read.detach();
//
//    while(1){
//        printf("%d\n",dev.transmit({0,0,0,0,0,0},ETH_P_ARP,100));
//
//    }
//    memset((uint8_t *) &t,0x40,1);
//    memset((uint8_t *) &t+1,0x40,1);

//    IPDevice dev(ip,MAC);
//    ICMPManager mgr(ip,dev);
//
//
//
//    while(1){
//        auto readBuf = dev.receive();
////        printf("%x ",ntohs(readBuf->));
//
//        if(readBuf->proto == ICMPV4){
//            readBuf->len = ntohs(readBuf->len);
//            mgr.icmpIncoming(readBuf);
//        } else{
//            printf("unknown type\n");
//        }
//    }

    IPManager mgr(ip,MAC);
























    sleep(2112);






    return 0;
}
