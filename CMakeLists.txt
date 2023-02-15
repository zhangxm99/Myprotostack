cmake_minimum_required(VERSION 3.21)
project(Mytcpip)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(IF IF)
aux_source_directory(LinkLayer LK)
aux_source_directory(NetworkLayer NW)
aux_source_directory(TransportLayer TP)

add_executable(Mytcpip main.cpp ${IF} ${LK} ${NW} ${TP})