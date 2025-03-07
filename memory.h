#ifndef MEMORY_H
#define MEMORY_H

struct ROM
{
    static constexpr unsigned int MAX_ROM = 4096;
    unsigned char ROM[MAX_ROM];
    void Initialize();
    unsigned char& operator[](unsigned int);
};

struct RAM
{
    static constexpr unsigned int MAX_RAM = 256;
    unsigned char RAM[MAX_RAM];
    void Initialize();
    unsigned char& operator[](unsigned int);
};

#endif