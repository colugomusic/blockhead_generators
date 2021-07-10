#include "instance.h"
#include "audio.h"
#include "plugin.h"

namespace berk {

Instance::Instance(Plugin* plugin)
	: blink::SynthInstance(plugin)
	, plugin_(plugin)
{
}

std::shared_ptr<blink::SynthUnit> Instance::make_unit()
{
	return std::make_shared<Audio>(this);
}

} // berk