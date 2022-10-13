#pragma once

#include "InventoryAction.h"

namespace ai
{
    class SuggestWhatToDoAction : public InventoryAction
    {
    public:
        SuggestWhatToDoAction(PlayerbotAI* ai, std::string name = "suggest what to do");
        virtual bool Execute(Event event);
        virtual bool isUseful();

    protected:
        typedef void (SuggestWhatToDoAction::*Suggestion) ();
        std::vector<Suggestion> suggestions;
        void instance();
        void specificQuest();
        void grindMaterials();
        void grindReputation();
        void something();
        void trade();
        void spam(std::string msg, uint8 flags = 0, bool worldChat = false, bool guild = false);

        std::vector<uint32> GetIncompletedQuests();

    private:
        static std::map<std::string, int> instances;
        static std::map<std::string, int> factions;
        int32 _locale;
    };

    class SuggestTradeAction : public SuggestWhatToDoAction
    {
    public:
        SuggestTradeAction(PlayerbotAI* ai);
        virtual bool Execute(Event event);
        virtual bool isUseful() { return true; }
    };
}
