#pragma once

#include <blink/effect_instance.hpp>

namespace gainer {

class Plugin;

class Instance : public blink::EffectInstance
{
public:

	Instance(Plugin* plugin);

	auto get_plugin() const { return plugin_; }

private:

	auto make_unit() -> std::shared_ptr<blink::EffectUnit> override;

	Plugin* plugin_;
};

} // gainer