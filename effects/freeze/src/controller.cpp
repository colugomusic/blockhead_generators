#include "controller.h"
#include "plugin.h"

namespace freeze {

Controller::Controller(const Plugin* plugin, const FreezeBuffer& buffer)
	: plugin_(plugin)
	, buffer_(&buffer)
{
}

void Controller::process(
	const AudioData& data,
	const blink_EffectBuffer& effect_data,
	blink_SR SR,
	const blink::BlockPositions& block_positions)
{
	block_positions_ = &block_positions;
	effect_data_ = &effect_data;

	const auto pitch = data.envelopes.pitch.search_vec(block_positions) + data.sliders.pitch.search_vec(block_positions) + 60.0f;
	const auto formant = data.envelopes.formant.search_vec(block_positions);

	ff_ = blink::math::convert::p_to_ff(formant * 24.0f);
	size_ = float(SR) / blink::math::convert::p_to_ff(pitch);
}

} // freeze