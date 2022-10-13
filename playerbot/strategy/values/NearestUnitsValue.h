#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class NearestUnitsValue : public ObjectGuidListCalculatedValue
	{
	public:
        NearestUnitsValue(PlayerbotAI* ai, string name = "nearest units", float range = sPlayerbotAIConfig.sightDistance, bool ignoreLos = false) :
            ObjectGuidListCalculatedValue(ai, name, 2), range(range), ignoreLos(ignoreLos) {}

	public:
        std::list<ObjectGuid> Calculate()
        {
            std::list<Unit*> targets;
            FindUnits(targets);

            std::list<ObjectGuid> results;
            for(std::list<Unit *>::iterator i = targets.begin(); i!= targets.end(); ++i)
            {
                Unit* unit = *i;
                if ((ignoreLos || sServerFacade.IsWithinLOSInMap(bot, unit)) && AcceptUnit(unit))
                    results.push_back(unit->GetObjectGuid());
            }
            return results;
        }

    protected:
        virtual void FindUnits(std::list<Unit*> &targets) = 0;
        virtual bool AcceptUnit(Unit* unit) = 0;

    protected:
        float range;
        bool ignoreLos;
	};
}
