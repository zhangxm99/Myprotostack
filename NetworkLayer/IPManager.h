//
// Created by 张晰铭 on 2023/2/14.
//

#ifndef MYTCPIP_IPMANAGER_H
#define MYTCPIP_IPMANAGER_H

#define TIMEUP 300


#include "IPDevice.h"
#include "ICMPManager.h"
#include <unordered_map>
#include <algorithm>
#include <netinet/in.h>

struct pseudohdr{
    uint32_t src;
    uint32_t dst;
    uint8_t ZERO;
    uint8_t proto;
    uint16_t len;
    uint8_t data[];
} __attribute__((packed));

struct udp_hdr
{
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
    uint8_t data[];
}__attribute__((packed));

struct tcp_hdr {
    uint16_t sport;
    uint16_t dport;
    uint32_t seq;
    uint32_t ack;
    uint8_t rsvd : 4;
    uint8_t hl : 4;
    uint8_t flags;
    uint16_t win;
    uint16_t csum;
    uint16_t urp;
    uint8_t data[];
} __attribute__((packed));

class SplitIPManager{
private:
    vector<DataView<ip_hdr,sizeof(eth_hdr)>> IPlst{};
    time_t start_time;
public:
    SplitIPManager():start_time(time(nullptr)){}
    /*!
     * if insert tail packet then return true,else still wait
     * @param ipdata
     * @return 0 means you should use get() to retrieve the data ,-1 means wait, -2 means time up ,you should destruct this obj
     */
    int32_t insert(DataView<ip_hdr,sizeof(eth_hdr)> &ipdata){
        auto ipHdr = ipdata.hdr;
        IPlst.emplace_back(ipdata);
        if((*(((uint8_t*)ipHdr+6))) & 0x20){
            /* not the last packet*/
            if(time(nullptr) - start_time > TIMEUP){
                return -2;
            }
            return -1;
        } else{
            std::sort(IPlst.begin(),IPlst.end(),[](auto &a,auto &b){
                auto offset_a =  ntohs(*((uint16_t*)a.hdr+3)) & 0x1fff;
                auto offset_b =  ntohs(*((uint16_t*)b.hdr+3)) & 0x1fff;
                return offset_a < offset_b;
            });
            for(int32_t i = 1,pre = -1;i < IPlst.size();i++){
                auto offset_a =  ntohs(*((uint16_t*)IPlst[i].hdr+3)) & 0x1fff;
                auto offset_b =  ntohs(*((uint16_t*)IPlst[i-1].hdr+3)) & 0x1fff;
                if(pre != -1){
                    if(offset_b - offset_a != pre){
                        return -2;
                    }
                } else{
                    pre = offset_b - offset_a;
                }
            }
            if((ntohs(*((uint16_t*)IPlst[0].hdr+3)) & 0x1fff) != 0) return -2;

            return 0;
        }
    }

    DataView<ip_hdr,sizeof(eth_hdr)> get(){
        uint32_t totalLen = (ntohs(*((uint16_t*)IPlst.back().hdr+3)) & 0x1fff)*8 + IPlst.back().hdr->len;
        DataView<ip_hdr,sizeof(eth_hdr)> IPres(new uint8_t[totalLen + sizeof(eth_hdr)]);
        memcpy(IPres.pData.get(),IPlst[0].pData.get(),IPlst[0].hdr->len + sizeof(eth_hdr));
        IPres.hdr->len = IPlst[0].hdr->len;
        for(int32_t i = 1;i < IPlst.size();i++){
            auto dataLen = IPlst[i].hdr->len - 4*IPlst[i].hdr->ihl;
            memcpy((uint8_t*)IPres.hdr + IPres.hdr->len,IPlst[i].hdr->data,dataLen);
            IPres.hdr->len += dataLen;
        }
        return IPres;
    }

};

class IPManager {
private:
    IPDevice dev;
    ICMPManager icmpMgr;

    queue<DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)>> UDPq;
    queue<DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)>> TCPq;

    unordered_map<uint16_t,SplitIPManager> splitMp;

    [[noreturn]] void readLoop();
    uint16_t checksum(void *addr, int count);
    void insertQ(queue<DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)>>&,
                    DataView<ip_hdr,sizeof(eth_hdr)> ,uint32_t );
public:
    uint32_t myIP;
    IPManager(char *,char *);

    DataView<pseudohdr, sizeof(eth_hdr) + sizeof(ip_hdr) - sizeof(pseudohdr)> read(uint32_t protoType);
    int write(uint32_t ip,uint8_t proto, uint8_t *ipData, int len);


};


#endif //MYTCPIP_IPMANAGER_H
