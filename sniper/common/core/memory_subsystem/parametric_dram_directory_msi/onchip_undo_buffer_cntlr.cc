#include "onchip_undo_buffer_cntlr.h"
#include "onchip_undo_buffer.h"
#include "epoch_manager.h"
#include "hooks_manager.h"
#include "magic_server.h"
#include "shmem_perf.h"
#include "stats.h"
#include "simulator.h"

namespace ParametricDramDirectoryMSI
{
   OnChipUndoBufferCntlr::OnChipUndoBufferCntlr(core_id_t core_id,
                                                MemoryManager *memory_manager,
                                                AddressHomeLookup *tag_directory_home_lookup,
                                                Semaphore *user_thread_sem,
                                                Semaphore *network_thread_sem,
                                                UInt32 cache_block_size,
                                                ShmemPerfModel *shmem_perf_model,
                                                UInt8 acs_gap) : m_core_id(core_id), m_memory_manager(memory_manager), m_cache_block_size(cache_block_size), m_acs_gap(acs_gap)
   {
      m_onchip_undo_buffer = new OnChipUndoBuffer();
      Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __acs, (UInt64)this);
      Sim()->getHooksManager()->registerHook(HookType::HOOK_SIM_END, __print, (UInt64)this);

      registerStatsMetric("picl", 0, "dram-writes", &m_total_dram_writes);
      // registerStatsMetric("picl", 0, "dram-rate", &m_dram_rate);
   }

   OnChipUndoBufferCntlr::~OnChipUndoBufferCntlr()
   {
      delete m_onchip_undo_buffer;
   }

   void OnChipUndoBufferCntlr::acs(UInt64 eid)
   {
      UInt64 system_eid = EpochManager::getGlobalSystemEID();
      UInt64 current_acs = (system_eid > m_acs_gap) ? (system_eid - m_acs_gap) : 0;

      printf("%lu [ACS %lu]\n", (UInt64)this, current_acs);

      m_onchip_undo_buffer->getOldEntries(current_acs);
   }

   void OnChipUndoBufferCntlr::print(UInt64 eid)
   {
      // printf("ON-CHIP UNDO BUFFER 1: [%lu]\n", m_onchip_undo_buffer->getNumWrites());
      UInt64 app_writes = Sim()->getStatsManager()->getMetricObject("dram", 0, "writes")->recordMetric();
      UInt64 log_writes = Sim()->getStatsManager()->getMetricObject("picl", 0, "log-writes")->recordMetric();
      
      m_total_dram_writes = app_writes + log_writes;
      m_dram_rate = (float) m_total_dram_writes / app_writes;

      printf("PiCL Writes: (%lu + %lu) = %lu | %.2f\n", app_writes, log_writes, m_total_dram_writes, m_dram_rate);
   }

   void OnChipUndoBufferCntlr::sendDataToNVM(CacheBlockInfo *cache_block_info)
   {
      // printf("Enviando %lu do OnChipUndoBuffer para a DRAM", cache_block_info->getEpochID());

      // IntPtr address = m_master->m_cache->tagToAddress(block_info->getTag());
      // Byte data_buf[getCacheBlockSize()];
      // getMemoryManager()->sendMsg(PrL1PrL2DramDirectoryMSI::ShmemMsg::CP_REP,
      //                             MemComponent::ONCHIP_UNDO_BUFFER, MemComponent::DRAM,
      //                             m_core_id_master, getHome(address), /* requester and receiver */
      //                             address, data_buf, getCacheBlockSize(),
      //                             HitWhere::UNKNOWN, &m_dummy_shmem_perf, ShmemPerfModel::_SIM_THREAD);
   }

}
