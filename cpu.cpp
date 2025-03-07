#include <cstdio>
#include "cpu.h"

void CPU::Reset(RAM& ram, ROM& rom){
    PC = 0;
    SP = 0;
    A = B = C = D = E = F = G = 0;
    H = false;
    ram.Initialize();
    rom.Initialize();
}

unsigned char CPU::FetchByte (unsigned int& Cycles, ROM& memory)
{
    unsigned char Data = memory[PC];
    PC++;
    Cycles--;
    return Data;
}

unsigned char CPU::FetchByteRAM (unsigned int& Cycles, RAM& memory, unsigned char address)
{
    Cycles--;
    return memory[address];
}

void CPU::Execute (unsigned int Cycles, ROM& memory, RAM& ram)
{
    while (Cycles > 0)
    {
        unsigned char Ins = CPU::FetchByte(Cycles, memory);
        switch(Ins)
        {
            case INS_LOAD:
            {
                unsigned char value1 = CPU::FetchByte(Cycles, memory);
                unsigned char value2 = CPU::FetchByte(Cycles, memory);

                switch( ((value1 & 0b00111000) >> 3) )
                {
                    case 0:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            A = value2 & 0b00111111;
                        }
                        else
                        {
                            A = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 1:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            B = value2 & 0b00111111;
                        }
                        else
                        {
                            B = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 2:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            C = value2 & 0b00111111;
                        }
                        else
                        {
                            C = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 3:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            D = value2 & 0b00111111;
                        }
                        else
                        {
                            D = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 4:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            E = value2 & 0b00111111;
                        }
                        else
                        {
                            E = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 5:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            F = value2 & 0b00111111;
                        }
                        else
                        {
                            F = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 6:
                        if ((value1 & 0b00000100) >> 2 == 0)
                        {
                            G = value2 & 0b00111111;
                        }
                        else
                        {
                            G = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00111111;
                        }
                        break;

                    case 7:
                        if ((value1 & 0b00000100) >> 2== 0)
                        {
                            H = value2 & 0b00000001;
                        }
                        else
                        {
                            H = (CPU::FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2)) & 0b00000001;
                        }
                        break;
                    
                    default:
                            printf("instruction not handled %d\n", Ins);
                            break;
                }
                Cycles--;
            }

        }
    }
}