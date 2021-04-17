#pragma once

#include <blink/chord_spec.hpp>
#include <blink/envelope_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeSpec grain_size();
extern blink::EnvelopeSpec grain_transpose();
extern blink::EnvelopeSpec uniformity();
extern blink::EnvelopeSpec harmonics_amount();
extern blink::EnvelopeSpec harmonics_spread();

} // envelopes

extern blink::ChordSpec harmonics_scale();

} // paramaters