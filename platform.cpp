#include "platform.h"
#include "raylib.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	InitWindow(windowWidth, windowHeight, title);
    
    image = GenImageColor(textureWidth, textureHeight, (Color) {199, 240, 216, 255});
    texture = LoadTextureFromImage(image);
    texture_rect = {0, 0, float(textureWidth), float(textureHeight)};
    screen_rect = {0, 0, float(windowWidth), float(windowHeight)};

    UnloadImage(image);
}

Platform::~Platform()
{
	CloseWindow();
}

void Platform::Update(void const* buffer)
{
    ClearBackground((Color) {199, 240, 216, 255});
    UpdateTexture(texture, buffer);
    DrawTexturePro(texture, texture_rect, screen_rect, (Vector2) {0, 0}, 0, (Color) {67, 82, 61, 255});
}
