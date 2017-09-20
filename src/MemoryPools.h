#ifndef RAMGRAPH_MEMORYPOOLS_H                                                       
#define RAMGRAPH_MEMORYPOOLS_H 

#include "FixedObjectPool.h"

#include "RamCloud.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

  class MemoryPools {
    public:
      static FixedObjectPool<ReadOpAndBuf> readOpAndBufPool;

//      template <typename... Args>
//      ReadOpAndBuf* 
//      newReadOpAndBuf(Args&&... args) {
//        return readOpAndBufPool.construct(static_cast<Args&&>(args)...);
//      };
//
//      static void
//      deleteReadOpAndBuf(ReadOpAndBuf* p) {
//        readOpAndBufPool.destroy(p);
//      };

  };

}; // namespace RAMGraph

#endif // RAMGRAPH_MEMORYPOOLS_H
