#pragma once

#include "../Action.h"

namespace ai
{
    class TellLosAction : public Action {
    public:
        TellLosAction(PlayerbotAI* ai) : Action(ai, "los") {}
        virtual bool Execute(Event event);

    private:
        void ListUnits(std::string title, std::list<ObjectGuid> units);
        void ListGameObjects(std::string title, std::list<ObjectGuid> gos);
    };

}
