#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "UseItemAction.h"

namespace ai
{
    class CheckMountStateAction : public UseItemAction {
    public:
        CheckMountStateAction(PlayerbotAI* ai) : UseItemAction(ai, "check mount state", true) {}

        virtual bool Execute(Event event);
        virtual bool isPossible() { return true; }
        virtual bool isUseful();

    private:
        static uint32 MountSpeed(const SpellEntry* const spellInfo, const bool canFly = false);
        std::vector<uint32> GetBestMountSpells(const bool canFly);

        static uint32 MountSpeed(const ItemPrototype* proto, const bool canFly = false);        
        std::vector<Item*> GetBestMounts(const bool canFly);

        bool MountWithBestMount(const bool canFly = false);

        bool Mount();
    };

}
