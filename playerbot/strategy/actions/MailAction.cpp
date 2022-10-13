#include "botpch.h"
#include "Mail.h"
#include "../../playerbot.h"
#include "MailAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../Helpers.h"

using namespace ai;

map<std::string, MailProcessor*> MailAction::processors;

class TellMailProcessor : public MailProcessor
{
public:
    virtual bool Before(PlayerbotAI* ai)
    {
        ai->TellMaster("=== Mailbox ===", PLAYERBOT_SECURITY_ALLOW_ALL, false);
        tells.clear();
        return true;
    }
    virtual bool Process(int index, Mail* mail, PlayerbotAI* ai)
    {
        Player* bot = ai->GetBot();
        time_t cur_time = time(0);
        int days = (cur_time - mail->deliver_time) / 3600 / 24;
        std::ostringstream out;
        out << "#" << (index+1) << " ";
        if (!mail->money && !mail->has_items)
            out << "|cffffffff" << mail->subject;

        if (mail->money)
        {
            out << "|cffffff00" << ChatHelper::formatMoney(mail->money);
            if (!mail->subject.empty()) out << " |cffa0a0a0(" << mail->subject << ")";
        }

        if (mail->has_items)
        {
            for (MailItemInfoVec::iterator i = mail->items.begin(); i != mail->items.end(); ++i)
            {
                Item* item = bot->GetMItem(i->item_guid);
                int count = item ? item->GetCount() : 1;
                ItemPrototype const *proto = sObjectMgr.GetItemPrototype(i->item_template);
                if (proto)
                {
                    out << ChatHelper::formatItem(proto, count);
                    if (!mail->subject.empty()) out << " |cffa0a0a0(" << mail->subject << ")";
                }
            }
        }
        out  << ", |cff00ff00" << days << " day(s)";
        tells.push_front(out.str());
        return true;
    }

    virtual bool After(PlayerbotAI* ai)
    {
        for (std::list<std::string>::iterator i = tells.begin(); i != tells.end(); ++i)
            ai->TellMaster(*i, PLAYERBOT_SECURITY_ALLOW_ALL, false);

        return true;
    }

    static TellMailProcessor instance;

private:
    std::list<std::string> tells;
};

class TakeMailProcessor : public MailProcessor
{
public:
    virtual bool Process(int index, Mail* mail, PlayerbotAI* ai)
    {
        Player* bot = ai->GetBot();
        if (!CheckBagSpace(bot))
        {
            ai->TellError("Not enough bag space");
            return false;
        }

        ObjectGuid mailbox = FindMailbox(ai);
        if (mail->money)
        {
            std::ostringstream out;
            out << mail->subject << ", |cffffff00" << ChatHelper::formatMoney(mail->money) << "|cff00ff00 processed";
            ai->TellMaster(out.str(), PLAYERBOT_SECURITY_ALLOW_ALL, false);

            WorldPacket packet;
            packet << mailbox;
            packet << mail->messageID;
            bot->GetSession()->HandleMailTakeMoney(packet);
            RemoveMail(bot, mail->messageID, mailbox);
        }
        else if (!mail->items.empty())
        {
            std::list<uint32> guids;
            for (MailItemInfoVec::iterator i = mail->items.begin(); i != mail->items.end(); ++i)
            {
                ItemPrototype const *proto = sObjectMgr.GetItemPrototype(i->item_template);
                if (proto)
                    guids.push_back(i->item_guid);
            }

            for (std::list<uint32>::iterator i = guids.begin(); i != guids.end(); ++i)
            {
                WorldPacket packet;
                packet << mailbox;
                packet << mail->messageID;
#ifndef MANGOSBOT_ZERO
                packet << *i;
#endif
                Item* item = bot->GetMItem(*i);
                std::ostringstream out;
                out << mail->subject << ", " << ChatHelper::formatItem(item->GetProto()) << "|cff00ff00 processed";

                bot->GetSession()->HandleMailTakeItem(packet);
                ai->TellMaster(out.str(), PLAYERBOT_SECURITY_ALLOW_ALL, false);
            }

            RemoveMail(bot, mail->messageID, mailbox);
        }
        return true;
    }

    static TakeMailProcessor instance;

private:
    bool CheckBagSpace(Player* bot)
    {
        uint32 totalused = 0, total = 16;
        for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
        {
            if (bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                totalused++;
        }
        uint32 totalfree = 16 - totalused;
        for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
        {
            if (const Bag* const pBag = (Bag*) bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag))
            {
                ItemPrototype const* pBagProto = pBag->GetProto();
                if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                    totalfree += pBag->GetFreeSlots();
            }

        }

        return totalfree >= 2;
    }
};

class DeleteMailProcessor : public MailProcessor
{
public:
    virtual bool Process(int index, Mail* mail, PlayerbotAI* ai)
    {
        std::ostringstream out;
        out << "|cffffffff" << mail->subject << "|cffff0000 deleted";
        RemoveMail(ai->GetBot(), mail->messageID, FindMailbox(ai));
        ai->TellMaster(out.str(), PLAYERBOT_SECURITY_ALLOW_ALL, false);
        return true;
    }

    static DeleteMailProcessor instance;
};

class ReadMailProcessor : public MailProcessor
{
public:
    virtual bool Process(int index, Mail* mail, PlayerbotAI* ai)
    {
        std::ostringstream out, body;
        out << "|cffffffff" << mail->subject;
        ai->TellMaster(out.str(), PLAYERBOT_SECURITY_ALLOW_ALL, false);
#ifdef MANGOSBOT_TWO

#else
        if (mail->itemTextId)
        {
            body << "\n" << sObjectMgr.GetItemText(mail->itemTextId);
            ai->TellMaster(body.str(), PLAYERBOT_SECURITY_ALLOW_ALL, false);
        }
#endif
        return true;
    }
    static ReadMailProcessor instance;
};

TellMailProcessor TellMailProcessor::instance;
TakeMailProcessor TakeMailProcessor::instance;
DeleteMailProcessor DeleteMailProcessor::instance;
ReadMailProcessor ReadMailProcessor::instance;

bool MailAction::Execute(Event event)
{
    Player* master = GetMaster();
    if (!master && event.getSource() != "rpg action")
        return false;

    if (!MailProcessor::FindMailbox(ai))
    {
        ai->TellError("There is no mailbox nearby");
        return false;
    }

    if (processors.empty())
    {
        processors["?"] = &TellMailProcessor::instance;
        processors["take"] = &TakeMailProcessor::instance;
        processors["delete"] = &DeleteMailProcessor::instance;
        processors["read"] = &ReadMailProcessor::instance;
    }

    std::string text = event.getParam();
    if (text.empty())
    {
        ai->TellMaster("whisper 'mail ?' to query mailbox, 'mail take/delete/read filter' to take/delete/read mails by filter");
        return false;
    }

    std::vector<std::string> ss = split(text, ' ');
    std::string action = ss[0];
    std::string filter = ss.size() > 1 ? ss[1] : "";
    MailProcessor* processor = processors[action];
    if (!processor)
    {
        std::ostringstream out; out << action << ": I don't know how to do that";
        ai->TellMaster(out.str());
        return false;
    }

    if (!processor->Before(ai))
        return false;

    std::vector<Mail*> mailList;
    time_t cur_time = time(0);
    for (PlayerMails::iterator itr = bot->GetMailBegin(); itr != bot->GetMailEnd(); ++itr)
    {
        if ((*itr)->state == MAIL_STATE_DELETED || cur_time < (*itr)->deliver_time)
            continue;

        Mail *mail = *itr;
        mailList.push_back(mail);
    }

    std::map<int, Mail*> filtered = filterList(mailList, filter);
    for (map<int, Mail*>::iterator i = filtered.begin(); i != filtered.end(); ++i)
    {
        if (!processor->Process(i->first, i->second, ai))
            break;
    }

    return processor->After(ai);
}

void MailProcessor::RemoveMail(Player* bot, uint32 id, ObjectGuid mailbox)
{
    WorldPacket packet;
    packet << mailbox;
    packet << id;
#ifndef MANGOSBOT_ZERO
    packet << (uint32)0; //mailTemplateId
#endif
    bot->GetSession()->HandleMailDelete(packet);
}

ObjectGuid MailProcessor::FindMailbox(PlayerbotAI* ai)
{
    std::list<ObjectGuid> gos = *ai->GetAiObjectContext()->GetValue<std::list<ObjectGuid> >("nearest game objects");
    ObjectGuid mailbox;
    for (std::list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); ++i)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (go && go->GetGoType() == GAMEOBJECT_TYPE_MAILBOX)
        {
            mailbox = go->GetObjectGuid();
            break;
        }
    }

    return mailbox;
}