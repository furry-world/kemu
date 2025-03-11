#include "cpu.h"

void CPU::Reset(Memory& ram, Memory& rom, System& system){
    PC = 0;
    SP = 0;

    ram.Initialize(0);
    rom.Initialize(63);
    system.Initialize();

    cyclesToExecute = 0;
}

uint8_t CPU::FetchByte (float& Cycles, Memory& memory)
{
    uint8_t Data = memory[PC];
    PC++;
    Cycles--;
    return Data;
}

uint8_t CPU::FetchByteRAM (float& Cycles, Memory& memory, uint8_t address)
{
    uint8_t Data = memory[address];
    Cycles--;
    return Data;
}

void CPU::StoreByteRAM (float& Cycles, Memory& memory, uint8_t address, uint8_t value)
{
    Cycles--;
    memory[address] = value;
}

void CPU::Execute (float Cycles, Memory& rom, Memory& ram, System& system, Platform& interface)
{
    cyclesToExecute += Cycles;

    while (cyclesToExecute > 0)
    {
        uint8_t Ins = CPU::FetchByte(cyclesToExecute, rom);

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

                if (SP < 15)
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
                        PC += WordLen[In]-1;
                        cyclesToExecute -= WordLen[In];
                    }
                }
                else
                {
                    if (Registers[(value1 & 0b00111000) >> 3] != CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2))
                    {
                        uint8_t In = CPU::FetchByte(cyclesToExecute, rom);
                        PC += WordLen[In]-1;
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

                if ((value1 & 0b00111000) >> 3 != 7)
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

                if ((value1 & 0b00111000) >> 3 != 7)
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
                    system.flag = 1;
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

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    system.flag = 1;
                } 

                cyclesToExecute--;
            } break;

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
                    PC += WordLen[In]-1;
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

                Registers[7] = Registers[(value1 & 0b00111000) >> 3] & 0b00100000;

                Registers[(value1 & 0b00111000) >> 3] = (Registers[(value1 & 0b00111000) >> 3] << 1) & 0b00111111;

                cyclesToExecute--;
            } break;

            case INS_SHIFTR:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);

                Registers[7] = Registers[(value1 & 0b00111000) >> 3] & 0b00000001;

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] >> 1;

                cyclesToExecute--;
            } break;

            case INS_ILOAD:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                if ((value1 & 0b00111000) >> 3 != 7)
                {
                    Registers[(value1 & 0b00111000) >> 3] = CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2 + Registers[3]);
                }
                else
                {
                    Registers[(value1 & 0b00111000) >> 3] = CPU::FetchByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2 + Registers[3]) & 0b00000001;
                }

                cyclesToExecute -= 2;
            } break;

            case INS_ISTORE:
            {
                uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                StoreByteRAM(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2 + Registers[3], Registers[(value1 & 0b00111000) >> 3]);

                cyclesToExecute -= 2;
            } break;

            default:
            {
                if ((Ins & 0b00111000) >> 3 == 4)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    Registers[Ins & 0b00000111] = CPU::FetchByteRAM(cyclesToExecute, rom, (uint16_t)(value1 + value2));

                    cyclesToExecute--;
                }
                else if ((Ins & 0b00111000) >> 3 == 5)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    rom[(uint16_t)(value1 + value2)] = Registers[Ins & 0b00000111];

                    cyclesToExecute--;
                }
                else if ((Ins & 0b00111000) >> 3 == 6)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    Registers[Ins & 0b00000111] = CPU::FetchByteRAM(cyclesToExecute, rom, (uint16_t)(value1 + value2 + Registers[3]));

                    cyclesToExecute -= 2;
                }
                else if ((Ins & 0b00111000) >> 3 == 7)
                {
                    uint8_t value1 = CPU::FetchByte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchByte(cyclesToExecute, rom);

                    rom[(uint16_t)(value1 + value2 + Registers[3])] = Registers[Ins & 0b00000111];

                    cyclesToExecute -= 2;
                }
                else
                {
                    printf("instruction not handled %u at PC address %u\n", Ins, PC);
                    exit(0);
                }
            }
        }

        if (system.flag == 1)
        {
            switch(Registers[6])
            {
                case 0:
                    system.VideoApplyPixel(Registers[4], Registers[5]);
                    break;
                case 1:
                    system.VideoApply(Registers[4], Registers[5], Registers[3]);
                    break;
                case 2:
                    interface.SetFreq(system.GetSound(Registers[5]));
                    break;
            }
            system.flag = 0;
        }
        
        ram[239] = 0;

        if (IsKeyDown(KEY_UP))
        {
            ram[239] = ram[239] | 0b00100000;
        }

        if (IsKeyDown(KEY_DOWN))
        {
            ram[239] = ram[239] | 0b00010000;
        }

        if (IsKeyDown(KEY_LEFT))
        {
            ram[239] = ram[239] | 0b00001000;
        }

        if (IsKeyDown(KEY_RIGHT))
        {
            ram[239] = ram[239] | 0b00000100;
        }

        if (IsKeyDown(KEY_COMMA))
        {
            ram[239] = ram[239] | 0b00000010;
        }

        if (IsKeyDown(KEY_PERIOD))
        {
            ram[239] = ram[239] | 0b00000001;
        }
    }
}
