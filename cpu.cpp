#include "cpu.h"

void CPU::Reset(Memory& ram, Memory& rom, System& system, Platform& interface){
    PC = 0;
    SP = 0;

    for (int i = 0; i < 8; i++)
    {
        Registers[i] = 0;
    }

    ram.Initialize(0);
    rom.Initialize(31);
    system.Initialize();

    srand(time(0));

    counter = 0;

    cyclesToExecute = 0;

    NewNote = 0;
    Note = 0;

    samplesGenerated = 0;

    HiLo = 0;
}

uint8_t CPU::FetchOpcodeHyte (float& Cycles, Memory& memory)
{
    uint8_t Data = memory[PC];
    PC++;
    PC = PC & 0b0000111111111111;
    Cycles--;
    return Data;
}

uint8_t CPU::FetchHyte (float& Cycles, Memory& memory, uint16_t address)
{
    uint8_t Data = memory[address];
    Cycles--;
    return Data;
}

void CPU::StoreHyte (float& Cycles, Memory& memory, uint16_t address, uint8_t value)
{
    Cycles--;
    memory[address] = value;
}

int CPU::Execute (float Cycles, Memory& rom, Memory& ram, System& system, Platform& interface, bool Single)
{
    cyclesToExecute += Cycles;
    Executed = false;

    while (cyclesToExecute > 0 || (Single && !Executed))
    {
        startCycles = cyclesToExecute;

        uint8_t Ins = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

        switch(Ins)
        {
            case INS_CALL:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                CPU::StoreHyte(cyclesToExecute, ram, 240 + (SP * 2), PC & 0b0000000000111111);
                CPU::StoreHyte(cyclesToExecute, ram, 240 + (SP * 2) + 1, (PC & 0b0000111111000000) >> 6);

                if (SP < 8)
                {
                    SP++;
                }
                else
                {
                    return 2;
                }

                PC = (value1 << 6) + value2;

                cyclesToExecute -= 3;

            } break;

            case INS_RET:
            {
                if (SP > 0)
                {
                    SP--;
                }
                else
                {
                    return 3;
                }

                uint8_t Value1 = CPU::FetchHyte(cyclesToExecute, ram, 240 + (SP * 2));
                uint8_t Value2 = CPU::FetchHyte(cyclesToExecute, ram, 240 + (SP * 2) + 1);

                PC = (Value2 << 6) + Value1;

                ram[240 + (SP * 2)] = 0;
                ram[240 + (SP * 2) + 1] = 0;

                cyclesToExecute--;

            } break;

            case INS_JMP:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                PC = (value1 << 6) + value2;

                cyclesToExecute -= 3;
            } break;

            case INS_EQU:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    if (Registers[(value1 & 0b00111000) >> 3] == (value2 & 0b00111111))
                    {
                        uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                        if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                        {
                            PC += 2;
                            cyclesToExecute -= 3;
                        }
                        else
                        {
                            PC += WordLen[In]-1;
                            cyclesToExecute -= WordLen[In];
                        }
                    }
                }
                else
                {
                    if (Registers[(value1 & 0b00111000) >> 3] == CPU::FetchHyte(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2))
                    {
                        uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                        if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                        {
                            PC += 2;
                            cyclesToExecute -= 3;
                        }
                        else
                        {
                            PC += WordLen[In]-1;
                            cyclesToExecute -= WordLen[In];
                        }
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_NEQU:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                if ((value1 & 0b00000100) >> 2 == 0) {

                    if (Registers[(value1 & 0b00111000) >> 3] != (value2 & 0b00111111))
                    {
                        uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                        if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                        {
                            PC += 2;
                            cyclesToExecute -= 3;
                        }
                        else
                        {
                            PC += WordLen[In]-1;
                            cyclesToExecute -= WordLen[In];
                        }
                    }
                }
                else
                {
                    if (Registers[(value1 & 0b00111000) >> 3] != CPU::FetchHyte(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2))
                    {
                        uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                        if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                        {
                            PC += 2;
                            cyclesToExecute -= 3;
                        }
                        else
                        {
                            PC += WordLen[In]-1;
                            cyclesToExecute -= WordLen[In];
                        }
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_RJMP:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                PC += value1 & 0b00111111;

                cyclesToExecute--;
            } break;

            case INS_ADD:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

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
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

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
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

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
                        Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2);
                        if (((value1 & 0b00000011) << 6) + value2 == 237) ram[237] = rand() % 64;
                    }
                    else
                    {
                        Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2) & 0b00000001;
                        if (((value1 & 0b00000011) << 6) + value2 == 237) ram[237] = rand() % 64;
                    }
                }

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    system.flag = 1;
                }

                cyclesToExecute--;
            } break;

            case INS_STORE:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                StoreHyte(cyclesToExecute, ram, ((value1 & 0b00000011) << 6) + value2, Registers[(value1 & 0b00111000) >> 3]);

                cyclesToExecute--;
            } break;

            case INS_RLOAD:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[value1 & 0b00000111];

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    system.flag = 1;
                }

                cyclesToExecute--;
            } break;

            case INS_REQU:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                if (Registers[(value1 & 0b00111000) >> 3] == Registers[value1 & 0b00000111])
                {
                    uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                    {
                        PC += 2;
                        cyclesToExecute -= 3;
                    }
                    else
                    {
                        PC += WordLen[In]-1;
                        cyclesToExecute -= WordLen[In];
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_NREQU:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                if (Registers[(value1 & 0b00111000) >> 3] != Registers[value1 & 0b00000111])
                {
                    uint8_t In = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    if ((In & 0b00111000) >> 3 == 4 || (In & 0b00111000) >> 3 == 5 || (In & 0b00111000) >> 3 == 6 || (In & 0b00111000) >> 3 == 7)
                    {
                        PC += 2;
                        cyclesToExecute -= 3;
                    }
                    else
                    {
                        PC += WordLen[In]-1;
                        cyclesToExecute -= WordLen[In];
                    }
                }
                cyclesToExecute--;
            } break;

            case INS_OR:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] | Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_AND:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] & Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_XOR:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] ^ Registers[value1 & 0b00000111];

                cyclesToExecute--;
            } break;

            case INS_SHIFTL:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[7] = (Registers[(value1 & 0b00111000) >> 3] & 0b00100000) >> 5;

                Registers[(value1 & 0b00111000) >> 3] = (Registers[(value1 & 0b00111000) >> 3] << 1) & 0b00111111;

                cyclesToExecute--;
            } break;

            case INS_SHIFTR:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                Registers[7] = Registers[(value1 & 0b00111000) >> 3] & 0b00000001;

                Registers[(value1 & 0b00111000) >> 3] = Registers[(value1 & 0b00111000) >> 3] >> 1;

                cyclesToExecute--;
            } break;

            case INS_ILOAD:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                if ((value1 & 0b00111000) >> 3 != 7)
                {
                    Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, ram, (((value1 & 0b00000011) << 6) + value2 + Registers[2]) & 0b11111111);
                }
                else
                {
                    Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, ram, (((value1 & 0b00000011) << 6) + value2 + Registers[2]) & 0b11111111) & 0b00000001;
                }

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    system.flag = 1;
                }

                cyclesToExecute -= 2;
            } break;

            case INS_ISTORE:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                StoreHyte(cyclesToExecute, ram, (((value1 & 0b00000011) << 6) + value2 + Registers[2]) & 0b11111111, Registers[(value1 & 0b00111000) >> 3]);

                cyclesToExecute -= 2;
            } break;

            case INS_NPLOAD:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                uint8_t address = value1 & 0b00000011;
                address = (address << 6) + value2;

                uint16_t finalAddr = CPU::FetchHyte(cyclesToExecute, ram, address);
                finalAddr = (finalAddr << 6) + CPU::FetchHyte(cyclesToExecute, ram, address+1) & 0b0000111111111111;

                if ((value1 & 0b00111000) >> 3 != 7)
                {
                     Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, rom, finalAddr);
                }
                else
                {
                    Registers[(value1 & 0b00111000) >> 3] = CPU::FetchHyte(cyclesToExecute, rom, finalAddr) & 0b00000001;
                }

                if ((value1 & 0b00111000) >> 3 == 6)
                {
                    system.flag = 1;
                }

                cyclesToExecute--;
            } break;

            case INS_NPSTORE:
            {
                uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                uint8_t address = value1 & 0b00000011;
                address = (address << 6) + value2;

                uint16_t finalAddr = CPU::FetchHyte(cyclesToExecute, ram, address);
                finalAddr = (finalAddr << 6) + CPU::FetchHyte(cyclesToExecute, ram, address+1) & 0b0000111111111111;

                if (rom.type == true) CPU::StoreHyte(cyclesToExecute, rom, finalAddr, Registers[(value1 & 0b00111000) >> 3]);
                else cyclesToExecute--;

                cyclesToExecute--;
            } break;

            default:
            {
                if ((Ins & 0b00111000) >> 3 == 4)
                {
                    uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                    uint16_t address = value1;
                    address = (address << 6) + value2;

                    if ((Ins & 0b00000111) != 7)
                    {
                        Registers[Ins & 0b00000111] = CPU::FetchHyte(cyclesToExecute, rom, address);
                    }
                    else
                    {
                        Registers[Ins & 0b00000111] = CPU::FetchHyte(cyclesToExecute, rom, address) & 0b00000001;
                    }

                    if ((value1 & 0b00111000) >> 3 == 6)
                    {
                        system.flag = 1;
                    }

                    cyclesToExecute--;
                }
                else if ((Ins & 0b00111000) >> 3 == 5)
                {
                    uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                    uint16_t address = value1;
                    address = (address << 6) + value2;

                    if (rom.type == true) CPU::StoreHyte(cyclesToExecute, rom, address, Registers[Ins & 0b00000111]);
                    else cyclesToExecute--;

                    cyclesToExecute--;
                }
                else if ((Ins & 0b00111000) >> 3 == 6)
                {
                    uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                    uint16_t address = value1;
                    address = (address << 6) + value2;

                    if ((Ins & 0b00000111) != 7)
                    {
                        Registers[Ins & 0b00000111] = CPU::FetchHyte(cyclesToExecute, rom, (address + Registers[2]) & 0b0000111111111111);
                    }
                    else
                    {
                        Registers[Ins & 0b00000111] = CPU::FetchHyte(cyclesToExecute, rom, (address + Registers[2]) & 0b0000111111111111) & 0b00000001;
                    }

                    if ((value1 & 0b00111000) >> 3 == 6)
                    {
                        system.flag = 1;
                    }

                    cyclesToExecute -= 2;
                }
                else if ((Ins & 0b00111000) >> 3 == 7)
                {
                    uint8_t value1 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);
                    uint8_t value2 = CPU::FetchOpcodeHyte(cyclesToExecute, rom);

                    uint16_t address = value1;
                    address = (address << 6) + value2;

                    if (rom.type == true) CPU::StoreHyte(cyclesToExecute, rom, (address + Registers[2]) & 0b0000111111111111, Registers[Ins & 0b00000111]);
                    else cyclesToExecute--;

                    cyclesToExecute -= 2;
                }
                else
                {
                    return 1;
                }
            }
        }

        if (Single) Executed = !Executed;

        float cyclesElapsed = startCycles - cyclesToExecute;

        for (int samples = 0; samples < cyclesElapsed; samples++)
        {
            if (Note == 0)
            {
                interface.Add(0);
            }
            else if (Note == 63)
            {
                interface.Add(1);
            }
            else
            {
                if (samplesGenerated >= system.GetSound(Note))
                {
                    samplesGenerated = 0;
                    HiLo = !HiLo;
                }

            interface.Add(HiLo);
            samplesGenerated++;
            }
        }

        Note = NewNote;

        counter += cyclesElapsed;

        if (counter >= 20000)
        {
            if (ram[238] > 0) ram[238]--;
            counter -= 20000;
        }

        if (system.flag == 1)
        {
            switch(Registers[6])
            {
                case 0:
                    system.VideoPixelOn(Registers[4], Registers[5]);
                    break;
                case 1:
                    system.VideoApply(Registers[4], Registers[5], Registers[3]);
                    break;
                case 2:
                    NewNote = Registers[5];
                    break;
                case 3:
                    system.VideoPixelOff(Registers[4], Registers[5]);
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
    return 0;
}

std::string CPU::Disassemble (int InstructionCount, Memory& rom)
{
    uint16_t vPC = PC;

    std::string str = ">";

    for (int i = 0; i < InstructionCount; i++)
    {
        uint8_t Ins = rom[vPC];

        switch(Ins)
        {
            case INS_CALL:
            {
                uint16_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;
                strIns = std::format("CALL ${:X}\n", (value1 << 6) + value2);

                str += strIns;

                vPC += 3;

            } break;

            case INS_RET:
            {

                std::string strIns = "RETURN\n";
                str += strIns;

                vPC++ ;

            } break;

            case INS_JMP:
            {
                uint16_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;
                strIns = std::format("JUMP ${:X}\n", (value1 << 6) + value2);

                str += strIns;

                vPC += 3;
            } break;

            case INS_EQU:
            {
                uint16_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    std::string strIns;
                    strIns = std::format("EQUAL {} #!{:o}\n", char(65 + (value1>>3)), value2);

                    str += strIns;

                    vPC += 3;
                }
                else
                {
                    std::string strIns;
                    strIns = std::format("EQUAL {} ${:X}\n", char(65 + (value1>>3)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
            } break;

            case INS_NEQU:
            {
                uint16_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    std::string strIns;
                    strIns = std::format("NOTEQUAL {} #!{:o}\n", char(65 + (value1>>3)), value2);

                    str += strIns;

                    vPC += 3;
                }
                else
                {
                    std::string strIns;
                    strIns = std::format("NOTEQUAL {} ${:X}\n", char(65 + (value1>>3)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
            } break;

            case INS_RJMP:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns = std::format("RJUMP !{:o}\n", value1);

                str += strIns;

                vPC += 2;

            } break;

            case INS_ADD:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("ADD {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_SUB:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("SUBTRACT {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_LOAD:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                if ((value1 & 0b00000100) >> 2 == 0)
                {
                    std::string strIns;

                    strIns = std::format("LOAD {} #!{:o}\n", char(65 + (value1>>3)), value2);

                    str += strIns;

                    vPC += 3;
                }
                else
                {
                    std::string strIns;

                    strIns = std::format("LOAD {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
            } break;

            case INS_STORE:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;

                strIns = std::format("STORE {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                str += strIns;

                vPC += 3;

            } break;

            case INS_RLOAD:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("LOAD {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_REQU:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("EQUAL {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_NREQU:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("NOTEQUAL {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_OR:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("OR {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_AND:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("AND {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_XOR:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("XOR {} {}\n", char(65 + (value1>>3)), char(65 + (value1 & 0b00000111)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_SHIFTL:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("SHIFTL {}\n", char(65 + (value1>>3)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_SHIFTR:
            {
                uint8_t value1 = rom[vPC+1];

                std::string strIns;

                strIns = std::format("SHIFTR {}\n", char(65 + (value1>>3)));

                str += strIns;

                vPC += 2;
            } break;

            case INS_ILOAD:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;

                strIns = std::format("ILOAD {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                str += strIns;

                vPC += 3;
            } break;

            case INS_ISTORE:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;

                strIns = std::format("ISTORE {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                str += strIns;

                vPC += 3;
            } break;

            case INS_NPLOAD:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;

                strIns = std::format("NPLOAD {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                str += strIns;

                vPC += 3;
            } break;

            case INS_NPSTORE:
            {
                uint8_t value1 = rom[vPC+1];
                uint8_t value2 = rom[vPC+2];

                std::string strIns;

                strIns = std::format("NPSTORE {} ${:X}\n", char(65 + (value1>>3)), ((value1 & 0b00000011) << 6) + value2);

                str += strIns;

                vPC += 3;
            } break;

            default:
            {
                if ((Ins & 0b00111000) >> 3 == 4)
                {
                    uint16_t value1 = rom[vPC+1];
                    uint8_t value2 = rom[vPC+2];

                    std::string strIns;

                    strIns = std::format("PLOAD {} ${:X}\n", char(65 + (Ins & 0b00000111)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
                else if ((Ins & 0b00111000) >> 3 == 5)
                {
                    uint16_t value1 = rom[vPC+1];
                    uint8_t value2 = rom[vPC+2];

                    std::string strIns;

                    strIns = std::format("PSTORE {} ${:X}\n", char(65 + (Ins & 0b00000111)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
                else if ((Ins & 0b00111000) >> 3 == 6)
                {
                    uint16_t value1 = rom[vPC+1];
                    uint8_t value2 = rom[vPC+2];

                    std::string strIns;

                    strIns = std::format("IPLOAD {} ${:X}\n", char(65 + (Ins & 0b00000111)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
                else if ((Ins & 0b00111000) >> 3 == 7)
                {
                    uint16_t value1 = rom[vPC+1];
                    uint8_t value2 = rom[vPC+2];

                    std::string strIns;

                    strIns = std::format("IPSTORE {} ${:X}\n", char(65 + (Ins & 0b00000111)), (value1 << 6) + value2);

                    str += strIns;

                    vPC += 3;
                }
                else
                {
                    std::string strIns = "??\n";
                    str += strIns;
                    vPC++;
                }
            }
        }
    }
    return str;
}


std::string CPU::NoteToString ()
{
    static const char* noteNames[] = {
        "OFF",
        "F-1",
        "F#1",
        "G-1",
        "G#1",
        "A-1",
        "A#1",
        "B-1",
        "C-2",
        "C#2",
        "D-2",
        "D#2",
        "E-2",
        "F-2",
        "F#2",
        "G-2",
        "G#2",
        "A-2",
        "A#2",
        "B-2",
        "C-3",
        "C#3",
        "D-3",
        "D#3",
        "E-3",
        "F-3",
        "F#3",
        "G-3",
        "G#3",
        "A-3",
        "A#3",
        "B-3",
        "C-4",
        "C#4",
        "D-4",
        "D#4",
        "E-4",
        "F-4",
        "F#4",
        "G-4",
        "G#4",
        "A-4",
        "A#4",
        "B-4",
        "C-5",
        "C#5",
        "D-5",
        "D#5",
        "E-5",
        "F-5",
        "F#5",
        "G-5",
        "G#5",
        "A-5",
        "A#5",
        "B-5",
        "C-6",
        "C#6",
        "D-6",
        "D#6",
        "E-6",
        "F-6",
        "F#6",
        "ON"
    };

    return noteNames[Note];
}
