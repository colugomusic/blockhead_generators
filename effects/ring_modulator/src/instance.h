#pragma once

#include <blink/effect_instance.hpp>

namespace ring_modulator {

class Plugin;

class Instance : public blink::EffectInstance
{
public:

	Instance(Plugin* plugin);

	Plugin* get_plugin() const { return plugin_; }

private:

	std::shared_ptr<blink::EffectUnit> make_unit() override;

	Plugin* plugin_;
};

} // ring_modulator