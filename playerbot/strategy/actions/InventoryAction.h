#pragma once

#include "../Action.h"
#include "../ItemVisitors.h"

namespace ai
{
    class InventoryAction : public Action {
    public:
        InventoryAction(PlayerbotAI* ai, std::string name) : Action(ai, name) {}

    protected:
        void IterateItems(IterateItemsVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        void TellItems(map<uint32, int> items, std::map<uint32, bool> soulbound);
        void TellItem(ItemPrototype const * proto, int count, bool soulbound);
        std::list<Item*> parseItems(std::string text, IterateItemsMask mask = ITERATE_ALL_ITEMS);
        uint32 GetItemCount(FindItemVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        std::string parseOutfitName(std::string outfit);
        ItemIds parseOutfitItems(std::string outfit);
        ItemIds FindOutfitItems(std::string name);

    private:
        void IterateItemsInBags(IterateItemsVisitor* visitor);
        void IterateItemsInEquip(IterateItemsVisitor* visitor);
        void IterateItemsInBank(IterateItemsVisitor* visitor);
    };
}
