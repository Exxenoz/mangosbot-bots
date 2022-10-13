#pragma once

namespace ai
{
    class RTSCStrategy : public Strategy
    {
    public:
        RTSCStrategy(PlayerbotAI* ai);
        virtual std::string getName() { return "RTSC"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

}
