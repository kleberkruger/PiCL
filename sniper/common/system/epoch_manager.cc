#include "epoch_manager.h"
#include "hooks_manager.h"
#include "magic_server.h"
#include "simulator.h"

EpochManager::EpochManager() : m_system_eid(1)
{
}

EpochManager::~EpochManager()
{
}

void EpochManager::start()
{
    Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __increment, (UInt64)this);
}

void EpochManager::increment(UInt64 simtime)
{
    UInt64 progress = MagicServer::getGlobalInstructionCount();
    printf("[EPOCH ID: %lu] - [%lu]\n", m_system_eid, progress);

    m_system_eid++;
}

UInt64 EpochManager::getGlobalSystemEID()
{
    return Sim()->getEpochManager()->getSystemEID();
}
