#include <cstdio>
#include <cstdlib>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct ROM
{
    static constexpr int MAX_ROM = 4096;
    Byte ROM[MAX_ROM];

    void Initialize()
    {
        for (int i = 0; i < MAX_ROM; i++)
        {
            ROM[i] = 0;
        }
    }

    Byte& operator[]( u32 Address)
    {
        return ROM[Address];
    }
};

struct RAM
{
    static constexpr int MAX_RAM = 256;
    Byte RAM[MAX_RAM];

    void Initialize()
    {
        for (int i = 0; i < MAX_RAM; i++)
        {
            RAM[i] = 0;
        }
    }

    Byte& operator[]( u32 Address)
    {
        return RAM[Address];
    }
};

struct CPU{

    Word PC;
    Byte SP;

    Byte A, B, C, D, E, F, G;
    bool H;

    void Reset(RAM& ram, ROM& rom){
        PC = 0;
        SP = 0;
        A = B = C = D = E = F = G = 0;
        H = false;
        ram.Initialize();
        rom.Initialize();
    }

    static constexpr Byte INS_LOAD = 0x06;

    Byte FetchByte (u32& Cycles, ROM& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Byte FetchByteRAM (u32& Cycles, RAM& memory, Byte address)
    {
        Cycles--;
        return memory[address];
    }

    void Execute (u32 Cycles, ROM& memory, RAM& ram)
    {
        while (Cycles > 0)
        {
            Byte Ins = FetchByte(Cycles, memory);
            switch(Ins)
            {
                case INS_LOAD:
                {
                    Byte value1 = FetchByte(Cycles, memory);
                    Byte value2 = FetchByte(Cycles, memory);

                    switch( ((value1 & 0b00111000) >> 3) )
                    {
                        case 0:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                A = value2;
                            }
                            else
                            {
                                A = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 1:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                B = value2;
                            }
                            else
                            {
                                B = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 2:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                C = value2;
                            }
                            else
                            {
                                C = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 3:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                D = value2;
                            }
                            else
                            {
                                D = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 4:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                E = value2;
                            }
                            else
                            {
                                E = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 5:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                F = value2;
                            }
                            else
                            {
                                F = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 6:
                            if ((value1 & 0b00000100) >> 2 == 0)
                            {
                                G = value2;
                            }
                            else
                            {
                                G = FetchByteRAM(Cycles, ram, ((value1 & 0b00000011) << 6) + value2);
                            }
                            break;

                        case 7:
                            if ((value1 & 0b00000100) >> 2== 0)
                            {
                                H = value2 & 0b00000001;
                            }
                            else
                            {
                                H = FetchByteRAM(Cycles, ram, (((value1 & 0b00000011) << 6) + value2) & 0b00000001);
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
};

int main() {
    RAM ram;
    CPU cpu;
    ROM rom;
    cpu.Reset(ram, rom);
    // PRG Start
    ram[0x0F] = 0b00001111;
    rom[0x000] = cpu.INS_LOAD;
    rom[0x001] = 0b00000100;
    rom[0x002] = 0b00001111;
    rom[0x003] = cpu.INS_LOAD;
    rom[0x004] = 0b00001000;
    rom[0x005] = 0b00111111;
    // PRG End
    cpu.Execute(9, rom, ram);
    printf("A: %d | B: %d | C: %d | D: %d | E: %d | F: %d | G: %d | H: %d\n", cpu.A, cpu.B, cpu.C, cpu.D, cpu.E, cpu.F, cpu.G, cpu.H);
}