#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace lofi {

Instance::Instance(Plugin* plugin)
	: blink::EffectInstance(plugin)
	, plugin_(plugin)
{
}

std::shared_ptr<blink::EffectUnit> Instance::make_unit()
{
	return std::make_shared<Audio>(this);
}

} // lofi