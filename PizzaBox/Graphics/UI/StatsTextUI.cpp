#include "StatsTextUI.h"

#include <sstream>
#include <iomanip>

#include <rttr/registration.h>

#include "Tools/EngineStats.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<StatsTextUI>("StatsTextUI")
		.constructor<const std::string&, const Rect&, const std::string&>()
		.method("Render", &StatsTextUI::Render);
}
#pragma warning( pop )


StatsTextUI::StatsTextUI(const std::string& name_, const Rect& rect_, const std::string& fontName_) : TextUI(name_, rect_, false, "FPS: 0.00", fontName_, nullptr){
}

StatsTextUI::~StatsTextUI(){
}

void StatsTextUI::Render(){
	std::stringstream fpsStream;
	fpsStream << std::fixed << std::setprecision(2) << EngineStats::GetFloat("Average FPS");

	std::stringstream aftStream;
	aftStream << std::fixed << std::setprecision(2) << EngineStats::GetFloat("Average Frame Time");

	SetText("FPS: " + fpsStream.str() + " (" + aftStream.str() + " ms)");
	TextUI::Render();
}