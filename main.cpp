#include "cpu.h"
#include <iostream>

int main(int argc, char** argv) {

    if (argc != 3)
	{
		printf("Usage: %s <file> <size>\n", argv[0]);
		std::exit(0);
	}

    char const* romFilename = argv[1];

    int videoScale = atoi(argv[2]);

    Platform platform("kemu - Game N' Wave emulator", 60 * videoScale, 40 * videoScale, 60, 40);


    Memory ram(256);
    Memory rom(4096);
    System system;

    CPU cpu;

    cpu.Reset(ram, rom, system);

    rom.LoadFile(romFilename);

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {

        cpu.Execute(GetFrameTime() * 1000.00, rom, ram, system, platform);

        BeginDrawing();
        system.Update();
        platform.Update(&system);

        EndDrawing();
    }

    CloseWindow(); 

    return 0;
}