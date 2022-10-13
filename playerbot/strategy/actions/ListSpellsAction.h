#pragma once

#include "../Action.h"
#include "InventoryAction.h"


namespace ai
{
    class ListSpellsAction : public InventoryAction {
    public:
        ListSpellsAction(PlayerbotAI* ai, string name = "spells") : InventoryAction(ai, name) {}

        virtual bool Execute(Event event);

        virtual std::list<pair<uint32, string> > GetSpellList(string filter = "");
    private:
        static std::map<uint32, SkillLineAbilityEntry const*> skillSpells;
        static set<uint32> vendorItems;
    };

}
