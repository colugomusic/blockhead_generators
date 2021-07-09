#pragma once

#include <blink_effect.h>
#include <blink/block_positions.hpp>
#include <blink/traverser.hpp>
#include "audio_data.h"
#include "buffer.h"

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

class Freeze;

class Controller
{
public:

	Controller(const Freeze* plugin, const FreezeBuffer& buffer, const blink::Traverser& traverser);

	void process(
		const AudioData& data,
		const blink_EffectBuffer& effect_data,
		blink_SR SR);

	const blink::BlockPositions& block_positions() const { return traverser_->block_positions(); }

	const ml::DSPVectorInt& reset() const { return traverser_->get_resets(); }
	const ml::DSPVector& ff() const { return ff_; }
	const ml::DSPVector& size() const { return size_; }

	const blink_EffectBuffer& effect_data() const { return *effect_data_; }
	const FreezeBuffer& buffer() const { return *buffer_; }

private:

	const Freeze* plugin_;
	const blink_EffectBuffer* effect_data_;
	const blink::Traverser* traverser_;

	ml::DSPVector ff_;
	ml::DSPVector size_;
	const FreezeBuffer* buffer_;
};