#include "botpch.h"
#include "../../playerbot.h"
#include "ResetAiAction.h"
#include "../../PlayerbotDbStore.h"

using namespace ai;

bool ResetAiAction::Execute(Event& event)
{
    ResetValues();
    ResetStrategies();

    if (fullReset)
    {
        ai->TellError("AI was reset to defaults");
    }

    return true;
}

void ResetAiAction::ResetStrategies()
{
    bool loadStrats = !fullReset;

    ai->ResetStrategies(loadStrats);

    if (fullReset)
        sPlayerbotDbStore.Save(ai);
}

void ResetAiAction::ResetValues()
{
    uint64 guid = ai->GetBot()->GetObjectGuid().GetRawValue();

    QueryResult* results = PlayerbotDatabase.PQuery("SELECT `value` FROM `ai_playerbot_db_store` WHERE `guid` = '%lu' and `key` = 'value'", guid);
    if (results)
    {
        list<string> values;
        do
        {
            Field* fields = results->Fetch();
            string val = fields[0].GetString();

            vector<string> parts = split(val, '>');
            if (parts.size() != 2) continue;

            string name = parts[0];
            string text = parts[1];

            UntypedValue* value = context->GetUntypedValue(name);
            if (!value) continue;

            if (fullReset)
                value->Reset();
            else
                value->Load(text);
            
        } while (results->NextRow());

        if(fullReset)
            PlayerbotDatabase.PExecute("DELETE FROM `ai_playerbot_db_store` WHERE `guid` = '%lu' and `key` = 'value'", guid);            
    }
}

bool ResetStratsAction::Execute(Event& event)
{
    ResetStrategies();
   
    if (fullReset)
    {
        ai->TellError("Strategies reset to defaults");
    }

    return true;
}

bool ResetValuesAction::Execute(Event& event)
{
    ResetValues();

    if (fullReset)
    {
        ai->TellError("Settings reset to defaults");
    }

    return true;
}