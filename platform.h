#pragma once

#include <cstdint>
#include "raylib.h"
#include "memory.h"


class Platform
{
	friend class Imgui;

public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();
	void Update(void const* buffer);
	void ProcessInput(uint8_t* keys);

private:

	Image image;
	Texture2D texture;
	Rectangle screen_rect;
	Rectangle texture_rect;
};