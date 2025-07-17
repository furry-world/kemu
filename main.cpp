#include "cpu.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

bool preferences = false;
bool debugFlag = false;
bool about = false;
bool isPaused = false;
bool isLocked = true;
bool errorMsg = false;

int currentSelection = 0;
int error;

const float x_spacing = 19;
const float y_spacing = 10;

int main() {

    Platform platform("kemu - Game N' Wave emulator", 600, 440, 60, 40);

    Font font = LoadFont("C64_Pro_Mono-STYLE.ttf");

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    Memory ram(256, true);
    Memory rom(4096, false);
    System system;

    CPU cpu;

    cpu.Reset(ram, rom, system, platform);

    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());

    while(!WindowShouldClose())
    {

        if (fileDialogState.SelectFilePressed)
        {

            char fileNameToLoad[512] = { 0 };

            if (IsFileExtension(fileDialogState.fileNameText, ".gnw"))
            {
                strcpy(fileNameToLoad, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                cpu.Reset(ram, rom, system, platform);
                rom.LoadFile(fileNameToLoad);
                if (isLocked) isLocked = false;
            }
            fileDialogState.SelectFilePressed = false;
        }


        if (!isLocked)
        {
            if (!isPaused)
            {
                error = cpu.Execute(GetFrameTime() * 1200000.00, rom, ram, system, platform, false);
            }
            else
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    error = cpu.Execute(0, rom, ram, system, platform, true);
                }
            }
        }

        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        if (IsKeyPressed(KEY_R))
        {
            cpu.PC = 0;
            cpu.SP = 0;
            for (int i = 0; i < 8; i++)
            {
                cpu.Registers[i] = 0;
            }

            system.Initialize();

            cpu.counter = 0;
            
            cpu.NewNote = 0;
            cpu.Note = 0;
        }

        system.Update();

        BeginDrawing();
        platform.Update(&system);

        if (GuiButton((Rectangle) {0, 0, 150, 40}, "Open file")) fileDialogState.windowActive = true;
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

            DrawTextEx(font, cpu.Disassemble(10, rom).c_str(), (Vector2) {925, 100}, 16, 0, DARKGRAY);

            char states[256];
            sprintf(states, "PC: %02X\nSP: %02o\nA: %02o\nB: %02o\nC: %02o\nD: %02o\nE: %02o\nF: %02o\nG: %02o\nH: %02o\nNote: %02o\nInput: %02o\nTimer: %02o", cpu.PC, cpu.SP, cpu.Registers[0], cpu.Registers[1], cpu.Registers[2], cpu.Registers[3], cpu.Registers[4], cpu.Registers[5], cpu.Registers[6], cpu.Registers[7], cpu.Note, ram[239], ram[238]);
            DrawTextEx(font, states, (Vector2) {610, 40}, 16, 0, DARKGRAY);

            if (GuiButton((Rectangle) {1000, 600, 150, 40}, "Slice 1")) currentSelection = 0;
            if (GuiButton((Rectangle) {1000, 640, 150, 40}, "Slice 2")) currentSelection = 1;
            if (GuiButton((Rectangle) {1000, 680, 150, 40}, "Slice 3")) currentSelection = 2;
            if (GuiButton((Rectangle) {1000, 720, 150, 40}, "Slice 4")) currentSelection = 3;

            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 32; y++)
                {
                    char oct[4];
                    snprintf(oct, 4, "%02o", ram[(y*8+x)]);
                    DrawTextEx(font, oct, (Vector2) {5 + x * x_spacing, 500 + y * y_spacing}, 8, 0, DARKGRAY);
                }
            }

            for (int y = 0; y < 32; y++)
            {
                char hex[4];
                snprintf(hex, 4, "%02X", y*32 + currentSelection * 1024);

                DrawTextEx(font, hex, (Vector2) {250,  500 + y * y_spacing}, 8, 0, DARKGRAY);

                for (int x = 0; x < 32; x++)
                {
                    char oct[4];

                    Color tCol;

                    if (cpu.PC == currentSelection * 1024 + (y*32+x))
                    {
                        tCol = RED;
                    }
                    else
                    {
                        tCol = DARKGRAY;
                    }

                    snprintf(oct, 4, "%02o", rom[currentSelection * 1024 + (y*32+x)]);
                    DrawTextEx(font, oct, (Vector2) {300 + x * x_spacing,  500 + y * y_spacing}, 8, 0, tCol);
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

        if (GuiButton((Rectangle) {450, 0, 150, 40}, "About")) about = !about;

        if (about)
        {
            int result = GuiPanel((Rectangle){125, 150, 350, 150}, "About");

            DrawText("KEMU - a Game N' Wave emulator\nMade by Maya Trimino and \nRea Irena Schindler\nusing Raylib and Raygui", 130, 185, 20, DARKGRAY);
        }

        if (error != 0 && !errorMsg)
        {
            cpu.Reset(ram, rom, system, platform);
            isLocked = true;
            errorMsg = true;
        }

        if (errorMsg)
        {
            int result;

            if (error == 1)
            {
            result = GuiMessageBox((Rectangle){150, 190, 300, 100},
                    "Error!", "Unknown instruction!", "OK");
            }
            else if (error == 2)
            {
                result = GuiMessageBox((Rectangle){150, 190, 300, 100},
                    "Error!", "Stack pointer overflow!", "OK");
            }
            else
            {
                result = GuiMessageBox((Rectangle){150, 190, 300, 100},
                    "Error!", "Stack pointer underflow!", "OK");
            }

            if (result >= 0)
            {
                error = 0;
                errorMsg = false;
            }
        }

        GuiWindowFileDialog(&fileDialogState);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
