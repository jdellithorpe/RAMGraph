#ifndef RAMGRAPH_COMMON_H                                                       
#define RAMGRAPH_COMMON_H 

#include <stdint.h>

#include "RamCloud.h"
#include "Buffer.h"
#include "Transaction.h"
#include "Cycles.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

uint64_t htonll(uint64_t hostlonglong);
uint64_t ntohll(uint64_t hostlonglong);

struct ReadOpAndBuf {
  Buffer val;
  ReadOp op;

  ReadOpAndBuf(Transaction* tx, uint64_t tableId, const void* key, uint16_t
      keyLength, bool batch) 
      : val()
      , op(tx, tableId, key, keyLength, &val, batch) {

  }
};

} // namespace RAMGraph

#endif // RAMGRAPH_COMMON_H
