#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace wavebender {

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
}

} // wavebender