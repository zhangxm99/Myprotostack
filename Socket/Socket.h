//
// Created by 张晰铭 on 2023/2/18.
//

#ifndef MYTCPIP_SOCKET_H
#define MYTCPIP_SOCKET_H
#include "../NetworkLayer/IPManager.h"
#include "../TransportLayer/UDPManager.h"

enum SOCK_UDP{};
enum SOCK_TCP{};

template<typename T>
class Socket ;

template<>
class Socket<SOCK_UDP>{
private:
    UDPManager& udpManager;
    uint16_t selfport;
public:
    Socket(UDPManager &_mgr): udpManager(_mgr),selfport(udpManager.applyPort()){}

    int sendto(uint8_t *data,int len,uint32_t destip,uint16_t destport){
        return udpManager.writeData(destip,selfport,destport,data,len);
    }
    /*!
     * @param buf receive buffer addr
     * @param len receive buffer length
     * @return sender ip addr(network byte order) and port
     */
    pair<uint32_t ,uint16_t> recv_from(uint8_t *buf,int len){
        auto [ip,port,content,sz] = udpManager.getData(selfport);
        memcpy(buf,content.hdr,sz);
        return {ip,port};
    }
    void bind(uint16_t port){
        if(udpManager.usePort(port) == -1) exit(1);
        selfport = port;
    }
};


#endif //MYTCPIP_SOCKET_H
