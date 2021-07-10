#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace freeze {

Instance::Instance(Plugin* plugin)
	: blink::EffectInstance(plugin)
	, plugin_(plugin)
{
}

std::shared_ptr<blink::EffectUnit> Instance::make_unit()
{
	return std::make_shared<Audio>(this);
}

void Instance::reset()
{
	buffer.clear();
	master_unit = nullptr;
}

} // freeze