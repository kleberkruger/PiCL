#include "onchip_undo_buffer_cntlr.h"
#include "onchip_undo_buffer.h"
#include "epoch_manager.h"
#include "hooks_manager.h"
#include "magic_server.h"
#include "shmem_perf.h"
#include "simulator.h"

namespace ParametricDramDirectoryMSI
{
   OnChipUndoBufferCntlr::OnChipUndoBufferCntlr(MemoryManager *memory_manager, UInt8 acs_gap)
       : m_memory_manager(memory_manager), m_acs_gap(acs_gap)
   {
      printf("CRIADO OnChipUndoBufferCntlr: %lu\n", (UInt64) this);

      m_onchip_undo_buffer = new OnChipUndoBuffer();
      Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __acs, (UInt64)this);
   }

   OnChipUndoBufferCntlr::~OnChipUndoBufferCntlr()
   {
      delete m_onchip_undo_buffer;
   }

   void OnChipUndoBufferCntlr::acs(UInt64 eid)
   {
      UInt64 system_eid = EpochManager::getGlobalSystemEID();
      UInt64 current_acs = (system_eid > m_acs_gap) ? (system_eid - m_acs_gap) : 0;
      
      printf("[ACS %lu]\n", current_acs);
   }

   void OnChipUndoBufferCntlr::sendDataToNVM()
   {
      // IntPtr address = m_master->m_cache->tagToAddress(block_info->getTag());
      // Byte data_buf[getCacheBlockSize()];
      // getMemoryManager()->sendMsg(PrL1PrL2DramDirectoryMSI::ShmemMsg::CP_REP,
      //                             MemComponent::LAST_LEVEL_CACHE, MemComponent::TAG_DIR,
      //                             m_core_id_master, getHome(address), /* requester and receiver */
      //                             address, data_buf, getCacheBlockSize(),
      //                             HitWhere::UNKNOWN, &m_dummy_shmem_perf, ShmemPerfModel::_SIM_THREAD);
   }
   
}
