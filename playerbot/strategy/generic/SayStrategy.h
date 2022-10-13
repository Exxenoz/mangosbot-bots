#pragma once

namespace ai
{
    class SayStrategy : public Strategy
    {
    public:
        SayStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual std::string getName() { return "say"; }
    };


}
