#pragma once

#include "../Action.h"

namespace ai
{
    class CustomStrategyEditAction : public Action {
    public:
        CustomStrategyEditAction(PlayerbotAI* ai) : Action(ai, "cs") {}
        virtual bool Execute(Event event);

    private:
        bool PrintHelp();
        bool PrintActionLine(uint32 idx, std::string command);
        bool Print(std::string name);
        bool Edit(std::string name, uint32 idx, std::string command);
    };

}
