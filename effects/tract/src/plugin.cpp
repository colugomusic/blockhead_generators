#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>

using namespace blink;

namespace tract {

Plugin::Plugin()
	: params_(this)
{
}

blink::EffectInstance* Plugin::make_instance()
{
	return new Instance(this);
}

Plugin* g_plugin = nullptr;

} // tract

blink_UUID blink_get_plugin_uuid() { return tract::Plugin::UUID; }
blink_UUID blink_get_plugin_name() { return tract::Plugin::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (tract::g_plugin) return blink_StdError_AlreadyInitialized;

	tract::g_plugin = new tract::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!tract::g_plugin) return blink_StdError_NotInitialized;

	delete tract::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!tract::g_plugin) return blink_StdError_NotInitialized;

	tract::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!tract::g_plugin) return blink_EffectInstance{0};

	return bind::effect_instance(tract::g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!tract::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (tract::Instance*)(instance.proc_data);

	tract::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!tract::g_plugin) return 0;

	return tract::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!tract::g_plugin) return 0;

	return tract::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(tract::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(tract::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(tract::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}