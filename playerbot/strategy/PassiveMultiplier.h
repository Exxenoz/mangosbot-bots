#pragma once
#include "Action.h"
#include "Multiplier.h"

namespace ai
{
    class PassiveMultiplier : public Multiplier
    {
    public:
        PassiveMultiplier(PlayerbotAI* ai);

    public:
        virtual float GetValue(Action* action);

    private:
        static std::list<string> allowedActions;
        static std::list<string> allowedParts;
    };

}
