#include "onchip_undo_buffer.h"

UndoEntry::UndoEntry(CacheBlockInfo *cache_block_info) {}

UndoEntry::~UndoEntry() {}

OnChipUndoBuffer::OnChipUndoBuffer(UInt16 num_entries) : m_num_entries(num_entries), m_buffer(num_entries) {}

OnChipUndoBuffer::~OnChipUndoBuffer() {}

bool OnChipUndoBuffer::createUndoEntry(CacheBlockInfo *cache_block_info)
{
   printf("Criando entrada para o endereco: [%lu]\n", cache_block_info->getTag());
   return true;
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
