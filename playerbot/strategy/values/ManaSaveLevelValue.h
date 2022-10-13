#pragma once
#include "../Value.h"

namespace ai
{
    class ManaSaveLevelValue : public ManualSetValue<double>
	{
	public:
        ManaSaveLevelValue(PlayerbotAI* ai) : ManualSetValue<double>(ai, 1.0, "mana save level") {}

        virtual std::string Save() { ostringstream out; out << value; return out.str(); }
        virtual bool Load(std::string text) { value = atof(text.c_str()); return true; }
    };
}
