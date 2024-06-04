#pragma once

#include <blink/data.hpp>

struct AudioData {
	struct {
		blink::ChordData scale;
	} chord;
	struct {
		blink::EnvData amp;
		blink::EnvData pan;
		blink::EnvData pitch;
		blink::EnvData speed;
		blink::EnvData grain_size;
		blink::EnvData grain_transpose;
		blink::EnvData grain_uniformity;
		blink::EnvData harmonics_amount;
		blink::EnvData harmonics_spread;
		blink::EnvData noise_amount;
		blink::EnvData noise_color;
	} env;
	struct {
		blink::SliderRealData amp;
		blink::SliderRealData pan;
		blink::SliderRealData pitch;
		blink::SliderRealData speed;
		blink::SliderRealData noise_width;
		blink::SliderIntData sample_offset;
	} slider;
	struct {
		blink::OptionData noise_mode;
		blink::OptionData reverse_mode;
	} option;
	struct {
		blink::OptionData loop;
		blink::OptionData reverse;
	} toggle;
	const blink_WarpPoints* warp_points;
};
