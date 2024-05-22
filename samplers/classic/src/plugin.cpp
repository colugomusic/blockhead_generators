#define BLINK_EXPORT
#include <blink/blink_plugin_impl.hpp>
//#include <blink/bind_sampler.hpp>
//#include <blink/errors.hpp>

//namespace classic { Plugin* g_plugin {}; }

static blink::Plugin plugin;

using namespace blink;

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid                = {"bd64e4c8-f788-433b-a42a-d375afd92503"};
	out.name                = {"Classic"};
	out.version             = {PLUGIN_VERSION};
	out.enable_warp_markers = {true};
	return out;
}

auto blink_get_sampler_info() -> blink_SamplerInfo {
	blink_SamplerInfo out = {0};
	out.baked_waveform_could_be_different = {false};
	out.requires_preprocessing            = {false};
	return out;
}

auto blink_init() -> blink_Error {
	//if (model) return blink_StdError_AlreadyInitialized;
	//model = std::make_unique<Model>();
	return BLINK_OK;
}

auto blink_terminate() -> blink_Error {
	//if (!model) return blink_StdError_NotInitialized; 
	//model.reset();
	return BLINK_OK;
}

auto blink_make_instance() -> blink_InstanceIndex {
	///if (!model) return {-1};
	// TODO:
}

auto blink_destroy_instance(blink_InstanceIndex instance_idx) -> blink_Error {
	///if (!model) return blink_StdError_NotInitialized;
	// TODO:
}

auto blink_sampler_preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info) -> blink_Error {
	return blink_StdError_NotImplemented;
}

auto blink_sampler_sample_deleted(blink_ID sample_id) -> blink_Error {
	return blink_StdError_NotImplemented;
}

auto blink_get_num_parameters() -> size_t {
	///if (!model) return 0;
	return plugin.param.uuid.size();
}

auto blink_get_parameter_uuid(blink_ParamIndex index) -> blink_UUID {
	return plugin.param.uuid[index.value];
}

auto blink_get_parameter_by_uuid(blink_UUID uuid) -> blink_ParamIndex {
	const auto pos = std::find(plugin.param.uuid.begin(), plugin.param.uuid.end(), uuid);
	if (pos == plugin.param.uuid.end()) {
		return {-1};
	}
	const auto index = std::distance(plugin.param.uuid.begin(), pos);
	return {static_cast<int32_t>(index)};
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	// TODO:
	//return get_std_error_string(*model, blink_StdError(error));
}

auto blink_sampler_draw(const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	//if (!model) return blink_StdError_NotInitialized;
	// TODO:
	//return draw(*model, *buffer, *unit_state, n, out);
	return BLINK_OK;
}

auto blink_sampler_process(blink_UnitIndex unit_idx, const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, float* out) -> blink_Error {
	auto unit_process_fn = [](const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out) -> blink_Error {
		// TODO:
	};
	auto unit_reset_fn = []() -> void {
		// TODO:
	};
	auto instance_reset_fn = []() -> void {
		// TODO:
	};
	return blink::sampler_process(&plugin, unit_idx, *buffer, unit_state, out, unit_process_fn, unit_reset_fn, instance_reset_fn);
}

auto blink_int_slider_from_string(blink_IntSliderIndex sld_idx, const char* str, int64_t* out) -> blink_Bool {
	return blink::from_string(plugin, sld_idx, str, out);
}

auto blink_int_slider_to_string(blink_IntSliderIndex sld_idx, int64_t value) -> blink_TempString {
	return blink::to_string(plugin, sld_idx, value);
}

auto blink_int_slider_constrain(blink_IntSliderIndex sld_idx, int64_t value) -> int64_t {
	return blink::constrain(plugin, sld_idx, value);
}

auto blink_int_slider_decrement(blink_IntSliderIndex sld_idx, int64_t value, blink_Bool precise) -> int64_t {
	return blink::decrement(plugin, sld_idx, value, precise);
}

auto blink_int_slider_drag(blink_IntSliderIndex sld_idx, int64_t start_value, int64_t amount, blink_Bool precise) -> int64_t {
	return blink::drag(plugin, sld_idx, start_value, amount, precise);
}

auto blink_int_slider_get_default_value(blink_IntSliderIndex sld_idx) -> int64_t {
	return blink::get_default_value(plugin, sld_idx);
}

auto blink_int_slider_increment(blink_IntSliderIndex sld_idx, int64_t value, blink_Bool precise) -> int64_t {
	return blink::increment(plugin, sld_idx, value, precise);
}
