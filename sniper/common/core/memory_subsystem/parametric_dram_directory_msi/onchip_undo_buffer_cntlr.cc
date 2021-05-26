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
      bzero(&stats, sizeof(stats));

      registerStatsMetric("onchip_undo_buffer", m_core_id, "log_writes", &stats.log_writes);
      registerStatsMetric("onchip_undo_buffer", m_core_id, "total_dram_writes", &stats.total_writes);
      registerStatsMetric("onchip_undo_buffer", m_core_id, "avg_log_writes_by_epoch", &stats.avg_log_writes_by_epoch);
      registerStatsMetric("onchip_undo_buffer", m_core_id, "overhead_rate", &stats.overhead_rate);

      m_onchip_undo_buffer = new OnChipUndoBuffer();

      Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __acs, (UInt64)this);
      Sim()->getHooksManager()->registerHook(HookType::HOOK_SIM_END, __print, (UInt64)this);
   }

   OnChipUndoBufferCntlr::~OnChipUndoBufferCntlr()
   {
      delete m_onchip_undo_buffer;
   }

   // TODO: Escrever uma classe para registar as métricas de cada ACS (quantas escritas por época?)
   void OnChipUndoBufferCntlr::acs(UInt64 eid)
   {
      UInt64 system_eid = EpochManager::getGlobalSystemEID();
      UInt64 current_acs = (system_eid > m_acs_gap) ? (system_eid - m_acs_gap) : 0;

      printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
      printf("Cntlr: [%lu] | [ACS %lu]\n", (UInt64)this, current_acs);
      m_onchip_undo_buffer->print();
      std::queue<UndoEntry> log_entries = m_onchip_undo_buffer->getOldEntries(current_acs);
      while (!log_entries.empty())
      {
         auto entry = log_entries.front();
         sendDataToNVM(entry);
         log_entries.pop();
      }
      m_onchip_undo_buffer->print();
      printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
      printf("\n");
   }

   void OnChipUndoBufferCntlr::print(UInt64 eid)
   {
      // printf("ON-CHIP UNDO BUFFER 1: [%lu]\n", m_onchip_undo_buffer->getNumWrites());
      // UInt64 app_writes = Sim()->getStatsManager()->getMetricObject("dram", 0, "writes")->recordMetric();
      // UInt64 log_writes = Sim()->getStatsManager()->getMetricObject("picl", 0, "log-writes")->recordMetric();

      // registerStatsMetric("dram", memory_manager->getCore()->getId(), "writes", &m_writes);

      // m_total_dram_writes = app_writes + log_writes;
      // m_dram_rate = (float)m_total_dram_writes / app_writes;

      // printf("PiCL Writes: (%lu + %lu) = %lu | %.2f\n", app_writes, log_writes, m_total_dram_writes, m_dram_rate);
   }

   void OnChipUndoBufferCntlr::sendDataToNVM(const UndoEntry &entry)
   {
      // printf("Enviando UndoEntry [tag = %lu | valid_from_eid = %lu | valid_till_eid = %lu]\n", entry.getTag(), entry.getValidFromEID(), entry.getValidTillEID());

      // IntPtr address = m_master->m_cache->tagToAddress(block_info->getTag());
      // Byte data_buf[getCacheBlockSize()];
      // getMemoryManager()->sendMsg(PrL1PrL2DramDirectoryMSI::ShmemMsg::CP_REP,
      //                             MemComponent::ONCHIP_UNDO_BUFFER, MemComponent::DRAM,
      //                             m_core_id_master, getHome(address), /* requester and receiver */
      //                             address, data_buf, getCacheBlockSize(),
      //                             HitWhere::UNKNOWN, &m_dummy_shmem_perf, ShmemPerfModel::_SIM_THREAD);
      stats.log_writes++;
   }

}
