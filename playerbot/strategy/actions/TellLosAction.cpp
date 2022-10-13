#include "botpch.h"
#include "../../playerbot.h"
#include "TellLosAction.h"


using namespace ai;

bool TellLosAction::Execute(Event event)
{
    std::string param = event.getParam();

    if (param.empty() || param == "targets")
    {
        ListUnits("--- Targets ---", *context->GetValue<std::list<ObjectGuid> >("possible targets"));
        ListUnits("--- Targets (All) ---", *context->GetValue<std::list<ObjectGuid> >("all targets"));
    }

    if (param.empty() || param == "npcs")
    {
        ListUnits("--- NPCs ---", *context->GetValue<std::list<ObjectGuid> >("nearest npcs"));
    }

    if (param.empty() || param == "corpses")
    {
        ListUnits("--- Corpses ---", *context->GetValue<std::list<ObjectGuid> >("nearest corpses"));
    }

    if (param.empty() || param == "gos" || param == "game objects")
    {
        ListGameObjects("--- Game objects ---", *context->GetValue<std::list<ObjectGuid> >("nearest game objects"));
    }

    if (param.empty() || param == "players")
    {
        ListUnits("--- Friendly players ---", *context->GetValue<std::list<ObjectGuid> >("nearest friendly players"));
    }

    return true;
}

void TellLosAction::ListUnits(std::string title, std::list<ObjectGuid> units)
{
    ai->TellMaster(title);

    for (std::list<ObjectGuid>::iterator i = units.begin(); i != units.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit)
            ai->TellMaster(unit->GetName());
    }

}
void TellLosAction::ListGameObjects(std::string title, std::list<ObjectGuid> gos)
{
    ai->TellMaster(title);

    for (std::list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); i++)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (go)
            ai->TellMaster(chat->formatGameobject(go));
    }
}
