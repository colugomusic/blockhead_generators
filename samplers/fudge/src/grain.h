#pragma once

struct Grain
{
	bool on = false;
	bool fade_in = true;
	float pos[2];
	float size = 1024.0f;
	float window = 256.0f;
	float frame = 0.0f;
	float frame_amp = 0.0f;
	float duck = 1.0f;
	float ff = 1.0f;

	Grain()
	{
		pos[0] = 0.0f;
		pos[1] = 0.0f;
	}
};