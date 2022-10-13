#ifndef _RandomPlayerbotFactory_H
#define _RandomPlayerbotFactory_H

#include "Common.h"
#include "PlayerbotAIBase.h"

class WorldPacket;
class Player;
class Unit;
class Object;
class Item;

using namespace std;

class RandomPlayerbotFactory
{
    public:
        RandomPlayerbotFactory(uint32 accountId);
		virtual ~RandomPlayerbotFactory() {}

	public:
        bool CreateRandomBot(uint8 cls, std::unordered_map<uint8, std::vector<std::string>>& names);
        static void CreateRandomBots();
        static void CreateRandomGuilds();
        static void CreateRandomArenaTeams();
        static std::string CreateRandomGuildName();

	private:
        std::string CreateRandomBotName(uint8 gender);
        static std::string CreateRandomArenaTeamName();

    private:
        uint32 accountId;
        static std::map<uint8, std::vector<uint8> > availableRaces;
};

#endif
