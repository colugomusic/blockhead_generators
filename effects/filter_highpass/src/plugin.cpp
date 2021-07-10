#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace highpass {

Plugin::Plugin()
{
	auto spec_env_freq = std_params::envelopes::filter_frequency();
	auto spec_env_res = std_params::envelopes::resonance();
	auto spec_env_mix = std_params::envelopes::mix();

	spec_env_freq.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_res.flags |= blink_EnvelopeFlags_DefaultActive;

	env_freq_ = add_parameter(spec_env_freq);
	env_res_ = add_parameter(spec_env_res);
	env_mix_ = add_parameter(spec_env_mix);
}

blink::EffectInstance* Plugin::make_instance()
{
	return new Instance(this);
}

Plugin* g_plugin = nullptr;

} // highpasss

blink_UUID blink_get_plugin_uuid() { return highpass::Plugin::UUID; }
blink_UUID blink_get_plugin_name() { return highpass::Plugin::NAME; }
const char* blink_get_plugin_category() { return BLINK_STD_CATEGORY_FILTERS; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (highpass::g_plugin) return blink_StdError_AlreadyInitialized;

	highpass::g_plugin = new highpass::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!highpass::g_plugin) return blink_StdError_NotInitialized;

	delete highpass::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!highpass::g_plugin) return blink_StdError_NotInitialized;

	highpass::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!highpass::g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(highpass::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!highpass::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (highpass::Instance*)(instance.proc_data);

	highpass::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!highpass::g_plugin) return 0;

	return highpass::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!highpass::g_plugin) return 0;

	return highpass::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(highpass::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(highpass::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(highpass::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}