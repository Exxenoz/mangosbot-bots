#include "botpch.h"
#include "../../playerbot.h"
#include "OutfitListValue.h"

using namespace ai;
using namespace std;

string OutfitListValue::Save()
{
    ostringstream out;
    bool first = true;
    for (Outfit::iterator i = value.begin(); i != value.end(); ++i)
    {
        if (!first) out << "^";
        else first = false;
        out << *i;
    }
    return out.str();
}

bool OutfitListValue::Load(std::string text)
{
    value.clear();

    std::vector<std::string> ss = split(text, '^');
    for (std::vector<std::string>::iterator i = ss.begin(); i != ss.end(); ++i)
    {
        value.push_back(*i);
    }
    return true;
}
