//
// Created by 张晰铭 on 2023/2/2.
//

#ifndef MYTCPIP_TUNTAP_H
#define MYTCPIP_TUNTAP_H

#include "string"
#include <memory>
using namespace std;

template<typename T,int skip>
struct DataView{
    shared_ptr<unsigned char> pData;
    T *hdr;
    explicit DataView(unsigned char *_pData):pData(_pData),hdr((T*)(pData.get()+skip)) {}
    explicit DataView(shared_ptr<unsigned char > &&_pData):pData(move(_pData)),hdr((T*)(pData.get()+skip)){}

    template<typename U,int s>
    explicit operator DataView<U,s>(){
        return DataView<U,s>(move(pData));
    }
};


class Tuntap {
private:
    int fd;
    void set_if_route();
    void set_if_up();
    void set_if_addr();


public:
    Tuntap();
    ~Tuntap();

    int tun_read(unsigned char*,int);
    int tun_write(unsigned char*,int);


};

struct netdev{
    uint32_t addr; //IP addr
    unsigned char hwaddr[6]; //MAC addr
};


#endif //MYTCPIP_TUNTAP_H
