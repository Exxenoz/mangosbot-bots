#pragma once

#include "CombatStrategy.h"

namespace ai
{
    class PullStrategy : public CombatStrategy
    {
    public:
        PullStrategy(PlayerbotAI* ai, std::string action) : CombatStrategy(ai)
        {
            this->action = action;
        }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual std::string getName() { return "pull"; }
        virtual NextAction** getDefaultActions();

    private:
        std::string action;
    };

    class PossibleAdsStrategy : public Strategy
    {
    public:
        PossibleAdsStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual std::string getName() { return "ads"; }
    };
}
