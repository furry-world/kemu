#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>


struct Memory
{
    Memory(unsigned int);
    void Initialize(unsigned char);
    std::vector<uint8_t> Mem;
    uint8_t& operator[](uint32_t);
    void LoadFile(char const*);

private:   
    uint32_t MAX_MEM;
};

#endif