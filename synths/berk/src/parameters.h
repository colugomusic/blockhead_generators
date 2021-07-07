#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeSpec amp();
extern blink::EnvelopeSpec pan();
extern blink::EnvelopeSpec pitch();
extern blink::EnvelopeSpec buzz();
extern blink::EnvelopeSpec index();
extern blink::EnvelopeSpec diameter();
extern blink::EnvelopeSpec tongue_index();
extern blink::EnvelopeSpec tongue_diameter();
extern blink::EnvelopeSpec fricative_intensity();
extern blink::EnvelopeSpec quality();

} // envelopes
} // parameters