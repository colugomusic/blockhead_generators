#pragma once

#include <blink/envelope_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeSpec amp();
extern blink::EnvelopeSpec pan();
extern blink::EnvelopeSpec pitch();
extern blink::EnvelopeSpec formant();
extern blink::EnvelopeSpec index();
extern blink::EnvelopeSpec diameter();
extern blink::EnvelopeSpec tongue_index();
extern blink::EnvelopeSpec tongue_diameter();
extern blink::EnvelopeSpec fricative_intensity();

} // envelopes
} // parameters