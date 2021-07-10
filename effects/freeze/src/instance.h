#pragma once

#include <blink/effect_instance.hpp>
#include "buffer.h"

namespace freeze {

class Plugin;
class Audio;

class Instance : public blink::EffectInstance
{
public:

	Instance(Plugin* plugin);

	Plugin* get_plugin() const { return plugin_; }

	Audio* master_unit = nullptr;
	FreezeBuffer buffer;

private:

	std::shared_ptr<blink::EffectUnit> make_unit() override;
	void reset() override;

	Plugin* plugin_;
};

} // freeze