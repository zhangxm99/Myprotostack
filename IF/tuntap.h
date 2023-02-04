//
// Created by 张晰铭 on 2023/2/2.
//

#ifndef MYTCPIP_TUNTAP_H
#define MYTCPIP_TUNTAP_H

#include "string"
using namespace std;


class Tuntap {
private:
    int fd;
    void set_if_route();
    void set_if_up();


public:
    Tuntap();
    ~Tuntap();

    int tun_read(char*,int);
    int tun_write(char*,int);


};

struct netdev{
    uint32_t addr; //IP addr
    unsigned char hwaddr[6]; //MAC addr
};


#endif //MYTCPIP_TUNTAP_H
