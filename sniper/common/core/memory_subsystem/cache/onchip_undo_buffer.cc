#include "onchip_undo_buffer.h"
#include "stats.h"

#include <deque>
#include <algorithm>

UndoEntry::UndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info) : m_tag(cache_block_info->getTag()), m_valid_from_eid(cache_block_info->getEpochID()), m_valid_till_eid(system_eid) {}

UndoEntry::UndoEntry(const UndoEntry &orig) : m_tag(orig.m_tag), m_valid_from_eid(orig.m_valid_from_eid), m_valid_till_eid(orig.m_valid_till_eid) {}

UndoEntry::~UndoEntry() {}

OnChipUndoBuffer::OnChipUndoBuffer(UInt16 num_entries) : m_num_entries(num_entries), m_buffer()
{
   m_buffer.reserve(num_entries);
}

OnChipUndoBuffer::~OnChipUndoBuffer() {}

bool OnChipUndoBuffer::createUndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info)
{
   try
   {
      m_buffer.push_back(UndoEntry(system_eid, cache_block_info));
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
   for (auto entry : m_buffer)
   {
      if (entry.getValidFromEID() <= acs_eid)
         old_entries.push(entry);
   }

   m_buffer.erase(std::remove_if(m_buffer.begin(), m_buffer.end(), [&](UndoEntry e)
                                 { return e.getValidFromEID() <= acs_eid; }), m_buffer.end());

   return old_entries;
}

std::queue<UndoEntry> OnChipUndoBuffer::getAllEntries()
{
   std::queue<UndoEntry> entries(std::deque<UndoEntry>(m_buffer.begin(), m_buffer.end()));
   m_buffer.clear();
   return entries;
}

// void printf_on_center(char *str)
// {
//    printf("---%*s%*s---\n", 10 + strlen(str) / 2, str, 10 - strlen(str) / 2, "");
// }

void OnChipUndoBuffer::print()
{
   printf("============================================================\n");
   printf("ON-CHIP UNDO BUFFER\n");
   printf("------------------------------------------------------------\n");
   printf("                TAG    FROM    TILL\n");
   for (std::size_t i = 0; i < m_buffer.size(); i++)
   {
      auto entry = m_buffer.at(i);
      printf("%3lu: [%13lu | %5lu | %5lu]\n", i, entry.getTag(), entry.getValidFromEID(), entry.getValidTillEID());
   }
   printf("============================================================\n");
}
