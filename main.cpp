#include <iostream>
#include "IF/tuntap.h"
#include "LinkLayer/Netdevice.h"
#include <unistd.h>
#include <algorithm>
#include <arpa/inet.h>
#include "LinkLayer/arp.h"
using namespace std;



int main() {
    char *ip = "10.0.0.4";
    char *MAC = "7e:1a:6d:c7:c8:f8";

    Netdevice dev(ip,MAC);
    ArpManager arpmgr(ref(dev));

    while(1){
        auto buf = dev.receive();
        auto ethHdr = (eth_hdr*)buf;
        printf("%x ",ntohs(ethHdr->ethertype));
        if(ntohs(ethHdr->ethertype) == ETH_P_ARP){
            arpmgr.arpIncoming(ethHdr);
            printf("ARP RECEIVED\n");
        } else{
            printf("unknown type\n");
        }
    }

























    sleep(2112);






    return 0;
}
