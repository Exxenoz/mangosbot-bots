#pragma once
#include "../Value.h"

namespace ai
{
    class LineTargetValue : public UnitCalculatedValue
	{
	public:
        LineTargetValue(PlayerbotAI* ai, std::string name = "line target") : UnitCalculatedValue(ai, name) {}

    public:
        virtual Unit* Calculate();
	};
}
