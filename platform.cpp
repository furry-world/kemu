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
    stream = LoadAudioStream(SAMPLE_RATE, BIT_DEPTH, 1);
    SetAudioStreamCallback(stream, AudioInputCallback);
    AttachAudioStreamProcessor(stream, AudioProcessEffectHPF);
    AttachAudioStreamProcessor(stream, AudioProcessEffectLPF);

    sum = 0;
    samplesCollected = 0;
    samplesToFabricate = BUFFER_OFFSET;

    PlayAudioStream(stream);
}

Platform::~Platform()
{
    UnloadAudioStream(stream);
    CloseAudioDevice();
}

void Platform::Add(bool value)
{
    if (samplesToCollect < 0)
    {
        // we have collected enough 1-bit samples for a real native sample,
        // so let's generate one!
        sum += value * (samplesToCollect + 1);
        samplesCollected += samplesToCollect + 1;

        float sample = sum / samplesCollected;
        sample = short(MAX_SAMPLE_SIZE * sample * VOLUME_MULTIPLIER);
        que[indexWrite] = sample;
        indexWrite++;
        if (indexWrite > BUFFER_LENGTH) indexWrite = 0;

        samplesCollected = -samplesToCollect;
        sum = value * samplesCollected;

        samplesToCollect += 1200000.0 / SAMPLE_RATE;
    }

    sum += value;
    samplesToCollect--;
    samplesCollected++;
}

void Platform::AudioInputCallback(void *buffer, unsigned int frames)
{
    short *d = (short *)buffer;

    for (unsigned int i = 0; i < frames; i++)
    {
        if (samplesToFabricate > 0) {
            d[i] = 0;
            samplesToFabricate--;
            continue;
        }

        if (indexRead == indexWrite) {
            samplesToFabricate += BUFFER_OFFSET;
            d[i] = 0;
            continue;
        }

        d[i] = que[indexRead];
        indexRead++;
        if (indexRead > BUFFER_LENGTH) indexRead = 0;
    }
}

void Platform::AudioProcessEffectLPF(void *buffer, unsigned int frames)
{
    static float low[2] = { 0.0f, 0.0f };
    static const float cutoff = LPF_CUTOFF / SAMPLE_RATE;
    const float k = cutoff / (cutoff + 1 / (2 * PI)); // RC filter formula

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

void Platform::AudioProcessEffectHPF(void *buffer, unsigned int frames)
{
    static float high[2] = { 0.0f, 0.0f };
    static float old_in[2] = { 0.0f, 0.0f };
    static const float cutoff = HPF_CUTOFF / SAMPLE_RATE;
    const float k = 1 / (cutoff * TAU + 1); // RC filter formula

    float *bufferData = (float *)buffer;
    for (unsigned int i = 0; i < frames*2; i += 2)
    {
        const float l = bufferData[i];
        const float r = bufferData[i + 1];

        high[0] = k * (high[0] + l - old_in[0]);
        high[1] = k * (high[1] + r - old_in[1]);
        bufferData[i] = high[0];
        bufferData[i + 1] = high[1];
        old_in[0] = l;
        old_in[1] = r;
    }
}

void Platform::Update(void const* buffer)
{
    ClearBackground((Color) {199, 240, 216, 255});
    UpdateTexture(texture, buffer);
    DrawTexturePro(texture, texture_rect, screen_rect, (Vector2) {0, 0}, 0, (Color) {67, 82, 61, 255});
}
