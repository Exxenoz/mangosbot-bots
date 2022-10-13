#pragma once
#include "../Value.h"

namespace ai
{
    class AlwaysLootListValue : public ManualSetValue<set<uint32>&>
	{
	public:
        AlwaysLootListValue(PlayerbotAI* ai, std::string name = "always loot list") : ManualSetValue<set<uint32>&>(ai, list, name) {}

        virtual std::string Save();
        virtual bool Load(std::string value);

    private:
        set<uint32> list;
    };
}
