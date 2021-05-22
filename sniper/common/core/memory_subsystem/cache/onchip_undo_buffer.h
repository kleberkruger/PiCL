#ifndef ONCHIP_UNDO_BUFFER_H
#define ONCHIP_UNDO_BUFFER_H

#include <queue>
#include <deque>
#include "cache_block_info.h"

class UndoEntry
{
public:
   UndoEntry(CacheBlockInfo *cache_block_info);
   virtual ~UndoEntry();

private:
   UInt64 valid_from_eid;
   UInt64 valid_till_eid;
};

class OnChipUndoBuffer
{
private:
   // template <typename T, int MaxLen, typename Container = std::deque<T>>
   template <typename T, typename Container = std::deque<T>>
   class FixedQueue : public std::queue<T, Container>
   {
   public:
      FixedQueue(const UInt32 maxlen) : m_maxlen(maxlen) {}
      // virtual ~FixedQueue() {}

      void push(const T &value)
      {
         if (this->size() == m_maxlen)
            this->c.pop_front();

         std::queue<T, Container>::push(value);
      }
   private:
      const UInt16 m_maxlen;
   };

   const UInt16 m_num_entries;
   FixedQueue<UndoEntry> m_buffer;

public:

   OnChipUndoBuffer(UInt16 num_entries = 32);
   virtual ~OnChipUndoBuffer();

   bool createUndoEntry(CacheBlockInfo *cache_block_info);

   std::queue<CacheBlockInfo *> getOldEntries(UInt64 acs_eid);

   const UInt16 &getNumEntries() const { return m_num_entries; }
};

#endif /* ONCHIP_UNDO_BUFFER_H */
