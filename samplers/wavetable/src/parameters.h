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

} // envelopes

namespace sliders {

extern blink::SliderParameterSpec<float> noise_width();

} // sliders

} // parameters