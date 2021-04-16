#pragma once

#include <blink.h>

struct AudioData
{
	const blink_EnvelopeData* env_pitch;
	const blink_EnvelopeData* env_formant;
	const blink_SliderData* slider_pitch;
};