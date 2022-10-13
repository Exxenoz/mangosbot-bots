#include "botpch.h"
#include "../../playerbot.h"
#include "AttackersValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> AttackersValue::Calculate()
{
    set<Unit*> targets;

    std::list<ObjectGuid> result;

    if (!ai->AllowActivity(ALL_ACTIVITY))
        return result;

    AddAttackersOf(bot, targets);

    Group* group = bot->GetGroup();
    if (group)
        AddAttackersOf(group, targets);

    RemoveNonThreating(targets);
    
	for (set<Unit*>::iterator i = targets.begin(); i != targets.end(); i++)
		result.push_back((*i)->GetObjectGuid());

    if (bot->duel && bot->duel->opponent)
        result.push_back(bot->duel->opponent->GetObjectGuid());

	return result;
}

void AttackersValue::AddAttackersOf(Group* group, set<Unit*>& targets)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player *member = sObjectMgr.GetPlayer(itr->guid);
        if (!member || !sServerFacade.IsAlive(member) || member == bot || member->GetMapId() != bot->GetMapId() || sServerFacade.GetDistance2d(bot, member) > sPlayerbotAIConfig.sightDistance)
            continue;

        AddAttackersOf(member, targets);
    }
}

struct AddGuardiansHelper
{
    explicit AddGuardiansHelper(std::list<Unit*> &units) : units(units) {}
    void operator()(Unit* target) const
    {
        units.push_back(target);
    }

    std::list<Unit*> &units;
};

void AttackersValue::AddAttackersOf(Player* player, set<Unit*>& targets)
{
    if (!player || !player->IsInWorld() || player->IsBeingTeleported())
        return;

	list<Unit*> units;
	//MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(player, sPlayerbotAIConfig.sightDistance);
    //MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(units, u_check);
    //Cell::VisitAllObjects(player, searcher, sPlayerbotAIConfig.sightDistance);

    std::list<ObjectGuid> unitGuids = AI_VALUE(std::list<ObjectGuid>, "possible targets");

    for (auto unitGuid : unitGuids)
    {
        Unit* unit = ai->GetUnit(unitGuid);
        if (unit)
            units.push_back(unit);
    }

	for (std::list<Unit*>::iterator i = units.begin(); i != units.end(); i++)
    {
		if (!player->GetGroup())
		{
			Unit* unit = *i;
#ifdef CMANGOS
			if (!unit->getThreatManager().getThreat(player) && (!unit->getThreatManager().getCurrentVictim() || unit->getThreatManager().getCurrentVictim()->getTarget() != player))
#endif
#ifdef MANGOS
			if (!unit->GetThreatManager().getThreat(player))
#endif
				continue;
		}
        {
            Unit* unit = *i;
            targets.insert(unit);
            unit->CallForAllControlledUnits(AddGuardiansHelper(units), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);
        }
    }
}

void AttackersValue::RemoveNonThreating(set<Unit*>& targets)
{
    for(set<Unit *>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        Unit* unit = *tIter;
        if (!IsValidTarget(unit, bot))
        {
            set<Unit *>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }
}

bool AttackersValue::IsPossibleTarget(Unit *attacker, Player *bot, float range)
{
    Creature *c = dynamic_cast<Creature*>(attacker);

    bool rti = false;
    if (attacker && bot->GetGroup())
        rti = bot->GetGroup()->GetTargetIcon(7) == attacker->GetObjectGuid();

    PlayerbotAI* ai = bot->GetPlayerbotAI();
    
    bool leaderHasThreat = false;
    if (attacker && bot->GetGroup() && ai->GetMaster())
        leaderHasThreat = attacker->getThreatManager().getThreat(ai->GetMaster());

    bool isMemberBotGroup = false;
    if (bot->GetGroup() && ai->GetMaster() && ai->GetMaster()->GetPlayerbotAI() && !ai->GetMaster()->GetPlayerbotAI()->IsRealPlayer())
        isMemberBotGroup = true;

    bool inCannon = ai->IsInVehicle(false, true);

    bool enemy = ai->GetAiObjectContext()->GetValue<Unit*>("enemy player target")->Get();
    bool duel = false;
    if (attacker && bot->duel && bot->duel->opponent && attacker->GetObjectGuid() == bot->duel->opponent->GetObjectGuid())
        duel = true;

#ifndef MANGOSBOT_ZERO
    Player* arenaEnemy = dynamic_cast<Player*>(attacker);
    if (arenaEnemy)
    {
        if (arenaEnemy->InArena() && bot->InArena() && arenaEnemy->GetBGTeam() != bot->GetBGTeam())
            duel = true;
    }
#endif

    return attacker &&
        attacker->IsInWorld() &&
        attacker->GetMapId() == bot->GetMapId() &&
        !sServerFacade.UnitIsDead(attacker) &&
        !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1) &&
        !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE) &&
        (inCannon || !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE)) &&
        attacker->IsVisibleForOrDetect(bot, bot->GetCamera().GetBody(), true) &&
#ifdef CMANGOS
        !(attacker->IsStunned() && ai->HasAura("shackle undead", attacker)) &&
        !ai->HasAura("gouge", attacker) &&
#endif
#ifdef MANGOS
        //!attacker->hasUnitState(UNIT_STAT_STUNNED) &&
#endif
        !((attacker->IsPolymorphed() ||
        bot->GetPlayerbotAI()->HasAura("sap", attacker) ||
        //sServerFacade.IsCharmed(attacker) ||
        sServerFacade.IsFeared(attacker)) && !rti) &&
        //!sServerFacade.IsInRoots(attacker) &&
        (!sServerFacade.IsFriendlyTo(attacker, bot) || duel) &&
        bot->IsWithinDistInMap(attacker, range) &&
        !attacker->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) &&
        !(attacker->GetObjectGuid().IsPet() && enemy) &&
        !(attacker->GetCreatureType() == CREATURE_TYPE_CRITTER && !attacker->IsInCombat()) &&
        !(sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(attacker)) && (attacker->GetObjectGuid().IsPlayer() || attacker->GetObjectGuid().IsPet())) &&
        (!c || (
#ifdef MANGOS
            !c->IsInEvadeMode() &&
#endif
#ifdef CMANGOS
            !c->GetCombatManager().IsInEvadeMode() &&
#endif
            (
#ifdef CMANGOS
                (!isMemberBotGroup && ai->HasStrategy("attack tagged", BOT_STATE_NON_COMBAT)) || leaderHasThreat ||
                (!c->HasLootRecipient() &&
                    (!c->GetVictim() ||
                    c->GetVictim() &&
                    ((bot->IsInGroup(c->GetVictim())) ||
                        (ai->GetMaster() &&
                            c->GetVictim() == ai->GetMaster())))) ||
                c->IsTappedBy(bot)
#endif
#ifndef MANGOSBOT_TWO
#ifdef MANGOS
                !attacker->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED) || bot->IsTappedByMeOrMyGroup(c)
#endif
#endif
                )
            )
            );
}

bool AttackersValue::IsValidTarget(Unit *attacker, Player *bot)
{
    return  IsPossibleTarget(attacker, bot) &&
        (sServerFacade.GetThreatManager(attacker).getCurrentVictim() ||
            attacker->GetGuidValue(UNIT_FIELD_TARGET) || attacker->GetObjectGuid().IsPlayer() ||
            attacker->GetObjectGuid() == bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("pull target")->Get());
}

bool PossibleAddsValue::Calculate()
{
    PlayerbotAI *ai = bot->GetPlayerbotAI();
    std::list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<std::list<ObjectGuid> >("possible targets no los")->Get();
    std::list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<std::list<ObjectGuid> >("attackers")->Get();

    for (std::list<ObjectGuid>::iterator i = possible.begin(); i != possible.end(); ++i)
    {
        ObjectGuid guid = *i;
        if (find(attackers.begin(), attackers.end(), guid) != attackers.end()) continue;

        Unit* add = ai->GetUnit(guid);
        if (add && !add->GetGuidValue(UNIT_FIELD_TARGET) && !sServerFacade.GetThreatManager(add).getCurrentVictim() && sServerFacade.IsHostileTo(add, bot))
        {
            for (std::list<ObjectGuid>::iterator j = attackers.begin(); j != attackers.end(); ++j)
            {
                Unit* attacker = ai->GetUnit(*j);
                if (!attacker) continue;

                float dist = sServerFacade.GetDistance2d(attacker, add);
                if (sServerFacade.IsDistanceLessOrEqualThan(dist, sPlayerbotAIConfig.aoeRadius * 1.5f)) continue;
                if (sServerFacade.IsDistanceLessOrEqualThan(dist, sPlayerbotAIConfig.aggroDistance)) return true;
            }
        }
    }
    return false;
}
