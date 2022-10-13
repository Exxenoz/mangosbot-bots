#include "botpch.h"
#include "../../playerbot.h"
#include "ChangeStrategyAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ChangeCombatStrategyAction::Execute(Event event)
{
    std::string text = event.getParam();
    ai->ChangeStrategy(text.empty() ? getName() : text, BOT_STATE_COMBAT);
    if (event.getSource() == "co")
    {
        std::vector<std::string> splitted = split(text, ',');
        for (std::vector<std::string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        {
            const char* name = i->c_str();
            switch (name[0])
            {
            case '+':
            case '-':
            case '~':
                sPlayerbotDbStore.Save(ai);
                break;
            case '?':
                break;
            }
        }
    }
    return true;
}

bool ChangeNonCombatStrategyAction::Execute(Event event)
{
    std::string text = event.getParam();

    uint32 account = sObjectMgr.GetPlayerAccountIdByGUID(bot->GetObjectGuid());
    if (sPlayerbotAIConfig.IsInRandomAccountList(account) && ai->GetMaster() && ai->GetMaster()->GetSession()->GetSecurity() < SEC_GAMEMASTER)
    {
        if (text.find("loot") != string::npos || text.find("gather") != string::npos)
        {
            ai->TellError("You can change any strategy except loot and gather");
            return false;
        }
    }

    ai->ChangeStrategy(text, BOT_STATE_NON_COMBAT);

    if (event.getSource() == "nc")
    {
        std::vector<std::string> splitted = split(text, ',');
        for (std::vector<std::string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        {
            const char* name = i->c_str();
            switch (name[0])
            {
            case '+':
            case '-':
            case '~':
                sPlayerbotDbStore.Save(ai);
                break;
            case '?':
                break;
            }
        }
    }
    return true;
}

bool ChangeDeadStrategyAction::Execute(Event event)
{
    std::string text = event.getParam();
    ai->ChangeStrategy(text, BOT_STATE_DEAD);
    return true;
}
