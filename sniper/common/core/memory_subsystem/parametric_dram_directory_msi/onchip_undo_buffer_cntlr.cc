#include "onchip_undo_buffer_cntlr.h"
#include "onchip_undo_buffer.h"
#include "epoch_manager.h"
#include "hooks_manager.h"
#include "magic_server.h"
#include "memory_manager.h"
#include "shmem_perf.h"
#include "stats.h"
#include "simulator.h"
#include "config.hpp"
#include <cstring>

namespace ParametricDramDirectoryMSI
{
   OnChipUndoBufferCntlr::OnChipUndoBufferCntlr(core_id_t core_id_master,
                                                MemoryManager *memory_manager,
                                                AddressHomeLookup *tag_directory_home_lookup,
                                                UInt32 cache_block_size,
                                                ShmemPerfModel *shmem_perf_model)
       : m_core_id_master(core_id_master),
         m_memory_manager(memory_manager),
         m_tag_directory_home_lookup(tag_directory_home_lookup),
         m_shmem_perf_model(shmem_perf_model),
         m_cache_block_size(cache_block_size)
   {
      m_acs_gap = Sim()->getCfg()->hasKey("picl/acs_gap") ? Sim()->getCfg()->getInt("picl/acs_gap") : DEFAULT_ACS_GAP;
      m_onchip_undo_buffer = new OnChipUndoBuffer(Sim()->getCfg()->hasKey("onchip_undo_buffer/num_entries")
                                                      ? Sim()->getCfg()->getInt("onchip_undo_buffer/num_entries")
                                                      : DEFAULT_NUM_ENTRIES);

      Sim()->getHooksManager()->registerHook(HookType::HOOK_PERIODIC_INS, __async_cache_scan, (UInt64)this);
      Sim()->getHooksManager()->registerHook(HookType::HOOK_APPLICATION_EXIT, __persist_last_epochs, (UInt64)this);

      memset(&stats, 0, sizeof(stats));
      registerStatsMetric("onchip_undo_buffer", m_core_id_master, "log_writes", &stats.log_writes);
      registerStatsMetric("onchip_undo_buffer", m_core_id_master, "avg_log_writes_by_epoch", &stats.avg_log_writes_by_epoch);
   }

   OnChipUndoBufferCntlr::~OnChipUndoBufferCntlr()
   {
      delete m_onchip_undo_buffer;
   }

   void OnChipUndoBufferCntlr::insertUndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info)
   {
      if (m_onchip_undo_buffer->isFull())
      {
      }
      m_onchip_undo_buffer->insertUndoEntry(system_eid, cache_block_info);
   }

   void OnChipUndoBufferCntlr::flush()
   {
   }

   // TODO: Escrever uma classe para registar as métricas de cada ACS (quantas escritas por época?)
   void OnChipUndoBufferCntlr::asyncCacheScan(UInt64 eid)
   {
      UInt64 system_eid = EpochManager::getGlobalSystemEID();
      UInt64 current_acs = (system_eid > m_acs_gap) ? (system_eid - m_acs_gap) : 0;

      std::queue<UndoEntry> log_entries = m_onchip_undo_buffer->removeOldEntries(current_acs);
      while (!log_entries.empty())
      {
         auto entry = log_entries.front();
         sendDataToNVM(entry);
         log_entries.pop();
      }
      EpochManager::setGlobalPersistedEID(current_acs);
   }

   void OnChipUndoBufferCntlr::persistLastEpochs(UInt64 eid)
   {
      // registerStatsMetric("dram", memory_manager->getCore()->getId(), "writes", &m_writes);
      std::queue<UndoEntry> log_entries = m_onchip_undo_buffer->removeAllEntries();
      while (!log_entries.empty())
      {
         auto entry = log_entries.front();
         sendDataToNVM(entry);
         log_entries.pop();
      }
      printf("PiCL Writes: (%lu)\n", stats.log_writes);
   }

   void OnChipUndoBufferCntlr::sendDataToNVM(const UndoEntry &entry)
   {
      IntPtr address = getLogAddress();
      Byte data_buf[getCacheBlockSize()];
      getMemoryManager()->sendMsg(PrL1PrL2DramDirectoryMSI::ShmemMsg::CP_REP,
                                  MemComponent::ONCHIP_UNDO_BUFFER, MemComponent::DRAM,
                                  m_core_id_master, getHome(address), /* requester and receiver */
                                  address, data_buf, getCacheBlockSize(),
                                  HitWhere::UNKNOWN, &m_dummy_shmem_perf, ShmemPerfModel::_SIM_THREAD);
      stats.log_writes++;
   }

}
