#define BLINK_EXPORT
#include "plugin.h"
#include <blink/bind_sampler.hpp>
#include <blink/errors.hpp>

namespace fudge {

GUI& Plugin::gui()
{
	return gui_;
}

const SampleAnalysis* Plugin::get_analysis_data(blink_ID sample_id) const
{
	const auto pos = sample_analysis_.find(sample_id);

	if (pos == sample_analysis_.end()) return nullptr;

	return pos->second.get();
}

void Plugin::preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	auto pos = sample_analysis_.find(sample_info->id);

	if (pos != sample_analysis_.end()) return;
 
	auto analysis = std::make_shared<SampleAnalysis>();

	if (analyze(host, callbacks, *sample_info, analysis.get()))
	{
		sample_analysis_[sample_info->id] = analysis;
	}
}

void Plugin::on_sample_deleted(blink_ID id)
{
	const auto pos = sample_analysis_.find(id);

	if (pos == sample_analysis_.end()) return;

	sample_analysis_.erase(pos);
}

Plugin* g_plugin {};

}

using namespace blink;
using namespace fudge;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out {0};

	out.uuid = "795c8dc6-3b81-4397-abac-071bab83b10f";
	out.name = "Fudge";
	out.version = PLUGIN_VERSION;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

    g_plugin = new fudge::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	delete g_plugin;

	return BLINK_OK;
}

blink_SamplerInstance blink_make_sampler_instance()
{
	if (!g_plugin) return blink_SamplerInstance{ 0 };

	return bind::sampler_instance(g_plugin->add_instance());
}

blink_Error blink_destroy_sampler_instance(blink_SamplerInstance instance)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	return g_plugin->destroy_instance(std::move(instance));
}

blink_Bool blink_sampler_enable_warp_markers()
{
	return BLINK_TRUE;
}

blink_Bool blink_sampler_requires_preprocessing()
{
	return BLINK_TRUE;
}

blink_Error blink_sampler_preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

    g_plugin->preprocess_sample(host, callbacks, sample_info);

	return BLINK_OK;
}

blink_Error blink_sampler_sample_deleted(blink_ID sample_id)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

    g_plugin->on_sample_deleted(sample_id);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(g_plugin->get_parameter(uuid));
}

blink_Error blink_get_envelope_manipulator_target(blink_UUID uuid, blink_EnvelopeManipulatorTarget* out)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	return g_plugin->get_envelope_manipulator_target(uuid, out);
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

blink_Error blink_sampler_draw(const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

    g_plugin->gui().draw(*g_plugin, *buffer, *unit_state, n, out);

	return BLINK_OK;
}
