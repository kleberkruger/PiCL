#ifndef ONCHIP_UNDO_BUFFER_H
#define ONCHIP_UNDO_BUFFER_H

#include <queue>
#include <deque>
#include <vector>
#include "cache_block_info.h"
#include "mem_component.h"

class UndoEntry
{
public:

   UndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info);
   UndoEntry(const UndoEntry &orig);
   
   virtual ~UndoEntry();

   IntPtr getTag() const { return m_tag; }
   UInt64 getValidFromEID() const { return m_valid_from_eid; }
   UInt64 getValidTillEID() const { return m_valid_till_eid; }

private:
   IntPtr m_tag;
   UInt64 m_valid_from_eid;
   UInt64 m_valid_till_eid;
   // UInt64 data;
};

class OnChipUndoBuffer
{
private:

   // // template <typename T, int MaxLen, typename Container = std::deque<T>>
   // template <typename T, typename Container = std::deque<T>>
   // class FixedQueue : public std::queue<T, Container>
   // {
   // public:
   //    FixedQueue(const UInt32 maxlen) : m_maxlen(maxlen) {}
   //    // virtual ~FixedQueue() {}

   //    void push(const T &value)
   //    {
   //       // if (this->size() == m_maxlen)
   //       //    this->c.pop_front();
   //       if (m_maxlen != 0 && m_maxlen < this->size())
   //          throw "On-chip undo buffer limit exceeded";

   //       std::queue<T, Container>::push(value);
   //    }
   // private:
   //    const UInt16 m_maxlen;
   // };

   const UInt16 m_num_entries;
   // FixedQueue<UndoEntry> m_buffer;
   std::vector<UndoEntry> m_buffer;

public:

   OnChipUndoBuffer(UInt16 num_entries = 0);
   virtual ~OnChipUndoBuffer();

   bool createUndoEntry(UInt64 system_eid, CacheBlockInfo *cache_block_info);

   std::queue<UndoEntry> getOldEntries(UInt64 acs_eid);
   std::queue<UndoEntry> getAllEntries();

   void print();

   String getName(void) const { return "OnChipUndoBuffer"; }
   MemComponent::component_t getMemComponent() const { return MemComponent::ONCHIP_UNDO_BUFFER; }
   UInt16 getNumEntries() const { return m_num_entries; }
};

#endif /* ONCHIP_UNDO_BUFFER_H */
