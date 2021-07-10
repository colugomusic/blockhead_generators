#pragma once

#include <blink/synth_instance.hpp>

namespace test {

class Plugin;

class Instance : public blink::SynthInstance
{
public:

	Instance(Plugin* plugin);

	Plugin* get_plugin() const { return plugin_; }

private:

	std::shared_ptr<blink::SynthUnit> make_unit() override;

	Plugin* plugin_;
};

} // test