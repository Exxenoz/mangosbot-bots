#pragma once

#include "../Action.h"
#include "InventoryAction.h"


namespace ai
{
    class ListSpellsAction : public InventoryAction {
    public:
        ListSpellsAction(PlayerbotAI* ai, std::string name = "spells") : InventoryAction(ai, name) {}

        virtual bool Execute(Event event);

        virtual std::list<std::pair<uint32, string> > GetSpellList(std::string filter = "");
    private:
        static std::map<uint32, SkillLineAbilityEntry const*> skillSpells;
        static std::set<uint32> vendorItems;
    };

}
