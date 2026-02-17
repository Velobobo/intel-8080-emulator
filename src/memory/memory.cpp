#include "memory/memory.h"
#include <cstring>

u8 Memory::read(u16 addr){
    return data[addr];
};

void Memory::write(u16 addr,u8 value){
    data[addr]=value;
};

void Memory::reset(){
    std::memset(data,0,sizeof(data));
};