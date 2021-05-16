#include "epoch_manager.h"
#include "hooks_manager.h"
#include "magic_server.h"
#include "simulator.h"

EpochManager::EpochManager() : m_eid(0)
{
    printf("EpochManager created!\n");
}

EpochManager::~EpochManager()
{
    printf("EpochManager deleted!\n");
}

void EpochManager::start()
{
    printf("EpochManager::start()\n");
    Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __record, (UInt64)this);
}

void EpochManager::record(UInt64 simtime)
{
    UInt64 progress = MagicServer::getGlobalInstructionCount();
    printf("[EPOCH ID: %lu] - [%lu]\n", m_eid++, progress);
}
