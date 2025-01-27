#pragma once
#include "GenericActions.h"

namespace ai
{
    class MailProcessor
    {
    public:
        virtual bool Before(PlayerbotAI* ai) { return true; }
        virtual bool Process(int index, Mail* mail, PlayerbotAI* ai) = 0;
        virtual bool After(PlayerbotAI* ai) { return true; }

    public:
        static ObjectGuid FindMailbox(PlayerbotAI* ai);

    protected:
        void RemoveMail(Player* bot, uint32 id, ObjectGuid mailbox);
    };


    class MailAction : public ChatCommandAction
    {
    public:
        MailAction(PlayerbotAI* ai) : ChatCommandAction(ai, "mail") {}
        virtual bool Execute(Event& event) override;

    private:
        bool CheckMailbox();

    private:
        static map<string, MailProcessor*> processors;
    };

}
