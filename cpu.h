#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "platform.h"

struct CPU
{
    uint16_t PC;
    uint8_t SP;

    uint8_t Registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t WordLen[20] = {3, 3, 2, 1, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3};

    static constexpr uint8_t INS_CALL = 0x00;
    static constexpr uint8_t INS_JMP = 0x01;
    static constexpr uint8_t INS_RJMP = 0x02;
    static constexpr uint8_t INS_RET = 0x03;
    static constexpr uint8_t INS_EQU = 0x04;
    static constexpr uint8_t INS_NEQU = 0x05;
    static constexpr uint8_t INS_LOAD = 0x06;
    static constexpr uint8_t INS_STORE = 0x07;
    static constexpr uint8_t INS_REQU = 0x08;
    static constexpr uint8_t INS_NREQU = 0x09;
    static constexpr uint8_t INS_RLOAD = 0x0A;
    static constexpr uint8_t INS_ADD = 0X0B;
    static constexpr uint8_t INS_SUB = 0x0C;
    static constexpr uint8_t INS_OR = 0x0D;
    static constexpr uint8_t INS_AND = 0x0E;
    static constexpr uint8_t INS_XOR = 0x0F;
    static constexpr uint8_t INS_SHIFTL = 0x10;
    static constexpr uint8_t INS_SHIFTR = 0x11;
    static constexpr uint8_t INS_ILOAD = 0x12;
    static constexpr uint8_t INS_ISTORE = 0x13;
    static constexpr uint8_t INS_IPLOAD= 0x14;
    static constexpr uint8_t INS_IPSTORE = 0x15;

    float cyclesToExecute;

    void Reset(Memory&, Memory&, System&);

    uint8_t FetchByte (float&, Memory&);

    uint8_t FetchByteRAM (float&, Memory&, uint8_t);

    void StoreByteRAM (float&, Memory&, uint8_t, uint8_t);

    void Execute (float, Memory&, Memory&, System&, Platform&);
    
};

#endif