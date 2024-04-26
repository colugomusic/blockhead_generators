#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {


namespace envelopes {

EnvelopeSpec amp()
{
	auto out = std_params::envelopes::amp();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

EnvelopeSpec pan()
{
	return std_params::envelopes::pan();
}

EnvelopeSpec pitch()
{
	auto out = std_params::envelopes::pitch();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

EnvelopeSpec speed()
{
	auto out = std_params::envelopes::speed();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

} // envelopes

namespace sliders {

blink::SliderParameterSpec<float> noise_width()
{
	auto out = std_params::sliders::parameters::noise_width();

	out.flags = blink_ParamFlags_NonGlobal;

	return out;
}

} // sliders

} // parameters
