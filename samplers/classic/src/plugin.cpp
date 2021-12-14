#define BLINK_EXPORT
#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace classic {

Plugin::Plugin()
{
	auto spec_sld_noise_width = std_params::noise_width::slider_parameter();

	spec_sld_noise_width.flags = blink_SliderFlags_NonGlobal;

	option_noise_mode_ = add_parameter(std_params::noise_mode::option());
	sld_noise_width_ = add_parameter(spec_sld_noise_width);

	auto spec_env_amp = std_params::amp::envelope_parameter();
	auto spec_env_pan = std_params::pan::envelope_parameter();
	auto spec_env_pitch = std_params::pitch::envelope_parameter();

	spec_env_amp.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;

	env_amp_ = add_parameter(spec_env_amp);
	env_pan_ = add_parameter(spec_env_pan);
	env_pitch_ = add_parameter(spec_env_pitch);

	auto group_noise = add_group("Noise");
	{
		auto spec_env_noise_amount = std_params::noise_amount::envelope_parameter();
		auto spec_env_noise_color = std_params::noise_color::envelope_parameter();

		spec_env_noise_amount.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_color.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_amount.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));
		spec_env_noise_color.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));

		env_noise_amount_ = add_parameter(spec_env_noise_amount);
		env_noise_color_ = add_parameter(spec_env_noise_color);

		env_noise_amount_->set_group_index(group_noise);
		env_noise_color_->set_group_index(group_noise);
	}

	sld_amp_ = add_parameter(std_params::amp::slider_parameter());
	sld_pan_ = add_parameter(std_params::pan::slider_parameter());
	sld_pitch_ = add_parameter(std_params::pitch::slider_parameter());
	sld_sample_offset_ = add_parameter(std_params::sample_offset::slider_parameter());

	tog_loop_ = add_parameter(std_params::loop::toggle());
	tog_revers_ = add_parameter(std_params::reverse::toggle());
}

GUI& Plugin::gui()
{
	return gui_;
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
	NotImplemented,
};

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

blink_Error blink_sampler_draw(const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	if (!classic::g_plugin) return blink_StdError_NotInitialized;

	classic::g_plugin->gui().draw(classic::g_plugin, buffer, n, out);

	return BLINK_OK;
}