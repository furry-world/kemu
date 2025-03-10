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

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    stream = LoadAudioStream(44100, 16, 1);
    data = (short *)malloc(sizeof(short)*MAX_SAMPLES);
    writeBuf = (short *)malloc(sizeof(short)*MAX_SAMPLES_PER_UPDATE);
    SetAudioStreamCallback(stream, AudioInputCallback);

    PlayAudioStream(stream);
}

Platform::~Platform()
{
    free(data);
    free(writeBuf);

    UnloadAudioStream(stream);
    CloseAudioDevice();

	CloseWindow();
}

void Platform::SetFreq(float Value)
{
    Freq = Value;
}

void Platform::AudioInputCallback(void *buffer, unsigned int frames)
{
    float incr = Freq/44100.0f;
    short *d = (short *)buffer;

    for (unsigned int i = 0; i < frames; i++)
    {
        d[i] = (short)(MAX_SAMPLE_SIZE * (beepIdx < 0.1 ? 1 : -1));
        beepIdx += incr;
        if (beepIdx > 1.0f) beepIdx -= 1.0f;
    }
}

void Platform::Update(void const* buffer)
{
    ClearBackground((Color) {199, 240, 216, 255});
    UpdateTexture(texture, buffer);
    DrawTexturePro(texture, texture_rect, screen_rect, (Vector2) {0, 0}, 0, (Color) {67, 82, 61, 255});
}
