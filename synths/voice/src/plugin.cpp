#define BLINK_EXPORT

#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

Voice::Voice()
{
	//const auto get_index = [](float x, float y)
	//{
	//	x = 340.0f - x;
	//	y = 449.0f - y;
	//	auto angle = std::atan2(x, y);
	//	while (angle > 0.0f) angle -= 2.0f * float(M_PI);

	//	return (float(M_PI) + angle - (-0.24f)) * (38) / (0.64f * float(M_PI));
	//};

	//float test;

	//test = get_index(390.0f, 400.0f);
	//test = get_index(390.0f, 410.0f);
	//test = get_index(380.0f, 420.0f);
	//test = get_index(370.0f, 430.0f);
	//test = get_index(360.0f, 440.0f);
	//test = get_index(350.0f, 450.0f);
	//test = get_index(350.0f, 460.0f);
	//test = get_index(350.0f, 470.0f);
	//test = get_index(370.0f, 400.0f);
	//test = get_index(360.0f, 410.0f);
	//test = get_index(350.0f, 420.0f);
	//test = get_index(340.0f, 430.0f);
	//test = get_index(530.0f, 440.0f);
	//test = get_index(520.0f, 550.0f);
	//test = get_index(310.0f, 560.0f);
	//test = get_index(370.0f, 570.0f);

	auto spec_env_amp = std_params::envelopes::amp();
	
	spec_env_amp.default_value = 0.5f;
	spec_env_amp.flags |= blink_EnvelopeFlags_DefaultActive;

	env_amp_ = add_parameter(spec_env_amp);

	auto spec_env_formant = std_params::envelopes::generic::linear(0.0f, 1.0f, 0.5f);

	spec_env_formant.uuid = "d77e8d5a-c69f-4d97-a35e-0d063a18f9ba";
	spec_env_formant.name = "formant";
	spec_env_formant.flags |= blink_EnvelopeFlags_DefaultActive;

	auto spec_env_index = std_params::envelopes::generic::linear(4.0f, 44.0f, 22.0f);

	spec_env_index.uuid = "64e246db-0d4d-4dc2-9d34-78a3a23070bc";
	spec_env_index.name = "index";
	spec_env_index.flags |= blink_EnvelopeFlags_DefaultActive;

	auto spec_env_diameter = std_params::envelopes::generic::linear(-1.0f, 3.0f, 1.0f);

	spec_env_diameter.uuid = "ac0a8acd-29a4-42af-b75b-1d7ee2684b25";
	spec_env_diameter.name = "diameter";
	spec_env_diameter.flags |= blink_EnvelopeFlags_DefaultActive;

	auto spec_env_tongue_index = std_params::envelopes::generic::linear(12.0f, 29.0f, 12.9f);

	spec_env_tongue_index.uuid = "1059abc4-3a1c-42cc-a21b-235b2c08775b";
	spec_env_tongue_index.name = "tongue index";
	spec_env_tongue_index.flags |= blink_EnvelopeFlags_DefaultActive;

	auto spec_env_tongue_diameter = std_params::envelopes::generic::linear(2.05f, 3.5f, 2.43f);

	spec_env_tongue_diameter.uuid = "a14bfe06-b84d-4778-b50c-7290478cb6b3";
	spec_env_tongue_diameter.name = "tongue diameter";
	spec_env_tongue_diameter.flags |= blink_EnvelopeFlags_DefaultActive;

	auto spec_env_fricative_intensity = std_params::envelopes::generic::linear(0.0f, 1.0f, 1.0f);

	spec_env_fricative_intensity.uuid = "53c94f7f-d6b7-497e-8deb-9acd35f0d60e";
	spec_env_fricative_intensity.name = "fricative_intensity";
	spec_env_fricative_intensity.flags |= blink_EnvelopeFlags_DefaultActive;

	env_formant_ = add_parameter(spec_env_formant);
	env_index_ = add_parameter(spec_env_index);
	env_diameter_ = add_parameter(spec_env_diameter);
	env_tongue_index_ = add_parameter(spec_env_tongue_index);
	env_tongue_diameter_ = add_parameter(spec_env_tongue_diameter);
	env_fricative_intensity_ = add_parameter(spec_env_fricative_intensity);
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Voice* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid() { return Voice::UUID; }
blink_UUID blink_get_plugin_name() { return Voice::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Voice();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Synth blink_make_synth(int instance_group)
{
	if (!g_plugin) return blink_Synth { 0, 0 };

	return bind::make_synth<Audio>(g_plugin, instance_group);
}

blink_Error blink_destroy_synth(blink_Synth synth)
{
	return bind::destroy_synth(synth);
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