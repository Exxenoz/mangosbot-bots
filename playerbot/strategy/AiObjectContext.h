#pragma once

#include "../PlayerbotAIAware.h"
#include "Action.h"
#include "Value.h"
#include "NamedObjectContext.h"
#include "Strategy.h"

namespace ai
{
    class Action;

    class AiObjectContext : public PlayerbotAIAware
    {
    public:
        AiObjectContext(PlayerbotAI* ai);
        virtual ~AiObjectContext() {}

    public:
        virtual Strategy* GetStrategy(std::string name) { return strategyContexts.GetObject(name, ai); }
        virtual std::set<std::string> GetSiblingStrategy(std::string name) { return strategyContexts.GetSiblings(name); }
        virtual Trigger* GetTrigger(std::string name) { return triggerContexts.GetObject(name, ai); }
        virtual Action* GetAction(std::string name) { return actionContexts.GetObject(name, ai); }
        virtual UntypedValue* GetUntypedValue(std::string name) { return valueContexts.GetObject(name, ai); }

        template<class T>
        Value<T>* GetValue(std::string name)
        {
            return dynamic_cast<Value<T>*>(GetUntypedValue(name));
        }

        template<class T>
        Value<T>* GetValue(std::string name, std::string param)
        {
            return GetValue<T>((string(name) + "::" + param));
        }

        template<class T>
        Value<T>* GetValue(std::string name, int32 param)
        {
        	ostringstream out; out << param;
            return GetValue<T>(name, out.str());
        }

        std::set<std::string> GetValues()
        {
            return valueContexts.GetCreated();
        }

        std::set<std::string> GetSupportedStrategies()
        {
            return strategyContexts.supports();
        }

        std::set<std::string> GetSupportedActions()
        {
            return actionContexts.supports();
        }

        std::string FormatValues(std::string findName = "")
        {
            std::ostringstream out;
            std::set<std::string> names = valueContexts.GetCreated();
            for (std::set<std::string>::iterator i = names.begin(); i != names.end(); ++i)
            {
                UntypedValue* value = GetUntypedValue(*i);
                if (!value)
                    continue;

                if (!findName.empty() && i->find(findName) == string::npos)
                    continue;

                std::string text = value->Format();
                if (text == "?")
                    continue;

                out << "{" << *i << "=" << text << "}|";
            }
            out.seekp(-1, out.cur);
            return out.str();
        }

    public:
        virtual void Update();
        virtual void Reset();
        virtual void AddShared(NamedObjectContext<UntypedValue>* sharedValues)
        {
            valueContexts.Add(sharedValues);
        }
        std::list<std::string> Save();
        void Load(std::list<std::string> data);

        std::vector<std::string> performanceStack;
    protected:
        NamedObjectContextList<Strategy> strategyContexts;
        NamedObjectContextList<Action> actionContexts;
        NamedObjectContextList<Trigger> triggerContexts;
        NamedObjectContextList<UntypedValue> valueContexts;
    };
}
