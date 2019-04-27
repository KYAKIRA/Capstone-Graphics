#ifndef STATS_TEXT_UI_H
#define STATS_TEXT_UI_H

#include "TextUI.h"

namespace PizzaBox{
	class StatsTextUI : public TextUI{
	public:
		StatsTextUI(const std::string& name_, const Rect& rect_, const std::string& fontName_);
		virtual ~StatsTextUI() override;

		virtual void Render() override;
	};
}

#endif //!STATS_TEXT_UI_H