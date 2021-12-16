#include "controller.h"
#include "plugin.h"

namespace freeze {

Controller::Controller(const Plugin* plugin, const FreezeBuffer& buffer, const blink::Traverser& traverser)
	: plugin_(plugin)
	, buffer_(&buffer)
	, traverser_(&traverser)
{
}

void Controller::process(
	const AudioData& data,
	const blink_EffectBuffer& effect_data,
	blink_SR SR)
{
	effect_data_ = &effect_data;

	const auto pitch = data.envelopes.pitch.search_vec(traverser_->block_positions()) + data.sliders.pitch.search_vec(traverser_->block_positions()) + 60.0f;
	const auto formant = data.envelopes.formant.search_vec(traverser_->block_positions());

	ff_ = blink::math::convert::p_to_ff(blink::math::convert::uni_to_bi(formant) * 24.0f);
	size_ = float(SR) / blink::math::convert::p_to_ff(pitch);
}

} // freeze