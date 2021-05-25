#ifndef ONCHIP_UNDO_BUFFER_CNTLR_H
#define ONCHIP_UNDO_BUFFER_CNTLR_H

#include "onchip_undo_buffer.h"
#include "semaphore.h"
#include "shmem_perf.h"
#include "shmem_perf_model.h"

// Forward declarations
namespace ParametricDramDirectoryMSI
{
   class MemoryManager;
}
// class ShmemPerf;

namespace ParametricDramDirectoryMSI
{

   class OnChipUndoBufferCntlr
   {
   public:
      // OnChipUndoBufferCntlr(MemoryManager *memory_manager, UInt8 acs_gap = 3);
      OnChipUndoBufferCntlr(
          core_id_t core_id,
          MemoryManager *memory_manager,
          AddressHomeLookup *tag_directory_home_lookup,
          Semaphore *user_thread_sem,
          Semaphore *network_thread_sem,
          UInt32 cache_block_size,
          ShmemPerfModel *shmem_perf_model,
          UInt8 acs_gap = 3);

      virtual ~OnChipUndoBufferCntlr();

      OnChipUndoBuffer *getOnChipUndoBuffer() { return m_onchip_undo_buffer; }

   private:
      // MemComponent::component_t mem_component
      // String name,
      core_id_t m_core_id;
      MemoryManager *m_memory_manager;
      UInt32 m_cache_block_size;
      ShmemPerf m_dummy_shmem_perf;
      OnChipUndoBuffer *m_onchip_undo_buffer;
      UInt8 m_acs_gap;
      
      UInt64 m_total_dram_writes;
      float m_dram_rate;

      static SInt64 __acs(UInt64 arg, UInt64 val)
      {
         ((OnChipUndoBufferCntlr *)arg)->acs(val);
         return 0;
      }
      void acs(UInt64 eid);

      static SInt64 __print(UInt64 arg, UInt64 val)
      {
         ((OnChipUndoBufferCntlr *)arg)->print(val);
         return 0;
      }
      void print(UInt64 eid);

      void sendDataToNVM(CacheBlockInfo *cache_block_info);

      MemoryManager *getMemoryManager() { return m_memory_manager; }
      UInt32 getCacheBlockSize() { return m_cache_block_size; }
   };

}

#endif /* ONCHIP_UNDO_BUFFER_CNTLR_H */
