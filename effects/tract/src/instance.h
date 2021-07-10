#pragma once

#include <blink/effect_instance.hpp>

namespace tract {

class Plugin;

class Instance : public blink::EffectInstance
{
public:

	Instance(Plugin* plugin);

	Plugin* get_plugin() const { return plugin_; }

private:

	std::shared_ptr<blink::EffectUnit> make_unit() override;
	void stream_init() override;
	blink_EffectInstanceInfo get_info() const override { return info_; }

	Plugin* plugin_;
	blink_EffectInstanceInfo info_;
};

} // tract