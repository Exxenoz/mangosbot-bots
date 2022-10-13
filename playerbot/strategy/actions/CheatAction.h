#pragma once

#include "../Action.h"

namespace ai
{
    class CheatAction : public Action
    {
    public:
        CheatAction(PlayerbotAI* ai) : Action(ai, "cheat") {}
        virtual bool Execute(Event event);
    private:
        static BotCheatMask GetCheatMask(std::string cheat);
        static std::string GetCheatName(BotCheatMask cheatMask);
        void ListCheats();
    };
}
