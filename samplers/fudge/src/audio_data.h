#pragma once

#include <blink/data.hpp>

struct AudioData {
	struct {
		blink::uniform::Chord scale;
	} chord;
	struct {
		blink::uniform::Env amp;
		blink::uniform::Env pan;
		blink::uniform::Env pitch;
		blink::uniform::Env speed;
		blink::uniform::Env grain_size;
		blink::uniform::Env grain_transpose;
		blink::uniform::Env grain_uniformity;
		blink::uniform::Env harmonics_amount;
		blink::uniform::Env harmonics_spread;
		blink::uniform::Env noise_amount;
		blink::uniform::Env noise_color;
	} env;
	struct {
		blink::uniform::SliderReal amp;
		blink::uniform::SliderReal pan;
		blink::uniform::SliderReal pitch;
		blink::uniform::SliderReal speed;
		blink::uniform::SliderReal noise_width;
		blink::uniform::SliderInt sample_offset;
	} slider;
	struct {
		blink::uniform::Option noise_mode;
		blink::uniform::Option reverse_mode;
	} option;
	struct {
		blink::uniform::Option loop;
		blink::uniform::Option reverse;
	} toggle;
	const blink_WarpPoints* warp_points;
};
