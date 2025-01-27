#include "botpch.h"
#include "../../playerbot.h"
#include "QueryQuestAction.h"
#include "../../TravelMgr.h"

using namespace ai;

bool QueryQuestAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    Player *bot = ai->GetBot();
    WorldPosition botPos(bot);
    std::string text = event.getParam();
    bool travel = false;

    if (text.find("travel") != std::string::npos)
    {
        travel = true;
        chat->eraseAllSubStr(text, " travel");
    }

    PlayerbotChatHandler ch(bot);
    uint32 questId = ch.extractQuestId(text);
    if (!questId)
        return false;

    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        if(questId != bot->GetQuestSlotQuestId(slot))
            continue;

        ostringstream out;
        out << "--- " << chat->formatQuest(sObjectMgr.GetQuestTemplate(questId)) << " ";
        if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
        {
            out << "|c0000FF00completed|r ---";
            ai->TellPlayer(requester, out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
        }
        else
        {
            out << "|c00FF0000not completed|r ---";
            ai->TellPlayer(requester, out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            TellObjectives(requester, questId);
        }

        if (travel)
        {
            uint32 limit = 0;
            vector<TravelDestination*> allDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, true, true, -1);

            std::sort(allDestinations.begin(), allDestinations.end(), [botPos](TravelDestination* i, TravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

            for (auto dest : allDestinations) 
            {
                if (limit > 50)
                    continue;

                ostringstream out;

                uint32 tpoints = dest->getPoints(true).size();
                uint32 apoints = dest->getPoints().size();


                out << round(dest->distanceTo(botPos));

                out << " to " << dest->getTitle();

                out << " " << apoints;
                if (apoints < tpoints)
                    out << "/" << tpoints;
                out << " points.";

                if (!dest->isActive(bot))
                    out << " not active";

                ai->TellPlayer(requester, out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

                limit++;
            }
        }

        return true;
    }

    return false;
}

void QueryQuestAction::TellObjectives(Player* requester, uint32 questId)
{
    Quest const* questTemplate = sObjectMgr.GetQuestTemplate(questId);
    QuestStatusData questStatus = bot->getQuestStatusMap()[questId];

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
    {
        if (!questTemplate->ObjectiveText[i].empty())
            ai->TellPlayer(requester, questTemplate->ObjectiveText[i], PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

        if (questTemplate->ReqItemId[i])
        {
            int required = questTemplate->ReqItemCount[i];
            int available = questStatus.m_itemcount[i];
            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(questTemplate->ReqItemId[i]);
            TellObjective(requester, chat->formatItem(proto), available, required);
        }

        if (questTemplate->ReqCreatureOrGOId[i])
        {
            int required = questTemplate->ReqCreatureOrGOCount[i];
            int available = questStatus.m_creatureOrGOcount[i];

            if (questTemplate->ReqCreatureOrGOId[i] < 0)
            {
                GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(-questTemplate->ReqCreatureOrGOId[i]);
                if (info)
                    TellObjective(requester, info->name, available, required);
            }
            else
            {
                CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(questTemplate->ReqCreatureOrGOId[i]);
                if (info)
                    TellObjective(requester, info->Name, available, required);
            }
        }
    }
}

void QueryQuestAction::TellObjective(Player* requester, const string& name, int available, int required)
{
    ai->TellPlayer(requester, chat->formatQuestObjective(name, available, required), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
}