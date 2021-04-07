#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

#include "audio.h"
#include "convert.h"

using namespace blink;

namespace grain_size {

inline auto stepify(float v) -> float
{
	return v;
}

inline auto snap_value(float v, float step_size, float snap_amount)
{
	return stepify(std_params::snap_value(v, step_size, snap_amount));
}

inline float constrain(float v)
{
	return std::clamp(v, 0.0f, 1.0f);
};

inline auto increment(float v, bool precise)
{
	return constrain(stepify(std_params::increment<100, 1000>(v, precise)));
};

inline auto decrement(float v, bool precise)
{
	return constrain(stepify(std_params::decrement<100, 1000>(v, precise)));
};

inline auto drag(float v, int amount, bool precise) -> float
{
	return constrain(stepify(std_params::drag<100, 1000>(v, amount / 5, precise)));
};

inline auto display(float v)
{
	std::stringstream ss;

	ss << convert::linear_to_ms(v) << " ms";

	return ss.str();
}

inline auto from_string(const std::string& str) -> std::optional<float>
{
	auto value = std_params::find_number<float>(str);

	if (!value) return std::optional<float>();

	return convert::ms_to_linear(*value);
}

}

namespace sliders {

inline SliderSpec<float> grain_size()
{
	SliderSpec<float> out;

	out.constrain = grain_size::constrain;
	out.increment = grain_size::increment;
	out.decrement = grain_size::decrement;
	out.drag = grain_size::drag;
	out.display_value = grain_size::display;
	out.from_string = grain_size::from_string;
	out.default_value = 0.5f;

	return out;
}

} // sliders

namespace envelopes {

inline EnvelopeSpec grain_size()
{
	EnvelopeSpec out;

	out.uuid = "2a9c7ec6-68b4-40c1-aad9-b3c010cd9717";
	out.name = "Size";

	out.default_value = 0.5f;
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.stepify = grain_size::stepify;

	out.value_slider = sliders::grain_size();

	out.range.min.default_value = 0.0f;
	out.range.min.display_value = grain_size::display;
	out.range.max.default_value = 1.0f;
	out.range.max.display_value = grain_size::display;
	out.display_value = grain_size::display;

	return out;
}

} // envelopes

Fudge::Fudge()
{
	auto spec_sld_noise_width = std_params::sliders::parameters::noise_width();

	spec_sld_noise_width.flags = blink_SliderFlags_NonGlobal;

	option_noise_mode_ = add_parameter(std_params::options::noise_mode());
	sld_noise_width_ = add_parameter(spec_sld_noise_width);

	auto spec_env_amp = std_params::envelopes::amp();
	auto spec_env_pan = std_params::envelopes::pan();
	auto spec_env_pitch = std_params::envelopes::pitch();
	auto spec_env_speed = std_params::envelopes::speed();

	spec_env_amp.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_pan.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_speed.flags |= blink_EnvelopeFlags_DefaultActive;

	env_amp_ = add_parameter(spec_env_amp);
	env_pan_ = add_parameter(spec_env_pan);
	env_pitch_ = add_parameter(spec_env_pitch);
	env_speed_ = add_parameter(spec_env_speed);

	auto group_geometry = add_group("Geometry");
	auto spec_env_grain_size = envelopes::grain_size();

	spec_env_grain_size.group_index = group_geometry;

	env_grain_size_ = add_parameter(spec_env_grain_size);

	auto group_noise = add_group("Noise");
	auto spec_env_noise_amount = std_params::envelopes::noise_amount();
	auto spec_env_noise_color = std_params::envelopes::noise_color();

	spec_env_noise_amount.group_index = group_noise;
	spec_env_noise_color.group_index = group_noise;

	spec_env_noise_amount.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
	spec_env_noise_color.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
	spec_env_noise_amount.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));
	spec_env_noise_color.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));

	env_noise_amount_ = add_parameter(spec_env_noise_amount);
	env_noise_color_ = add_parameter(spec_env_noise_color);

	sld_amp_ = add_parameter(std_params::sliders::parameters::amp());
	sld_pan_ = add_parameter(std_params::sliders::parameters::pan());
	sld_pitch_ = add_parameter(std_params::sliders::parameters::pitch());
	sld_speed_ = add_parameter(std_params::sliders::parameters::speed());
	sld_sample_offset_ = add_parameter(std_params::sliders::parameters::sample_offset());

	tog_loop_ = add_parameter(std_params::toggles::loop());
	tog_revers_ = add_parameter(std_params::toggles::reverse());

}

GUI& Fudge::gui()
{
	return gui_;
}

const SampleAnalysis* Fudge::get_analysis_data(blink_ID sample_id) const
{
	const auto pos = sample_analysis_.find(sample_id);

	if (pos == sample_analysis_.end()) return nullptr;

	return pos->second.get();
}

void Fudge::preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	auto pos = sample_analysis_.find(sample_info->id);

	if (pos != sample_analysis_.end()) return;
 
	auto analysis = std::make_shared<SampleAnalysis>();

	if (analyze(host, callbacks, *sample_info, analysis.get()))
	{
		sample_analysis_[sample_info->id] = analysis;
	}
}

void Fudge::on_sample_deleted(blink_ID id)
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

Fudge* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid()
{
	return Fudge::UUID;
}

blink_UUID blink_get_plugin_name()
{
	return Fudge::NAME;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Fudge();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Sampler blink_make_sampler()
{
	if (!g_plugin) return blink_Sampler { 0 };

	return bind::make_sampler<Audio>(g_plugin);
}

blink_Error blink_destroy_sampler(blink_Sampler sampler)
{
	return bind::destroy_sampler(sampler);
}

blink_Bool blink_sampler_requires_preprocessing()
{
	return Audio::REQUIRES_PREPROCESS ? BLINK_TRUE : BLINK_FALSE;
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
	return bind::parameter(g_plugin->get_parameter_by_uuid(uuid));
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

blink_Error blink_sampler_get_waveform_positions(const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp)
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	g_plugin->gui().get_waveform_positions(g_plugin, buffer, n, out, derivatives, amp);

	return BLINK_OK;
}