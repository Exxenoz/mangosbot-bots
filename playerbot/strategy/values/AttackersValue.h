#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "NearestUnitsValue.h"

namespace ai
{
    class AttackersValue : public ObjectGuidListCalculatedValue
	{
	public:
        AttackersValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "attackers") {}
        std::list<ObjectGuid> Calculate();

	private:
        void AddAttackersOf(Group* group, std::set<Unit*>& targets);
        void AddAttackersOf(Player* player, std::set<Unit*>& targets);
		void RemoveNonThreating(std::set<Unit*>& targets);

    public:
        static bool IsPossibleTarget(Unit* attacker, Player *bot, float range = sPlayerbotAIConfig.sightDistance);
        static bool IsValidTarget(Unit* attacker, Player *bot);
    };

    class PossibleAddsValue : public BoolCalculatedValue
    {
    public:
        PossibleAddsValue(PlayerbotAI* const ai, std::string name = "possible adds") : BoolCalculatedValue(ai, name) {}
        virtual bool Calculate();
    };
}
