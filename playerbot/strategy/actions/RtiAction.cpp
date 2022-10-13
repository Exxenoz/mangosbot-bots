#include "botpch.h"
#include "../../playerbot.h"
#include "RtiAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/RtiTargetValue.h"

using namespace ai;

bool RtiAction::Execute(Event event)
{
    std::string text = event.getParam();
    std::string type = "rti";
    if (text.find("cc ") == 0)
    {
        type = "rti cc";
        text = text.substr(3);
    }
    if (text.empty() || text == "?")
    {
        std::ostringstream outRti; outRti << "rti" << ": ";
        AppendRti(outRti, "rti");
        ai->TellMaster(outRti);

        std::ostringstream outRtiCc; outRtiCc << "rti cc" << ": ";
        AppendRti(outRtiCc, "rti cc");
        ai->TellMaster(outRtiCc);
        return true;
    }

    context->GetValue<std::string>(type)->Set(text);
    std::ostringstream out; out << type << " set to: ";
    AppendRti(out, type);
    ai->TellMaster(out);
    return true;
}

void RtiAction::AppendRti(std::ostringstream & out, std::string type)
{
    out << AI_VALUE(string, type);

    std::ostringstream n; n << type << " target";
    Unit* target = AI_VALUE(Unit*, n.str());
    if (target)
        out << " (" << target->GetName() << ")";

}

bool MarkRtiAction::Execute(Event event)
{
    Group *group = bot->GetGroup();
    if (!group) return false;

    if (bot->InBattleGround())
        return false;

    Unit* target = NULL;
    std::list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<std::list<ObjectGuid> >("attackers")->Get();
    for (std::list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit)
            continue;

        // do not mark players
        if (unit->IsPlayer())
            continue;

        bool marked = false;
        for (int i = 0; i < 8; i++)
        {
            ObjectGuid guid = group->GetTargetIcon(i);
            if (guid == unit->GetObjectGuid())
            {
                marked = true;
                break;
            }
        }

        if (marked) continue;

        if (!target || (int)target->GetHealth() > (int)unit->GetHealth()) target = unit;
    }

    if (!target) return false;

    std::string rti = AI_VALUE(string, "rti");
    int index = RtiTargetValue::GetRtiIndex(rti);
#ifndef MANGOSBOT_TWO
    group->SetTargetIcon(index, target->GetObjectGuid());
#else
    group->SetTargetIcon(index, bot->GetObjectGuid(), target->GetObjectGuid());
#endif
    return true;
}

