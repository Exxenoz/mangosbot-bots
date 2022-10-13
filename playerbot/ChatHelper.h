#pragma once

using namespace std;

typedef set<uint32> ItemIds;
typedef set<uint32> SpellIds;

namespace ai
{
    class ChatHelper : public PlayerbotAIAware
    {
    public:
        ChatHelper(PlayerbotAI* ai);

    public:
        static std::string formatMoney(uint32 copper);
        static uint32 parseMoney(string& text);
        static ItemIds parseItems(string& text);
        uint32 parseSpell(string& text);
        static std::string formatQuest(Quest const* quest);
        static std::string formatItem(ItemPrototype const * proto, int count = 0, int total = 0);
        static std::string formatQItem(uint32 itemId);
        static std::string formatSpell(SpellEntry const *sInfo);
        static std::string formatGameobject(GameObject* go);
        static std::string formatWorldobject(WorldObject* wo);
        static std::string formatWorldEntry(int32 entry);
        static std::string formatQuestObjective(std::string name, int available, int required);
        static std::list<ObjectGuid> parseGameobjects(string& text);
        static std::list<int32> parseWorldEntries(string& text);

        static ChatMsg parseChat(string& text);
        static std::string formatChat(ChatMsg chat);

        static std::string formatClass(Player* player, int spec);
        static std::string formatClass(uint8 cls);
        static std::string formatRace(uint8 race);
        static std::string formatSkill(uint32 skill);
        static std::string formatBoolean(bool flag);

        static uint32 parseItemQuality(std::string text);
        static bool parseItemClass(std::string text, uint32 *itemClass, uint32 *itemSubClass);
        static uint32 parseSlot(std::string text);
        uint32 parseSkill(string& text);

        static bool parseable(std::string text);

        void eraseAllSubStr(std::string& mainStr, const std::string& toErase);

    private:
        static std::map<std::string, uint32> consumableSubClasses;
        static std::map<std::string, uint32> tradeSubClasses;
        static std::map<std::string, uint32> itemQualities;
        static std::map<std::string, uint32> projectileSubClasses;
        static std::map<std::string, uint32> slots;
        static std::map<std::string, uint32> skills;
        static std::map<std::string, ChatMsg> chats;
        static std::map<uint8, string> classes;
        static std::map<uint8, string> races;
        static std::map<uint8, std::map<uint8, string> > specs;
    };
};
