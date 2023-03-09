cmake_minimum_required(VERSION 3.21)
project(Myprotostack)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(IF IF)
aux_source_directory(LinkLayer LK)
aux_source_directory(NetworkLayer NW)
aux_source_directory(TransportLayer TP)

add_executable(Myprotostack main.cpp ${IF} ${LK} ${NW} TransportLayer/UDPManager.cpp Socket/Socket.h)