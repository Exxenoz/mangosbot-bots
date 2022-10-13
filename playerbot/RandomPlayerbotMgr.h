#ifndef _RandomPlayerbotMgr_H
#define _RandomPlayerbotMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"
#include "PlayerbotMgr.h"
#include "PlayerbotAIConfig.h"

class WorldPacket;
class Player;
class Unit;
class Object;
class Item;

using namespace std;

class CachedEvent
{
public:
    CachedEvent() : value(0), lastChangeTime(0), validIn(0), data("") {}
    CachedEvent(const CachedEvent& other) : value(other.value), lastChangeTime(other.lastChangeTime), validIn(other.validIn), data(other.data) {}
    CachedEvent(uint32 value, uint32 lastChangeTime, uint32 validIn, string data = "") : value(value), lastChangeTime(lastChangeTime), validIn(validIn), data(data) {}

public:
    bool IsEmpty() { return !lastChangeTime; }

public:
    uint32 value, lastChangeTime, validIn;
    string data;
};

class PerformanceMonitorOperation;

//https://gist.github.com/bradley219/5373998

class botPIDImpl;
class botPID
{
public:
    // Kp -  proportional gain
    // Ki -  Integral gain
    // Kd -  derivative gain
    // dt -  loop interval time
    // max - maximum value of manipulated variable
    // min - minimum value of manipulated variable
    botPID(double dt, double max, double min, double Kp, double Kd, double Ki);
    void adjust(double Kp, double Kd, double Ki);
    void reset();
   
    double calculate(double setpoint, double pv);
    ~botPID();

private:
    botPIDImpl* pimpl;
};

class RandomPlayerbotMgr : public PlayerbotHolder
{
    public:
        RandomPlayerbotMgr();
        virtual ~RandomPlayerbotMgr();
        static RandomPlayerbotMgr& instance()
        {
            static RandomPlayerbotMgr instance;
            return instance;
        }

        void LogPlayerLocation();

        virtual void UpdateAIInternal(uint32 elapsed, bool minimal = false);
	public:
        uint32 activeBots = 0;

        static bool HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args);
        bool IsRandomBot(Player* bot);
        bool IsRandomBot(uint32 bot);
        void InstaRandomize(Player* bot);
        void Randomize(Player* bot);
        void RandomizeFirst(Player* bot);
        void UpdateGearSpells(Player* bot);
        void ScheduleTeleport(uint32 bot, uint32 time = 0);
        void ScheduleChangeStrategy(uint32 bot, uint32 time = 0);
        void HandleCommand(uint32 type, const string& text, Player& fromPlayer, string channelName = "", Team team = TEAM_BOTH_ALLOWED);
        string HandleRemoteCommand(string request);
        void OnPlayerLogout(Player* player);
        void OnPlayerLogin(Player* player);
        void OnPlayerLoginError(uint32 bot);
        Player* GetRandomPlayer();
        std::vector<Player*> GetPlayers() { return players; };
        PlayerBotMap GetAllBots() { return playerBots; };
        void PrintStats();
        double GetBuyMultiplier(Player* bot);
        double GetSellMultiplier(Player* bot);
        void AddTradeDiscount(Player* bot, Player* master, int32 value);
        void SetTradeDiscount(Player* bot, Player* master, uint32 value);
        uint32 GetTradeDiscount(Player* bot, Player* master);
        void Refresh(Player* bot);
        void RandomTeleportForLevel(Player* bot, bool activeOnly);
        void RandomTeleportForLevel(Player* bot) { return RandomTeleportForLevel(bot, true); }
        void RandomTeleportForRpg(Player* bot, bool activeOnly);
        void RandomTeleportForRpg(Player* bot) { return RandomTeleportForRpg(bot, true); }
        int GetMaxAllowedBotCount();
        bool ProcessBot(Player* player);
        void Revive(Player* player);
        void ChangeStrategy(Player* player);
        void ChangeStrategyOnce(Player* player);
        uint32 GetValue(Player* bot, string type);
        uint32 GetValue(uint32 bot, string type);
        string GetData(uint32 bot, string type);
        void SetValue(uint32 bot, string type, uint32 value, string data = "");
        void SetValue(Player* bot, string type, uint32 value, string data = "");
        void Remove(Player* bot);
        void Hotfix(Player* player, uint32 version);
        uint32 GetBattleMasterEntry(Player* bot, BattleGroundTypeId bgTypeId, bool fake = false);
        const CreatureDataPair* GetCreatureDataByEntry(uint32 entry);
        uint32 GetCreatureGuidByEntry(uint32 entry);
        void LoadBattleMastersCache();
        std::map<uint32, std::map<uint32, std::map<uint32, bool> > > NeedBots;
        std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > BgBots;
        std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > VisualBots;
        std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > BgPlayers;
        std::map<uint32, std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > > ArenaBots;
        std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > Rating;
        std::map<uint32, std::map<uint32, std::map<uint32, uint32> > > Supporters;
        std::map<Team, std::vector<uint32>> LfgDungeons;
        void CheckBgQueue();
        void CheckLfgQueue();
        void CheckPlayers();
        static Item* CreateTempItem(uint32 item, uint32 count, Player const* player, uint32 randomPropertyId = 0);

        bool AddRandomBot(uint32 bot);

        std::map<Team, std::map<BattleGroundTypeId, std::list<uint32> > > getBattleMastersCache() { return BattleMastersCache; }

        float getActivityMod() { return activityMod; }
        float getActivityPercentage() { return activityMod * 100.0f; }
        void setActivityPercentage(float percentage) { activityMod = percentage / 100.0f; }

        bool arenaTeamsDeleted, guildsDeleted = false;

        std::mutex m_ahActionMutex;
	protected:
	    virtual void OnBotLoginInternal(Player * const bot);

    private:
        //.rndbot pid 0.001, 0.001, 0.0001
        botPID pid = botPID(1, 1, -1, 0.0001, 0.0001, 0.001);
        float activityMod = 0.25;
        uint32 GetEventValue(uint32 bot, string event);
        string GetEventData(uint32 bot, string event);
        uint32 SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn, string data = "");
        std::list<uint32> GetBots();
        std::list<uint32> GetBgBots(uint32 bracket);
        time_t BgCheckTimer;
        time_t LfgCheckTimer;
        time_t PlayersCheckTimer;
        uint32 AddRandomBots();
        bool ProcessBot(uint32 bot);
        void ScheduleRandomize(uint32 bot, uint32 time);
        void RandomTeleport(Player* bot);
        void RandomTeleport(Player* bot, std::vector<WorldLocation> &locs, bool hearth = false, bool activeOnly = false);
        uint32 GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ);
        void PrepareTeleportCache();
        typedef void (RandomPlayerbotMgr::*ConsoleCommandHandler) (Player*);

    private:
        std::vector<Player*> players;
        int processTicks;
        std::map<uint8, std::vector<WorldLocation> > locsPerLevelCache;
        std::map<uint32, std::vector<WorldLocation> > rpgLocsCache;
		map<uint32, std::map<uint32, std::vector<WorldLocation> > > rpgLocsCacheLevel;
        std::map<Team, std::map<BattleGroundTypeId, std::list<uint32> > > BattleMastersCache;
        std::map<uint32, std::map<string, CachedEvent> > eventCache;
        BarGoLink* loginProgressBar;
        std::list<uint32> currentBots;
        std::list<uint32> arenaTeamMembers;
        uint32 bgBotsCount;
        uint32 playersLevel = sPlayerbotAIConfig.randombotStartingLevel;
        PerformanceMonitorOperation* totalPmo;
};

#define sRandomPlayerbotMgr RandomPlayerbotMgr::instance()

#endif
