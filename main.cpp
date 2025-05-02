#include "cpu.h"
#include "portable-file-dialogs.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <format>

bool preferences = false;
bool debugFlag = false;
int currentSelection = 0;
bool isPaused = false;
bool isLocked = true;

const int x_spacing = 20;
const int y_spacing = 10;

int main() {

    Platform platform("kemu - Game N' Wave emulator", 600, 440, 60, 40);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    Memory ram(256, true);
    Memory rom(4096, false);
    System system;

    CPU cpu;

    cpu.Reset(ram, rom, system, platform);

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        if (!isLocked)
        {
            if (!isPaused)
            {
                cpu.Execute(GetFrameTime() * 1200000.00, rom, ram, system, platform, false);
            }
            else
            {
                if (IsKeyPressed(KEY_ENTER)) cpu.Execute(0, rom, ram, system, platform, true);
            }
        }

        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        system.Update();

        BeginDrawing();
        platform.Update(&system);

        if (GuiButton((Rectangle) {0, 0, 150, 40}, "Open file"))
        {
            isLocked = true;
            auto f = pfd::open_file("Choose files to read", pfd::path::home(),
            { "GNW files (.gnw)", "*.gnw"}).result();
            if (!f.empty()) 
            {
                cpu.Reset(ram, rom, system, platform);
                rom.LoadFile(f[0].c_str());
            }
            if (rom[0] != 31) isLocked = false;
        };

        if (GuiButton((Rectangle) {150, 0, 150, 40}, "Preferences")) preferences = true;

        if (preferences)
        {
            isPaused = true;
            int result = GuiMessageBox((Rectangle){150, 190, 300, 100}, "Preferences", "Please select a mode", "ROM;RAM");

            if (result == 1) 
            {
                rom.type = false;
                preferences = false;
                isPaused = false;
            }
            else if (result == 2)
            {
                rom.type = true;
                preferences = false;
                isPaused = false;
            }
            else if (result == 0)
            {
                preferences = false;
                isPaused = false;
            }
        }
        if (GuiButton((Rectangle) {300, 0, 150, 40}, "Debugger")) debugFlag = !debugFlag;

        if (debugFlag)
        {
            if (GetScreenWidth() != 1200)
            {
                SetWindowSize(1200, 880);
            }

            GuiPanel((Rectangle) {600, 0, 300, 440}, "K6 and Commander registers");
            GuiPanel((Rectangle) {900, 0, 300, 440}, "Disassembly");
            GuiPanel((Rectangle) {0, 440, 160, 440}, "RAM");
            GuiPanel((Rectangle) {160, 440, 1040, 440}, "ROM");
            DrawText(std::format("PC: {}\nSP: {}\nA: {}\nB: {}\nC: {}\nD: {}\nE: {}\nF: {}\nG: {}\nH: {}\nNote: {}\nInput: {}\nTimer: {}", cpu.PC, cpu.SP, cpu.Registers[0], cpu.Registers[1], cpu.Registers[2], cpu.Registers[3], cpu.Registers[4], cpu.Registers[5], cpu.Registers[6], cpu.Registers[7], cpu.Note, ram[239], ram[238]).c_str(), 610, 40, 20, DARKGRAY);

            if (GuiButton((Rectangle) {1000, 600, 150, 40}, "Slice 1")) currentSelection = 0;
            if (GuiButton((Rectangle) {1000, 640, 150, 40}, "Slice 2")) currentSelection = 1;
            if (GuiButton((Rectangle) {1000, 680, 150, 40}, "Slice 3")) currentSelection = 2;
            if (GuiButton((Rectangle) {1000, 720, 150, 40}, "Slice 4")) currentSelection = 3;

            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 32; y++)
                {
                    char oct[4];
                    snprintf(oct, 4, "%o", ram[(y*8+x)]);
                    DrawText(oct, 7 + x * x_spacing, 500 + y * y_spacing, 10, DARKGRAY);
                }
            }

            for (int y = 0; y < 32; y++)
            {
                char hex[4];
                snprintf(hex, 4, "%x", y*32 + currentSelection * 1024);

                DrawText(hex, 250,  500 + y * y_spacing, 10, DARKGRAY);

                for (int x = 0; x < 32; x++)
                {
                    char oct[4];
                    snprintf(oct, 4, "%o", rom[(currentSelection * 1024) + (y*32+x)]);
                    DrawText(oct, 300 + x * x_spacing,  500 + y * y_spacing, 10, DARKGRAY);
                }
            }
        }
        else
        {
            if (GetScreenWidth() != 600)
            {
                SetWindowSize(600, 440);
            }
        }

        GuiButton((Rectangle) {450, 0, 150, 40}, "About");

        EndDrawing();
    }

    CloseWindow(); 

    return 0;
}
