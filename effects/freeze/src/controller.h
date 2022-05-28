#pragma once

#include <blink_effect.h>
#include <blink/block_positions.hpp>
#include <blink/traverser.hpp>
#include "audio_data.h"
#include "buffer.h"

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

namespace freeze {

class Plugin;

class Controller
{
public:

	Controller(const Plugin* plugin, const FreezeBuffer& buffer);

	auto process(const AudioData& data, const blink_EffectBuffer& effect_data, blink_SR SR, const blink::BlockPositions& block_positions) -> void;

	auto& block_positions() const { return *block_positions_; }

	auto& ff() const { return ff_; }
	auto& size() const { return size_; }

	auto& effect_data() const { return *effect_data_; }
	auto& buffer() const { return *buffer_; }

private:

	const Plugin* plugin_;
	const blink_EffectBuffer* effect_data_;
	const blink::BlockPositions* block_positions_;

	ml::DSPVector ff_;
	ml::DSPVector size_;
	const FreezeBuffer* buffer_;
};

} // freeze