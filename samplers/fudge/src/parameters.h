#pragma once

#include <blink/chord_spec.hpp>
#include <blink/envelope_spec.hpp>

namespace parameters {
namespace envelopes {

extern blink::EnvelopeSpec grain_size();
extern blink::EnvelopeSpec grain_transpose();
extern blink::EnvelopeSpec uniformity();

} // envelopes

extern blink::ChordSpec grain_harmonics_scale();

} // paramaters