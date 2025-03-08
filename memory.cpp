#include "memory.h"

Memory::Memory(unsigned int Count)
{
    Memory::MAX_MEM = Count;
    Mem.reserve(Memory::MAX_MEM);
}

void Memory::Initialize(unsigned char Value)
{
    for (int i = 0; i < Memory::MAX_MEM; i++)
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