#include "platform.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	InitWindow(windowWidth, windowHeight, title);

    image = GenImageColor(textureWidth, textureHeight, (Color) {199, 240, 216, 255});
    texture = LoadTextureFromImage(image);
    texture_rect = {0, 0, float(textureWidth), float(textureHeight)};
    screen_rect = {0, 40, float(windowWidth), float(windowHeight-40)};

    UnloadImage(image);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    stream = LoadAudioStream(44100, 16, 1);
    SetAudioStreamCallback(stream, AudioInputCallback);
    AttachAudioStreamProcessor(stream, AudioProcessEffectLPF);

    PlayAudioStream(stream);
}

Platform::~Platform()
{
    UnloadAudioStream(stream);
    CloseAudioDevice();
}

void Platform::SetFreq(float Value)
{
    Freq = Value;
}

void Platform::AudioInputCallback(void *buffer, unsigned int frames)
{
    const int submul = 50;
    float incr = Freq/44100.0f;
    float subincr = incr/submul;
    short *d = (short *)buffer;

    float sum;

    for (unsigned int i = 0; i < frames; i++)
    {
        if (Freq == 0) {d[i] = 0;}
        else
        {
            sum = 0;
            for (unsigned int j = 0; j < submul; j++)
            {
                sum += beepIdx < 0.5 ? 1 : -1;
                beepIdx += subincr;
                if (beepIdx > 1.0f) beepIdx -= 1.0f;
            }
            d[i] = (short)(MAX_SAMPLE_SIZE * (sum / submul) * 0.2);
        }
    }
}

void Platform::AudioProcessEffectLPF(void *buffer, unsigned int frames)
{
    static float low[2] = { 0.0f, 0.0f };
    static const float cutoff = 20000.0f / 44100.0f;
    const float k = cutoff / (cutoff + 0.1591549431f);

    float *bufferData = (float *)buffer;
    for (unsigned int i = 0; i < frames*2; i += 2)
    {
        const float l = bufferData[i];
        const float r = bufferData[i + 1];

        low[0] += k * (l - low[0]);
        low[1] += k * (r - low[1]);
        bufferData[i] = low[0];
        bufferData[i + 1] = low[1];
    }
}

void Platform::Update(void const* buffer)
{
    ClearBackground((Color) {199, 240, 216, 255});
    UpdateTexture(texture, buffer);
    DrawTexturePro(texture, texture_rect, screen_rect, (Vector2) {0, 0}, 0, (Color) {67, 82, 61, 255});
}
