#ifndef CPU_H
#define CPU_H

#include "memory.h"

struct CPU
{
    unsigned short PC;
    unsigned char SP;

    unsigned char A, B, C, D, E, F, G;
    bool H;

    static constexpr unsigned char INS_LOAD = 0x06;

    void Reset(RAM&, ROM&);

    unsigned char FetchByte (unsigned int&, ROM&);

    unsigned char FetchByteRAM (unsigned int&, RAM&, unsigned char);

    void Execute (unsigned int, ROM&, RAM&);
    
};

#endif