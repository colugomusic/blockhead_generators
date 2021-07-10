#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace spring_reverb {

Plugin::Plugin()
{
	// TODO: do this properly
	auto spec_env_size = std_params::envelopes::resonance();
	auto spec_env_decay = std_params::envelopes::resonance();
	auto spec_env_mix = std_params::envelopes::mix();

	spec_env_size.name = "Size";
	spec_env_size.uuid = "65e00902-5318-4d76-9b86-2479dcce7f52";
	spec_env_decay.name = "Decay";
	spec_env_decay.uuid = "aa47aa82-0e26-4d3b-8f60-ddb5d57353e7";
	spec_env_size.default_value = 0.5f;
	spec_env_decay.default_value = 0.5f;
	spec_env_mix.default_value = 0.25f;

	spec_env_size.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_decay.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_mix.flags |= blink_EnvelopeFlags_DefaultActive;

	env_size_ = add_parameter(spec_env_size);
	env_decay_ = add_parameter(spec_env_decay);
	env_mix_ = add_parameter(spec_env_mix);
}

blink::EffectInstance* Plugin::make_instance()
{
	return new Instance(this);
}

Plugin* g_plugin = nullptr;

} // spring_reverb

blink_UUID blink_get_plugin_uuid() { return spring_reverb::Plugin::UUID; }
blink_UUID blink_get_plugin_name() { return spring_reverb::Plugin::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (spring_reverb::g_plugin) return blink_StdError_AlreadyInitialized;

	spring_reverb::g_plugin = new spring_reverb::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!spring_reverb::g_plugin) return blink_StdError_NotInitialized;

	delete spring_reverb::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!spring_reverb::g_plugin) return blink_StdError_NotInitialized;

	spring_reverb::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!spring_reverb::g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(spring_reverb::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!spring_reverb::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (spring_reverb::Instance*)(instance.proc_data);

	spring_reverb::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!spring_reverb::g_plugin) return 0;

	return spring_reverb::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!spring_reverb::g_plugin) return 0;

	return spring_reverb::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(spring_reverb::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(spring_reverb::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(spring_reverb::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}