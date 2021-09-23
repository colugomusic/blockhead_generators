#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace ring_modulator {

Plugin::Plugin()
{
	auto spec_env_pitch = std_params::envelopes::pitch();
	auto spec_env_mix = std_params::envelopes::mix();

	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;

	env_pitch_ = add_parameter(spec_env_pitch);
	env_mix_ = add_parameter(spec_env_mix);
}

Plugin* g_plugin = nullptr;

} // ring_modulator

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "881569c5-108b-4923-b30e-1e1ac6c1a04f";
	out.name = "Ring Modulator";
	out.category = BLINK_STD_CATEGORY_DESTRUCTION;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (ring_modulator::g_plugin) return blink_StdError_AlreadyInitialized;

	ring_modulator::g_plugin = new ring_modulator::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!ring_modulator::g_plugin) return blink_StdError_NotInitialized;

	delete ring_modulator::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!ring_modulator::g_plugin) return blink_StdError_NotInitialized;

	ring_modulator::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!ring_modulator::g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(ring_modulator::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!ring_modulator::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (ring_modulator::Instance*)(instance.proc_data);

	ring_modulator::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!ring_modulator::g_plugin) return 0;

	return ring_modulator::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!ring_modulator::g_plugin) return 0;

	return ring_modulator::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(ring_modulator::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(ring_modulator::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(ring_modulator::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}