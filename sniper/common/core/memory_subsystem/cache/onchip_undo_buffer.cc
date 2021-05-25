#include "onchip_undo_buffer.h"
#include "stats.h"

UndoEntry::UndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info) : m_tag(cache_block_info->getTag()), m_valid_from_eid(cache_block_info->getEpochID()), m_valid_till_eid(system_eid) {}

UndoEntry::UndoEntry(const UndoEntry &orig) : m_tag(orig.m_tag), m_valid_from_eid(orig.m_valid_from_eid), m_valid_till_eid(orig.m_valid_till_eid) {}

UndoEntry::~UndoEntry() {}

OnChipUndoBuffer::OnChipUndoBuffer(UInt16 num_entries) : m_num_entries(num_entries), m_buffer() {}

OnChipUndoBuffer::~OnChipUndoBuffer() {}

bool OnChipUndoBuffer::createUndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info)
{
   static UInt64 num_writes = 0;
   try
   {
      // m_buffer.push(UndoEntry(system_eid, cache_block_info));
      m_buffer.push_back(UndoEntry(system_eid, cache_block_info));
      printf("NumWrites [%lu]\n", num_writes++);
      return true;
   }
   catch (const char *msg)
   {
      // code to handle exception
      return false;
   }
}

std::queue<UndoEntry> OnChipUndoBuffer::getOldEntries(UInt64 acs_eid)
{
   std::queue<UndoEntry> old_entries;
   // for (auto entry : m_buffer)
   // {
   //    if (entry.getValidFromEID() <= acs_eid) 
   //       old_entries.push(entry);
   // }
   std::vector<std::vector<UndoEntry>::iterator> erases;
   for (auto it = m_buffer.begin(); it != m_buffer.end(); ++it)
   {
      if (it->getValidFromEID() <= acs_eid)
      {
         old_entries.push(*it);
         erases.push_back(it);
      }
   }
   for (auto it : erases)
      m_buffer.erase(it);

   return old_entries;
}
