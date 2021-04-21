#pragma once

#include <blink.h>

struct AudioData
{
	const blink_OptionData* option_noise_mode;
	const blink_EnvelopeData* env_amp;
	const blink_EnvelopeData* env_pan;
	const blink_EnvelopeData* env_pitch;
	const blink_EnvelopeData* env_speed;
	const blink_EnvelopeData* env_noise_amount;
	const blink_EnvelopeData* env_noise_color;
	const blink_SliderData* slider_amp;
	const blink_SliderData* slider_pan;
	const blink_SliderData* slider_pitch;
	const blink_SliderData* slider_speed;
	const blink_IntSliderData* slider_sample_offset;
	const blink_SliderData* slider_noise_width;
	const blink_ToggleData* toggle_loop;
	const blink_ToggleData* toggle_reverse;
	const blink_WarpPoints* warp_points;
};