#include "memory.h"

void ROM::Initialize()
{
    for (unsigned int i = 0; i < MAX_ROM; i++)
    {
        ROM[i] = 0;
    }
}

unsigned char& ROM::operator[](unsigned int Address)
{
    return ROM[Address];
}

void RAM::Initialize()
{
    for (unsigned int i = 0; i < MAX_RAM; i++)
    {
        RAM[i] = 0;
    }
}

unsigned char& RAM::operator[](unsigned int Address)
{
    return RAM[Address];
}