#pragma once

#include "../Action.h"
#include "QuestAction.h"

namespace ai
{
    class TalkToQuestGiverAction : public QuestAction {
    public:
        TalkToQuestGiverAction(PlayerbotAI* ai) : QuestAction(ai, "talk to quest giver") {}

    protected:
        virtual bool ProcessQuest(Quest const* quest, WorldObject* questGiver);

    private:        
        void TurnInQuest(Quest const* quest, WorldObject* questGiver, std::ostringstream& out);
        void RewardNoItem(Quest const* quest, WorldObject* questGiver, std::ostringstream& out);
        void RewardSingleItem(Quest const* quest, WorldObject* questGiver, std::ostringstream& out);
        std::set<uint32> BestRewards(Quest const* quest);
        void RewardMultipleItem(Quest const* quest, WorldObject* questGiver, std::ostringstream& out);
        void AskToSelectReward(Quest const* quest, std::ostringstream& out, bool forEquip);
    };

}