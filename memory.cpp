#include "memory.h"

Memory::Memory(unsigned int Count, bool Type)
{
    MAX_MEM = Count;
    Mem.reserve(MAX_MEM);
    type = Type;
}

void Memory::Initialize(uint8_t Value)
{
    for (int i = 0; i < MAX_MEM; i++)
    {
        Mem[i] = Value;
    }
}

uint8_t& Memory::operator[](uint32_t Address)
{
    return Mem[Address];
}

void Memory::LoadFile(char const* FileName)
{
	std::ifstream file(FileName, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        if (size > MAX_MEM)
        {
            printf("Error! ROM size exceeds limit!\n");
            exit(0);
        }
		char* buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

        for (long i = 0; i < size; ++i)
		{
			Mem[i] = buffer[i];
		}

		delete[] buffer;
    }
}

void System::Initialize()
{
    for (int i = 0; i < 2400; i++)
    {
        Mem[i] = 0;
        Pixel[i] = 0;
    }
}

void System::VideoApply(uint8_t x, uint8_t y, uint8_t data)
{
    if (x < 60 && y < 40) Pixel[y*60+x] = (data & 0b00100000) > 0;
    if (x+1 < 60 && y < 40) Pixel[y*60+x+1] = (data & 0b00010000) > 0;
    if (x+2 < 60 && y < 40) Pixel[y*60+x+2] = (data & 0b00001000) > 0;
    if (x+3 < 60 && y < 40) Pixel[y*60+x+3] = (data & 0b00000100) > 0;
    if (x+4 < 60 && y < 40) Pixel[y*60+x+4] = (data & 0b00000010) > 0;
    if (x+5 < 60 && y < 40) Pixel[y*60+x+5] = (data & 0b00000001) > 0;
}

void System::VideoPixelOn(uint8_t x, uint8_t y)
{
    if (x < 60 && y < 40) Pixel[y*60+x] = 1;
}

void System::VideoPixelOff(uint8_t x, uint8_t y)
{
    if (x < 60 && y < 40) Pixel[y*60+x] = 0;
}

float System::GetSound(uint8_t value)
{
    return PitchTable[value];
}

void System::Update()
{

    for (int i = 0; i < 2400; i++)
    {
        Mem[i] = (Pixel[i] == 1) ? 0xFFFFFFFF : 0x00000000;
    }
}