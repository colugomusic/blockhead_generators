#include "parameters.h"
#include "convert.h"
#include "parameters/damper.hpp"
#include "parameters/feedback.hpp"
#include "parameters/fm_amount.hpp"
#include "parameters/fm_ratio.hpp"
#include "parameters/harmonics_amount.hpp"
#include "parameters/harmonics_scale.hpp"
#include "parameters/harmonics_scale_snap_amount.hpp"
#include "parameters/harmonics_spread.hpp"
#include "parameters/harmonics_width.hpp"
#include "parameters/mix.hpp"
#include "parameters/pitch.hpp"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace resonator {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
	env.feedback = plugin->add_parameter(params::feedback::envelope_parameter());
	env.damper = plugin->add_parameter(params::damper::envelope_parameter());

	const auto group_fm = "FM";
	{
		env.fm.amount = plugin->add_parameter(params::fm_amount::envelope_parameter());
		env.fm.ratio = plugin->add_parameter(params::fm_ratio::envelope_parameter());

		env.fm.amount->set_group_name(group_fm);
		env.fm.ratio->set_group_name(group_fm);
	}

	const auto group_harmonics = "Harmonics";
	{
		chords.harmonics.scale = plugin->add_parameter(params::harmonics_scale::chord());
		env.harmonics.amount = plugin->add_parameter(params::harmonics_amount::envelope_parameter());
		env.harmonics.spread = plugin->add_parameter(params::harmonics_spread::envelope_parameter());
		env.harmonics.scale_snap_amount = plugin->add_parameter(params::harmonics_scale_snap_amount::envelope_parameter());
		env.harmonics.width = plugin->add_parameter(params::harmonics_width::envelope_parameter());

		chords.harmonics.scale->set_group_name(group_harmonics);
		env.harmonics.amount->set_group_name(group_harmonics);
		env.harmonics.spread->set_group_name(group_harmonics);
		env.harmonics.scale_snap_amount->set_group_name(group_harmonics);
	}

	env.mix = plugin->add_parameter(params::mix::envelope_parameter());
	sliders.pitch = plugin->add_parameter(params::pitch::slider_parameter());
}

} // resonator
