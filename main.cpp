#include <cstdio>
#include "memory.h"
#include "cpu.h"

int main() {
    RAM ram;
    CPU cpu;
    ROM rom;
    cpu.Reset(ram, rom);
    // PRG Start
    ram[0x0F] = 0b00001111;
    rom[0x000] = cpu.INS_LOAD;
    rom[0x001] = 0b00100100;
    rom[0x002] = 0b00001111;
    rom[0x003] = cpu.INS_LOAD;
    rom[0x004] = 0b00001000;
    rom[0x005] = 0b00111111;
    // PRG End
    cpu.Execute(9, rom, ram);
    printf("A: %d | B: %d | C: %d | D: %d | E: %d | F: %d | G: %d | H: %d\n", cpu.A, cpu.B, cpu.C, cpu.D, cpu.E, cpu.F, cpu.G, cpu.H);
}