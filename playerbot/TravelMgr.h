#pragma once

#include "MoveSplineInitArgs.h"
#include <boost/functional/hash.hpp>
#include "ObjectMgr.h"
#include "PlayerbotAIConfig.h"
#include "GridDefines.h"
#include "SharedDefines.h"

class ByteBuffer;

namespace G3D
{
    class Vector2;
    class Vector3;
    class Vector4;
}

namespace ai
{
    //Constructor types for WorldPosition
    enum WorldPositionConst
    {
        WP_RANDOM = 0,
        WP_CENTROID = 1,
        WP_MEAN_CENTROID = 2,
        WP_CLOSEST = 3
    };

    class GuidPosition;

    typedef std::pair<int, int> mGridPair;

    //Extension of WorldLocation with distance functions.
    class WorldPosition : public WorldLocation
    {
    public:
        //Constructors
        WorldPosition() : WorldLocation() {};
        WorldPosition(const WorldLocation loc) : WorldLocation(loc) {}
        WorldPosition(const WorldPosition& pos) : WorldLocation(pos), visitors(pos.visitors) {}
        WorldPosition(const std::string str) { std::stringstream  out(str); out >> mapid; out >> coord_x; out >> coord_y; out >> coord_z;  out >> orientation; }
        WorldPosition(uint32 mapid, float x, float y, float z = 0, float orientation = 0) : WorldLocation(mapid, x, y, z, orientation) {}
        WorldPosition(uint32 mapId, const Position& pos) : WorldLocation(mapId, pos.GetPositionX(), pos.GetPositionY(),pos.GetPositionZ(), pos.GetPositionO()) {}
        WorldPosition(const WorldObject* wo) { if (wo) { set(WorldLocation(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(), wo->GetOrientation())); } }
        WorldPosition(CreatureDataPair const* cdPair) { if (cdPair) { set(WorldLocation(cdPair->second.mapid, cdPair->second.posX, cdPair->second.posY, cdPair->second.posZ, cdPair->second.orientation)); } }
        WorldPosition(GameObjectDataPair const* cdPair) { if (cdPair) { set(WorldLocation(cdPair->second.mapid, cdPair->second.posX, cdPair->second.posY, cdPair->second.posZ, cdPair->second.orientation)); } }
        WorldPosition(GuidPosition gpos);
        WorldPosition(std::vector<WorldPosition*> list, WorldPositionConst conType);
        WorldPosition(std::vector<WorldPosition> list, WorldPositionConst conType);
        WorldPosition(uint32 mapid, GridPair grid) : WorldLocation(mapid, (int32(grid.x_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET, (int32(grid.y_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET, 0, 0) {}
        WorldPosition(uint32 mapid, CellPair cell) : WorldLocation(mapid, (int32(cell.x_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET, (int32(cell.y_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET, 0, 0) {}
        WorldPosition(uint32 mapid, mGridPair grid) : WorldLocation(mapid, (32 - grid.first)* SIZE_OF_GRIDS, (32 - grid.second)* SIZE_OF_GRIDS, 0, 0) {}

        //Setters
        void set(const WorldLocation& pos) { mapid = pos.mapid; coord_x = pos.coord_x; coord_y = pos.coord_y; coord_z = pos.coord_z; orientation = pos.orientation; }
        void set(const WorldPosition& pos) { mapid = pos.mapid; coord_x = pos.coord_x; coord_y = pos.coord_y; coord_z = pos.coord_z; orientation = pos.orientation; visitors= pos.visitors; }
        void set(ObjectGuid guid);
        void setMapId(uint32 id) { mapid = id; }
        void setX(float x) { coord_x = x; }
        void setY(float y) { coord_y = y; }
        void setZ(float z) { coord_z = z; }
        void setO(float o) {orientation = o;}

        void addVisitor() { visitors++; }
        void remVisitor() { visitors--; }

        //Getters
        operator bool() const { return  mapid != 0 || coord_x != 0 || coord_y != 0 || coord_z != 0; }
        bool operator==(const WorldPosition& p1) { return mapid == p1.mapid && coord_x == p1.coord_x && coord_y == p1.coord_y && coord_z == p1.coord_z && orientation == p1.orientation; }
        bool operator!=(const WorldPosition& p1) { return mapid != p1.mapid || coord_x != p1.coord_x || coord_y != p1.coord_y || coord_z != p1.coord_z || orientation != p1.orientation; }
        
        WorldPosition& operator+=(const WorldPosition& p1) { coord_x += p1.coord_x; coord_y += p1.coord_y; coord_z += p1.coord_z; return *this; }
        WorldPosition& operator-=(const WorldPosition& p1) { coord_x -= p1.coord_x; coord_y -= p1.coord_y; coord_z -= p1.coord_z; return *this; }

        WorldPosition& operator*=(const float& s) { coord_x *= s; coord_y *= s; coord_z *= s; return *this; }
        WorldPosition& operator/=(const float& s) { coord_x /= s; coord_y /= s; coord_z /= s; return *this; }

        uint32 getMapId() const { return mapid; }
        float getX() const { return coord_x; }
        float getY() const { return coord_y; }
        float getZ() const { return coord_z; }
        float getO() const { return orientation; }
        G3D::Vector3 getVector3();
        virtual std::string print();
        std::string to_string() {std::stringstream out; out << mapid; out << coord_x; out << coord_y; out << coord_z;  out << orientation; return out.str();};

        void printWKT(std::vector<WorldPosition> points, std::ostringstream& out, uint32 dim = 0, bool loop = false);
        void printWKT(std::ostringstream& out) { printWKT({ *this }, out); }

        uint32 getVisitors() { return visitors; }

        bool isOverworld() { return mapid == 0 || mapid == 1 || mapid == 530 || mapid == 571; }
        bool isInWater() { return getTerrain() ? getTerrain()->IsInWater(coord_x, coord_y, coord_z) : false; };
        bool isUnderWater() { return getTerrain() ? getTerrain()->IsUnderWater(coord_x, coord_y, coord_z) : false; };

        WorldPosition relPoint(WorldPosition* center) { return WorldPosition(mapid, coord_x - center->coord_x, coord_y - center->coord_y, coord_z - center->coord_z, orientation); }
        WorldPosition offset(WorldPosition* center) { return WorldPosition(mapid, coord_x + center->coord_x, coord_y + center->coord_y, coord_z + center->coord_z, orientation); }
        float size() { return sqrt(pow(coord_x, 2.0) + pow(coord_y, 2.0) + pow(coord_z, 2.0)); }

        //Slow distance function using possible map transfers.
        float distance(WorldPosition* to);
        float distance(WorldPosition to) { return distance(&to); };

        float fDist(WorldPosition* to);
        float fDist(WorldPosition to) { return fDist(&to); };

        //Returns the closest point from the list.
        WorldPosition* closest(std::vector<WorldPosition*> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->distance(i) < this->distance(j); }); }
        WorldPosition closest(std::vector<WorldPosition> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->distance(i) < this->distance(j); }); }

        WorldPosition* furtest(std::vector<WorldPosition*> list) { return *std::max_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->distance(i) < this->distance(j); }); }
        WorldPosition furtest(std::vector<WorldPosition> list) { return *std::max_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->distance(i) < this->distance(j); }); }

        template<class T>
        std::pair<T, WorldPosition>  closest(std::list<std::pair<T, WorldPosition>> list) { return *std::min_element(list.begin(), list.end(), [this](std::pair<T, WorldPosition> i, std::pair<T, WorldPosition> j) {return this->distance(i.second) < this->distance(j.second); }); }
        template<class T>
        std::pair<T, WorldPosition> closest(std::list<T> list) { return closest(GetPosList(list)); }

        template<class T>
        std::pair<T, WorldPosition>  closest(std::vector<std::pair<T, WorldPosition>> list) { return *std::min_element(list.begin(), list.end(), [this](std::pair<T, WorldPosition> i, std::pair<T, WorldPosition> j) {return this->distance(i.second) < this->distance(j.second); }); }
        template<class T>
        std::pair<T, WorldPosition> closest(std::vector<T> list) { return closest(GetPosVector(list)); }


        //Quick square distance in 2d plane.
        float sqDistance2d(WorldPosition to) { return (getX() - to.getX()) * (getX() - to.getX()) + (getY() - to.getY()) * (getY() - to.getY()); };
        float sqDistance2d(WorldPosition* to) { return (getX() - to->getX()) * (getX() - to->getX()) + (getY() - to->getY()) * (getY() - to->getY()); };

        //Quick square distance calculation without map check. Used for getting the minimum distant points.
        float sqDistance(WorldPosition to) { return (getX() - to.getX()) * (getX() - to.getX()) + (getY() - to.getY()) * (getY() - to.getY()) + (getZ() - to.getZ()) * (getZ() - to.getZ()); };
        float sqDistance(WorldPosition* to) { return (getX() - to->getX()) * (getX() - to->getX()) + (getY() - to->getY()) * (getY() - to->getY()) + (getZ() - to->getZ()) * (getZ() - to->getZ()); };

        //Returns the closest point of the list. Fast but only works for the same map.
        WorldPosition* closestSq(std::vector<WorldPosition*> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return sqDistance(i) < sqDistance(j); }); }
        WorldPosition closestSq(std::vector<WorldPosition> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return sqDistance(i) < sqDistance(j); }); }

        float getAngleTo(WorldPosition endPos) { float ang = atan2(endPos.getY() - getY(), endPos.getX() - getX()); return (ang >= 0) ? ang : 2 * M_PI_F + ang; };
        float getAngleBetween(WorldPosition dir1, WorldPosition dir2) { return abs(getAngleTo(dir1) - getAngleTo(dir2)); };

        WorldPosition limit(WorldPosition center, float maxDistance) { WorldPosition pos(*this); pos -= center; float size = pos.size(); if (size > maxDistance) { pos /= pos.size(); pos *= maxDistance; pos += center; } return pos; }

        WorldPosition lastInRange(std::vector<WorldPosition> list, float minDist = -1, float maxDist = -1);
        WorldPosition firstOutRange(std::vector<WorldPosition> list, float minDist = -1, float maxDist = -1);

        float mSign(WorldPosition* p1, WorldPosition* p2) { return(getX() - p2->getX()) * (p1->getY() - p2->getY()) - (p1->getX() - p2->getX()) * (getY() - p2->getY()); }
        bool isInside(WorldPosition* p1, WorldPosition* p2, WorldPosition* p3);

        void distancePartition(std::vector<float> distanceLimits, WorldPosition* to, std::vector<std::vector<WorldPosition*>>& partition);
        std::vector<std::vector<WorldPosition*>> distancePartition(std::vector<float> distanceLimits, std::vector<WorldPosition*> points);

        //Map functions. Player independent.
        const MapEntry* getMapEntry() const { return sMapStore.LookupEntry(mapid); };
        uint32 getInstanceId() const { for (auto& map : sMapMgr.Maps()) { if (map.second->GetId() == getMapId()) return map.second->GetInstanceId(); }; return 0; }
        Map* getMap() const { return sMapMgr.FindMap(mapid, getMapEntry()->Instanceable() ? getInstanceId() : 0); }
        const TerrainInfo* getTerrain() { return getMap() ? getMap()->GetTerrain() : NULL; }

#if defined(MANGOSBOT_TWO) || MAX_EXPANSION == 2
        bool IsInLineOfSight(WorldPosition pos, float heightMod = 0.5f) { return mapid == pos.mapid && getMap() && getMap()->IsInLineOfSight(coord_x, coord_y, coord_z + heightMod, pos.coord_x, pos.coord_y, pos.coord_z + heightMod, 0, true); }
#else
        bool IsInLineOfSight(WorldPosition pos, float heightMod = 0.5f) { return mapid == pos.mapid && getMap() && getMap()->IsInLineOfSight(coord_x, coord_y, coord_z + heightMod, pos.coord_x, pos.coord_y, pos.coord_z + heightMod, true); }
#endif

        bool isOutside() { WorldPosition high(*this); high.setZ(getZ() + 500.0f); return IsInLineOfSight(high); }

#if defined(MANGOSBOT_TWO) || MAX_EXPANSION == 2
        const float getHeight() { return getMap()->GetHeight(0, getX(), getY(), getZ()); }
#else
        const float getHeight() { return getMap() ? getMap()->GetHeight(getX(), getY(), getZ()) : getZ(); }
#endif

        const float currentHeight() { return getZ() - getHeight(); }

        std::set<Transport*> getTransports(uint32 entry = 0);

        GridPair getGridPair() { return MaNGOS::ComputeGridPair(getX(), getY()); };
        std::vector<GridPair> getGridPairs(WorldPosition secondPos);
        std::vector<WorldPosition> fromGridPair(GridPair gridPair);

        CellPair getCellPair() { return MaNGOS::ComputeCellPair(getX(), getY()); }
        std::vector<WorldPosition> fromCellPair(CellPair cellPair);
        std::vector<WorldPosition> gridFromCellPair(CellPair cellPair);

        mGridPair getmGridPair() {
            return std::make_pair((int)(32 - getX() / SIZE_OF_GRIDS), (int)(32 - getY() / SIZE_OF_GRIDS)); }

        std::vector<mGridPair> getmGridPairs(WorldPosition secondPos);
        std::vector<WorldPosition> frommGridPair(mGridPair gridPair);

        void loadMapAndVMap(uint32 mapId, uint32 instanceId, int x, int y);
        void loadMapAndVMap(uint32 instanceId) {loadMapAndVMap(getMapId(), instanceId, getmGridPair().first, getmGridPair().second); }
        void loadMapAndVMaps(WorldPosition secondPos, uint32 instanceId);

        //Display functions
        WorldPosition getDisplayLocation();
        float getDisplayX() { return getDisplayLocation().getY() * -1.0; }
        float getDisplayY() { return getDisplayLocation().getX(); }

        uint16 getAreaFlag() { return sTerrainMgr.GetAreaFlag(getMapId(), getX(), getY(), getZ()); };
        AreaTableEntry const* getArea();
        std::string getAreaName(bool fullName = true, bool zoneName = false);
        int32 getAreaLevel();

        std::vector<WorldPosition> fromPointsArray(std::vector<G3D::Vector3> path);

        //Pathfinding
        std::vector<WorldPosition> getPathStepFrom(WorldPosition startPos, Unit* bot);
        std::vector<WorldPosition> getPathFromPath(std::vector<WorldPosition> startPath, Unit* bot, uint8 maxAttempt = 40);
        std::vector<WorldPosition> getPathFrom(WorldPosition startPos, Unit* bot) { return getPathFromPath({ startPos }, bot); };
        std::vector<WorldPosition> getPathTo(WorldPosition endPos, Unit* bot) { return endPos.getPathFrom(*this, bot); }
        bool isPathTo(std::vector<WorldPosition> path, float maxDistance = sPlayerbotAIConfig.targetPosRecalcDistance) { return !path.empty() && distance(path.back()) < maxDistance; };
        bool cropPathTo(std::vector<WorldPosition>& path, float maxDistance = sPlayerbotAIConfig.targetPosRecalcDistance);
        bool canPathTo(WorldPosition endPos, Unit* bot) { return endPos.isPathTo(getPathTo(endPos, bot)); }

        float getPathLength(std::vector<WorldPosition> points) { float dist; for (auto& p : points) if (&p == &points.front()) dist = 0; else dist += std::prev(&p, 1)->distance(p); return dist; }

        bool GetReachableRandomPointOnGround(Player* bot, float radius, bool randomRange = true) {
#ifndef MANGOSBOT_TWO         
            return getMap()->GetReachableRandomPointOnGround(coord_x, coord_y, coord_z, radius, randomRange);
#else
            return getMap()->GetReachableRandomPointOnGround(bot->GetPhaseMask(), coord_x, coord_y, coord_z, radius, randomRange);
#endif
        }

        uint32 getUnitsNear(std::list<ObjectGuid>& units, float radius);
        uint32 getUnitsAggro(std::list<ObjectGuid>& units, Player* bot);

        //Creatures
        std::vector<CreatureDataPair const*> getCreaturesNear(float radius = 0, uint32 entry = 0);
        //GameObjects
        std::vector<GameObjectDataPair const*> getGameObjectsNear(float radius = 0, uint32 entry = 0);
    private:
        uint32 visitors = 0;        
    };

    inline ByteBuffer& operator<<(ByteBuffer& b, WorldPosition& guidP)
    {
        b << guidP.getMapId();
        b << guidP.getX();
        b << guidP.getY();
        b << guidP.getZ();
        b << guidP.getO();
        b << guidP.getVisitors();
        return b;
    }

    inline ByteBuffer& operator>>(ByteBuffer& b, WorldPosition& g)
    {
        uint32 mapid;
        float coord_x;
        float coord_y;
        float coord_z;
        float orientation;
        uint32 visitors = 0;
        b >> mapid;
        b >> coord_x;
        b >> coord_y;
        b >> coord_z;
        b >> orientation;
        b >> visitors;

        return b;
    }

    //Generic creature finder
    class FindPointCreatureData
    {
    public:
        FindPointCreatureData(WorldPosition point1 = WorldPosition(), float radius1 = 0, uint32 entry1 = 0) { point = point1; radius = radius1; entry = entry1; }

        bool operator()(CreatureDataPair const& dataPair);
        std::vector<CreatureDataPair const*> GetResult() const { return data; };
    private:
        WorldPosition point;
        float radius;
        uint32 entry;

        std::vector<CreatureDataPair const*> data;
    };

    //Generic gameObject finder
    class FindPointGameObjectData
    {
    public:
        FindPointGameObjectData(WorldPosition point1 = WorldPosition(), float radius1 = 0, uint32 entry1 = 0) { point = point1; radius = radius1; entry = entry1; }

        bool operator()(GameObjectDataPair const& dataPair);
        std::vector<GameObjectDataPair const*> GetResult() const { return data; };
    private:
        WorldPosition point;
        float radius;
        uint32 entry;

        std::vector<GameObjectDataPair const*> data;
    };

    class GuidPosition : public ObjectGuid, public WorldPosition
    {
    public:
        GuidPosition() : ObjectGuid(), WorldPosition() {}
        GuidPosition(ObjectGuid guid) { ObjectGuid::Set(guid); WorldPosition::set(guid); };
        GuidPosition(ObjectGuid guid, WorldPosition pos) : ObjectGuid(guid), WorldPosition(pos) {};
        GuidPosition(uint64 const& guid, WorldPosition const& pos) : ObjectGuid(guid), WorldPosition(pos) {};
        //template<class T>
        //GuidPosition(ObjectGuid guid, T) : ObjectGuid(guid) {WorldPosition::set(WorldPosition(T))};
        GuidPosition(CreatureDataPair const* dataPair) : ObjectGuid(HIGHGUID_UNIT, dataPair->second.id, dataPair->first), WorldPosition(dataPair) {};
        GuidPosition(GameObjectDataPair const* dataPair) : ObjectGuid(HIGHGUID_GAMEOBJECT, dataPair->second.id, dataPair->first), WorldPosition(dataPair) {};
        GuidPosition(WorldObject* wo) : WorldPosition(wo) { ObjectGuid::Set(wo->GetObjectGuid()); };
        GuidPosition(HighGuid hi, uint32 entry, uint32 counter = 1, WorldPosition pos = WorldPosition()) : ObjectGuid(hi, entry, counter), WorldPosition(pos) {};
        //GuidPosition(const GuidPosition& guidp) {this->Set(guidp);  this->setLocation(((WorldPosition)guidp).getLocation()); };

        CreatureData* GetCreatureData() { return IsCreature() ? sObjectMgr.GetCreatureData(GetCounter()) : nullptr; }
        CreatureInfo const* GetCreatureTemplate()const {return IsCreature() ? sObjectMgr.GetCreatureTemplate(GetEntry()) : nullptr; };

        GameObjectInfo const* GetGameObjectInfo() { return IsGameObject() ? sObjectMgr.GetGameObjectInfo(GetEntry()) : nullptr; };

        WorldObject* GetWorldObject() { return getMap() ? getMap()->GetWorldObject(*this) : nullptr;}
        Creature* GetCreature() const;
        Unit* GetUnit() const;
        GameObject* GetGameObject();
        Player* GetPlayer() const;

        bool HasNpcFlag(NPCFlags flag) { return IsCreature() && GetCreatureTemplate()->NpcFlags & flag; }
        bool isGoType(GameobjectTypes type) { return IsGameObject() && GetGameObjectInfo()->type == type; }

        const FactionTemplateEntry* GetFactionTemplateEntry() const;
        const ReputationRank GetReactionTo(const GuidPosition& other);
        bool IsFriendlyTo(const GuidPosition& other) { return (GetFactionTemplateEntry() && other.GetFactionTemplateEntry()) ? (GetReactionTo(other) > REP_NEUTRAL) : false; }
        bool IsHostileTo(const GuidPosition& other) { return (GetFactionTemplateEntry() && other.GetFactionTemplateEntry()) ? (GetReactionTo(other) < REP_NEUTRAL) : false; }

        bool isDead(); //For loaded grids check if the unit/object is unloaded/dead.

        uint16 IsPartOfAPool();
        uint16 GetGameEventId();
        bool IsEventUnspawned();

        virtual std::string print();

        operator bool() const { return  WorldPosition(*this) && !IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }
    };

    inline ByteBuffer& operator<<(ByteBuffer& b, GuidPosition& guidP)
    {
        b << (ObjectGuid)guidP;
        b << (WorldPosition)guidP;


        return b;
    }
        
    inline ByteBuffer& operator>>(ByteBuffer& b, GuidPosition& g)
    {
        ObjectGuid guid;
        WorldPosition pos;

        b >> guid;
        b >> pos;

        g = GuidPosition(guid, pos);

        return b;
    }

    template<class T>
    std::list<std::pair<T, WorldPosition>> GetPosList(std::list<T> oList) { std::list<std::pair<T, WorldPosition>> retList; for (auto& obj : oList) retList.push_back(std::make_pair(obj, WorldPosition(obj))); return retList; };

    template<class T>
    std::vector<std::pair<T, WorldPosition>> GetPosVector(std::vector<T> oList) { std::vector<std::pair<T, WorldPosition>> retList; for (auto& obj : oList) retList.push_back(std::make_pair(obj, WorldPosition(obj))); return retList; };

    class mapTransfer
    {
    public:
        mapTransfer(WorldPosition pointFrom1, WorldPosition pointTo1, float portalLength1 = 0.1f)
            : pointFrom(pointFrom1), pointTo(pointTo1), portalLength(portalLength1) {}

        bool isFrom(WorldPosition point) { return point.getMapId() == pointFrom.getMapId(); }
        bool isTo(WorldPosition point) { return point.getMapId() == pointTo.getMapId(); }

        WorldPosition* getPointFrom() { return &pointFrom; }
        WorldPosition* getPointTo() { return &pointTo; }

        bool isUseful(WorldPosition point) { return isFrom(point) || isTo(point); }
        float distance(WorldPosition point) { return isUseful(point) ? (isFrom(point) ? point.distance(pointFrom) : point.distance(pointTo)) : 200000; }

        bool isUseful(WorldPosition start, WorldPosition end) { return isFrom(start) && isTo(end); }
        float distance(WorldPosition start, WorldPosition end) { return (isUseful(start, end) ? (start.distance(pointFrom) + portalLength + pointTo.distance(end)) : 200000); }
        float fDist(WorldPosition start, WorldPosition end) { return start.fDist(pointFrom) + portalLength + pointTo.fDist(end); }
    private:
        WorldPosition pointFrom, pointTo;
        float portalLength = 0.1f;
    };

    //A destination for a bot to travel to and do something.
    class TravelDestination
    {
    public:
        TravelDestination() {}        
        TravelDestination(float radiusMin1, float radiusMax1) { radiusMin = radiusMin1; radiusMax = radiusMax1; }
        TravelDestination(std::vector<WorldPosition*> points1, float radiusMin1, float radiusMax1) { points = points1;  radiusMin = radiusMin1; radiusMax = radiusMax1; }

        void addPoint(WorldPosition* pos) { points.push_back(pos); }
        bool hasPoint(WorldPosition* pos) { return std::find(points.begin(), points.end(), pos) != points.end(); }
        void setExpireDelay(uint32 delay) { expireDelay = delay; }
        void setCooldownDelay(uint32 delay) { cooldownDelay = delay; }
        void setMaxVisitors(uint32 maxVisitors1 = 0, uint32 maxVisitorsPerPoint1 = 0) { maxVisitors = maxVisitors1; maxVisitorsPerPoint = maxVisitorsPerPoint1; }

        std::vector<WorldPosition*> getPoints(bool ignoreFull = false);
        uint32 getExpireDelay() { return expireDelay; }
        uint32 getCooldownDelay() { return cooldownDelay; }
        void addVisitor() { visitors++; }
        void remVisitor() { visitors--; }
        uint32 getVisitors() { return visitors; }

        virtual Quest const* GetQuestTemplate() { return NULL; }

        virtual bool isActive(Player* bot) { return false; }
        bool isFull(bool ignoreFull = false);

        virtual std::string getName() { return "TravelDestination"; }
        virtual int32 getEntry() { return 0; }
        virtual std::string getTitle() { return "generic travel destination"; }

        WorldPosition* nearestPoint(WorldPosition* pos) {return nearestPoint(*pos);};
        WorldPosition* nearestPoint(WorldPosition pos);
        float distanceTo(WorldPosition* pos) { return nearestPoint(pos)->distance(pos); }
        float distanceTo(WorldPosition pos) { return nearestPoint(pos)->distance(pos); }
        bool onMap(WorldPosition* pos) {return nearestPoint(pos)->getMapId() == pos->getMapId();}
        virtual bool isIn(WorldPosition* pos, float radius = 0) { return onMap(pos) && distanceTo(pos) <= (radius? radius : radiusMin); }
        virtual bool isOut(WorldPosition* pos, float radius = 0) { return !onMap(pos) || distanceTo(pos) > (radius? radius : radiusMax); }
        float getRadiusMin() { return radiusMin; }

        std::vector<WorldPosition*> touchingPoints(WorldPosition* pos);
        std::vector<WorldPosition*> sortedPoints(WorldPosition* pos);
        std::vector<WorldPosition*> nextPoint(WorldPosition* pos, bool ignoreFull = true);
    protected:
        std::vector<WorldPosition*> points;
        float radiusMin = 0;
        float radiusMax = 0;

        uint32 visitors = 0;
        uint32 maxVisitors = 0;
        uint32 maxVisitorsPerPoint = 0;
        uint32 expireDelay = 5 * 1000;
        uint32 cooldownDelay = 60 * 1000;
    };

    //A travel target that is always inactive and jumps to cooldown.
    class NullTravelDestination : public TravelDestination
    {
    public:
        NullTravelDestination(uint32 cooldownDelay1 = 5 * 60 * 1000) : TravelDestination() { cooldownDelay = cooldownDelay1;};

        virtual Quest const* GetQuestTemplate() { return NULL; }

        virtual bool isActive(Player* bot) { return false; }
        
        virtual std::string getName() { return "NullTravelDestination"; }
        virtual std::string getTitle() { return "no destination"; }

        virtual bool isIn(WorldPosition* pos) { return true; }
        virtual bool isOut(WorldPosition* pos) { return false; }

    protected:
    };


    //A travel target specifically related to a quest.
    class QuestTravelDestination : public TravelDestination
    {
    public:
        QuestTravelDestination(uint32 questId1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) { questId = questId1; questTemplate = sObjectMgr.GetQuestTemplate(questId); }

        virtual Quest const* GetQuestTemplate() { return questTemplate;  }

        virtual bool isActive(Player* bot) { return bot->IsActiveQuest(questId); }
        
        virtual std::string getName() { return "QuestTravelDestination"; }
        virtual int32 getEntry() { return 0; }
        virtual std::string getTitle();
    protected:
        uint32 questId;
        Quest const* questTemplate;
    };

    //A quest giver or taker.
    class QuestRelationTravelDestination : public QuestTravelDestination
    {
    public:
        QuestRelationTravelDestination(uint32 quest_id1, uint32 entry1, uint32 relation1, float radiusMin1, float radiusMax1) :QuestTravelDestination(quest_id1, radiusMin1, radiusMax1) { entry = entry1; relation = relation1; }

        virtual bool isActive(Player* bot);

        virtual CreatureInfo const* getCreatureInfo() { return entry > 0 ? ObjectMgr::GetCreatureTemplate(entry) : nullptr; }

        virtual std::string getName() { return "QuestRelationTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual std::string getTitle();
        virtual uint32 getRelation() { return relation; }
    private:
        uint32 relation;
        int32 entry;
    };

    //A quest objective (creature/gameobject to grind/loot)
    class QuestObjectiveTravelDestination : public QuestTravelDestination
    {
    public:
        QuestObjectiveTravelDestination(uint32 quest_id1, uint32 entry1, uint32 objective1, float radiusMin1, float radiusMax1) : QuestTravelDestination(quest_id1, radiusMin1, radiusMax1) {
            objective = objective1; entry = entry1;
        }

        bool isCreature() { return GetQuestTemplate()->ReqCreatureOrGOId[objective] > 0; }

        int32 ReqCreature() {
            return isCreature() ? GetQuestTemplate()->ReqCreatureOrGOId[objective] : 0;
        }
        int32 ReqGOId() {
            return !isCreature() ? abs(GetQuestTemplate()->ReqCreatureOrGOId[objective]) : 0;
        }
        uint32 ReqCount() { return GetQuestTemplate()->ReqCreatureOrGOCount[objective]; }

        virtual bool isActive(Player* bot);

        virtual std::string getName() { return "QuestObjectiveTravelDestination"; }

        virtual int32 getEntry() { return entry; }

        virtual std::string getTitle();

        virtual uint32 getObjective() { return objective; }
    private:
        uint32 objective;
        int32 entry;
    };

    //A location with rpg target(s) based on race and level
    class RpgTravelDestination : public TravelDestination
    {
    public:
        RpgTravelDestination(uint32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1;
        }

        virtual bool isActive(Player* bot);

        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual std::string getName() { return "RpgTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual std::string getTitle();
    protected:
        int32 entry;
    };

    //A location with zone exploration target(s) 
    class ExploreTravelDestination : public TravelDestination
    {
    public:
        ExploreTravelDestination(uint32 areaId1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            areaId = areaId1;
        }

        virtual bool isActive(Player* bot);

        virtual std::string getName() { return "ExploreTravelDestination"; }
        virtual int32 getEntry() { return NULL; }
        virtual std::string getTitle() { return title; }
        virtual void setTitle(std::string newTitle) { title = newTitle; }
        virtual uint32 getAreaId() { return areaId; }
    protected:
        uint32 areaId;
        std::string title = "";
    };

    //A location with zone exploration target(s) 
    class GrindTravelDestination : public TravelDestination
    {
    public:
        GrindTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1;
        }

        virtual bool isActive(Player* bot);
        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual std::string getName() { return "GrindTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual std::string getTitle();
    protected:
        int32 entry;
    };

    //A location with a boss
    class BossTravelDestination : public TravelDestination
    {
    public:
        BossTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1; cooldownDelay = 1000;
        }

        virtual bool isActive(Player* bot);
        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual std::string getName() { return "BossTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual std::string getTitle();
    protected:
        int32 entry;
    };
   
    //A quest destination container for quick lookup of all destinations related to a quest.
    struct QuestContainer
    {
        std::vector<QuestTravelDestination*> questGivers;
        std::vector<QuestTravelDestination*> questTakers;
        std::vector<QuestTravelDestination*> questObjectives;
    };

    //
    enum TravelState
    {
        TRAVEL_STATE_IDLE = 0,
        TRAVEL_STATE_TRAVEL_PICK_UP_QUEST = 1,
        TRAVEL_STATE_WORK_PICK_UP_QUEST = 2,
        TRAVEL_STATE_TRAVEL_DO_QUEST = 3,
        TRAVEL_STATE_WORK_DO_QUEST = 4,
        TRAVEL_STATE_TRAVEL_HAND_IN_QUEST = 5,
        TRAVEL_STATE_WORK_HAND_IN_QUEST = 6,
        TRAVEL_STATE_TRAVEL_RPG = 7,
        TRAVEL_STATE_TRAVEL_EXPLORE = 8,
        MAX_TRAVEL_STATE
    };

    enum TravelStatus
    {
        TRAVEL_STATUS_NONE = 0,
        TRAVEL_STATUS_PREPARE = 1,
        TRAVEL_STATUS_TRAVEL = 2,
        TRAVEL_STATUS_WORK = 3,
        TRAVEL_STATUS_COOLDOWN = 4,
        TRAVEL_STATUS_EXPIRED = 5,
        MAX_TRAVEL_STATUS
    };

    //Current target and location for the bot to travel to.
    //The flow is as follows:
    //PREPARE   (wait until no loot is near)
    //TRAVEL    (move towards target until close enough) (rpg and grind is disabled)
    //WORK      (grind/rpg until the target is no longer active) (rpg and grind is enabled on quest mobs)
    //COOLDOWN  (wait some time free to do what the bot wants)
    //EXPIRE    (if any of the above actions take too long pick a new target)
    class TravelTarget : AiObject
    {
    public:
        TravelTarget(PlayerbotAI* ai) : AiObject(ai) {};
        TravelTarget(PlayerbotAI* ai, TravelDestination* tDestination1, WorldPosition* wPosition1) : AiObject(ai) { setTarget(tDestination1, wPosition1);}
        ~TravelTarget();
        
        void setTarget(TravelDestination* tDestination1, WorldPosition* wPosition1, bool groupCopy1 = false);
        void setStatus(TravelStatus status);
        void setExpireIn(uint32 expireMs) { statusTime = getExpiredTime() + expireMs; }
        void incRetry(bool isMove) { if (isMove) moveRetryCount+=2; else extendRetryCount+=2; }
        void decRetry(bool isMove) { if (isMove && moveRetryCount > 0) moveRetryCount--; else if (extendRetryCount > 0) extendRetryCount--; }
        void setRetry(bool isMove, uint32 newCount = 0) { if (isMove) moveRetryCount = newCount; else extendRetryCount = newCount; }
        void setGroupCopy(bool isGroupCopy = true) { groupCopy = isGroupCopy; }
        void setForced(bool forced1) { forced = forced1; }
        void setRadius(float radius1) { radius = radius1; }

        void copyTarget(TravelTarget* target);
        void addVisitors();
        void releaseVisitors();

        float distance(Player* bot) { WorldPosition pos(bot);  return wPosition->distance(&pos); };
        WorldPosition* getPosition() { return wPosition; };
        TravelDestination* getDestination() { return tDestination; };
        uint32 getEntry() { if (!tDestination) return 0; return tDestination->getEntry(); }
        PlayerbotAI* getAi() { return ai; }

        uint32 getExpiredTime() { return WorldTimer::getMSTime() - startTime; }
        uint32 getTimeLeft() { return statusTime - getExpiredTime(); }
        uint32 getMaxTravelTime() { return (1000.0 * distance(bot)) / bot->GetSpeed(MOVE_RUN); }
        uint32 getRetryCount(bool isMove) { return isMove ? moveRetryCount: extendRetryCount; }

        bool isTraveling(); 
        bool isActive();   
        bool isWorking();
        bool isPreparing();
        bool isMaxRetry(bool isMove) { return isMove ? (moveRetryCount > 10) : (extendRetryCount > 5); }
        TravelStatus getStatus() { return m_status; }

        TravelState getTravelState();

        bool isGroupCopy() { return groupCopy; }
        bool isForced() { return forced; }
    protected:
        TravelStatus m_status = TRAVEL_STATUS_NONE;

        uint32 startTime = WorldTimer::getMSTime();
        uint32 statusTime = 0;

        bool forced = false;
        float radius = 0;
        bool groupCopy = false;
        bool visitor = true;

        uint32 extendRetryCount = 0;
        uint32 moveRetryCount = 0;

        TravelDestination* tDestination = NULL;
        WorldPosition* wPosition = NULL;
    };

    //General container for all travel destinations.
    class TravelMgr
    {
    public:
        TravelMgr() {};
        void Clear();

        static void SetMobAvoidArea();
        static void SetMobAvoidAreaMap(uint32 mapId);

        void LoadQuestTravelTable();

        template <class D, class W, class URBG>
        void weighted_shuffle
        (D first, D last
            , W first_weight, W last_weight
            , URBG&& g)
        {
            while (first != last && first_weight != last_weight)
            {
                std::discrete_distribution<int> dd(first_weight, last_weight);
                auto i = dd(g);

                if (i)
                {
                    swap(*first, *std::next(first, i));
                    swap(*first_weight, *std::next(first_weight, i));
                }
                ++first;
                ++first_weight;
            }
        }

        std::vector <WorldPosition*> getNextPoint(WorldPosition* center, std::vector<WorldPosition*> points, uint32 amount = 1);
        std::vector <WorldPosition> getNextPoint(WorldPosition center, std::vector<WorldPosition> points, uint32 amount = 1);
        QuestStatusData* getQuestStatus(Player* bot, uint32 questId);
        bool getObjectiveStatus(Player* bot, Quest const* pQuest, uint32 objective);
        uint32 getDialogStatus(Player* pPlayer, int32 questgiver, Quest const* pQuest);
        std::vector<TravelDestination*> getQuestTravelDestinations(Player* bot, int32 questId = -1, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000, bool ignoreObjectives = false);
        std::vector<TravelDestination*> getRpgTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000);
        std::vector<TravelDestination*> getExploreTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false);
        std::vector<TravelDestination*> getGrindTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000);
        std::vector<TravelDestination*> getBossTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 25000);


        void setNullTravelTarget(Player* player);

        void addMapTransfer(WorldPosition start, WorldPosition end, float portalDistance = 0.1f, bool makeShortcuts = true);
        void loadMapTransfers();
        float mapTransDistance(WorldPosition start, WorldPosition end);
        float fastMapTransDistance(WorldPosition start, WorldPosition end);

        NullTravelDestination* nullTravelDestination = new NullTravelDestination();
        WorldPosition* nullWorldPosition = new WorldPosition();

        void addBadVmap(uint32 mapId, int x, int y) { badVmap.push_back(std::make_tuple(mapId, x, y)); }
        void addBadMmap(uint32 mapId, int x, int y) { badMmap.push_back(std::make_tuple(mapId, x, y)); }
        bool isBadVmap(uint32 mapId, int x, int y) { return std::find(badVmap.begin(), badVmap.end(), std::make_tuple(mapId, x, y)) != badVmap.end(); }
        bool isBadMmap(uint32 mapId, int x, int y) { return std::find(badMmap.begin(), badMmap.end(), std::make_tuple(mapId, x, y)) != badMmap.end(); }


        void printGrid(uint32 mapId, int x, int y, std::string type);
        void printObj(WorldObject* obj, std::string type);

        void logEvent(PlayerbotAI* ai, std::string eventName, std::string info1 = "", std::string info2 = "");
        void logEvent(PlayerbotAI* ai, std::string eventName, ObjectGuid guid, std::string info2);

        int32 getAreaLevel(uint32 area_id);
        void loadAreaLevels();
    //protected:
        void logQuestError(uint32 errorNr, Quest* quest, uint32 objective = 0, uint32 unitId = 0, uint32 itemId = 0);

        std::vector<uint32> avoidLoaded;

        std::vector<QuestTravelDestination*> questGivers;
        std::vector<RpgTravelDestination*> rpgNpcs;
        std::vector<GrindTravelDestination*> grindMobs;
        std::vector<BossTravelDestination*> bossMobs;

        std::unordered_map<uint32, ExploreTravelDestination*> exploreLocs;
        std::unordered_map<uint32, QuestContainer*> quests;
        std::unordered_map<uint64, GuidPosition> pointsMap;

        std::unordered_map<uint32, int32> areaLevels;

        std::vector<tuple<uint32, int, int>> badVmap, badMmap;

        std::unordered_map<std::pair<uint32, uint32>, std::vector<mapTransfer>, boost::hash<std::pair<uint32, uint32>>> mapTransfersMap;
    };
}

#define sTravelMgr MaNGOS::Singleton<TravelMgr>::Instance()

