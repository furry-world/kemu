files	= main.cpp memory.cpp cpu.cpp platform.cpp
opts	= -I"/home/cykabruh/raylib/src" -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 

kemu: $(files)
	g++ $(files) $(opts)
