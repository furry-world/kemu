#include "cpu.h"
#include <iostream>

int main(int argc, char** argv) {

    if (argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		std::exit(0);
	}

    char const* romFilename = argv[1];


    Memory ram(256);
    Memory rom(4096);
    CPU cpu;

    cpu.Reset(ram, rom);

    rom.LoadFile(romFilename);
    
    while(true)
    {
        std::cout << "Press enter to advance code.\n";
        std::cin.get();
        cpu.Execute(100, rom, ram);
    }
}