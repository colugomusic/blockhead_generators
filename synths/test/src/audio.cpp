/* TODELETE:
#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include <blink/dsp.hpp>

using namespace blink;

namespace test {

Audio::Audio(Instance* instance)
	: SynthUnit(instance)
	, plugin_(instance->get_plugin())
{
	reset();
}

blink_Error Audio::process(const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out) {
}

void Audio::reset()
{
}

} // test
*/