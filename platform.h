#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "math.h"
#include "raylib.h"

#define MAX_SAMPLES               512
#define MAX_SAMPLES_PER_UPDATE   4096
#define MAX_SAMPLE_SIZE         32767


class Platform
{
	friend class Imgui;

public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();
	void Update(void const* buffer);
	void SetFreq(float);


private:

	static void AudioInputCallback(void *buffer, unsigned int frames);

	Image image;
	Texture2D texture;
	Rectangle screen_rect;
	Rectangle texture_rect;
	AudioStream stream;
};

static float beep_volume;
static float beep_period;
static short *data;
static short *writeBuf;
static float Freq;

static int WaveLength = 1;
static float audioFrequency = 440.0f;
static float beepIdx = 0.0f;
static float oldFrequency = 1.0f;
static int ReadCursor = 0;