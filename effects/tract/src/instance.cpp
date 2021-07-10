#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace tract {

Instance::Instance(Plugin* plugin)
	: blink::EffectInstance(plugin)
	, plugin_(plugin)
	, info_(blink::EffectInstance::get_info())
{
}

std::shared_ptr<blink::EffectUnit> Instance::make_unit()
{
	return std::make_shared<Audio>(this);
}

void Instance::stream_init()
{
	const auto MIN_MODEL_SR = BASE_MODEL_SR / 2;
	const auto MAX_MODEL_SR = BASE_MODEL_SR * 2;

	info_.min_delay = int((float(SR()) / MAX_MODEL_SR) * kFloatsPerDSPVector);
	info_.max_delay = int((float(SR()) / MIN_MODEL_SR) * kFloatsPerDSPVector);
}

}