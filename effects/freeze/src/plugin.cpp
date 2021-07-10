#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace freeze {

Plugin::Plugin()
{
	auto spec_env_pitch = std_params::envelopes::pitch();
	auto spec_env_formant = std_params::envelopes::formant();
	auto spec_env_mix = std_params::envelopes::mix();
	auto spec_sld_pitch = std_params::sliders::parameters::pitch();

	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_formant.flags |= blink_EnvelopeFlags_DefaultActive;

	env_pitch_ = add_parameter(spec_env_pitch);
	env_formant_ = add_parameter(spec_env_formant);
	env_mix_ = add_parameter(spec_env_mix);

	add_parameter(spec_sld_pitch);
}

blink::EffectInstance* Plugin::make_instance()
{
	return new Instance(this);
}

Plugin* g_plugin = nullptr;

} // freeze

blink_UUID blink_get_plugin_uuid() { return freeze::Plugin::UUID; }
blink_UUID blink_get_plugin_name() { return freeze::Plugin::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (freeze::g_plugin) return blink_StdError_AlreadyInitialized;

	freeze::g_plugin = new freeze::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!freeze::g_plugin) return blink_StdError_NotInitialized;

	delete freeze::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!freeze::g_plugin) return blink_StdError_NotInitialized;

	freeze::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!freeze::g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(freeze::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!freeze::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (freeze::Instance*)(instance.proc_data);

	freeze::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!freeze::g_plugin) return 0;

	return freeze::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!freeze::g_plugin) return 0;

	return freeze::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(freeze::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(freeze::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(freeze::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}