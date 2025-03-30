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
    Memory(unsigned int, bool);
    void Initialize(unsigned char);
    std::vector<uint8_t> Mem{};
    uint8_t& operator[](uint32_t);
    void LoadFile(char const*);
    bool type;

private:   
    uint32_t MAX_MEM;
};

struct System
{
    void Initialize();
    uint32_t Mem[2400];
    uint8_t& operator[](uint16_t);
    bool Pixel[2400];

    float PitchTable[64] = {0, 43.65, 46.25, 49, 51.91, 55, 58.27, 61.74,
        65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98, 103.83, 110, 116.54, 123.47,
        130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185, 196, 207.65, 220, 233.08, 246.94,
        261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.30, 440, 466.16, 493.88, 
        523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880, 932.33, 987.77,
        1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91 , 1479.98, 1567.98};
    void VideoApply(uint8_t, uint8_t, uint8_t);
    void VideoPixelOn(uint8_t, uint8_t);
    void VideoPixelOff(uint8_t, uint8_t);
    float GetSound(uint8_t);
    void Update();
    bool flag;
};

#endif