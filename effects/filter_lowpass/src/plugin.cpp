#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace lowpass {

Plugin::Plugin()
{
	auto spec_env_freq = std_params::filter_frequency::envelope_parameter();
	auto spec_env_res = std_params::resonance::envelope_parameter();
	auto spec_env_mix = std_params::mix::envelope_parameter();

	spec_env_freq.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_res.flags |= blink_EnvelopeFlags_DefaultActive;

	env_freq_ = add_parameter(spec_env_freq);
	env_res_ = add_parameter(spec_env_res);
	env_mix_ = add_parameter(spec_env_mix);
}

Plugin* g_plugin = nullptr;

} // lowpass

using namespace lowpass;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "3bb94b4c-ec38-4557-8666-8fe2e446b83c";
	out.name = "Lowpass";
	out.category = BLINK_STD_CATEGORY_FILTERS;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (lowpass::g_plugin) return blink_StdError_AlreadyInitialized;

	lowpass::g_plugin = new lowpass::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!lowpass::g_plugin) return blink_StdError_NotInitialized;

	delete lowpass::g_plugin;

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!lowpass::g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(lowpass::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!lowpass::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (lowpass::Instance*)(instance.proc_data);

	lowpass::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!lowpass::g_plugin) return 0;

	return lowpass::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!lowpass::g_plugin) return 0;

	return lowpass::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(lowpass::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(lowpass::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(lowpass::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

CMRC_DECLARE(filter_lowpass);

blink_ResourceData blink_get_resource_data(const char* path)
{
	if (g_plugin->resources().has(path)) return g_plugin->resources().get(path);

	const auto fs = cmrc::filter_lowpass::get_filesystem();

	if (!fs.exists(path)) return { 0, 0 };
	if (!fs.is_file(path)) return { 0, 0 };

	const auto file = fs.open(path);

	return g_plugin->resources().store(path, file);
}