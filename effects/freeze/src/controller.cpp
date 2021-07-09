#include "controller.h"
#include "plugin.h"


Controller::Controller(const Freeze* plugin, const FreezeBuffer& buffer, const blink::Traverser& traverser)
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

	const auto pitch = (plugin_->env_pitch().search_vec(data.env_pitch, traverser_->block_positions()) + data.slider_pitch->value) + 60.0f;
	const auto formant = plugin_->env_formant().search_vec(data.env_formant, traverser_->block_positions());

	ff_ = blink::math::convert::p_to_ff(blink::math::convert::uni_to_bi(formant) * 24.0f);
	size_ = float(SR) / blink::math::convert::p_to_ff(pitch);
}
