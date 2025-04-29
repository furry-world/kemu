#include "cpu.h"
#include <string>
#include "portable-file-dialogs.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>

bool isRunning = false;
bool preferences = false;

int main() {

    Platform platform("kemu - Game N' Wave emulator", 600, 440, 60, 40);


    Memory ram(256, true);
    Memory rom(4096, false);
    System system;

    CPU cpu;

    cpu.Reset(ram, rom, system, platform);

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        if(isRunning)
        {
            cpu.Execute(GetFrameTime() * 1000000.00, rom, ram, system, platform);
            if (ram[238] > 0) ram[238]--;
        }

        BeginDrawing();
        system.Update();
        platform.Update(&system, isRunning);

        if(GuiButton((Rectangle) {0, 0, 150, 40}, "Open file"))
        {
            isRunning = false;
            auto f = pfd::open_file("Choose files to read", pfd::path::home(),
            { "GNW files (.gnw)", "*.gnw"}).result();
            if (!f.empty()) 
            {
                cpu.Reset(ram, rom, system, platform);
                rom.LoadFile(f[0].c_str());
            }
            isRunning = true;
        };
        if (GuiButton((Rectangle) {150, 0, 150, 40}, "Preferences")) preferences = true;
        if (preferences)
        {
            isRunning = false;
            int result = GuiMessageBox((Rectangle){150, 190, 300, 100}, "Preferences", "Please select a mode", "RAM;ROM");

            if (result == 1) 
            {
                rom.type = true;
                isRunning = true;
                preferences = false;
            }
            else if (result == 2)
            {
                rom.type = false;
                isRunning = true;
                preferences = false;
            }
            else if (result == 0)
            {
                isRunning = true;
                preferences = false;
            }
        }
        GuiButton((Rectangle) {300, 0, 150, 40}, "Debugger");
        GuiButton((Rectangle) {450, 0, 150, 40}, "About");

        EndDrawing();
    }

    CloseWindow(); 

    return 0;
}
