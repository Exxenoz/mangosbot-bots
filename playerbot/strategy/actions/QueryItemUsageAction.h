#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class QueryItemUsageAction : public InventoryAction {
    public:
        QueryItemUsageAction(PlayerbotAI* ai, std::string name = "query item usage") : InventoryAction(ai, name) {}
        virtual bool Execute(Event event);

    protected:
        uint32 GetCount(ItemPrototype const *item);
        std::string QueryItem(ItemPrototype const *item, uint32 count, uint32 total);
#ifdef CMANGOS
        std::string QueryItemUsage(ItemPrototype const *item);
#endif
#ifdef MANGOS
        bool QueryItemUsage(ItemPrototype const *item);
#endif
        std::string QueryItemPrice(ItemPrototype const *item);
        std::string QueryQuestItem(uint32 itemId, const Quest *questTemplate, const QuestStatusData *questStatus);
        std::string QueryQuestItem(uint32 itemId);
        std::string QueryItemPower(uint32 itemId);

    private:
        ostringstream out;

    };
}
