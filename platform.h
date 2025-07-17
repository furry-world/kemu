#pragma once

#include <cstdint>
#include "math.h"
#include "raylib.h"
#include <queue>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#define MAX_SAMPLES               512
#define MAX_SAMPLES_PER_UPDATE   4096
#define MAX_SAMPLE_SIZE         32767

#define MINIMUM_BUFFER_LENGTH     256


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
};

static std::queue<bool> que;
static float samplesToCollect = 0;
