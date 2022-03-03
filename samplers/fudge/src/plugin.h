#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "instance.h"
#include "sample_analysis.h"
#include "parameters.h"

namespace fudge {

class Plugin : public blink::SamplerPlugin
{
public:

	const Parameters params;

	Plugin() : params(this) {}

	GUI& gui();

	const SampleAnalysis* get_analysis_data(blink_ID sample_id) const;

	void preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info);
	void on_sample_deleted(blink_ID id);

private:

	auto make_instance() -> std::unique_ptr<blink::SamplerInstance> override { return std::make_unique<Instance>(this); }

	std::map<blink_ID, std::shared_ptr<SampleAnalysis>> sample_analysis_;
	GUI gui_;
};

} // fudge