#include "onchip_undo_buffer.h"
#include "stats.h"

UndoEntry::UndoEntry(CacheBlockInfo *cache_block_info) {}

UndoEntry::~UndoEntry() {}

OnChipUndoBuffer::OnChipUndoBuffer(UInt16 num_entries) : m_num_entries(num_entries), m_num_writes(0), m_buffer(num_entries) 
{
   registerStatsMetric("picl", 0, "log-writes", &m_num_writes);
}

OnChipUndoBuffer::~OnChipUndoBuffer() {}

bool OnChipUndoBuffer::createUndoEntry(CacheBlockInfo *cache_block_info)
{
   try
   {
      m_buffer.push(UndoEntry(cache_block_info));
      m_num_writes++;

      // printf("Criando entrada para o endereco: [%lu]\n", cache_block_info->getTag());
      // printf("OnChip Undo Buffer: [%lu]\n", n);

      return true;
   }
   catch (const char *msg)
   {
      // code to handle exception
      return false;
   }
}

std::queue<CacheBlockInfo *> OnChipUndoBuffer::getOldEntries(UInt64 acs_eid)
{
   std::queue<CacheBlockInfo *> old_entries;
   while (!m_buffer.empty())
   {
      auto undo_entry = m_buffer.front();
      m_buffer.pop();
   }
   return old_entries;
}

// Números do gcc:
// Baseline: 1450001
// Extras:   1495619
// PiCL:     2945620
// Taxa:     2,03%
