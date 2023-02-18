//
// Created by 张晰铭 on 2023/2/18.
//

#ifndef MYTCPIP_PROTOTYPE_H
#define MYTCPIP_PROTOTYPE_H
#include "stdio.h"

class Prototype;
class Father{
public:
    virtual void print();
};

class Son:public Father {
public:
    virtual void print(int a) override {
        printf("Son's function!");
    }

};

#endif //MYTCPIP_PROTOTYPE_H
