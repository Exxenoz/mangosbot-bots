#include "botpch.h"
#include "../../playerbot.h"
#include "RangeAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool RangeAction::Execute(Event event)
{
    std::string param = event.getParam();
    if (param == "?")
    {
        PrintRange("spell");
        PrintRange("heal");
        PrintRange("shoot");
        PrintRange("flee");
    }
    int pos = param.find(" ");
    if (pos == string::npos) return false;

    std::string qualifier = param.substr(0, pos);
    std::string value = param.substr(pos + 1);

    if (value == "?")
    {
        float curVal = AI_VALUE2(float, "range", qualifier);
        std::ostringstream out;
        out << qualifier << " range: ";
        if (abs(curVal) >= 0.1f) out << curVal;
        else out << ai->GetRange(qualifier) << " (default)";
        ai->TellMaster(out.str());
        PrintRange(qualifier);
        return true;
    }

    float newVal = (float) atof(value.c_str());
    context->GetValue<float>("range", qualifier)->Set(newVal);
    std::ostringstream out;
    out << qualifier << " range set to: " << newVal;
    ai->TellMaster(out.str());
    return true;
}

void RangeAction::PrintRange(std::string type)
{
    float curVal = AI_VALUE2(float, "range", type);

    std::ostringstream out;
    out << type << " range: ";
    if (abs(curVal) >= 0.1f) out << curVal;
    else out << ai->GetRange(type) << " (default)";

    ai->TellMaster(out.str());
}

