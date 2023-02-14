cmake_minimum_required(VERSION 3.21)
project(Mytcpip)

set(CMAKE_CXX_STANDARD 14)

add_executable(Mytcpip main.cpp IF/tuntap.cpp LinkLayer/arp.cpp LinkLayer/Netdevice.cpp LinkLayer/EtherManager.cpp LinkLayer/EtherManager.h NetworkLayer/IPDevice.cpp NetworkLayer/IPDevice.h NetworkLayer/ipv4_hdr.h NetworkLayer/ICMPManager.cpp NetworkLayer/ICMPManager.h NetworkLayer/IPManager.cpp NetworkLayer/IPManager.h)
