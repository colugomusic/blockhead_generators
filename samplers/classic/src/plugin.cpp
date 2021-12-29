#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace classic {

Plugin* g_plugin = nullptr;

} // classic

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "bd64e4c8-f788-433b-a42a-d375afd92503";
	out.name = "Classic";
	out.version = PLUGIN_VERSION;

	return out;
}

blink_Error blink_init()
{
	if (classic::g_plugin) return blink_StdError_AlreadyInitialized;

	classic::g_plugin = new classic::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!classic::g_plugin) return blink_StdError_NotInitialized;

	delete classic::g_plugin;

	return BLINK_OK;
}

blink_SamplerInstance blink_make_sampler_instance()
{
	if (!classic::g_plugin) return blink_SamplerInstance{ 0 };

	return bind::sampler_instance(classic::g_plugin->add_instance());
}

blink_Error blink_destroy_sampler_instance(blink_SamplerInstance instance)
{
	if (!classic::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (classic::Instance*)(instance.proc_data);

	classic::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

blink_Bool blink_sampler_enable_warp_markers()
{
	return BLINK_TRUE;
}

blink_Bool blink_sampler_requires_preprocessing()
{
	return BLINK_FALSE;
}

blink_Error blink_sampler_preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	return blink_StdError_NotImplemented;
}

blink_Error blink_sampler_sample_deleted(blink_ID sample_id)
{
	return blink_StdError_NotImplemented;
}

int blink_get_num_groups()
{
	if (!classic::g_plugin) return 0;

	return classic::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!classic::g_plugin) return 0;

	return classic::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(classic::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(classic::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(classic::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

blink_Error blink_sampler_draw(const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	if (!classic::g_plugin) return blink_StdError_NotInitialized;

	classic::g_plugin->gui().draw(*classic::g_plugin, *buffer, *unit_state, n, out);

	return BLINK_OK;
}