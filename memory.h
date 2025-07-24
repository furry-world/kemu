#pragma once

#include <cstdint>
#include <vector>
#include <fstream>


struct Memory
{
    Memory(unsigned int, bool);
    void Initialize(uint8_t);
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

    float PitchTable[64] = {0, 13745, 12973, 12245, 11558, 10909, 10297, 9718, 9173, 8658, 8172, 7714, 7280, 6872, 6486, 6122, 5778, 5454, 5148, 4859, 4587, 4329, 4086, 3857, 3640, 3436, 3243, 3061, 2889, 2727, 2574, 2429, 2293, 2164, 2043, 1928, 1820, 1718, 1621, 1530, 1444, 1363, 1287, 1215, 1146, 1082, 1021, 964, 910, 859, 811, 765, 722, 682, 643, 607, 573, 541, 511, 482, 455, 429, 405, 0};
    void VideoApply(uint8_t, uint8_t, uint8_t);
    void VideoPixelOn(uint8_t, uint8_t);
    void VideoPixelOff(uint8_t, uint8_t);
    float GetSound(uint8_t);
    void Update();
    bool flag;
};
