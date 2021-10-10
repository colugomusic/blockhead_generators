#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

#include "instance.h"
#include "parameters.h"

using namespace blink;

namespace fudge {

Plugin::Plugin()
{
	option_noise_mode_ = add_parameter(std_params::options::noise_mode());
	sld_noise_width_ = add_parameter(parameters::sliders::noise_width());

	env_amp_ = add_parameter(parameters::envelopes::amp());
	env_pan_ = add_parameter(parameters::envelopes::pan());
	env_pitch_ = add_parameter(parameters::envelopes::pitch());
	env_speed_ = add_parameter(parameters::envelopes::speed());

	auto group_geometry = add_group("Geometry");
	{
		env_grain_size_ = add_parameter(parameters::envelopes::grain_size());
		env_grain_transpose_ = add_parameter(parameters::envelopes::grain_transpose());
		env_uniformity_ = add_parameter(parameters::envelopes::uniformity());

		env_grain_size_->set_group_index(group_geometry);
		env_grain_transpose_->set_group_index(group_geometry);
		env_uniformity_->set_group_index(group_geometry);
	}

	auto group_harmonics = add_group("Harmonics");
	{
		chord_harmonics_scale_ = add_parameter(parameters::harmonics_scale());
		env_harmonics_amount_ = add_parameter(parameters::envelopes::harmonics_amount());
		env_harmonics_spread_ = add_parameter(parameters::envelopes::harmonics_spread());

		chord_harmonics_scale_->set_group_index(group_harmonics);
		env_harmonics_amount_->set_group_index(group_harmonics);
		env_harmonics_spread_->set_group_index(group_harmonics);
	}

	auto group_noise = add_group("Noise");
	{
		auto spec_env_noise_amount = std_params::envelopes::noise_amount();
		auto spec_env_noise_color = std_params::envelopes::noise_color();

		spec_env_noise_amount.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_color.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_amount.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));
		spec_env_noise_color.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));

		env_noise_amount_ = add_parameter(spec_env_noise_amount);
		env_noise_color_ = add_parameter(spec_env_noise_color);

		env_noise_amount_->set_group_index(group_noise);
		env_noise_color_->set_group_index(group_noise);
	}

	sld_amp_ = add_parameter(std_params::sliders::parameters::amp());
	sld_pan_ = add_parameter(std_params::sliders::parameters::pan());
	sld_pitch_ = add_parameter(std_params::sliders::parameters::pitch());
	sld_speed_ = add_parameter(std_params::sliders::parameters::speed());
	sld_sample_offset_ = add_parameter(std_params::sliders::parameters::sample_offset());
	tog_loop_ = add_parameter(std_params::toggles::loop());
	tog_revers_ = add_parameter(std_params::toggles::reverse());
}

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

Plugin* g_plugin = nullptr;

}

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "795c8dc6-3b81-4397-abac-071bab83b10f";
	out.name = "Fudge";
	out.version = PLUGIN_VERSION;

	return out;
}

blink_Error blink_init()
{
	if (fudge::g_plugin) return blink_StdError_AlreadyInitialized;

    fudge::g_plugin = new fudge::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!fudge::g_plugin) return blink_StdError_NotInitialized;

	delete fudge::g_plugin;

	return BLINK_OK;
}

blink_SamplerInstance blink_make_sampler_instance()
{
	if (!fudge::g_plugin) return blink_SamplerInstance{ 0 };

	return bind::sampler_instance(fudge::g_plugin->add_instance());
}

blink_Error blink_destroy_sampler_instance(blink_SamplerInstance instance)
{
	if (!fudge::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (fudge::Instance*)(instance.proc_data);

	fudge::g_plugin->destroy_instance(obj);

	return BLINK_OK;
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
	if (!fudge::g_plugin) return blink_StdError_NotInitialized;

    fudge::g_plugin->preprocess_sample(host, callbacks, sample_info);

	return BLINK_OK;
}

blink_Error blink_sampler_sample_deleted(blink_ID sample_id)
{
	if (!fudge::g_plugin) return blink_StdError_NotInitialized;

    fudge::g_plugin->on_sample_deleted(sample_id);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!fudge::g_plugin) return 0;

	return fudge::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!fudge::g_plugin) return 0;

	return fudge::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(fudge::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(fudge::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(fudge::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

blink_Error blink_sampler_draw(const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	if (!fudge::g_plugin) return blink_StdError_NotInitialized;

    fudge::g_plugin->gui().draw(fudge::g_plugin, buffer, n, out);

	return BLINK_OK;
}
