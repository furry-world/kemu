#!/bin/sh
x86_64-w64-mingw32-g++ main.cpp memory.cpp cpu.cpp platform.cpp -std=c++23 -I winblows/include/ -L winblows/lib/ -static -static-libgcc -static-libstdc++ -lraylib -lopengl32 -lgdi32 -lwinmm
