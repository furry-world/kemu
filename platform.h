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
#define BUFFER_LENGTH	        262144
#define BUFFER_OFFSET	        1024

#define SAMPLE_RATE             44100
#define BIT_DEPTH               16

#define VOLUME_MULTIPLIER       0.2

#define HPF_CUTOFF				500.0f
#define LPF_CUTOFF				1500.0f


#define TAU 6.2831853071

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
	static void AudioProcessEffectHPF(void *buffer, unsigned int frames);

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
static short que[BUFFER_LENGTH] = { };
static float samplesToCollect = 0;
static int samplesToFabricate;
