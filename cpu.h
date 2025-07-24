#pragma once

#include "memory.h"
#include "platform.h"
#include "ctime"
#include <string>
#include <format>

struct CPU
{
    uint16_t PC;
    uint8_t SP;

    float counter = 0;
    bool Executed = false;

    float startCycles;

    uint8_t Registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t WordLen[22] = {3, 3, 2, 1, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3};

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
    static constexpr uint8_t INS_NPLOAD = 0x14;
    static constexpr uint8_t INS_NPSTORE = 0x15;

    uint8_t Note;

    uint8_t NewNote;

    bool HiLo;

    unsigned int samplesGenerated;

    float cyclesToExecute;

    void Reset(Memory&, Memory&, System&, Platform&);

    uint8_t FetchOpcodeHyte (float&, Memory&);

    uint8_t FetchHyte (float&, Memory&, uint16_t);

    void StoreHyte (float&, Memory&, uint16_t, uint8_t);

    int Execute (float, Memory&, Memory&, System&, Platform&, bool Single);

    std::string Disassemble (int InstructionCount, Memory&);

    std::string NoteToString ();

};
