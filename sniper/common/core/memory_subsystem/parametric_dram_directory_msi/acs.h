#ifndef ASYNC_CACHE_SCAN_H
#define ASYNC_CACHE_SCAN_H

#include "cache_cntlr.h"
#include "onchip_undo_buffer_cntlr.h"

// Forward declarations
namespace ParametricDramDirectoryMSI
{
   class MemoryManager;
}
// class ShmemPerf;

namespace ParametricDramDirectoryMSI
{
   class AsyncCacheScanner
   {
   public:
      AsyncCacheScanner(CacheCntlr *cache_cntlr, OnChipUndoBufferCntlr *onchip_undo_buffer_cntlr);
      ~AsyncCacheScanner();

      friend class MemoryManager;

   private:
      struct
      {
      } stats;
   };
}

#endif /* ASYNC_CACHE_SCAN_H */
