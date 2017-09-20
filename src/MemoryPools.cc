#include "MemoryPools.h"

namespace RAMGraph {
  FixedObjectPool<ReadOpAndBuf> MemoryPools::readOpAndBufPool(100000);
}
