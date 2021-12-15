#pragma once

#include <blink/parameters/chord_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/parameters/slider_parameter_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeParameterSpec amp();
extern blink::EnvelopeParameterSpec pan();
extern blink::EnvelopeParameterSpec pitch();
extern blink::EnvelopeParameterSpec speed();
extern blink::EnvelopeParameterSpec grain_size();
extern blink::EnvelopeParameterSpec grain_transpose();
extern blink::EnvelopeParameterSpec uniformity();
extern blink::EnvelopeParameterSpec harmonics_amount();
extern blink::EnvelopeParameterSpec harmonics_spread();

} // envelopes

namespace sliders {

extern blink::SliderParameterSpec<float> noise_width();

} // sliders

extern blink::ChordSpec harmonics_scale();

} // parameters