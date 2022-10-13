#pragma once
#include "../Value.h"
#include "NearestUnitsValue.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class PossibleTargetsValue : public NearestUnitsValue
	{
	public:
        PossibleTargetsValue(PlayerbotAI* ai, std::string name = "possible targets", float range = sPlayerbotAIConfig.sightDistance, bool ignoreLos = false) :
          NearestUnitsValue(ai, name, range, ignoreLos) {}

    protected:
        virtual void FindUnits(std::list<Unit*> &targets);
        virtual bool AcceptUnit(Unit* unit);

	};

    class AllTargetsValue : public PossibleTargetsValue
	{
	public:
        AllTargetsValue(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance) :
            PossibleTargetsValue(ai, "all targets", range, true) {}
	};
}
