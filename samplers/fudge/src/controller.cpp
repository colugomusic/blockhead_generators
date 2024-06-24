/* TODELETE:
#include "controller.h"
#include "audio_data.h"
#include "convert.h"
#include <blink/dsp.hpp>
#include <blink/search.hpp>
#include <blink/transform/stretch.hpp>

using namespace blink;

namespace fudge {


float Controller::get_harmonic_ratio(int index, int harmonic) const {
	return snap_ratio_to_scale(index, 1.0f + (float(harmonic + 1) * spread_[index]));
}

float Controller::snap_ratio_to_scale(int index, float ff) const {
	const auto scale = scale_[index]; 
	if (scale == 0) return ff; 
	const auto pitch = blink::math::convert::ff_to_p(ff); 
	return blink::math::convert::p_to_ff(blink::snap_pitch_to_scale(pitch, scale));
}

} // fudge
*/
