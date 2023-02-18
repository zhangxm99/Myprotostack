//#include <iostream>
//#include "IF/tuntap.h"
//#include "LinkLayer/Netdevice.h"
#include <unistd.h>
#include "LinkLayer/arp.h"
#include "LinkLayer/EtherManager.h"
#include "NetworkLayer/IPManager.h"
#include "TransportLayer/UDPManager.h"
#include "Socket/Socket.h"

#include <queue>
#include <memory>
#include <netinet/in.h>

#include "thread"
using namespace std;



int main() {
    char *ip = "10.0.0.4";
    char *MAC = "7e:1a:6d:c7:c8:f9";
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


//    UDPManager mgr(<#initializer#>);
//    while(1) {
//        auto[sip, port, dtview,sz] = mgr.getData(100);
//        mgr.writeData(sip, 100, port, dtview.hdr, sz);
//    }
    IPManager ipmgr(ip,MAC);
    Socket<SOCK_UDP> socket(ipmgr);
    socket.bind(100);
    while(1) {
        char buf[100];
        auto [ip,port] = socket.recv_from((uint8_t*)buf,100);
        printf("client say: %s\n",buf);
        scanf("%s",buf);

        socket.sendto((uint8_t*)buf, strlen(buf)+1,ip,port);
    }
























    sleep(2112);






    return 0;
}
