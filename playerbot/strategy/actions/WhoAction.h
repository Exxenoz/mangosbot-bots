#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class WhoAction : public InventoryAction {
    public:
        WhoAction(PlayerbotAI* ai) : InventoryAction(ai, "who") {}

    public:
        virtual bool Execute(Event event);

    private:
        std::string QueryTrade(std::string text);
        std::string QuerySkill(std::string text);
        std::string QuerySpec(std::string text);
    };

}
