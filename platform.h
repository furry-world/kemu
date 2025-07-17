#pragma once

#include <cstdint>
#include "math.h"
#include "raylib.h"
#include <queue>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#define MAX_SAMPLES             512
#define MAX_SAMPLES_PER_UPDATE  4096
#define MAX_SAMPLE_SIZE         32767
#define BUFFER_LENGTH	        4096

#define SAMPLE_RATE             44100
#define BIT_DEPTH               16

#define VOLUME_MULTIPLIER       0.2

class Platform
{

public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();
	void Update(void const* buffer);
	void Add(bool value);

private:

	static void AudioInputCallback(void *buffer, unsigned int frames);
	static void AudioProcessEffectLPF(void *buffer, unsigned int frames);

	Image image;
	Texture2D texture;
	Rectangle screen_rect;
	Rectangle texture_rect;
	AudioStream stream;

    float sum;
    int samplesCollected;
};

static int indexRead = 0;
static int indexWrite = 0;
static short que[1048576] = { };
static float samplesToCollect = 0;
