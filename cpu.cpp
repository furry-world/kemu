#include "cpu.h"

void CPU::Reset(Memory& ram, Memory& rom, Video& video){
    PC = 0;
    SP = 0;

    ram.Initialize(0);
    rom.Initialize(63);
    video.Initialize();

    cyclesToExecute = 0;
}

uint8_t CPU::FetchByte (int& Cycles, Memory& memory)
{
    uint8_t Data = memory[PC];
    PC++;
    Cycles--;
    return Data;
}

uint8_t CPU::FetchByteRAM (int& Cycles, Memory& memory, uint8_t address)
{
    uint8_t Data = memory[address];
    Cycles--;
    return Data;
}

void CPU::StoreByteRAM (int& Cycles, Memory& memory, uint8_t address, uint8_t value)
{
    Cycles--;
    memory[address] = value;
}

void CPU::Execute (int Cycles, Memory& rom, Memory& ram, Video& video)
{
    cyclesToExecute += Cycles;

    while (cyclesToExecute > 0)
    {

        uint8_t Ins = CPU::FetchByte(cyclesToExecute, rom);
        // printf("Operation: %d\n", Ins);
        switch(Ins)
        {
            case INS_CALL:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);
                
                uint16_t Adsr = 0;
                Adsr += value1;
                Adsr = (Adsr << 6)  + value2;

                CPU::StoreByteRAM(cyclesToExecute, ram, 240 + SP, PC & 0b0000000000111111);
                CPU::StoreByteRAM(cyclesToExecute, ram, 240 + SP + 1, (PC & 0b0000111111000000) >> 6);

                if (SP < 14)
                {
                    SP++;    
                }
                else
                {
                    exit(0);
                }

                PC = (value1 << 6) + value2;

                cyclesToExecute--;

            } break;

            case INS_RET:
            {
                uint8_t Value1 = CPU::FetchByteRAM(cyclesToExecute, ram, 240 + SP);
                uint8_t Value2 = CPU::FetchByteRAM(cyclesToExecute, ram, 240 + SP + 1);

                PC = (Value1 << 6) + Value2;

                CPU::StoreByteRAM(cyclesToExecute, ram, 240 + SP, 0);
                CPU::StoreByteRAM(cyclesToExecute, ram, 240 + SP + 1, 0);

                if (SP > 0)
                {
                    SP--;    
                }
                else
                {
                    exit(0);
                }

                cyclesToExecute--;

            } break;

            case INS_JMP:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                uint16_t Adsr = 0;
                Adsr += value1;
                Adsr = (Adsr << 6)  + value2;

                PC = (value1 << 6) + value2;

                cyclesToExecute -= 3;
            } break;

            case INS_EQU:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    if (Registers[(value1 & 0b00111000) >> 3] == value2 & 0b00111111)
                    {
                        uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                        PC += WordLen[In]-1;
                        cyclesToExecute -= WordLen[In];
                    }
                }
                else
                {
                    if (Registers[(value1 & 0b00111000) >> 3] == CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2))
                    {
                        uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                        PC += WordLen[In]-1;
                        cyclesToExecute -= WordLen[In];
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_NEQU:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    if (Registers[(value1 & 0b00111000) >> 3] != value2 & 0b00111111 )
                    {
                        uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                        PC += WordLen[In];
                        cyclesToExecute -= WordLen[In];
                    }
                }
                else
                {
                    if (Registers[(value1 & 0b00111000) >> 3] != CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2))
                    {
                        uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                        PC += WordLen[In];
                        cyclesToExecute -= WordLen[In];
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_RJMP:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                PC += value1 & 0b11000000;

                cyclesToExecute--;
            } break;

            case INS_ADD:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                if (Registers[(value1 & 0b00111000) >> 3] != 7)
                {
                    Registers[(value1 & 0b00111000) >> 3] += Registers[(value1 & 0b00000111)];

                }
                else
                {
                    Registers[(value1 & 0b00111000) >> 3] += Registers[(value1 & 0b00000111)] & 0b00000001;
                }

                if (Registers[(value1 & 0b00111000) >> 3] > 0b00111111)
                {
                    Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] & 0b00111111;
                    Registers[7] = 1;
                }
                else
                {
                    Registers[7] = 0;
                }

                cyclesToExecute--;
            } break;

            case INS_SUB:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                if (Registers[(value1 & 0b00111000) >> 3] != 7)
                {
                    Registers[(value1 & 0b00111000) >> 3] -= Registers[(value1 & 0b00000111)];

                }
                else
                {
                    Registers[(value1 & 0b00111000) >> 3] -= Registers[(value1 & 0b00000111)] & 0b00000001;
                }

                if (Registers[(value1 & 0b00111000) >> 3] > 0b00111111)
                {
                    Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] & 0b00111111;
                    Registers[7] = 1;
                }
                else
                {
                    Registers[7] = 0;
                }

                cyclesToExecute--;
            } break;
            
            case INS_LOAD:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    if ((value1 & 0b00111000) >> 3 != 7)
                    {
                        Registers[(value1 & 0b00111000) >> 3] = value2 & 0b00111111;
                    }
                    else
                    {
                        Registers[(value1 & 0b00111000) >> 3] = value2 & 0b00000001;
                    }
                }
                else
                {
                    if ((value1 & 0b00111000) >> 3 != 7)
                    {
                        Registers[(value1 & 0b00111000) >> 3] = CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2);
                    }
                    else
                    {
                        Registers[(value1 & 0b00111000) >> 3] = CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2) & 0b00000001;
                    }
                }

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    video.flag = 1;
                } 

                cyclesToExecute--;
                break;
            }
            case INS_STORE:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                StoreByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2, Registers[(value1 & 0b00111000) >> 3]);

                cyclesToExecute--;
            } break;

            case INS_RLOAD:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[value1 & 0b00000111];

                if (Registers[(value1 & 0b00111000) >> 3] == 6)
                {
                    video.flag = 1;
                } 

                cyclesToExecute--;
            }

            case INS_REQU:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                if (Registers[(value1 & 0b00111000) >> 3] == Registers[value1 & 0b00000111])
                {
                    uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                    PC += WordLen[In]-1;
                    cyclesToExecute -= WordLen[In];
                }
                cyclesToExecute--;
            } break;

            case INS_NREQU:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                if (Registers[(value1 & 0b00111000) >> 3] != Registers[value1 & 0b00000111])
                {
                    uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                    PC += WordLen[In];
                    cyclesToExecute -= WordLen[In];
                }
                cyclesToExecute--;
            } break;

            case INS_OR:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] | Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_AND:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] & Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_XOR:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] ^ Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_SHIFTL:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] << Registers[7];

                cyclesToExecute--;
            } break;

            case INS_SHIFTR:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] >> Registers[7];

                cyclesToExecute--;
            } break;
            default:
            {
                if ((Ins & 0b00111000) >> 3 == 4)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    Registers[Ins & 0b00000111] = CPU::FetchByteRAM(cyclesToExecute, rom, (value1 >> 6) + value2);

                    cyclesToExecute--;
                }
                else if ((Ins & 0b00111000) >> 3 == 5)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    rom[(value1 >> 6) + value2] = Registers[Ins & 0b00000111];

                    cyclesToExecute -= 2;
                }
                else
                {
                    printf("instruction not handled %u at PC address %u\n", Ins, PC);
                    exit(0);
                }
            }
        }

        if (video.flag == 1)
        {
            video.Apply(Registers[4], Registers[5], Registers[3]);
            video.flag = 0;
        }

        printf("A: %d | B: %d | C: %d | D: %d | E: %d | F: %d | G: %d | H: %d\n", Registers[0], Registers[1], Registers[2], Registers[3], Registers[4], Registers[5], Registers[6], Registers[7]);
    }
}