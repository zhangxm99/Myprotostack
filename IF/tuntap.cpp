//
// Created by 张晰铭 on 2023/2/2.
//

#include "tuntap.h"
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <sys/ioctl.h>

Tuntap::Tuntap() {
    struct ifreq ifr{};

    int err{};

    if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
        fputs("Can't open tuntap device\n",stderr);
        exit(1);
    }

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
        close(fd);
        fputs("Can't open tuntap device, are you a root user?\n",stderr);
        exit(1);
    }

    set_if_up();
    set_if_route();

    printf("virtual interface name is tap0,ip address is 10.0.0.0/24\n");

}

Tuntap::~Tuntap() {
    close(fd);
}

int Tuntap::tun_read(char *buf, int len) {
    return read(fd,buf,len);
}

int Tuntap::tun_write(char *buf, int len) {
    return write(fd,buf,len);
}

inline void Tuntap::set_if_up() {
    if(system("ip link set dev tap0 up") != 0){
        fputs("Can't set if up\n",stderr);
        exit(1);
    }
}

inline void Tuntap::set_if_route() {
    if(system("ip route add dev tap0 10.0.0.0/24") != 0){
        fputs("Can't set route for tap0\n",stderr);
        exit(1);
    }
}


