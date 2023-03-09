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
    shared_ptr<uint8_t> pData;
    T *hdr;
    explicit DataView(uint8_t *_pData):pData(_pData),hdr((T*)(pData.get()+skip)) {}
    explicit DataView(shared_ptr<uint8_t> &_p):pData(_p),hdr((T*)(pData.get()+skip)) {}
    explicit DataView(shared_ptr<uint8_t > &&_pData):pData(move(_pData)),hdr((T*)(pData.get()+skip)){}

    template<typename U,int s>
    explicit operator DataView<U,s>(){
        return DataView<U,s>(pData);
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

    int tun_read(uint8_t*,int);
    int tun_write(uint8_t*,int);



};

struct netdev{
    uint32_t addr; //IP addr
    uint8_t hwaddr[6]; //MAC addr
};


#endif //MYTCPIP_TUNTAP_H
