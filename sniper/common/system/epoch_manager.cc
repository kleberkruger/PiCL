#include "epoch_manager.h"
#include "hooks_manager.h"
#include "simulator.h"
#include "stats.h"

EpochManager::EpochManager() : m_system_eid(1), m_persisted_eid(0) 
{
    registerStatsMetric("epoch", 0, "system_eid", &m_system_eid);
    registerStatsMetric("epoch", 0, "persisted_eid", &m_persisted_eid);
}

EpochManager::~EpochManager() {}

void EpochManager::start()
{
    Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __increment, (UInt64)this);
}

void EpochManager::increment(UInt64 simtime)
{
    m_system_eid++;
}

UInt64 EpochManager::getGlobalSystemEID()
{
    return Sim()->getEpochManager()->getSystemEID();
}

void EpochManager::setGlobalPersistedEID(const UInt64 persisted_eid)
{
    return Sim()->getEpochManager()->setPersistedEID(persisted_eid);
}
