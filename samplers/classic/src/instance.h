#pragma once

#include <blink/sampler_instance.hpp>

namespace classic {

class Plugin;

class Instance : public blink::SamplerInstance
{
public:

	Instance(Plugin* plugin);

	Plugin* get_plugin() const { return plugin_; }

private:

	std::shared_ptr<blink::SamplerUnit> make_unit() override;

	Plugin* plugin_;
};

} // classic