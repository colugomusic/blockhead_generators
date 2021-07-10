#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace fudge {

Instance::Instance(Plugin* plugin)
	: blink::SamplerInstance(plugin)
	, plugin_(plugin)
{
}

std::shared_ptr<blink::SamplerUnit> Instance::make_unit()
{
	return std::make_shared<Audio>(this);
}

} // fudge