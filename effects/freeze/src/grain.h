#pragma once

struct Grain
{
	bool on{};
	bool dry{};
	float size = 1024.0f;
	float period = 1024.0f;
	float window = 256.0f;
	float frame = 0.0f;
	float frame_amp = 0.0f;
	float duck = 1.0f;
	float ff = 1.0f;
};