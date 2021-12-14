#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace freeze {

Plugin::Plugin()
{
	auto spec_env_pitch = std_params::pitch::envelope_parameter();
	auto spec_env_formant = std_params::formant::envelope_parameter();
	auto spec_env_mix = std_params::mix::envelope_parameter();
	auto spec_sld_pitch = std_params::pitch::slider_parameter();

	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_formant.flags |= blink_EnvelopeFlags_DefaultActive;

	env_pitch_ = add_parameter(spec_env_pitch);
	env_formant_ = add_parameter(spec_env_formant);
	env_mix_ = add_parameter(spec_env_mix);

	add_parameter(spec_sld_pitch);
}

Plugin* g_plugin = nullptr;

} // freeze

using namespace freeze;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "cde37ae4-a943-4b2c-b792-3eeba8107a6b";
	out.name = "Freeze";
	out.category = BLINK_STD_CATEGORY_DESTRUCTION;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

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

CMRC_DECLARE(freeze);

blink_ResourceData blink_get_resource_data(const char* path)
{
	if (g_plugin->resources().has(path)) return g_plugin->resources().get(path);

	const auto fs = cmrc::freeze::get_filesystem();

	if (!fs.exists(path)) return { 0, 0 };
	if (!fs.is_file(path)) return { 0, 0 };

	const auto file = fs.open(path);

	return g_plugin->resources().store(path, file);
}