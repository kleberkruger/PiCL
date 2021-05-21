#ifndef ONCHIP_UNDO_BUFFER_CNTLR_H
#define ONCHIP_UNDO_BUFFER_CNTLR_H

#include "onchip_undo_buffer.h"

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
      OnChipUndoBufferCntlr(MemoryManager *memory_manager, UInt8 acs_gap = 3);
      virtual ~OnChipUndoBufferCntlr();

   private:
      MemoryManager *m_memory_manager;
      OnChipUndoBuffer *m_onchip_undo_buffer;
      UInt8 m_acs_gap;

      static SInt64 __acs(UInt64 arg, UInt64 val)
      {
         ((OnChipUndoBufferCntlr *)arg)->acs(val);
         return 0;
      }
      void acs(UInt64 eid);

      void sendDataToNVM();

      MemoryManager *getMemoryManager() { return m_memory_manager; }
   };

}

#endif /* ONCHIP_UNDO_BUFFER_CNTLR_H */
