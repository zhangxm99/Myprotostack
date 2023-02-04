cmake_minimum_required(VERSION 3.21)
project(Mytcpip)

set(CMAKE_CXX_STANDARD 17)

add_executable(Mytcpip main.cpp LinkLayer/if_ether.h IF/tuntap.cpp IF/tuntap.h LinkLayer/arp.cpp LinkLayer/arp.h LinkLayer/netdevice.cpp LinkLayer/netdevice.h)
