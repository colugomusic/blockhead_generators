#pragma once

#include <blink/chord_spec.hpp>
#include <blink/envelope_spec.hpp>
#include <blink/slider_parameter_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeSpec amp();
extern blink::EnvelopeSpec pan();
extern blink::EnvelopeSpec pitch();
extern blink::EnvelopeSpec speed();
extern blink::EnvelopeSpec grain_size();
extern blink::EnvelopeSpec grain_transpose();
extern blink::EnvelopeSpec uniformity();
extern blink::EnvelopeSpec harmonics_amount();
extern blink::EnvelopeSpec harmonics_spread();

} // envelopes

namespace sliders {

extern blink::SliderParameterSpec<float> noise_width();

} // sliders

extern blink::ChordSpec harmonics_scale();

} // parameters