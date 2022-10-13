#include "botpch.h"
#include "../../playerbot.h"
#include "TellItemCountAction.h"
#include "../values/ItemCountValue.h"

using namespace ai;

bool TellItemCountAction::Execute(Event event)
{
    string text = event.getParam();
    std::list<Item*> found = parseItems(text);
    std::map<uint32, uint32> itemMap;
    std::map<uint32, bool> soulbound;
    for (std::list<Item*>::iterator i = found.begin(); i != found.end(); i++)
    {
        ItemPrototype const* proto = (*i)->GetProto();
        itemMap[proto->ItemId] += (*i)->GetCount();
        soulbound[proto->ItemId] = (*i)->IsSoulBound();
    }

    ai->TellMaster("=== Inventory ===");
    for (map<uint32, uint32>::iterator i = itemMap.begin(); i != itemMap.end(); ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i->first);
        TellItem(proto, i->second, soulbound[i->first]);
    }

    return true;
}
