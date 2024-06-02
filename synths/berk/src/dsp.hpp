#pragma once

#include <blink/dsp.hpp>
#include <blink/search.hpp>
#include "model.h"

namespace dsp {

auto process(Model* model, UnitDSP* unit_dsp, const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out) -> blink_Error {
	// TODO:
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	// TODO:
}

} // dsp