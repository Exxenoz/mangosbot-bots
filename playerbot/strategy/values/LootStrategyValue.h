#pragma once
#include "../../LootObjectStack.h"
#include "../Value.h"

namespace ai
{
    class LootStrategyValue : public ManualSetValue<LootStrategy*>
	{
	public:
        LootStrategyValue(PlayerbotAI* ai, std::string name = "loot strategy") : ManualSetValue<LootStrategy*>(ai, normal, name) {}
        virtual ~LootStrategyValue() { delete defaultValue; }

        virtual std::string Save();
        virtual bool Load(std::string value);

        static LootStrategy *normal, *gray, *all, *disenchant;
        static LootStrategy* instance(std::string name);
    };
}
