#include "NetworkLayer/IPManager.h"
#include "Socket/Socket.h"
#include "TransportLayer/UDPManager.h"

#include <netinet/in.h>

using namespace std;



int main() {
    /*--------------------------------------*/
    char *ip = "10.0.0.4";
    char *MAC = "7e:1a:6d:c7:c8:f9";

    IPManager ipmgr(ip,MAC);
    UDPManager udpmgr(ipmgr);
    /*--------------------------------------*/
    Socket<SOCK_UDP> socket(udpmgr);
    socket.bind(1);
    while(1) {
        char buf[100];
        auto [ip,port] = socket.recv_from((uint8_t*)buf,100);
        printf("client say: %s\n",buf);
        scanf("%s",buf);

        socket.sendto((uint8_t*)buf, strlen(buf)+1,ip,port);
    }


    return 0;
}
