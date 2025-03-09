#ifndef MEMORY_H
#define MEMORY_H

#pragma once

#include <cstdint>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>


struct Memory
{
    Memory(unsigned int);
    void Initialize(unsigned char);
    std::vector<uint8_t> Mem{};
    uint8_t& operator[](uint32_t);
    void LoadFile(char const*);

private:   
    uint32_t MAX_MEM;
};

struct Video
{
    void Initialize();
    uint32_t Mem[2400];
    uint8_t& operator[](uint16_t);
    bool Pixel[2400];
    void Apply(uint8_t, uint8_t, uint8_t);
    void Update();
    bool flag;
};

#endif