#pragma once

#include <blink/manipulators/manipulator_option_target.hpp>
#include <blink/manipulators/manipulator_slider_target.hpp>
#include <blink/manipulators/manipulator_toggle_target.hpp>
#include <blink/plugin.hpp>

namespace classic {

struct ManipulatorTargets
{
	enum class Index
	{
		Slider_Amp,
		Slider_Pan,
		Slider_Pitch,
		//Slider_NoiseAmount,
		//Slider_NoiseColor,
		//Slider_NoiseWidth,
		//Option_NoiseMode,
		//Toggle_Loop,
		//Toggle_Reverse,
	};

	struct Sliders
	{
		std::shared_ptr<blink::ManipulatorSliderTarget> amp;
		std::shared_ptr<blink::ManipulatorSliderTarget> pan;
		std::shared_ptr<blink::ManipulatorSliderTarget> pitch;
		
		//struct
		//{
		//	std::shared_ptr<blink::ManipulatorSliderTarget> amount;
		//	std::shared_ptr<blink::ManipulatorSliderTarget> color;
		//	std::shared_ptr<blink::ManipulatorSliderTarget> width;
		//} noise;
	} sliders;

	//struct Options
	//{
	//	std::shared_ptr<blink::ManipulatorOptionTarget> noise_mode;
	//} options;

	//struct Toggles
	//{
	//	std::shared_ptr<blink::ManipulatorToggleTarget> loop;
	//	std::shared_ptr<blink::ManipulatorToggleTarget> reverse;
	//} toggles;
	
	ManipulatorTargets(blink::Plugin* plugin);
};

} // classic
