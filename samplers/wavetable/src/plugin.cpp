#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

#include "audio.h"
#include "parameters.h"

using namespace blink;

Wavetable::Wavetable()
{
	option_noise_mode_ = add_parameter(std_params::options::noise_mode());
	sld_noise_width_ = add_parameter(parameters::sliders::noise_width());

	env_amp_ = add_parameter(parameters::envelopes::amp());
	env_pan_ = add_parameter(parameters::envelopes::pan());
	env_pitch_ = add_parameter(parameters::envelopes::pitch());
	env_speed_ = add_parameter(parameters::envelopes::speed());

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

GUI& Wavetable::gui()
{
	return gui_;
}

const SampleAnalysis* Wavetable::get_analysis_data(blink_ID sample_id) const
{
	const auto pos = sample_analysis_.find(sample_id);

	if (pos == sample_analysis_.end()) return nullptr;

	return pos->second.get();
}

void Wavetable::preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	auto pos = sample_analysis_.find(sample_info->id);

	if (pos != sample_analysis_.end()) return;
 
	auto analysis = std::make_shared<SampleAnalysis>();

	if (analyze(host, callbacks, *sample_info, analysis.get()))
	{
		sample_analysis_[sample_info->id] = analysis;
	}
}

void Wavetable::on_sample_deleted(blink_ID id)
{
	const auto pos = sample_analysis_.find(id);

	if (pos == sample_analysis_.end()) return;

	sample_analysis_.erase(pos);
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Wavetable* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid()
{
	return Wavetable::UUID;
}

blink_UUID blink_get_plugin_name()
{
	return Wavetable::NAME;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Wavetable();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Sampler blink_make_sampler(int instance_group)
{
	if (!g_plugin) return blink_Sampler { 0, 0 };

	return bind::make_sampler<Audio>(g_plugin);
}

blink_Error blink_destroy_sampler(blink_Sampler sampler)
{
	return bind::destroy_sampler(sampler);
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
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	g_plugin->preprocess_sample(host, callbacks, sample_info);

	return BLINK_OK;
}

blink_Error blink_sampler_sample_deleted(blink_ID sample_id)
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

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

const char* blink_get_error_string(blink_Error error)
{
	switch (Error(error))
	{
		case Error::AlreadyInitialized: return "already initialized";
		case Error::NotInitialized: return "not initialized";
		default: return "unknown error";
	}
}

blink_Error blink_sampler_draw(const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	g_plugin->gui().draw(g_plugin, buffer, n, out);

	return BLINK_OK;
}