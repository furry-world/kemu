#include "cpu.h"
#include <iostream>
#include "raylib.h"
#include "platform.h"

int main(int argc, char** argv) {

    if (argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		std::exit(0);
	}

    int videoScale = 10;

    char const* romFilename = argv[1];

    Platform platform("kemu - Game N' Wave emulator", 60 * videoScale, 40 * videoScale, 60, 40);


    Memory ram(256);
    Memory rom(4096);
    Video video;

    CPU cpu;

    cpu.Reset(ram, rom, video);

    rom.LoadFile(romFilename);

    while(!WindowShouldClose())
    {

        if (IsKeyDown(KEY_SPACE))
        {
            cpu.Execute(23, rom, ram, video);
            std::cout << "Press  to advance the program.\n";
        }

        BeginDrawing();
        video.Update();
        platform.Update(&video);

        EndDrawing();
    }

    CloseWindow(); 

    return 0;
}